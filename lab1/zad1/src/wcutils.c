#include "wcutils.h"
#include <stdio.h>
#include <stdlib.h>

// Table
WC_Table* createTable(int size){
    WC_Table* table = calloc(1, sizeof(WC_Table));

    table->amount = 0;
    table->capacity = size;
    table->blocks = calloc(table->capacity, sizeof(WC_Block));

    return table;
}

void removeTable(WC_Table* table){
    for(int i=0; i<table->amount; i++){
        free(&table->blocks[i]);
    }

    free(table->blocks);
    free(table);
}

// Operations
int countFile(WC_Table* table, const char* filePath, const char* tempPath){
    char command[1000];

    // Wykonanie komendy wc na podanym pliku i zapisanie wyniku komendy do pliku o sciezce tempPath
    strcpy(command, "wc -w -l -m < ");
    strcpy(command, filePath);
    strcpy(command, " >> ");
    strcpy(command, tempPath);
    
    system(command);

    FILE* file = fopen(tempPath, "r");

    if(!file){
        return -1;
    }

    int index = table->amount;
    table->amount++;

    if(table->amount >= table->capacity){
        table->capacity *= 2;
        table->blocks = realloc(table->blocks, table->capacity * sizeof(WC_Block));
    }

    WC_Block* block = &table->blocks[index];
    fscanf(file, "%d %d %d", block->lines, block->words, block->chars);

    return index;
}

char* getBlockContent(const WC_Table* table, int index){
    WC_Block* block = &table->blocks[index];
    char* buffer;

    sprintf(buffer, '#%d |lines: %d |words: %d |chars: %d', index, block->lines, block->words, block->chars);

    return buffer;
}

int removeBlock(WC_Table* table, int index){
    if(index > -1 && index < table->amount){
        free(&table->blocks[index]);

        for(int i=index+1; i<table->amount; i++){
            table->blocks[i-1] = table->blocks[i];
        }

        table->amount--;

        return RETURN_CODE_SUCCESS;
    }

    return RETURN_CODE_OUT_OF_RANGE;
}

void printBlock(const WC_Table* table, int index){
    if(index > -1 && index < table->amount){
        WC_Block* block = &table->blocks[index];

        printf('#%d |lines: %d |words: %d |chars: %d|\n', 
            index, block->lines, block->words, block->chars);
    }
}

void printTable(const WC_Table* table){
    printf('---Table---\n');
    for(int i=0; i<table->amount; i++){
        printBlock(table, i);
    }
}