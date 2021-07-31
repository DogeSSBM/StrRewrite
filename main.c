#include "Includes.h"

typedef struct Node_s{
    char *str;
    uint *ruleOccurances;   // number of times each rule occurs in str
    uint totalOccurances;
    struct Node_s **child;
}Node;

typedef struct NodeList_s{
    Node *node;
    struct NodeList_s *next;
}NodeList;

typedef struct{
    char *find;
    char *replace;
}Rule;

typedef struct{
    uint numRules;
    Rule *rule;
}RuleSet;

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

Rule parseRule(const char *str)
{
    Rule rule = {0};
    char *arrow = strstr(str, "->");
    if(arrow==NULL){
        printf("Error parsing \"%s\".\nRules must be written \"find->replace\n", str);
        exit(-1);
    }else if(strstr(arrow+1, "->")!=NULL){
        printf("Error parsing \"%s\".\nRules must have only one \"->\"\n", str);
        exit(-1);
    }
    const uint flen = arrow-str;
    rule.find = malloc(flen+1);
    rule.replace = malloc(strlen(arrow+2)+1);
    strncpy(rule.find, str, flen);
    strcpy(rule.replace, arrow+2);
    return rule;
}

void printExamples(char *prog)
{
    const char *example[] = {
        "\"AA->B\" \"BA->CA\" \"AC->B\" \"CB->AB\" ACABBACABCB",
        "\"0->1\" \"1->2\" \"2->3\" 000",
        "\"01->10\" \"11->00\" 10101011"
    };
    for(uint i = 0; i < 3; i++)
        printf("\nExample %u:\n%s %s\n", i+1, prog, example[i]);
}

RuleSet parseRuleSet(const uint argc, char **argv)
{
    if(argc < 3){
        printf("Usage:\n%s <rule 1> [rule 2] [rule ...] <input string>\n", argv[0]);
        printf("\trules: find->replace \n");
        printExamples(argv[0]);
        exit(-1);
    }
    RuleSet rs = {
        .numRules = argc-2,
        .rule = malloc(sizeof(Rule)*(argc-2)),
    };

    for(uint i = 0; i < argc-2; i++)
        rs.rule[i] = parseRule(argv[i+1]);
    return rs;
}

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

Node* searchStrList(NodeList *list, const char *str)
{
    while(list != NULL){
        if(strcmp(list->node->str, str) == 0)
            return list->node;
        list = list->next;
    }
    return NULL;
}

void createOccurances(Node *n, const RuleSet rs)
{
    n->ruleOccurances = calloc(rs.numRules, sizeof(uint));
    for(uint i = 0; i < rs.numRules; i++){
        n->ruleOccurances[i] = numMatches(n->str, rs.rule[i].find);
        n->totalOccurances += n->ruleOccurances[i];
    }
}

NodeList* rewrite(Node *n, const RuleSet rs, NodeList *list)
{
    static uint count = 0;
    printf("str: %s, count: %u\n", n->str, count++);
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
    printRuleSet(rs);
    NodeList *list = calloc(1, sizeof(NodeList));
    list->node = calloc(1, sizeof(Node));
    list->node->str = strdup(argv[argc-1]);
    list = rewrite(list->node, rs, list);
    freeList(list);
    freeRuleSet(rs);

    return 0;
}
