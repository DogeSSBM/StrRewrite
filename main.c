#include "Includes.h"


/*
BAAABBABAAAAB
AAA -> B

   1      23
I:BAAABBABAAAAB
|             12
+---1: BBBBABAAAAB
|   +---1: BBBBABBAB   // UNIQUE A
|   +---2: BBBBABABB   // UNIQUE B
|
|        1
+---2: BAAABBABBAB
|   +---1: BBBBABBAB   // DUPE A
|
|        1
+---3: BAAABBABABB
    +---1: BBBBABABB   // DUPE B
*/

typedef struct Branch_s{
    char *str;
    uint numBranches;
    struct Branch_s **branch;
}Branch;

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

void grow(Branch *root, const char *find, const char *replace, const uint level, const uint m)
{
    indent(4, level);
    root->numBranches = numMatches(root->str, find);
    printf("%d: %s: %d\n", m, root->str, root->numBranches);
    if(root->numBranches == 0){
        root->branch = NULL;
        return;
    }
    root->branch = calloc(root->numBranches, sizeof(Branch*));
    const int oldlen = strlen(root->str);
    const int finlen = strlen(find);
    const int replen = strlen(replace);
    const int newlen = oldlen+(replen-finlen);
    if(newlen<0){
        printf("err: newlen < 0\n");
        printf("%s: line %d\n", __FILE__, __LINE__);
        exit(-1);
    }

    for(uint i = 0; i < root->numBranches; i++){
        char *newstr = calloc(newlen+1, sizeof(char));
        char *ob = strstr(root->str, find);
        for(uint j = 0; j < i; j++){
            ob = strstr(ob+1, find);
        }
        int oblen = ob-root->str;
        memcpy(newstr, root->str, oblen);
        char *nb = newstr+oblen;
        memcpy(nb, replace, replen);
        char *na = nb+replen;
        int nalen = newlen-(na - newstr)+1;
        char *oa = ob+finlen;
        int oalen = oldlen-(oa - root->str)+1;
        if(oalen != nalen)
            printf("Size mismatch\n");
        memcpy(na, oa, nalen);
        root->branch[i] = malloc(sizeof(Branch));
        root->branch[i]->str = newstr;
        ob++;
        grow(root->branch[i], find, replace, level+1, i+1);
    }
    // const char *n = root->str;
    // for(uint i = 0; i < root->numBranches; i++){
    //     char *newstr = calloc(newlen+1, sizeof(char));
    //
    //     // copy whole string
    //     memcpy(newstr, root->str, newlen);
    //
    //     // find next occurence in basestr
    //     n = strstr(n, find);
    //     // find len up to occurence
    //     const int olen = n-root->str;
    //     // copy replacement into newstr offset by olen
    //     memcpy(newstr+olen, replace, replen);
    //
    //     // find after replacement in newstr
    //     char *after = newstr+olen+replen;
    //     // find after occurence in basestr;
    //     n+=finlen;
    //     // copy after occurence basestr to after replacement newstr
    //     memcpy(after, n, strlen(n));
    //
    //     root->branch[i] = malloc(sizeof(Branch));
    //     root->branch[i]->str = newstr;
    //     grow(root->branch[i], find, replace, level+1);
    // }
}

// bool apply(Branch *root, const char *find, const char *replace)
// {
//     root->numBranches = numMatches(root->str, find);
//     for(uint i = 0; i < root->numBranches; i++){
//         if(root->branch[i] == NULL)
//
//     }
// }

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
