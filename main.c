#include "Includes.h"

typedef enum{T_STR, T_VAR}TermType;

typedef struct Term_s{
    TermType type;
    union{
        struct{
            
        };
    }
}Term;

typedef struct Rule_s{

}Rule;

char *readFile
(char *filePath)
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

int main
(int argc, char **argv)
{
    char *text = readFile(argc == 2? argv[1] : "./Test.txt");

    return 0;
}
