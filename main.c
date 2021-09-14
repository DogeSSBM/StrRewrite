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

Term *appendTerm
(Term *list, Term *term)
{
    if(list == NULL)
        return term;
    Term *current = list;
    while(current->next != NULL)
        current = current->next;
    current->next = term;
    return list;
}

Term *currentTerm
(Term *list)
{
    if(list == NULL){
        printf("Cannot get current Term from NULL list\n");
        exit(-1);
    }
    while(list->next != NULL)
        list = list->next;
    return list;
}

Rule *appendRule
(Rule *list, Rule *rule)
{
    if(list == NULL)
        return rule;
    Rule *current = list;
    while(current->next != NULL)
        current = current->next;
    current->next = rule;
    return list;
}

Rule *currentRule
(Rule *list)
{
    if(list == NULL){
        printf("Cannot get current Rule from NULL list\n");
        exit(-1);
    }
    while(list->next != NULL)
        list = list->next;
    return list;
}

RuleSet *appendRuleSet
(RuleSet *list, RuleSet *ruleset)
{
    if(list == NULL)
        return ruleset;
    RuleSet *current = list;
    while(current->next != NULL)
        current = current->next;
    current->next = ruleset;
    return list;
}

RuleSet *currentRuleSet
(RuleSet *list)
{
    if(list == NULL){
        printf("Cannot get current RuleSet from NULL list\n");
        exit(-1);
    }
    while(list->next != NULL)
        list = list->next;
    return list;
}

uint getLen
(char *pos)
{
    uint len = 0;
    while(isalnum(*pos)){
        pos++;
        len++;
    }
    return len;
}

int main
(int argc, char **argv)
{
    char *source = readFile(argc == 2? argv[1] : "./Test.txt");
    char *current = source;
    RuleSet *ruleset = NULL;

    while(*current != '\0'){
        switch(*current){
            case '@':
                current++;
                const uint len = getLen(current);
                RuleSet *rs = calloc(1, sizeof(RuleSet));
                rs->name = calloc(len+1, 1);
                memcpy(rs->name, current, len);
                ruleset = appendRuleSet(ruleset, rs);
                current = strpbrk(current, "$_");
                break;
            case '$':
                current++;
                if()
                const uint len = getLen(current);
                if()
        }
    }
    return 0;
}
