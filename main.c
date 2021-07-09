#include "Includes.h"

typedef struct Branch_s{
    char *str;
    uint numBranches;
    struct Branch_s **branch;
}Branch;

typedef struct{
    uint num;
    char **find;
    char **replace;
}RuleSet;

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
        c = strstr(c, find);
        c++;
    }
    return strstr(c, find);
}

// prints level indents of ilen spaces
void indent(const uint ilen, const uint level)
{
    for(uint i = 0; i < level; i++){
        for(uint j = 0; j < ilen; j++){
            printf(" ");
        }
    }
}

void show(Branch *root, const uint level)
{
    indent(4, level);
    printf("%u %s:\n", level, root->str);
    for(uint i = 0; i < root->numBranches; i++){
        indent(4, level);
        printf("%u:\n", i);
        if(root->branch[i] != NULL)
            show(root->branch[i], level+1);
    }
}

Branch* search(Branch *root, const char *find, const uint level){
    if(root == NULL || root->str == NULL)
        return NULL;
    if(root->str != find && strcmp(root->str, find)==0)
        return root;
    Branch *ret = NULL;
    for(uint i = 0; i < root->numBranches; i++){
        if((ret = search(root->branch[i], find, level+1))!=NULL){
            indent(4, level);
            printf("Found previous match for %s:\n", find);
            indent(4, level);
            printf("%u:[%u]\n",level+1, i);
            return ret;
        }
    }
    return ret;
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

// constructs new string replacing the nth match of find in src with replace
void replaceN(char *dest, const char *src, const char *find, const char *replace, const uint n)
{
    // get nth match
    const char *srcMatch = getMatchN(src, find, n);

    // get length up to the nth match
    const uint beforeMatchLen = srcMatch-src;
    strncat(dest, src, beforeMatchLen);
    // printf("%u: %s\n",n,dest);
    strcat(dest, replace);
    strcat(dest, srcMatch+strlen(find));
}

Branch* merdge(Branch *branch, Branch *target)
{
    if(strcmp(branch->str, target->str) == 0 && branch != target)
        return branch;
    Branch *ret = NULL;
    for(uint i = 0; i < branch->numBranches; i++){
        if((ret = merdge(branch->branch[i], target))!=NULL)
            return ret;
    }
    return NULL;
}

void grow(Branch *root, Branch *branch, const char *find, const char *replace, const uint level, const uint m)
{
    indent(4, level);
    branch->numBranches = numMatches(branch->str, find);
    printf("%d: %s: %d\n", m, branch->str, branch->numBranches);
    if(branch->numBranches == 0){
        branch->branch = NULL;
        return;
    }
    branch->branch = calloc(branch->numBranches, sizeof(Branch*));
    const uint newlen = newStrLen(branch->str, find, replace);

    for(uint i = 0; i < branch->numBranches; i++){
        branch->branch[i] = malloc(sizeof(Branch));
        branch->branch[i]->str = calloc(newlen+1, sizeof(char));
        replaceN(branch->branch[i]->str, branch->str, find, replace, i);
        Branch *f = search(root, branch->branch[i]->str, 0);
        if(f != NULL){
            printf("found\n");
            free(branch->branch[i]->str);
            free(branch->branch[i]);
            branch->branch[i] = f;
            return;
        }else{
            grow(root, branch->branch[i], find, replace, level+1, i+1);
        }
        // indent(2, level);
        // printf("%d: %s", i, branch->branch[i]->str);
    }
}

void destroy(Branch *branch)
{
    for(uint i = 0; i < branch->numBranches; i++){
        if(branch->branch[i] != NULL)
            destroy(branch->branch[i]);
    }
    if(branch->str!=NULL)
        free(branch->str);
    if(branch->branch!=NULL)
        free(branch->branch);
    free(branch);
}

void prune(Branch *branch)
{
    for(uint i = 0; i < branch->numBranches; i++){
        if(branch->branch[i] != NULL)
            destroy(branch->branch[i]);
        if(branch->branch!=NULL)
            free(branch->branch);
    }
    branch->numBranches = 0;
}

RuleSet parseRules(const uint argc, char const *argv[])
{
    if(argc < 3){
        printf("Usage:\n%s <rule 1> [rule 2] [rule ...] <input string>\n", argv[0]);
        printf("\trules: find->replace \n");
        exit(-1);
    }
    RuleSet rules = {
        .num = argc-2,
        .find = malloc(sizeof(char*)*(argc-2)),
        .replace = malloc(sizeof(char*)*(argc-2))
    };
    for(uint i = 1; i < argc-1; i++){
        const char *arrow = strstr(argv[i], "->");
        if(arrow==NULL){
            printf("Error. argv[%u]: \"%s\"\n Rules must be written as:\nfind->replace\n", i, argv[i]);
            exit(-1);
        }else if(strstr(arrow+1, "->")!=NULL){
            printf("Error. argv[%u]: \"%s\"\n Rules must have only one ->\n", i, argv[i]);
            exit(-1);
        }
        const uint len = arrow-argv[i];
        rules.find[i-1] = malloc(len+1);
        rules.replace[i-1] = malloc(strlen(arrow+2)+1);
        strncpy(rules.find[i-1], argv[1], len);
        strcpy(rules.replace[i-1], arrow+2);
    }
    return rules;
}

Branch* create(const char *str)
{
    Branch *tree = malloc(sizeof(Branch));
    const uint len = strlen(str);
    tree->str = malloc(len+1);
    memcpy(tree->str, str, len);
    return tree;
}

int main(int argc, char const *argv[])
{
    RuleSet rules = parseRules(argc, argv);
    Branch *tree = create(argv[argc-1]);
    for(uint i = 0; i < rules.num; i++){
        printf("[%u]: %s -> %s\n", i, rules.find[i], rules.replace[i]);
        grow(tree, tree, rules.find[i], rules.replace[i], 0, 0);
        prune(tree);
    }

    destroy(tree);

    return 0;
}
