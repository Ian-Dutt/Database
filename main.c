#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "dblang.h"

int main(){

    FILE *in = fopen("sample.txt", "r");
    if(in == NULL){ 
        perror("Unable to open the file"); 
        return 1;
    }

    int size;
    Value *values = lexer(in, &size);

    interpret_lang(values, size);

    fclose(in);
    free_lang(values, size);

    //Database *db = create_database("Test");
    //// FILE *fp = fopen("database.db", "w");
    //// if(fp == NULL){
    ////     delete_database(db);
    ////     return -1;
    //// }

    //int cols = 3;
    //const char *columns[] = {"First", "Second", "Age"};
    //TYPES types[] = {CHARS, CHARS, INT};
    //create_table(db, "Names", columns, types, cols);    

    //Table *table = find_table(db, "Names");
    
    //void *row = alloc_row(table);

    //if(row == NULL){
    //    delete_database(db);
    //    return 1;
    //}

    //strcpy(row, "This is the first column");
    //strcpy((char *)row + 32, "This is the second column");
    //*(int *)((char *)row + 64) = 10;

    //insert_row(table, row);
    //row = alloc_row(table);

    //if(row == NULL){
    //    delete_database(db);
    //    return 1;
    //}

    //strcpy(row, "This is the first column");
    //strcpy((char *)row + 32, "This is the second column");
    //*(int *)((char *)row + 64) = 10;


    //insert_row(table, row);
    
    //int cols2 = 3;
    //const char *columns2[] = {"Full Name", "Pay", "Time"};
    //TYPES types2[] = {CHARS, DOUBLE, INT};
    //create_table(db, "Jobs", columns2, types2, cols2);    
    //table = find_table(db, "Jobs");

    //row = alloc_row(table);

    //if(row == NULL){
    //    delete_database(db);
    //    return 1;
    //}

    //strcpy(row, "Ian A. Dutt");
    //*(double *)((char *)row + CHARS_SIZE) = 80510;
    //*(int *)((char *)row + CHARS_SIZE + sizeof(double)) = 10;


    //insert_row(table, row);

    //print_tables(stdout, db);

    //delete_database(db);
    // fclose(fp);
    return 0;
}