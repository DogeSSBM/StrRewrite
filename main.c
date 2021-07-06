#include "Includes.h"

typedef struct Branch_s{
    char *str;
    uint numBranches;
    struct Branch_s **branch;
}Branch;

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

void prune(Branch *branch)
{
    for(uint i = 0; i < branch->numBranches; i++){
        if(branch->branch[i] != NULL)
            prune(branch->branch[i]);
    }
    if(branch->str!=NULL)
        free(branch->str);
    if(branch->branch!=NULL)
        free(branch->branch);
    free(branch);
}

int main(int argc, char const *argv[])
{
    if(argc != 4){
        printf("Usage:\n\t%s <inputstring> <find> <replace>\n", argv[0]);
        exit(-1);
    }

    Branch *tree = malloc(sizeof(Branch));
    const uint len = strlen(argv[1]);
    tree->str = malloc(len+1);
    memcpy(tree->str, argv[1], len+1);

    grow(tree, tree, argv[2], argv[3], 0, 0);
    // show(tree, 0);

    return 0;
}
