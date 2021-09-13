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

char *getNextArrowBeforeEnd
(char *pos, char *end)
{
    char *sArrowStart = strstr(pos, "->");
    if(sArrowStart >= end || sArrowStart == NULL)
        return NULL;
    char *dArrowStart = strstr(pos, "<->");
    return dArrowStart < sArrowStart? dArrowStart : sArrowStart;
}

char *getSeqEnd
(char *seqStart)
{
    char *seqEnd = strstr(seqStart+1, "@");
    return seqEnd == NULL ? seqStart+strlen(seqStart) : seqEnd;
}

char *getStartOfLine
(char *pos)
{
    while(*pos != '\n')
        pos--;
    return pos+1;
}

char *getStartOfNextLine
(char *pos)
{
    while(*pos != '\n')
        pos++;
    return pos+1;
}

uint getTermLen
(char *pos)
{
    char *end = strpbrk(pos, "$_<\n-");
    if(end == NULL)
        err();
    return end-pos;
}

char *getTermEnd
(char *pos)
{
    pos += *pos=='$' || *pos == '_';
    char *end = strpbrk(pos, "$_<\n-");
    if(end == NULL)
        err();
    return end;
}

Term *parseTerm
(char *pos)
{
    TermType t;
    switch(*pos){
        case '$':
            t = T_STR;
            break;
        case '_':
            t = T_VAR;
            break;
        case '-':
        case '<':
        case '\n':
            return NULL;
        default:
            err();
    }
    pos++;
    uint len = getTermLen(pos);
    Term *term = calloc(1, sizeof(Term));
    term->type = t;
    if(t == T_STR){
        term->text = calloc(len+1, sizeof(char));
        memcpy(term->text, pos, len);
        printf("$%s", term->text);
    }else{
        term->name = calloc(len+1, sizeof(char));
        memcpy(term->name, pos, len);
        printf("_%s", term->name);
    }
    return term;
}

Rule *parseRule
(char *pos, char *end)
{
    char *arrowStart = getNextArrowBeforeEnd(pos, end);
    if(arrowStart == NULL)
        return NULL;
    char *arrowEnd = strchr(arrowStart, '>');
    if(*arrowEnd != '>' || arrowEnd != arrowStart+(*arrowStart == '<' ? 2 : 1))
        err();
    char *rStart = arrowEnd+1;
    char *lStart = getStartOfLine(arrowStart);

    Rule *rule = calloc(1, sizeof(Rule));
    pos = lStart;
    rule->l = parseTerm(pos);
    Term *term = rule->l;
    pos=getTermEnd(pos);
    while((term->next = parseTerm(pos))!=NULL){
        pos=getTermEnd(pos);
        term = term->next;
    }
    printf("%s", *arrowStart == '<' ? "<->" : "->");
    pos = rStart;
    rule->r = parseTerm(pos);
    term = rule->r;
    pos = getTermEnd(pos);
    while((term->next = parseTerm(pos))!=NULL){
        pos=getTermEnd(pos);
        term = term->next;
    }
    printf("\n");
    return rule;
}

Rule *parseRuleSeq
(char *pos, char *end)
{
    Rule *rules = calloc(1, sizeof(Rule));
    Rule *current = rules;
    current = parseRule(pos, end);
    pos = getStartOfNextLine(pos);
    while((current->next = parseRule(pos, end))!=NULL){
        pos = getStartOfNextLine(pos);
        current = current->next;
    }
    return rules;
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
        char *pos = getStrN(source, "@", i)+2;
        char *end = strpbrk(pos, " \t\n");
        uint len = end-pos;

        current->name = calloc(len+1, sizeof(char));
        memcpy(current->name, pos, len);
        current->name[len] = '\0';

        printf("Parsing rule sequence \"@%s\"\n", current->name);
        end = getSeqEnd(pos);
        current->rules = parseRuleSeq(getStartOfNextLine(pos), end);

        // Rule *currentRule = NULL;
        // Rule *nextRule = NULL;
        // end = getSeqEnd(pos);
        // char *rulePos = getStartOfLine(getNextArrowBeforeEnd(pos, end));
        // while((nextRule = parseRule(rulePos))!=NULL){
        //     if(current->rules == NULL){
        //         currentRule = nextRule;
        //         current->rules = currentRule;
        //     }else{
        //         currentRule->next = nextRule;
        //         currentRule = currentRule->next;
        //     }
        //     rulePos = getStartOfNextLine(rulePos);
        // }
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
