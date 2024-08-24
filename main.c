#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"

int main(){

    Database *db = create_database("Test");
    // FILE *fp = fopen("database.db", "w");
    // if(fp == NULL){
    //     delete_database(db);
    //     return -1;
    // }

    int cols = 3;
    const char *columns[] = {"First", "Second", "Age"};
    TYPES types[] = {CHARS, CHARS, INT};
    create_table(db, "Names", columns, types, cols);    

    Table *table = find_table(db, "Names");
    
    void *row = alloc_row(table);

    if(row == NULL){
        delete_database(db);
        return 1;
    }

    strcpy(row, "This is the first column");
    strcpy((char *)row + 32, "This is the second column");
    *(int *)((char *)row + 64) = 10;

    insert_row(table, row);
    int age;
    if(get_column_int(table, 0, "Age", &age) == 1){
        printf("Age of the person in row 1 is %d\n", age);
    }

    print_tables(stdout, db);

    delete_database(db);
    // fclose(fp);
    return 0;
}