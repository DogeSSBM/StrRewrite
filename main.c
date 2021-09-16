#include "Includes.h"

State applyRewrites
(RuleSet *rs, char *str)
{
    State
}

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
