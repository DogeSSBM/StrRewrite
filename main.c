#include "Includes.h"
#define err()   printf("Err at line %i\n", __LINE__)
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

typedef struct RuleSet_s{
    char *name;
    Rule *rules;
    struct RuleSet_s *next;
}RuleSet;

typedef struct Sym_s{
    SymType type;
    char *start;
    char *stop;
    union{
        Term *term;
        Rule *rule;
        RuleSet *ruleset;
    }
    struct Sym_s *next;
}Sym;

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

Sym *appendSym
(Sym)

int main
(int argc, char **argv)
{
    char *source = readFile(argc == 2? argv[1] : "./Test.txt");

    return 0;
}
