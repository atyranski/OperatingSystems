#ifndef WC_UTILS_H
#define WC_UTILS_H

#define RETURN_CODE_SUCCESS 0;
#define RETURN_CODE_OUT_OF_RANGE 1;
#define RETURN_CODE_FILE_NOT_FOUND 2;
#define RETURN_CODE_FILE_DONT_EXIST -1;

typedef struct WC_Block{
    int lines;
    int words;
    int chars;
} WC_Block;

typedef struct WC_Table{
    WC_Block** blocks;
    int amount;
    int capacity;
} WC_Table;

// Table
WC_Table* createTable(int size);

int removeTable(WC_Table* table);

// Operations
int countFile(WC_Table* table, const char* filePath, const char* tempPath);

int removeBlock(WC_Table* table, int index);

#endif