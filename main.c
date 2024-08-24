#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"

int main(){

    Database *db = create_database("Test");

    const char *columns[] = {"First", "Second"};
    TYPES types[] = {CHARS, CHARS};
    create_table(db, "Names", columns, types, 2);    
    
    void *row = alloc_row(db, "Names");

    if(row == NULL){
        delete_database(db);
        return 1;
    }

    strcpy(row, "This is the first column");
    strcpy((char *)row + 32, "This is the second column");

    insert_row(db, "Names", row);

    print_tables(stdout, db);

    delete_database(db);
    return 0;
}