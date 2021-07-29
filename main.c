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

RuleSet parseRuleSet(const uint argc, char const *argv[])
{
    if(argc < 3){
        printf("Usage:\n%s <rule 1> [rule 2] [rule ...] <input string>\n", argv[0]);
        printf("\trules: find->replace \n");
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

bool inList(NodeList *list, Node *n)
{
    while(list != NULL){
        if(list->node == n)
            return true;
        list = list->next;
    }
    return false;
}

Node* searchStrList(NodeList *list, const char *str)
{
    while(list != NULL){
        if(list->node->str == str)
            return list->node;
        list = list->next;
    }
    return NULL;
}

NodeList* flatten(NodeList *list, Node *n)
{
    for(uint i = 0; i < n->totalOccurances; i++){
        if(!inList(list, n->child[i])){
            NodeList *head = calloc(1, sizeof(NodeList));
            head->node = n;
            head->next = list;
            list = head;
        }
    }
    return list;
}

void createOccurances(Node *n, const RuleSet rs)
{
    n->ruleOccurances = calloc(rs.numRules, sizeof(uint));
    for(uint i = 0; i < rs.numRules; i++){
        n->ruleOccurances[i] = numMatches(n->str, rs.rule[i].find);
        n->totalOccurances += n->ruleOccurances[i];
    }
}

void createChildren(Node *n, const RuleSet rs, NodeList *list)
{
    n->child = calloc(n->totalOccurances, sizeof(Node*));
    uint current = 0;
    for(uint i = 0; i < rs.numRules; i++){
        for(uint j = 0; j < n->ruleOccurances[i]; j++){
            char *newstr = replaceN(n->str, rs.rule[i].find, rs.rule[i].replace, j);
            if((n->child[current+j] = searchStrList(list, newstr))==NULL){
                n->child[current+j] = calloc(1, sizeof(Node));
                n->str = newstr;
                list = flatten(list, n->child[current+j]);
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
}

void freeList(NodeList *list)
{
    while(list != NULL){
        NodeList *newhead = list->next;
        free(list->node->str);
        free(list->node->ruleOccurances);
        free(list->node->child);
        free(list);
        list = newhead;
    }
}

NodeList* rewrite(char *str, const RuleSet rs, NodeList *list)
{
    Node *n =
    createOccurances(n, rs);
    createChildren(n, rs);
    return
}

int main(int argc, char const *argv[])
{
    RuleSet rs = parseRuleSet(argc, argv);
    printRuleSet(rs);
    rewrite(argv[argc-1], rs, )
    freeRuleSet(rs);

    return 0;
}
