#ifndef DATABASE
#define DATABASE

#include <stdlib.h>
#include <stdio.h>

#define CHARS_SIZE 32

typedef enum _types{
    INT,
    LONG,
    CHARS,
    CHAR,
    NONE_TYPE
} TYPES;

typedef struct _table{
    char **columns;
    TYPES *types;
    size_t row_size;
    void **rows;
    int num_rows;
    int num_cols;
    char *name;
} Table;

typedef struct _database{
    char *name;
    Table **tables;
    int num_tables;
} Database;

#ifdef INIT_DB_CONSTS
size_t types_sizes[NONE_TYPE] = {
    sizeof(int),
    sizeof(long),
    sizeof(char) * CHARS_SIZE,
    sizeof(char),
};
#else
extern size_t types_sizes[NONE_TYPE];
#endif

Database *create_database(char *handle);

void delete_database(Database *db);

int create_table(Database *db, const char *name, const char **columns, TYPES *types, int num_cols);

void delete_tables(Table **tables, int num_tables);

void delete_table(Table *table);

void print_tables(FILE *out, Database *db);

void print_table(FILE *out, Table *table);

int insert_row(Database*db, const char *table_name, void *row);

void *alloc_row(Database *db, const char *name);

#endif