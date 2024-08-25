#include <dblang.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

Action get_action_from_str(const char *str){
    Action result = NONE_ACT;

    if(strcmp(str, "CREATE") == 0) result = CREATE;
    else if(strcmp(str, "TABLE") == 0) result = TABLE;
    else if(strcmp(str, "DATABASE") == 0) result = DATABASE;
    else if(strcmp(str, "INT") == 0) result = TYPE_INT;
    else if(strcmp(str, "CHAR") == 0) result = TYPE_CHAR;
    else if(strcmp(str, "CHARS") == 0) result = TYPE_CHARS;
    else if(strcmp(str, "DOUBLE") == 0) result = TYPE_DOUBLE;
    else if(strcmp(str, "LONG") == 0) result = TYPE_LONG;
    else if(strcmp(str, "INSERT") == 0) result = INSERT;
    else if(strcmp(str, "ROW") == 0) result = ROW;

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
        
    }

    return 1;

}

Value *lexer(FILE *in, int *size){
    char buffer[128];
    int c;
    int i;
    int capacity = 20;
    int index = 0;
    Value *values = malloc(sizeof(Value) * 20);
    while((c = fgetc(in)) != EOF){
        // Read until a space/newline
        if(isspace(c)){
            buffer[i] = '\0';
            // check if the previous one was empty
            if(i == 0){
                continue;
            }
            i = 0;
            Value new_value = {0};
            new_value.act = get_action_from_str(buffer);
            get_value_from_action(&new_value, buffer);

            if(index >= capacity){
                capacity = capacity * 2;
                void *tmp = realloc(values, capacity * sizeof(Value));
                if(tmp == NULL){
                    perror("Unable to create room for the program");
                    free(values);
                    return NULL;
                }

                values = tmp;
            }            

            values[index++] = new_value;
        }
    }
    *size = index;
    return values;
}

int interpret_lang(Value *lang, int size){
    perror("TODO: interpreter_lang");
    return -1;
}