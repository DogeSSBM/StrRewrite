#include "Includes.h"
#define err()   {printf("Err at line %i\n", __LINE__)}
typedef enum{T_STR, T_VAR}TermType;

typedef struct Term_s{
    TermType type;
    char *name; // NULL if T_STR.
    char *text; // text of string if T_STR. text of T_VAR if bound. NULL if free.
    struct Term_s *next;
}Term;

typedef struct Rule_s{
    Term *l;
    Term *r;
    struct Rule_s *next;
}Rule;

typedef struct RuleSetSeq_s{
    char *name;
    Rule *rules;
    struct RuleSetSeq_s *next;
}RuleSetSeq;

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

uint strCount
(char *source, char *str)
{
    uint count = 0;
    while((source = strstr(source, str)) != NULL){
        count++;
        source++;
    }
    // printf("count: %u\n", count);
    return count;
}

char *getStrN
(char *source, char *str, const uint n)
{
    uint count = 0;
    while((source = strstr(source, str)) != NULL && count < n){
        count++;
        source++;
    }
    return source == NULL ? NULL : source-1;
}

RuleSetSeq *parseSeq
(char *source)
{
    const uint count = strCount(source, "@");
    if(count == 0)
        return NULL;
    RuleSetSeq *seq = calloc(1, sizeof(RuleSetSeq));
    RuleSetSeq *current = seq;
    for(uint i = 0; i < count; i++){
        printf("%u/%u\n", i, count);
        char *pos = getStrN(source, "@", i)+2;
        printf("%c\n", *pos);
        char *end = strpbrk(pos, " \t\n");

        uint len = end-pos;
        printf("len: %u\n", len);

        current->name = calloc(len+1, sizeof(char));
        memcpy(current->name, pos, len);
        current->name[len] = '\0';
        printf("parsing rule seq %u \"%s\"\n", i, current->name);
        if(i+1<count)
            current->next = calloc(1, sizeof(RuleSetSeq));
        current = current->next;
    }
    printf("done\n");
    return seq;
}

int main
(int argc, char **argv)
{
    char *text = readFile(argc == 2? argv[1] : "./Test.txt");
    parseSeq(text);
    return 0;
}
