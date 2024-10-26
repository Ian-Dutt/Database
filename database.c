#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#define INIT_DB_CONSTS
#include "database.h"
#undef INIT_DB_CONSTS
#define USE_CUSTOM_ALLOC
#include "allocator.h"

Database *create_database(char *handle){
    set_log_file();

    Database *db = c_malloc(sizeof(Database));
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
    Table *new_table = c_alloc(1, sizeof(Table));
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
    new_table->columns = c_alloc(MAX_COLUMNS, sizeof(char *));
    if(new_table->columns == NULL){
        perror("Unable to create columns");
        delete_table(new_table);
        return -1;
    }
    for(int i = 0; i < cols; ++i){
        new_table->columns[i] = c_strdup(columns[i]);
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
    new_table->name = c_strdup(name);
    if(new_table->name == NULL){
        perror("Unable to allocate memory for table name");
        delete_table(new_table);
        return -1;
    }

    // Types
    new_table->types = c_alloc(MAX_COLUMNS, sizeof(TYPES));
    if(new_table->types == NULL){
        perror("Unable to allocate memory for column types");
        delete_table(new_table);
        return -1;
    }

    new_table->indices = c_alloc(MAX_COLUMNS, sizeof(size_t));
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

    void *tmp = c_realloc(db->tables, sizeof(Table) * (db->num_tables + 1));
    if(tmp == NULL){
        perror("Unable to allocate memory for new column types. This corrupted the memory address and the program is exiting");
        return -1;
    }
    db->tables = tmp;
    db->tables[db->num_tables] = new_table;
    // //puts("Here create");
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
            fprintf(out, " %-32s |", type_to_str(curr, table->types[j]));
            curr += types_sizes[table->types[j]];
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

    return c_alloc(1, table->row_size);
}

int insert_row(Table *table, void *row){
    if(table == NULL) return -1;

    void *tmp = c_realloc(table->rows, sizeof(void *) * (table->num_rows + 1));
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
    if(db == NULL) return;
    delete_tables(db->tables, db->num_tables);
    c_free(db->name);
    c_free(db);
    alloc_stats();
}

void delete_result(Result *res){
   size_t i;
   if(res == NULL) return;
   c_free(res->types);
   for(i = 0; i < res->cols; ++i){
      c_free(res->col_names[i]);
   }
   c_free(res->col_names);
   for(i = 0; i < res->len; ++i){
      c_free(res->data[i]);
   }
   c_free(res->data);
   c_free(res);
}

void delete_table(Table *table){
    for(int j = 0; j < table->num_cols; ++j){
        c_free(table->columns[j]);
    }
    c_free(table->columns);
    for(int j = 0; j < table->num_rows; ++j){
        c_free(table->rows[j]);
    }
    c_free(table->rows);
    c_free(table->types);
    c_free(table->name);
    c_free(table->indices);
    c_free(table);
}
void delete_tables(Table **tables, int num_tables){
    for(int i = 0; i < num_tables; ++i){
        delete_table(tables[i]);
    }
    c_free(tables);
}

int save_table(FILE *db_file, Table *table){
    //puts(table->name);
    size_t string_len = strlen(table->name);
    // char *name;
    fwrite(&string_len, sizeof(size_t), 1 ,db_file);
    fwrite(table->name, sizeof(char), string_len, db_file);
    // size_t row_size;
    fwrite(&table->row_size, sizeof(size_t), 1, db_file);
    // int num_rows;
    fwrite(&table->num_rows, sizeof(int), 1, db_file);
    // int num_cols;
    fwrite(&table->num_cols, sizeof(int), 1, db_file);
    // void **rows;
    for(int i = 0; i < table->num_rows; ++i){
        fwrite(table->rows[i], table->row_size, 1, db_file);
    }

    // char **columns;
    for(int i = 0; i < table->num_cols; ++i){
        string_len = strlen(table->columns[i]);
        fwrite(&string_len, sizeof(size_t), 1 ,db_file);
        fwrite(table->columns[i], sizeof(char), string_len, db_file);
    }

    fwrite(table->types, sizeof(TYPES), table->num_cols, db_file);

    fwrite(table->indices, sizeof(size_t), table->num_cols, db_file);

    return 1;
}

void save_database(Database *db){
    char tmp[64];
    strcpy(tmp, db->name);
    strcat(tmp, ".db");
    FILE *db_file = fopen(tmp, "wb");
    if(db_file == NULL){
        perror("Unable to open the database file");
        return;
    }
    size_t string_len = strlen(db->name);
    //puts("Here");
    fwrite(&string_len, sizeof(size_t), 1 ,db_file);
    fwrite(db->name, sizeof(char), string_len, db_file);
    fwrite(&db->num_tables, sizeof(int), 1, db_file);
    //puts("Here");

    for(int i = 0; i < db->num_tables; ++i){
        //puts("Here");
        save_table(db_file, db->tables[i]);
    }

    fclose(db_file);
}


Database *read_database(const char *file){
    Database *new_db = NULL;

    FILE *db_file = fopen(file, "rb");
    if(db_file == NULL){
        perror("Unable to open file");
        return NULL;
    }
    new_db = c_malloc(sizeof(Database));
    assert(new_db);

    size_t string_len = 0;
    fread(&string_len, sizeof(size_t), 1, db_file);
    new_db->name = c_alloc(sizeof(char), (string_len + 1));
    assert(new_db->name);
    fread(new_db->name, sizeof(char), string_len, db_file);
    fread(&new_db->num_tables, sizeof(int), 1, db_file);
    new_db->tables = c_malloc(sizeof(Table));
    assert(new_db->tables);

    for(int i = 0; i < new_db->num_tables; ++i){
        new_db->tables[i] = c_malloc(sizeof(Table));
        Table *curr = new_db->tables[i];
        assert(curr);
        fread(&string_len, sizeof(size_t), 1, db_file);
        curr->name = c_alloc(sizeof(char), (string_len + 1));
        assert(curr->name);
        fread(curr->name, sizeof(char), string_len, db_file);

        // rowsize numrows columns
        fread(&curr->row_size, sizeof(size_t), 1, db_file);
        fread(&curr->num_rows, sizeof(int), 1, db_file);
        fread(&curr->num_cols, sizeof(int), 1, db_file);

        curr->columns = c_malloc(curr->num_cols * sizeof(char *));
        assert(curr->columns);
        curr->types = c_malloc(curr->num_cols * sizeof(TYPES));
        assert(curr->types);
        curr->indices = c_malloc(curr->num_cols * sizeof(size_t));
        assert(curr->indices);
        curr->rows = c_malloc(curr->num_rows * sizeof(void *));
        assert(curr->rows);

        for(int i = 0; i < curr->num_rows; ++i){
            curr->rows[i] = c_malloc(curr->row_size);
            assert(curr->rows[i]);
            fread(curr->rows[i], curr->row_size, 1, db_file);
        }

        for(int i = 0; i < curr->num_cols; ++i){
            fread(&string_len, sizeof(size_t), 1, db_file);
            curr->columns[i] = c_alloc(sizeof(char), string_len + 1);
            assert(curr->columns[i]);
            fread(curr->columns[i], sizeof(char), string_len, db_file);
        }

        fread(curr->types, sizeof(TYPES), curr->num_cols, db_file);

        fread(curr->indices, sizeof(size_t), curr->num_cols, db_file);
    }
    fclose(db_file);
    return new_db;
}

Result *get_columns(Database *db, const char *table, char **columns){
    // Get the table
#define NoneRes NULL
    Table *current_table = find_table(db, table);
    if(table == NULL){
        perror("Could Not find Table");
        return NoneRes;
    }

    typedef struct {
        size_t size;
        size_t tab_offset;
        size_t row_offset;
    } Index;

    size_t ret_size = 0;
    for(; columns[ret_size]; ret_size++);

    Index *indices = c_alloc(ret_size, sizeof(Index));
    if(indices == NULL){
        perror("Unable to allocate memory");
        return NoneRes;
    }
    TYPES *ret_types = c_alloc(ret_size, sizeof(Index));
    if(ret_types == NULL){
        perror("Unable to allocate memory");
        return NoneRes;
    }
    size_t ii = 0;
    size_t row_size = 0;
    for(size_t i = 0; i < ret_size; ++i){
        for(int j = 0; j < current_table->num_cols; ++j){
            if(strcmp(columns[i], current_table->columns[j]) == 0){
                row_size += types_sizes[current_table->types[j]];
                ret_types[ii] = current_table->types[j];
                indices[ii++] = (Index){
                    types_sizes[current_table->types[j]],
                    current_table->indices[j],
                    row_size - types_sizes[current_table->types[j]]
                };
                break;
            }else if(j == current_table->num_cols - 1){
                fprintf(stderr, "Could not find column '%s'\n", columns[i]);
                return NoneRes;
            }
        }
    }

    char **ret = c_alloc(current_table->num_rows + 1, sizeof(void *));
    if(ret == NULL){
        perror("Unable to allocate the return variable");
        return NoneRes;
    }
    for(int i = 0; i < current_table->num_rows; ++i){
        ret[i] = c_alloc(1, row_size);
        if(ret[i] == NULL){
            perror("Unable to allocate the return row variable");
            return NoneRes;
        }
        for(ii = 0; ii < ret_size; ++ii){
            memcpy(ret[i] + indices[ii].row_offset, (char *)current_table->rows[i] + indices[ii].tab_offset, indices[ii].size);
        }
    }


    Result *res = c_malloc(sizeof(Result));
 /* {
        current_table->num_rows,
        ret_size,
        (void *)ret,
        ret_types
    }; */

    res->len = current_table->num_rows;
    res->cols = ret_size;
    res->data = (void **)ret;
    res->types = ret_types;
    res->col_names = c_alloc(ret_size, sizeof(char *));
    for(ii = 0; ii < ret_size; ++ii){
      res->col_names[ii] = c_strdup(columns[ii]);
    }

    c_free(indices);

    return res;
#undef NoneRes
}

char print_buf[1000];
const char *type_to_str(void *data, TYPES type){
    switch(type){
        case INT:
            sprintf(print_buf, "%d", *(int *)data);
            break;
        case LONG:
            sprintf(print_buf, "%ld", *(long *)data);
            break;
        case DOUBLE:
            sprintf(print_buf, "%f", *(double *)data);
            break;
        case CHARS:
            sprintf(print_buf, "%s", (char *)data);
            break;
        case CHAR:
            sprintf(print_buf, "%c", *(char *)data);
            break;
        default:
            fprintf(stderr, "Unable to print type\n");
            exit(1);
    }
    return print_buf;
}

char *c_strdup(const char *s) {
    size_t size = strlen(s) + 1;
    char *p = c_malloc(size);
    if (p != NULL) {
        memcpy(p, s, size);
    }
    return p;
}

char *c_strndup(const char *s, size_t n) {
    char *p;
    size_t n1;

    for (n1 = 0; n1 < n && s[n1] != '\0'; n1++)
        continue;
    p = c_malloc(n + 1);
    if (p != NULL) {
        memcpy(p, s, n1);
        p[n1] = '\0';
    }
    return p;
}
