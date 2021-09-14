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
    if(*pos != '@' && *pos != '_' && *pos != '$'){
        printf("Dont know how to parse len of string beginning at %s\n", pos);
        err();
        exit(-1);
    }
    uint len = 0;
    pos++;
    while(isalnum(*pos)){
        pos++;
        len++;
    }
    return len;
}

Term *parseTerm
(char *pos)
{
    if(*pos!='$' && *pos!='_'){
        if(*pos!='-' && *pos!='<'){
            printf("unexpected char where -$_< expected\n%s\n", pos);
            exit(-1);
        }
        return NULL;
    }
    Term *term = calloc(1, sizeof(Term));
    term->type = *pos=='$'?T_STR:T_VAR;
    pos++;
    const uint len = getLen(pos);
    char **label = term->type==T_STR ? &(term->text) : &(term->name);
    *label = calloc(len+1, 1);
    memcpy(*label, pos, len);
    return term;
}

Term *dupTerm
(Term *term)
{
    if(term == NULL)
        return NULL;
    Term *dup = calloc(1, sizeof(Term));
    dup->type = term->type;
    char **label = term->type==T_STR ? &(term->text) : &(term->name);
    const uint len = strlen(*label);
    char **duplabel = dup->type==T_STR ? &(dup->text) : &(dup->name);
    *duplabel = calloc(len+1, 1);
    memcpy(*duplabel, *label, len);
}

Rule *reverseRule
(Rule *rule)
{
    Rule *rev = calloc(1, sizeof(Rule));
    Term *l = rule->l;
    Term *r = rule->r;
}

Rule *parseRule
(char *pos)
{
    char *start = pos;
    if(*pos!='$' && *pos!='_'){
        printf("no $/_ ? %s", pos);
        exit(-1);
    }
    Rule *rule = calloc(1, sizeof(Rule));
    Term *term = NULL;
    do{
        term = parseTerm(pos);
        rule->l = appendTerm(rule->l, term);
        pos++;
        pos = strpbrk(pos, "<-$_");
    }while(term!=NULL);

    term = NULL;
    do{
        term = parseTerm(pos);
        rule->r = appendTerm(rule->r, term);
        pos++;
        pos = strpbrk(pos, "<-$_");
    }while(term!=NULL);


}

RuleSet *parseRuleSetN
(char *pos)
{
    if(*pos!='@'){
        printf("no @? %s\n", pos);
        exit(-1);
    }
    pos++;
    const uint len = getLen(pos);
    RuleSet *rs = calloc(1, sizeof(RuleSet));
    rs->name = calloc(len+1, 1);
    memcpy(rs->name, current, len);
    printf("Parsing ruleset \"@%s\"\n", rs->name);
    Rule *rule = NULL;
    do{
        pos = strchr(pos, '\n');
        pos = strpbrk(pos, "$_");
        rule = parseRule(pos)
        rs->rules = appendRule(rs->rules, rule);
    }while(rule!=NULL);
    return rs;
}

void printRuleSet
(RuleSet *rs)
{
    while(rs != NULL){
        printf("RuleSet \"@%s\" -\n", rs->name);
        Rule *rule = rs->rules;
        while(rule != NULL){
            printf("\t");
            Term *term = rule->l;
            while(term!=NULL){
                printf("%c%s", term->type==T_STR?'$':'_', term->type==T_STR?term->text:term->name);
                term = term->next;
            }
            printf("->");
            term = rule->r;
            while(term!=NULL){
                printf("%c%s", term->type==T_STR?'$':'_', term->type==T_STR?term->text:term->name);
                term = term->next;
            }
            rule = rule->next;
        }
        rs = rs->next;
        printf("\n");
    }
}

int main
(int argc, char **argv)
{
    char *source = readFile(argc == 2? argv[1] : "./Test.txt");
    char *current = source;
    RuleSet rs = NULL;
    while((current = strchr(current, '@'))!=NULL){
        rs = appendRuleSet(rs, parseRuleSet(current));
        current++;
    }



    return 0;
}
