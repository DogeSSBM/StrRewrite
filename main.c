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

void printTerm
(Term *term)
{
    while(term!=NULL){
        printf("%c%s", term->type==T_STR?'$':'_', term->type==T_STR?term->text:term->name);
        term = term->next;
    }
}

void printRule
(Rule *rule)
{
    printf("\t");
    printTerm(rule->l);
    printf("->");
    printTerm(rule->r);
    printf("\n");
}

void printRuleSet
(RuleSet *rs)
{
    while(rs != NULL){
        printf("RuleSet \"@%s\" -\n", rs->name);
        Rule *rule = rs->rules;
        while(rule != NULL){
            printRule(rule);
            rule = rule->next;
        }
        rs = rs->next;
        printf("\n");
    }
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

bool isDelim
(char c)
{
    return c == '-' || c == '\n' || c == '<' || c == '@' || c == '_' || c == '$';
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
    while(!isDelim(*pos)){
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
    const uint len = getLen(pos);
    pos++;
    char **label = term->type==T_STR ? &(term->text) : &(term->name);
    *label = calloc(len+1, 1);
    memcpy(*label, pos, len);
    // printTerm(term);
    // printf("\n");
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
    return dup;
}

Rule *reverseRule
(Rule *rule)
{
    if(rule == NULL)
        return NULL;
    Rule *rev = calloc(1, sizeof(Rule));
    Term *term = rule->r;
    while(term != NULL){
        rev->l = appendTerm(rev->l, dupTerm(term));
        term = term->next;
    }
    term = rule->l;
    while(term != NULL){
        rev->r = appendTerm(rev->r, dupTerm(term));
        term = term->next;
    }
    return rev;
}

Rule *parseRule
(char *pos)
{
    // char *start = pos;
    if(*pos!='$' && *pos!='_'){
        printf("no $/_ ? %s", pos);
        exit(-1);
    }
    char *arrow = strstr(pos, "->");
    bool doubleArrow = *(arrow-1) == '<';
    arrow-=doubleArrow;
    Rule *rule = calloc(1, sizeof(Rule));
    Term *term = NULL;
    while(pos<arrow){
        term = parseTerm(pos);
        rule->l = appendTerm(rule->l, term);
        pos++;
        pos = strpbrk(pos, "$_");
    }//while(term!=NULL && pos<arrow);
    // printf("\t      %s\n", doubleArrow?"<->":"->");
    // bool doubleArrow = strstr(pos, "<->") == pos;
    term = NULL;
    char *end = strchr(pos, '\n');
    while(pos != NULL && pos < end){
        term = parseTerm(pos);
        rule->r = appendTerm(rule->r, term);
        pos++;
        pos = strpbrk(pos, "<-$_");
    }//while(term!=NULL);

    if(doubleArrow){
        rule = appendRule(rule, reverseRule(rule));
        // printf("appended reverse rule for double arrow\n");
    }

    return rule;
}

char *getEndOfRuleSet
(char *pos)
{
    char *end = strchr(pos, '@');
    if(end==NULL)
        end = strchr(pos, '\0');
    while(end>pos && isspace(*end))
        end--;
    return end;
}

RuleSet *parseRuleSet
(char *pos)
{
    if(*pos!='@'){
        printf("no @? %s\n", pos);
        exit(-1);
    }
    const uint len = getLen(pos);
    RuleSet *rs = calloc(1, sizeof(RuleSet));
    rs->name = calloc(len+1, 1);
    pos++;
    char *end = getEndOfRuleSet(pos);
    memcpy(rs->name, pos, len);
    printf("Parsing ruleset \"@%s\"\n", rs->name);
    Rule *rule = NULL;
    while(pos != NULL && pos<end){
        // printf("\t%2u:\n", count++);
        pos = strchr(pos, '\n');
        pos = strpbrk(pos, "$_");
        if(pos == NULL)
            return rs;
        rule = parseRule(pos);
        rs->rules = appendRule(rs->rules, rule);
    }//while(rule!=NULL);
    return rs;
}

RuleSet *parseAll
(char *pos)
{
    if(*pos!='@'){
        printf("no @? %s\n", pos);
        exit(-1);
    }
    RuleSet *rs = NULL;
    do{
        rs = appendRuleSet(rs, parseRuleSet(pos));
    }while((pos = strchr(pos+1, '@'))!=NULL);
    return rs;
}

int main
(int argc, char **argv)
{
    char *source = readFile(argc == 2? argv[1] : "./Test.txt");
    RuleSet *rs = parseAll(source);
    printRuleSet(rs);
    return 0;
}
