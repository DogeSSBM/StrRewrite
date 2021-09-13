#include "Includes.h"

#define err()   {printf("Err at line %i\n", __LINE__)}

typedef enum{T_STR, T_VAR, T_ARR, T_DARR, T_NULL}TokenType;

typedef struct Token_s{
    TokenType type;
    char *name;
    char *value;
    struct Token_s *next;
}Token;

typedef struct Rule_s{
    Token *l;
    Token *r;
    struct Rule_s *next;
}Rule;

char *nextToken
(Token *list, char *pos)
{
    if(*pos == '\0' || pos == NULL)
        return NULL;
    TokenType t = T_NULL;
    switch(*pos){
        case '$':
            t = T_STR;
            break;
        case '_':
            t = T_VAR;
            break;
        case '-':
            if(pos[1] != '>'){
                err();
            }
    }
    Token *term = calloc(1, sizeof(Token));

}

int main
(int argc, char **argv)
{
    err();
    return 0;
}
