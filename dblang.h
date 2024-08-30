#ifndef DB_LANG
#define DB_LANG

#include <stdio.h>

typedef enum e{
    CREATE,
    DATABASE,
    TABLE,
    INSERT,
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_CHARS,
    TYPE_LONG,
    VALUE_INT,
    VALUE_DOUBLE,
    VALUE_CHAR,
    VALUE_CHARS,
    VALUE_LONG,
    ROW,
    IDENTIFIER,
    SEP,
    TYPE_ID,
    NONE_ACT
} Action;

typedef struct {
    Action act;
    double number;
    char *string;
    struct pair {
        Action type;
        char *id;
    } type_id;
    
} Value;

Value *lexer(FILE *in, int *size);

int interpret_lang(Value *lang, int size);

#endif