#ifndef __DATABASE__
#define __DATABASE__

#include <stdlib.h>
#include <stdio.h>

#define CHARS_SIZE 32
#define MAX_COLUMNS 100

typedef enum _types{
    INT,
    LONG,
    DOUBLE,
    CHARS,
    CHAR,
    NONE_TYPE
} TYPES;

typedef struct _table{
    char *name;
    void **rows;
    char **columns;
    TYPES *types;
    size_t *indices;
    int num_rows;
    int num_cols;
    size_t row_size;
} Table;

typedef struct _database{
    char *name;
    Table **tables;
    int num_tables;
} Database;

typedef struct result
{
    size_t len;
    size_t cols;
    void **data;
    TYPES *types;
    char **col_names;
} Result;

typedef enum e{
    CREATE,
    DATABASE,
    SHOW,
    SAVE,
    READ,
    FROM,
    GET,
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
    EXPR_END,
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

#ifdef INIT_DB_CONSTS
size_t types_sizes[NONE_TYPE] = {
    sizeof(int),
    sizeof(long),
    sizeof(double),
    sizeof(char) * CHARS_SIZE,
    sizeof(char),
};
#else
extern size_t types_sizes[NONE_TYPE];
#endif

Database *create_database(char *handle);

void delete_database(Database *db);

void delete_result(Result *res);

int create_table(Database *db, const char *name, const char **columns, TYPES *types, int num_cols);

void delete_tables(Table **tables, int num_tables);

void delete_table(Table *table);

void print_tables(FILE *out, Database *db);

void print_table(FILE *out, Table *table);

int insert_row(Table *table, void *row);

void *alloc_row(Table *table);

int get_column_int(Table *table, int row, int col, int *result);
int get_column_long(Table *table, int row, int col, long *result);
int get_column_double(Table *table, int row, int col, double *result);
int get_column_char(Table *table, int row, int col, char *result);
int get_column_chars(Table *table, int row, int col, char **result);

Table *find_table(Database *db, const char *name);

void save_database(Database *db);

Database *read_database(const char *file);

Result *get_columns(Database *db , const char *table, char **columns); // Columns is null terminated

const char *type_to_str(void *data, TYPES type);

Value *lexer(FILE *in, int *size);

void *interpret_lang(Database *db, Value *lang, int size);

void *db_command(Database *db, const char *command);

void free_lang(Value *lang, int size);

TYPES act_to_type(Action act);

char *c_strdup(const char *);
char *c_strndup(const char *, size_t);

#endif
