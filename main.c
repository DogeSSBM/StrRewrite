#include "Includes.h"

#define STRNUM      7

int main
(int argc, char **argv)
{
    char *source = readFile("./Test.txt");
    RuleSet *rs = parseAll(source);
    printRuleSet(rs);

    const char *testStr[STRNUM] = {
        "abcd",
        "(abcdef)",
        "sdup(xyz)",
        "poof()dup",
        "sdup(yes)",
        "sdup(yo)dup(me)",
        "DD(asdf)"
    };

    printf("--------------------------------\n\n");

    Rule *rule = rs->rules;
    do{
        printf("Rule ");
        printRule(rule);
        for(uint i = 0; i < STRNUM; i++){

        }

    }while((rule = rule->next) != NULL);

    free(source);
    freeAll(rs);
    return 0;
}
