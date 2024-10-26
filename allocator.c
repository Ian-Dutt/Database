#include <stdio.h>
#include <stdlib.h>
#define USE_CUSTOM_ALLOC
#include "allocator.h"

typedef struct linked_list {
    struct linked_list *next;
    struct linked_list *prev;
    Alloced value;
} LinkedList;

#define MAX_BLOCKS 10000
static LinkedList *head = NULL;
static LinkedList *tail = NULL;
static int blocks_size = 0;
static FILE *log_out = NULL;
AllocStats stats = {0};

#define log_file log_out

void set_log_file(){
    if(log_out != NULL){
        return;
    }
    log_out = fopen(".alloc_log", "w");
    if(log_out == NULL){
        fprintf(stderr, "Unable to open alloc log file\n");
        exit(1);
    }
}

void close_log_file(){
    if(log_out != NULL){
        fclose(log_out);
        log_out = NULL;
    }
}

LinkedList *search_blocks(void *memory){
    LinkedList *tmp;
    for(tmp = head; tmp; tmp = tmp->next){
        if(memory == tmp->value.memory){
            return tmp;
        }
    }
    return NULL;
}

LinkedList *new_node(){
    if(head == NULL || tail == NULL){
        head = calloc(1, sizeof(LinkedList));
        if(head == NULL){
            fprintf(stderr, "Unable to create linked list\n");
            exit(1);
        }
        tail = head;
    }else{
        tail->next = calloc(1, sizeof(LinkedList));
        if(tail->next == NULL){
            fprintf(stderr, "Unable to create linked list\n");
            exit(1);
        }
        tail->next->prev = tail;
        tail = tail->next;
    }
    blocks_size++;
    return tail;
}

void *_c_alloc(size_t len, size_t size, int line, const char *function){
    if(log_file == NULL){
        set_log_file();
    }
    Alloced *new_block = &(new_node()->value);
    void *memory = calloc(len, size);

    if(memory == NULL){
        fprintf(stderr, "Unable to allocate chunck of size %u in %s at line %d\n", len * size, function, line);
        close_log_file();
        exit(1);
    }
    new_block->memory = memory;
    new_block->size = len * size;
    new_block->line = line;
    new_block->function = function;
    stats.allocated += new_block->size;
    fprintf(log_file, "%s:%d allocated block [%p] with a size of %u\n", function, line, memory, new_block->size);
    return memory;
}

void *_c_realloc(void *memory, size_t len, size_t size, int line, const char *function){
    if(memory == NULL){
        return _c_alloc(len, size, line, function);
    }

    Alloced *new_block = &(search_blocks(memory)->value);
    if(memory == NULL){
        fprintf(stderr, "Block [%p] was not allocated with c_alloc/c_realloc\n", memory);
        exit(1);
    }
    size_t prev_loc = (size_t) memory;
    void *rememory = realloc(memory, len * size);
    if(rememory == NULL){
        c_free(memory);
        fprintf(stderr, "Unable to reallocate chunck of size %u in %s at line %d\n", len * size, function, line);
        close_log_file();
        exit(1);
    }
    fprintf(log_file, "%s:%d reallocated block [0x%x] with a size of %u to [%p] with a size of %u\n", function, line, prev_loc, new_block->size, rememory, len * size);
    new_block->memory = rememory;
    new_block->size = len * size;
    new_block->line = line;
    new_block->function = function;
    stats.allocated += new_block->size;
    return rememory;
}

void _c_free(void *memory, int line, const char *function){
    LinkedList *index = search_blocks(memory);
    if(index == NULL){
        fprintf(log_file, "%s:%d trying to free a block [%p] that was not allocated by c_alloc/c_realloc\n", function, line, memory);
        free(memory);
        return;
    }
    fprintf(log_file, "%s:%d freeing block [%p] with size of %u\n", function, line, memory, index->value.size);
    if(index == head){
        head = index->next;
    }
    if(index == tail){
        tail = index->prev;
    }
    if(index->prev){
        index->prev->next = index->next;
    }
    if(index->next){
        index->next->prev = index->prev;
    }
    stats.freed += index->value.size;
    blocks_size--;
    free(memory);
    free(index);
}

const AllocStats *alloc_stats(){
    stats.inuse = stats.allocated - stats.freed;
    fprintf(log_file,
        "Custom Allocator Information\n"
        "  Memory Allocated: %u\n"
        "  Memory Free'd: %u\n"
        "  Memory In Use: %u\n"
        "  Blocks In Use:\n",
        stats.allocated, stats.freed, stats.inuse);
    LinkedList *tmp;
    for(tmp = head; tmp; tmp = tmp->next){
        fprintf(log_file, "    Block [%p] of size %u allocated by %s:%d\n", tmp->value.memory, tmp->value.size, tmp->value.function, tmp->value.line);
    }
    fprintf(log_file, "End Custom Alllocator Information\n");
    return (const AllocStats *const)(&stats);
}
