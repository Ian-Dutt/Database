#ifndef DB_LANG
#define DB_LANG

#include <stdio.h>

typedef enum {
    CREATE,
    DATABASE,
    TABLE,
    INSERT,
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_CHARS,
    TYPE_LONG,
    ROW,
    NONE_ACT
} Action;

typedef struct {
    Action act;
    double number;
    
} Value;

Value *lexer(FILE *in, int *size);

int interpret_lang(Value *lang, int size);

#endif