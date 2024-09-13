#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INIT_DB_CONSTS
#include "database.h"
#undef INIT_DB_CONSTS

Database *create_database(char *handle){
    /* Database *db = link_stored_database(handle); */

    Database *db = malloc(sizeof(Database));
    if(db == NULL){
        perror("Unable to create database");
        exit(1);
    }
    db->num_tables = 0;
    db->tables = NULL;
    db->name = handle;

    return db;
}

/*
typedef struct _table{
    size_t row_size;
} Table;*/
int create_table(Database *db, const char *name, const char **columns, TYPES *types, int cols){
    if(find_table(db, name) != NULL){
        perror("Tables with the same name is already created");
        return -1;
    }
    Table *new_table = calloc(1, sizeof(Table));
    if(new_table == NULL){
        perror("Unable to create table");
        return -1;
    }
    
    // Columns
    if(cols >= MAX_COLUMNS){
        perror("Tried to allocate more columns than supported");
        return -1;
    }
    new_table->num_cols = cols;
    new_table->columns = calloc(MAX_COLUMNS, sizeof(char *));
    if(new_table->columns == NULL){
        perror("Unable to create columns");
        delete_table(new_table);
        return -1;
    }
    for(int i = 0; i < cols; ++i){
        new_table->columns[i] = strdup(columns[i]);
        if(new_table->columns[i] == NULL){
            perror("Unable to name the columns of the table");
            delete_table(new_table);
            return -1;
        }
    }

    // Rows
    new_table->rows = NULL;
    new_table->num_rows = 0;

    // Name
    new_table->name = strdup(name);
    if(new_table->name == NULL){
        perror("Unable to allocate memory for table name");
        delete_table(new_table);
        return -1;
    }

    // Types
    new_table->types = calloc(MAX_COLUMNS, sizeof(TYPES));
    if(new_table->types == NULL){
        perror("Unable to allocate memory for column types");
        delete_table(new_table);
        return -1;
    }

    new_table->indices = calloc(MAX_COLUMNS, sizeof(size_t));
    if(new_table->indices == NULL){
        perror("Unable to allocate memory for column indices");
        delete_table(new_table);
        return -1;
    }

    size_t row_size = 0;
    for(int i = 0; i < cols; ++i){
        new_table->types[i] = types[i];
        new_table->indices[i] = row_size;
        row_size += types_sizes[types[i]];
    }
    // memcpy(new_table->types, types, cols *sizeof(int));
    new_table->row_size = row_size;

    void *tmp = realloc(db->tables, sizeof(Table) * (db->num_tables + 1));
    if(tmp == NULL){
        perror("Unable to allocate memory for new column types. This corrupted the memory address and the program is exiting");
        return -1;
    }
    db->tables = tmp;
    db->tables[db->num_tables] = new_table;
    // puts("Here create");
    db->num_tables++;

    return 1;
}

void print_tables(FILE *out, Database *db){
    for(int i = 0; i < db->num_tables; ++i){
        print_table(out, db->tables[i]);
    }
}

void print_table(FILE *out, Table *table){
    fprintf(out, "Table: %s\n|", table->name);
    for(int i = 0; i < table->num_cols; ++i){
        fprintf(out, " %-32s |", table->columns[i]);
    }
    fputc('\n', out);
    for(int i = 0; i < table->num_rows; ++i){
        char *curr = table->rows[i];
        fputc('|', out);
        for(int j = 0; j < table->num_cols; ++j){
            switch(table->types[j]){
                case INT:
                    fprintf(out, " %-32d |", *(int *)curr);
                    curr += types_sizes[INT];
                    break;
                case LONG:
                    fprintf(out, " %-32ld |", *(long *)curr);
                    curr += types_sizes[LONG];
                    break;
                case DOUBLE:
                    fprintf(out, " %-32.4f |", *(double *)curr);
                    curr += types_sizes[LONG];
                    break;

                case CHARS:
                    fprintf(out, " %-32s |", curr);
                    curr += types_sizes[CHARS];
                    break;
                case CHAR:
                    fprintf(out, " %c                                |", *curr);
                    curr += types_sizes[CHAR];
                    break;
                default:
                    fprintf(stderr, "Unable to print type\n");
                    exit(1);
            }
        }
        fputc('\n', out);
    }
    fputc('\n', out);
}


int get_column_int(Table *table, int row, int col, int *result){
    if(row < 0 || row >= table->num_rows) return -1;
    if(col < 0 || col >= table->num_cols) return -1;

    if(table->types[col] != INT) return -1;        

    *result = *(int *)((char *)table->rows[row] + table->indices[col]); 

    return 1;
}

int get_column_long(Table *table, int row, int col, long *result){
    if(row < 0 || row >= table->num_rows) return -1;
    if(col < 0 || col >= table->num_cols) return -1;

    if(table->types[col] != LONG) return -1;        

    *result = *(long *)((char *)table->rows[row] + table->indices[col]); 

    return 1;
}

int get_column_double(Table *table, int row, int col, double *result){
    if(row < 0 || row >= table->num_rows) return -1;
    if(col < 0 || col >= table->num_cols) return -1;

    if(table->types[col] != LONG) return -1;        

    *result = *(double *)((char *)table->rows[row] + table->indices[col]); 

    return 1;
}

int get_column_char(Table *table, int row, int col, char *result){
    if(row < 0 || row >= table->num_rows) return -1;
    if(col < 0 || col >= table->num_cols) return -1;

    if(table->types[col] != CHAR) return -1;        

    *result = *(char *)((char *)table->rows[row] + table->indices[col]); 

    return 1;
}

int get_column_chars(Table *table, int row, int col, char **result){
    if(row < 0 || row >= table->num_rows) return -1;
    if(col < 0 || col >= table->num_cols) return -1;

    if(table->types[col] != CHARS) return -1;        

    *result = strndup(((char *)table->rows[row] + table->indices[col]), CHARS_SIZE);

    return 1;
}

Table *find_table(Database *db, const char *name){
    Table *table = NULL;
    for(int i = 0; i < db->num_tables; ++i){
        if(strcmp(db->tables[i]->name, name) == 0){
            table = db->tables[i];
            break;
        }
    }
    return table;
}

void *alloc_row(Table *table){
    if(table == NULL) return NULL;

    return calloc(1, table->row_size);
}

int insert_row(Table *table, void *row){
    if(table == NULL) return -1;

    void *tmp = realloc(table->rows, sizeof(void *) * (table->num_rows + 1));
    if(tmp == NULL){
        fprintf(stderr, "Unable to add new row to the table\n");
        return -1;
    }
    table->rows = tmp; 
    table->rows[table->num_rows] = row;
    table->num_rows++;

    return 1;
}

void delete_database(Database *db){
    delete_tables(db->tables, db->num_tables);
    free(db);
}

void delete_table(Table *table){
    for(int j = 0; j < table->num_cols; ++j){
        free(table->columns[j]);
    }
    free(table->columns);
    for(int j = 0; j < table->num_rows; ++j){
        free(table->rows[j]);
    }
    free(table->rows);
    free(table->types);
    free(table->name);
    free(table->indices);
    free(table);
}
void delete_tables(Table **tables, int num_tables){
    for(int i = 0; i < num_tables; ++i){
        delete_table(tables[i]);
    }
    free(tables);
}