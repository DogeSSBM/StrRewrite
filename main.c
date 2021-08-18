#include "Includes.h"
struct StrList_s;
struct Rule_s;
struct Node_s;
struct SubStr_s;
struct Rule_s;
struct RuleList_s;
struct RuleSet_s;

typedef struct StrList_s{
    char *str;
    struct StrList_s *next;
}StrList;

// typedef struct Rule_s{
//     uint id;
//     StrList *l;
//     StrList *r;
// }Rule;

typedef struct Node_s{
    struct Node_s *parent;
    char *str;
    struct Node_s *child;
    struct Node_s *next;
}Node;

typedef struct SubStr_s{
    char *str;
    uint len;
}SubStr;

typedef struct Rule_s{
    char *l;
    char *find;
    char *r;
    char *replace;
}Rule;

typedef struct RuleList_s{
    char *l;
    char *find;
    char *r;
    char *replace;
    uint ruleNum;
    struct RuleList_s *next;
}RuleList;

typedef struct RuleSet_s{
    uint numRules;
    Rule *rule;
}RuleSet;

void printExamples(char *prog)
{
    const char *example[] = {
        "\"AA->B\" \"BA->CA\" \"AC->B\" \"CB->AB\" ACABBACABCB",
        "\"0->1\" \"1->2\" \"2->3\" 000",
        "\"01->10\" \"11->00\" 10101011",
        "\"101<->010\" \"111->0\" 101010101000010",
        "\"0->1\" \"1->2\" \"2->3\" \"3->4\" \"4->5\" \"5->6\" \"6->7\" \"7->8\" \"8->9\" \"9->0\" 00"
    };
    for(uint i = 0; i < 5; i++)
        printf("\nExample %u:\n%s %s\n", i+1, prog, example[i]);
}

void printRule(const Rule rule)
{
    printf("{\"%s\"->\"%s\"}\n", rule.find, rule.replace);
}

void printRuleSet(const RuleSet rs)
{
    printf("----------------------------------\n");
    for(uint i = 0; i < rs.numRules; i++){
        printf("rule[%u] = ", i);
        printRule(rs.rule[i]);
    }
    printf("----------------------------------\n");
}

void freeRule(const Rule rule)
{
    if(rule.find != NULL)
        free(rule.find);
    if(rule.replace != NULL)
        free(rule.replace);
}

void freeRuleSet(RuleSet rs)
{
    if(rs.numRules == 0)
        return;
    for(uint i = 0; i < rs.numRules; i++)
        freeRule(rs.rule[i]);
    free(rs.rule);
}

uint ruleListLen(RuleList *list)
{
    uint ret = 0;
    while(list != NULL){
        list = list->next;
        ret++;
    }
    return ret;
}

RuleSet convertRuleListToSet(RuleList *list)
{
    const uint len = ruleListLen(list);
    RuleSet ret = {
        .numRules = len,
        .rule = malloc(sizeof(Rule)*(len)),
    };
    RuleList *next;
    for(uint i = 0; i < len; i++){
        ret.rule[i].find = list->find;
        ret.rule[i].replace = list->replace;
        next = list->next;
        free(list);
        list = next;
    }
    if(next != NULL){
        printf("Error! RuleList longer than expected\n");
        exit(-1);
    }
    return ret;
}

// returns true if a and b are both NULL or neither are NULL and are the same string, otherwise false
bool cmpNull(char *a, char *b)
{
    if(a==NULL && b==NULL)
        return true;
    if(a!=NULL && b!=NULL)
        return strcmp(a, b) == 0;
    return false;
}

// returns the first rule in list that matches paramaters, otherwise returns NULL
RuleList* ruleExists(char *find, char *replace, char *l, char *r, RuleList *list)
{
    RuleList *current = list;
    while(current != NULL){
        if(
            cmpNull(l,current->l) &&
            cmpNull(r,current->r) &&
            cmpNull(find, current->find) &&
            cmpNull(replace, current->replace)
        ){
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// checks to see if a rule matching paramaters already exists, if so, frees paramaters
// otherwise, creates a new RuleList with paramaters and appends the rest of the list to it
// returns the list
RuleList* appendRuleList(char *find, char *replace, char *l, char *r, RuleList *list)
{
    if(ruleExists(find, replace, l, r, list)){
        free(find);
        free(replace);
        free(l);
        free(r);
        return list;
    }

    RuleList *next = list;
    list = calloc(1, sizeof(RuleList));
    if(next != NULL)
        list->ruleNum = next->ruleNum+1;
    else
        list->ruleNum = 0;
    list->next = next;
    list->find = find;
    list->replace = replace;
    list->l = l;
    list->r = r;
    printf("rule %u: %s%s%s%s->%s\n", list->ruleNum, list->l, list->l!=NULL?",":"", list->find, list->r!=NULL?",":list->r,list->replace);
    return list;
}

// RuleList* parseSingleArrow(const char *str, RuleList *list)
// {
//     char *arrow = strstr(str, "->");
//     if(arrow == NULL){
//         printf("Error parsing \"%s\".\nRules must be written \"A<->B\" or \"A->B\"\n", str);
//         exit(-1);
//     }
//     if(strstr(arrow+1, "->")!=NULL){
//         printf("Error parsing \"%s\".\nRewrite rules must have only one \"->\"\n", str);
//         exit(-1);
//     }
//     if(strstr(str, "<->")){
//         printf("Error parsing \"%s\".\nRules can only contain\"<->\" or \"->\" not both\n", str);
//         exit(-1);
//     }
//     uint flen = arrow-str;
//     char *find = malloc(flen+1);
//     char *replace = malloc(strlen(arrow+2)+1);
//     strncpy(find, str, flen);
//     strcpy(replace, arrow+2);
//     return list = appendRuleList(find, replace, list);
// }

// returns the number of times find occurs in str
uint numMatches(const char *str, const char *find)
{
    const char *c = str;
    uint ret = 0;
    do{
        if((c = strstr(c, find))!=NULL){
            c++;
            ret++;
        }
    }while(c);
    return ret;
}

// returns the number of times str occurs in sec
uint subNumStr(const SubStr sec, const char *str)
{
    const uint len = strlen(str);
    if(sec.len == 0 || len>sec.len || sec.str == NULL)
        return 0;
    // current position in section
    const char *c = sec.str;
    // if str matches here or later it doesnt count because its length exceeds the section length
    const char *end = sec.str+(sec.len-len);
    uint num = 0;
    while((c = strstr(c, str)) != NULL && c < end){
        num++;
        c++;
    }
    return num;
}

// returns a pointer to the start of the nth occurence of str in sec
const char* getMatchSecN(const SubStr sec, const char *str, const uint n)
{
    const char *c = str;
    const uint len = strlen(str);
    const char *end = sec.str+sec.len;
    for(uint i = 0; i < n+1; i++){
        if((c = strstr(c, sec.str))==NULL || c+len >= end){
            printf("Could not get match number %u/%u of %s in sec %s/%u!",i,n,str,sec.str,sec.len);
            exit(-1);
        }
        c+=i+1<n;
    }
    return c;
}

// returns a pointer to the start of the nth occurence of find in str
const char* getMatchN(const char *str, const char *find, const uint n)
{
    const char *c = str;
    for(uint i = 0; i < n; i++){
        if((c = strstr(c, find))==NULL){
            printf("Could not get match number %u/%u of %s in %s!", i, n, find, str);
            exit(-1);
        }
        c++;
    }
    return strstr(c, find);
}

SubStr parseArrowSection(const char *str)
{
    SubStr arrow;
    const bool isDouble = (arrow.str = strstr(str, "<->")) != NULL;
    if(!isDouble && (arrow.str = strstr(str, "->")) == NULL){
        printf("Error parsing \"%s\".\nRules must contain\"<->\" or \"->\"\n", str);
        exit(-1);
    }else if(isDouble && strstr(arrow.str+1, "->") != arrow.str){
        printf("Error parsing \"%s\".\nRules must contain\"<->\" or \"->\" arrow borked\n", str);
        exit(-1);
    }else if(!isDouble && strstr(arrow.str+1, "->") != NULL){
        printf("Error parsing \"%s\".\nRules must have only one \"->\"\n", str);
        exit(-1);
    }
    arrow.len = isDouble?3:2;
    return arrow;
}

// returns a duplicated, null terminated string, version of sec
char* dupSec(const SubStr sec)
{
    if(sec.len == 0)
        return NULL;
    char *ret = calloc(sec.len+1, sizeof(char));
    memcpy(ret, sec.str, sec.len);
    return ret;
}

SubStr getSubR(const char *str, SubStr mid)
{
    SubStr r;
    r.str = mid.str+mid.len;
    r.len = strlen(str)-(r.str-str);
}

RuleList* parseRule(const char *str, RuleList *list)
{
    SubStr arrow = parseArrowSection(str);
    SubStr lsec = {
        .str = str,
        .len = arrow.str-str;
    };
    SubStr rsec;
    rsec.str = arrow.str+arrow.len;
    rsec.len = strlen(rsec.str);

    RuleList next;
    memset(&next, 0, sizeof(RuleList));
    next.ruleNum = list->ruleNum+1;
    uint commas = subNumStr(lsec, ",");
    if(commas == 2){
        const char *lc = getMatchSecN(lsec, ",", 0);
        const char *rc = getMatchSecN(lsec, ",", 1);
        if(rc>arrow.str){
            printf("Commas can only appear on the left hand side of the arrow\n");
            exit(-1);
        }
       SubStr l;
        l.str = lsec.str;
        l.len = lc-lsec.str
       SubStr m;
        m.str = lc+1;
        m.len = rc-m.str;
       SubStr r;
        r.str = rc+1;
        r.len = lsec.len-(2+l.len+m.len);

        next.l = dupSec(l);
        next.find = dupSec(m);
        next.r = dupSec(r);

        next.replace = calloc(l.len+m.len+r.len+1, sizeof(char));
        strcat(next.replace, next.l);
        memcpy(next.replace+l.len, rsec.str, rsec.len);
        strcat(next.replace, next.r);
    }else if(commas == 0){
        next.find = dupSec(lsec);
        next.replace = dupSec(rsec);
    }else{
        printf("Rules must contain 0 commas or 2 commas\n");
        exit(-1);
    }
    if(arrow.len == 3){
        // todo
        printf("Skipping inverse rule for double arrow\n");
    }
    return appendRuleList(next.find, next.replace, next.l, next.r, list);
}

// RuleList* parseArrow(const char *str, RuleList *list)
// {
//     char *arrow = strstr(str, "<->");
//     if(arrow == NULL)
//         return parseSingleArrow(str, list);
//     if(strstr(arrow+1, "<->")!=NULL){
//         printf("Error parsing \"%s\".\nEquivalence rules must have only one \"<->\"\n", str);
//         exit(-1);
//     }
//     char *singleArrow = strstr(str, "->");
//     if(singleArrow != arrow+1 || strstr(arrow+2, "->")){
//         printf("Error parsing \"%s\".\nRules can only contain\"<->\" or \"->\" not both\n", str);
//         exit(-1);
//     }
//     uint flen = arrow-str;
//     char *find1 = malloc(flen+1);
//     char *replace1 = malloc(strlen(arrow+3)+1);
//     strncpy(find1, str, flen);
//     strcpy(replace1, arrow+3);
//
//     char *find2 = strdup(replace1);
//     char *replace2 = strdup(find1);
//
//     return list = appendRuleList(find2, replace2, list = appendRuleList(find1, replace1, list));
// }

RuleSet parseRuleSet(const uint argc, char **argv)
{
    if(argc < 3){
        printf("Usage:\n%s <rule 1> [rule 2] [rule ...] <input string>\n", argv[0]);
        printf("\trules: find->replace \n");
        printExamples(argv[0]);
        exit(-1);
    }

    RuleList *list = NULL;
    for(uint i = 1; i < argc-1; i++)
        list = parseRule(argv[i], list);

    return convertRuleListToSet(list);
}

// returns the length of a string where replace has been substituted for one instance of find in str
uint newStrLen(const char *str, const char *find, const char *replace)
{
    const int ret = strlen(str)+(strlen(replace)-strlen(find));
    if(ret<0){
        printf("err: replaceLen < 0\n");
        printf("str: \"%s\", find: \"%s\", replace: \"%s\"\n", str, find, replace);
        exit(-1);
    }
    return (uint)ret;
}

// constructs a new string substituting replace for the Nth occourance of find in str
char* replaceN(char *str, const char *find, const char *replace, const uint n)
{
    char *ret = calloc(1, newStrLen(str, find, replace));

    // get nth match
    const char *matchPos = getMatchN(str, find, n);

    // get length up to the nth match
    const uint beforeMatchLen = matchPos-str;
    strncat(ret, str, beforeMatchLen);
    strcat(ret, replace);
    strcat(ret, matchPos+strlen(find));
    return ret;
}

// searches through list and returns the first node with matching str, or NULL if no match found
Node* searchStrList(NodeList *list, const char *str)
{
    while(list != NULL){
        if(strcmp(list->node->str, str) == 0)
            return list->node;
        list = list->next;
    }
    return NULL;
}

void printList(NodeList *list)
{
    uint count = 0;
    while(list != NULL){
        printf("str: %s, count: %u\n", list->node->str, count++);
        list = list->next;
    }
}

// counts the number of positions where the rule could be applied for every rule
void createOccurances(Node *n, const RuleSet rs)
{
    n->ruleOccurances = calloc(rs.numRules, sizeof(uint));
    for(uint i = 0; i < rs.numRules; i++){
        n->ruleOccurances[i] = numMatches(n->str, rs.rule[i].find, rs.rule[i].l, rs.rule[i].r);
        n->totalOccurances += n->ruleOccurances[i];
    }
}

// true if no more rules can be applied
bool isNormalForm(const char *str, const RuleSet rs)
{
    for(uint i = 0; i < rs.numRules; i++){
        if(numMatches(str, rs.rule[i].find)>0)
            return false;
    }
    return true;
}

void printNormalForms(NodeList *list, const RuleSet rs)
{
    uint count = 0;
    while(list != NULL){
        if(isNormalForm(list->node->str, rs))
            printf("Normal form %u: %s\n", ++count, list->node->str);
        list = list->next;
    }
}

NodeList* rewrite(Node *n, const RuleSet rs, NodeList *list)
{
    static uint count = 1;
    // printf("str: %s, count: %u\n", n->str, count++);
    printf("Rewrite: %u\n", count++);
    createOccurances(n, rs);
    n->child = calloc(n->totalOccurances, sizeof(Node*));
    uint current = 0;
    for(uint i = 0; i < rs.numRules; i++){
        for(uint j = 0; j < n->ruleOccurances[i]; j++){
            char *newstr = replaceN(n->str, rs.rule[i].find, rs.rule[i].replace, j);
            if((n->child[current+j] = searchStrList(list, newstr))==NULL){
                n->child[current+j] = calloc(1, sizeof(Node));
                n->child[current+j]->str = newstr;
                NodeList *head = calloc(1, sizeof(NodeList));
                head->next = list;
                head->node = n->child[current+j];
                list = rewrite(n->child[current+j], rs, head);
            }else{
                free(newstr);
            }
        }
        current+=n->ruleOccurances[i];
    }
    if(current != n->totalOccurances){
        printf("Total child nodes generated != n->totalOccurances\n");
        exit(-1);
    }
    return list;
}

void freeList(NodeList *list)
{
    while(list != NULL){
        NodeList *newhead = list->next;
        free(list->node->str);
        free(list->node->ruleOccurances);
        free(list->node->child);
        free(list->node);
        free(list);
        list = newhead;
    }
}

int main(int argc, char **argv)
{
    RuleSet rs = parseRuleSet(argc, argv);
    // printRuleSet(rs);
    // NodeList *list = calloc(1, sizeof(NodeList));
    // list->node = calloc(1, sizeof(Node));
    // list->node->str = strdup(argv[argc-1]);
    // list = rewrite(list->node, rs, list);
    // printNormalForms(list, rs);
    // freeList(list);
    freeRuleSet(rs);

    return 0;
}
