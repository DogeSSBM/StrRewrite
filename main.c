#include "Includes.h"

/*
#v1$abc#v2->#v2$ABC#v1
1abc2->2ABC1

#v1 $abc #v2 -> #v2 $ABC #v1
1 abc 2 -> 2 ABC 1

#v1 $* #v2 -> #v2 $* #v1
3*2 -> 2*3
*/

typedef enum{T_STR, T_VAR}TermType;

typedef struct Term_s{
    TermType type;
    union{
        char *str;
        struct{
            char *label;
            char *value;
        };
    };
    struct Term_s *next;
}Term;

typedef struct Rule_s{
    Term *l;
    Term *r;
    struct Rule_s *next;
}Rule;

char *getArrow
(char *str)
{
    char *arrow = strstr(str, "<->");
    if(arrow != NULL)
        return arrow;
    arrow = strstr(str, "->");
    if(arrow != NULL)
        return arrow;
    printf("Could not parse arrow in \"%s\"\n", str);
    return NULL;
}

Term *getTermsL
(char *str)
{
    Term *terms = NULL;
    char *arrow = getArrow(str);
    while(1){
        str = strpbrk(str, "#$");
        if(str == NULL || str >= arrow)
            return terms;

        Term *append = terms;
        terms = calloc(1, sizeof(Term));
        terms->next = append;
        terms->type = *str == '#' ? T_VAR : T_STR;
        const uint tlen = strcspn(str, " $#");
        terms->str
        while(str[])
        terms->str
        // uint len = 0;
        // do{
        //     str++;
        //     len += strcspn(str, " $#");
        // }while(*(str-1)=='\\');


        Term *append = terms;
        terms = calloc(1, sizeof(Term));
        terms->next = append;
    }
    return terms;
}

uint countTermsL
(char *str)
{
    char *arrow = getArrow(str);
    uint count = 0;
    while(1){
        str = strpbrk(str, "#$");
        if(str == NULL || str >= arrow)
            return count;
        count++;
        str++;
    }
    return count;
}

uint countTermsR
(char *str)
{
    char *arrow = getArrow(str);
    str = *arrow=='<' ? arrow+2 : arrow+1;
    uint count = 0;
    while(1){
        str = strpbrk(str, "#$");
        if(str == NULL)
            return count;
        count++;
        str++;
    }
    return count;
}

Term *termAllocN
(const uint n)
{
    Term *terms = NULL;
    for(uint i = 0; i < n; i++){
        Term *append = terms;
        terms = calloc(1, sizeof(Term));
        terms->next = append;
    }
    return terms;
}

uint termCount
(Term *t)
{
    uint count = 0;
    while(t != NULL){
        count++;
        t = t->next;
    }
    return count;
}

Term *dupeTerms
(Term *t)
{
    Term *ret = termAllocN(termCount(t));
    Term *d = ret;
    while(t != NULL && d != NULL){
        d->type = t->type;
        const uint strl = strlen(t->str);
        const uint valuel = strlen(t->value);
        d->str = malloc(strl+1);
        d->value = malloc(valuel+1);
        memcpy(d->str, t->str, strl);
        memcpy(d->value, t->value, valuel);
        d = d->next;
        t = t->next;
    }
    if(d != NULL || t != NULL){
        printf("Mismatch in number of terms???\n");
        exit(-1);
    }
    return ret;
}

R

Rule *parseRules
(int argc, char **argv)
{
    if(argc < 3){
        printf("Enter at least 1 rule followed by a starting string\n");
        exit(-1);
    }
    Rule *rules = NULL;
    for(uint i = 1; i < argc-1; i++){
        Rule *append = rules;
        rules = calloc(1, sizeof(Rule));
        rules->next = append;
        const uint lnum countTermsL(argv[i]);
        const uint rnum countTermsR(argv[i]);
        rules->l = termAllocN(lnum);
        rules->r = termAllocN(rnum);
        if(*(getArrow(argv[i])) == '<'){
            Rule *reverse = calloc(1, sizeof(Rule));
            reverse->l = dupeTerms(rules->r);
            reverse->r = dupeTerms(rules->l);
            reverse->next = rules;
            rules = reverse;
        }

    }

}

int main
(int argc, char **argv)
{
    Rule *rules = parseRules(argc, argv)
    return 0;
}
