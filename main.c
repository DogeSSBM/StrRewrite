#include "Includes.h"

typedef enum{K_NONE = 0, K_CB_O, K_CB_C, K_SB_O, K_SB_C, K_STR, K_VAR, K_SARR, K_DARR}TokenType;

typedef struct Token_s{
    struct Token_s *prev;
    TokenType type;
    char *start;
    char *end;
    uint len;
    struct Token_s *next;
}Token;

char* readFile(char *filePath)
{
    char *buffer = 0;
    long length;
    File *f = NULL;
    if((f = fopen(filePath, "rb")) == NULL){
        printf("Unable to open file \"%s\"\n", filePath);
        exit(-1);
    }

    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);
    buffer = calloc(length+1, sizeof(char));
    fread(buffer, 1, length, f);
    buffer[length] = '\0';
    fclose(f);
    return buffer;
}


Token *nextToken
(Token *current)
{
    current->next = calloc(1, sizeof(Token));
    current->next->prev = current;
    return current->next;
}

Token *tokenize
(char *text)
{
    Token *list = calloc(1, sizeof(Token));
    char *pos = text;
    while(1){
        char c = *pos;
        switch(c){
            case ' ':
            case '\t':
            case '\n':
                pos++;
                break;
            case '{':
                list->type = K_CB_O;
                list->start = pos;
                list->end = pos+1;
                list->len = 1;
        }
    }
}

int main
(int argc, char **argv)
{
    char *text = readFile(argc == 2? argv[1] : "./Test.txt");

    return 0;
}
