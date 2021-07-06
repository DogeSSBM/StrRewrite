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
    printf("%s\n", root->str);
    for(uint i = 0; i < root->numBranches; i++){
        if(root->branch[i] != NULL)
            show(root->branch[i], level+1);
    }
}

Branch* search(Branch *root, const char *find){
    if(root == NULL || root->str == NULL)
        return NULL;
    if(strcmp(root->str, find)==0)
        return root;
    Branch *ret = NULL;
    for(uint i = 0; i < root->numBranches; i++){
        if((ret = search(root->branch[i], find))!=NULL)
            return ret;
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

void grow(Branch *root, const char *find, const char *replace, const uint level, const uint m)
{
    if(level > 3)
        return;
    indent(2, level);
    root->numBranches = numMatches(root->str, find);
    printf("%d: %s: %d\n", m, root->str, root->numBranches);
    if(root->numBranches == 0){
        root->branch = NULL;
        return;
    }
    root->branch = calloc(root->numBranches, sizeof(Branch*));
    const uint newlen = newStrLen(root->str, find, replace);

    for(uint i = 0; i < root->numBranches; i++){
        root->branch[i] = malloc(sizeof(Branch));
        root->branch[i]->str = calloc(newlen+1, sizeof(char));
        replaceN(root->branch[i]->str, root->str, find, replace, i);
        // indent(2, level);
        // printf("%d: %s", i, root->branch[i]->str);
        grow(root->branch[i], find, replace, level+1, i+1);
    }
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

    grow(tree, argv[2], argv[3], 0, 0);
    //show(tree, 0);

    return 0;
}
