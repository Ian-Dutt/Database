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
    Table *new_table = calloc(1, sizeof(Table));
    if(new_table == NULL){
        perror("Unable to create table");
        return -1;
    }
    
    // Columns
    new_table->num_cols = cols;
    new_table->columns = calloc(cols, sizeof(char *));
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
    new_table->types = calloc(cols, sizeof(TYPES));
    if(new_table->types == NULL){
        perror("Unable to allocate memory for column types");
        delete_table(new_table);
        return -1;
    }
    size_t row_size = 0;
    for(int i = 0; i < cols; ++i){
        new_table->types[i] = types[i];
        row_size += types_sizes[types[i]];
    }
    // memcpy(new_table->types, types, cols *sizeof(int));
    new_table->row_size = row_size;

    db->tables = realloc(db->tables, sizeof(Table) * (db->num_tables + 1));
    if(new_table->types == NULL){
        perror("Unable to allocate memory for new column types. This corrupted the memory address and the program is exiting");
        delete_database(db);
        exit(1);
    }
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
            switch(table->types[i]){
                case INT:
                    fprintf(out, " %32d |", *(int *)curr);
                    curr += types_sizes[INT];
                    break;
                case LONG:
                    fprintf(out, " %32ld |", *(long *)curr);
                    curr += types_sizes[LONG];
                    break;
                case CHARS:
                    fprintf(out, " %-32s |", curr);
                    curr += types_sizes[CHARS];
                    break;
                case CHAR:
                    fprintf(out, "| %c                                |", *curr);
                    curr += types_sizes[CHAR];
                    break;
                default:
                    fprintf(stderr, "Unable to print type\n");
                    exit(1);
            }
        }
        fputc('\n', out);
    }
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

void *alloc_row(Database *db, const char *name){
    Table *table = find_table(db, name);

    if(table == NULL) return NULL;

    return calloc(1, table->row_size);
}

int insert_row(Database*db, const char *name, void *row){
    Table *table = find_table(db, name);

    if(table == NULL) return -1;

    table->rows = realloc(table->rows, sizeof(void *) * (table->num_rows + 1));
    if(table->rows == NULL){
        fprintf(stderr, "Unable to add new row to the table\n");
        exit(1);
    }
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
    // puts("Here");
    free(table->columns);
    for(int j = 0; j < table->num_rows; ++j){
        free(table->rows[j]);
    }
    // puts("Here");
    free(table->rows);
    // puts("Here");
    free(table->types);
    // puts("Here");
    free(table->name);
    // puts("Here");
    free(table);
    // puts("Here");
}
void delete_tables(Table **tables, int num_tables){
    for(int i = 0; i < num_tables; ++i){
        delete_table(tables[i]);
    }
    free(tables);
}