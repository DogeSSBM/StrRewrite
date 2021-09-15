#include "Includes.h"

typedef struct Node_s{
    char *str;
    Term *term;
    struct Node_s *children;
    struct Node_s *next;
}Node;

int main
(int argc, char **argv)
{
    char *source = readFile("./Test.txt");
    RuleSet *rs = parseAll(source);
    printRuleSet(rs);
    free(source);
    freeAll(rs);
    return 0;
}

/*

a_a

aaaa

aa
a a
a  a
 aa
 a a
  aa
*/
