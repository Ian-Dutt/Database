#include <dblang.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "database.h"

void free_lang(Value *lang, int size){
    for(int i = 0; i < size; ++i){
        if(lang[i].act == TYPE_ID)
            free(lang[i].type_id.id);
        else
            free(lang[i].string);
    }
    free(lang);
}

Action get_action_from_str(const char *str){
    // puts(str);
    Action result = IDENTIFIER;

    if(str[0] == '"') result = VALUE_CHARS;
    else if(str[0] == '\'') result = VALUE_CHAR;
    else if(strcmp(str, "CREATE") == 0) result = CREATE;
    else if(strcmp(str, "TABLE") == 0) result = TABLE;
    else if(strcmp(str, "DATABASE") == 0) result = DATABASE;
    else if(strcmp(str, "INT") == 0) result = TYPE_INT;
    else if(strcmp(str, "CHAR") == 0) result = TYPE_CHAR;
    else if(strcmp(str, "CHARS") == 0) result = TYPE_CHARS;
    else if(strcmp(str, "DOUBLE") == 0) result = TYPE_DOUBLE;
    else if(strcmp(str, "LONG") == 0) result = TYPE_LONG;
    else if(strcmp(str, "INSERT") == 0) result = INSERT;
    else if(strcmp(str, "ROW") == 0) result = ROW;
    else if(strcmp(str, ":-") == 0) result = SEP;
    else if(strchr(str, '=') != NULL) result = TYPE_ID;
    else if(strcmp(str, ";") == 0) result = EXPR_END;

    return result;
}

int get_value_from_action(Value *value, char *buffer){
    if(value->act == TYPE_ID){
        char *type = strchr(buffer, '=');
        *type = '\0';
        type++;

        value->type_id.type = get_action_from_str(type);
        value->type_id.id = strdup(buffer);
        if(value->type_id.id == NULL){
            perror("Unable to allocate new type id");
        }
    }else if(value->act == VALUE_CHARS){
        value->string = strndup(buffer + 1, strlen(buffer) - 2);
        if(value->string == NULL){
            perror("Unable to allocate new string");
        }
    }else if(value->act == VALUE_CHAR){
        value->string = strndup(buffer + 1, strlen(buffer) - 2);
        if(value->string == NULL){
            perror("Unable to allocate new string");
        }
    }else{
        char *end;
        value->number = strtof(buffer, &end);

        if(*end != '\0'){
            value->string = strdup(buffer);
            if(value->string == NULL){
                perror("Unable to allocate new identifier");
            }
        }else{
            value->act = VALUE_DOUBLE;
        }
    }

    return 1;

}

Value *add_value(Value *values, int *index, int capacity, char *buffer){
    // puts(buffer);
    Value new_value = {0};
    new_value.act = get_action_from_str(buffer);
    get_value_from_action(&new_value, buffer);

    if(*index >= capacity){
        capacity = capacity * 2;
        void *tmp = realloc(values, capacity * sizeof(Value));
        if(tmp == NULL){
            perror("Unable to create room for the program");
            free(values);
            return NULL;
        }

        values = tmp;
    }
    memset(buffer, 0, 128);
    values[*index] = new_value;
    *index = *index + 1;
    return values;
}

Value *lexer(FILE *in, int *size){
    char buffer[128] = {0};
    int c;
    int i = 0;
    int capacity = 20;
    int index = 0;
    Value *values = malloc(sizeof(Value) * 20);
    while((c = fgetc(in)) != EOF){
        // Read until a space/newline
        if(isspace(c)){
            // check if the previous one was empty
            if(i == 0){
                continue;
            }
            i = 0;
            values = add_value(values, &index, capacity, buffer);
        }else if(c == '"'){
            // if(buffer[0] == '\0') perror("Something went wrong, ");
            buffer[i++] = '"';
            while((c = fgetc(in)) != EOF && c != '"') buffer[i++] = c;
            buffer[i++] = '"';
            values = add_value(values, &index, capacity, buffer);
            i = 0;
        }else{
            buffer[i++] = c;
        }
    }
    values = add_value(values, &index, capacity, buffer);
    *size = index;
    return values;
}

int interpret_lang(Value *lang, int size){
    int i = 0;
    Database *db = NULL;
    for(i = 0; i < size; ++i){
        Value *current = &lang[i];
        switch (current->act)
        {
        case CREATE:
            if(i + 1 >= size){
                perror("CREATE expects 1 or more arguments");
                return -1;
            }

            if(lang[i + 1].act == DATABASE){
                if(i + 2 >= size){
                    perror("CREATE DATABASE expects 1 or more arguments");
                    return -1;
                }
                db = create_database(lang[i + 2].string);
                i += 2;
            }else if(lang[i + 1].act == TABLE){
                if(i + 5 >= size){
                    perror("usage: CREATE TABLE TableName :- ColName=TYPE ... ;");
                    return -1;
                }
                // int create_table(Database *db, const char *name, const char **columns, TYPES *types, int num_cols);

                if(db == NULL){
                    perror("No database chosen");
                    return -1;
                }
                
                int table_size = 0;
                for(int j = i + 4; table_size < size && lang[j].act != EXPR_END; ++table_size, j++){
                    if(lang[j].act != TYPE_ID){
                        printf("%d is not a valid type id\n", lang[j].act);
                        return -1;
                    }
                }
                // printf("table_size: %d\n",table_size);
                const char *table_name = lang[i + 2].string;
                TYPES *types = malloc(sizeof(TYPES) * table_size);

                if(types == NULL){
                    perror("Unable to allocate memory for types\n");
                    return -1;
                }
                char **columns = malloc(sizeof(char *) * table_size);
                if(columns == NULL){
                    perror("Unable to allocate memory for columns\n");
                    return -1;
                }
                int k;
                for(i = i + 4, k = 0; k < table_size; ++k, ++i){
                    types[k] = act_to_type(lang[i].type_id.type);
                    columns[k] = lang[i].type_id.id;
                }

                create_table(db, table_name, (const char **)columns, types, table_size);

                // print_tables(stdout, db);
                free(types);
                free(columns);
            }else{
                perror("Invalid CREATE command");
                return -1;
            }

            break;
        case INSERT:
            if(i + 5 >= size){
                perror("Not enough arguments for INSERT operation");
                return -1;
            }

            if(lang[i + 1].act != ROW){
                perror("INSERT expects ROW as second argument");
                return -1;
            }

            if(lang[i + 2].act != IDENTIFIER){
                perror("INSERT expects Table Name as third argument");
                return -1;
            }

            Table *table = find_table(db, lang[i + 2].string);

            if(table == NULL){
                printf("Table '%s' is hot present in the database\n", lang[i + 2].string);
                return -1;
            }

            char *row = alloc_row(table);
            if(row == NULL){
                perror("Unable to allocate table row");
                return -1;
            }
            int offset = 0;
            i = i + 4;
            for(int j = 0; j < table->num_cols; ++j, ++i){
                switch(table->types[j]){
                    case INT:
                        if(DOUBLE != act_to_type(lang[i].act)){
                            printf("%d\n", lang[i].act);
                            perror("Expected INT but got something else");
                            return -1;
                        }
                        if(floor(lang[i].number) != lang[i].number){
                            perror("Unable to convert double into int");
                            return -1;
                        }
                        *((int *)(row  + offset)) =  (int)lang[i].number;
                        offset += types_sizes[INT];
                        break;
                    case LONG:
                        if(DOUBLE != act_to_type(lang[i].act)){
                            perror("Expected LONG but got something else");
                            return -1;
                        }
                        if(floor(lang[i].number) != lang[i].number){
                            perror("Unable to convert double into long");
                            return -1;
                        }
                        *((long *)(row  + offset)) =  (long)lang[i].number;
                        offset += types_sizes[LONG];
                        break;
                    case DOUBLE:
                        if(DOUBLE != act_to_type(lang[i].act)){
                            perror("Expected DOUBLE but got something else");
                            return -1;
                        }
                        *((double *)(row  + offset)) = lang[i].number;
                        offset += types_sizes[DOUBLE];
                        break;
                    case CHAR:
                        if(CHAR != act_to_type(lang[i].act)){
                            printf("%d\n", lang[i].act);
                            perror("Expected CHAR but got something else");
                            return -1;
                        }
                        *((char *)(row + offset)) = *(lang[i].string);
                        offset += types_sizes[CHAR];
                        break;
                    case CHARS:
                        if(CHARS != act_to_type(lang[i].act)){
                            printf("%d\n", lang[i].act);
                            perror("Expected CHARS but got something else");
                            return -1;
                        }
                        strcpy((char *)(row + offset), lang[i].string);
                        offset += types_sizes[CHARS];
                        break;
                    case NONE_TYPE:
                        puts("No type");
                        // return -1;
                }
            }

            insert_row(table, row);
            print_tables(stdout, db);
            break;
        default:
            break;
        }
    }
    delete_database(db);
    return -1;
}


TYPES act_to_type(Action act){
    switch(act){
        case VALUE_INT:
        case TYPE_INT:
            return INT;

        case VALUE_LONG:
        case TYPE_LONG: 
            return LONG;

        case VALUE_DOUBLE:
        case TYPE_DOUBLE:
            return DOUBLE;

        case VALUE_CHARS:
        case TYPE_CHARS:
            return CHARS;

        case VALUE_CHAR:
        case TYPE_CHAR:
            return CHAR;

        default:
            return NONE_TYPE;
    }
}