#include <dblang.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

Action get_action_from_str(const char *str){
    Action result = IDENTIFIER;

    if(str[0] == '"') result = VALUE_CHARS;
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
    else if(strchr(str, ':') != NULL) result = TYPE_ID;

    return result;
}

int get_value_from_action(Value *value, char *buffer){
    if(value->act == NONE_ACT){
        char *end;
        value->number = strtof(buffer, &end);

        if(*end != '\0'){
            perror("Unable to turn value into a number");
            return -1;        
        }
        
    }else if(value->act == TYPE_ID){
        char *type = strchr(buffer, ':');
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
    }else{
        value->string = strdup(buffer);
        if(value->string == NULL){
            perror("Unable to allocate new identifier");
        }
    }

    return 1;

}

Value *add_value(Value *values, int *index, int capacity, char *buffer){
    puts(buffer);
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
    values[*index++] = new_value;
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
    for(i = 0; i < size; ++i){
        Value *current = &lang[i];
        switch (current->act)
        {
        case CREATE:
            if(i + 1 == size){
                perror("CREATE expects 1 or more arguments");
                return -1;
            }
            if(lang[i + 1].act != DATABASE || lang[i + 1].act != TABLE);
            break;
        
        default:
            break;
        }
    }
    return -1;
}