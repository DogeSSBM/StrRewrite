#pragma once

bool checkIfEmptyMatch
(char *str, Rule *rule)
{
    bool ret = false;
    if(ret |= str == NULL)
        printf("Cannot match empty str\n");
    if(ret |= rule == NULL)
        printf("Cannot match empty rule\n");
    return ret;
}

Rule *
matchRule
(char *str, Rule *rule)
{
    return rule;
}

bool
ruleSameLterms
(Rule *rule1, Rule *rule2)
{

}

bool
matchExistsInList
(Rule *list, Rule *match)
{
    if(match == NULL){
        printf("Cannot check if NULL match exists in match list\n");
        exit(-1);
    }
    Rule *prev = NULL;
    while(list != NULL){
        if(prev != NULL && !ruleSameLterms(prev, list)){
            printf("Rules in match list have different L terms\n");
            printRule(prev);
            printRule(list);
            exit(-1);
        }

        

        prev = list;
        list = list->next;
    }
    return false;
}

// Returns a list of all possible ways a rule can match a string
Rule *
matchRuleAll
(char *str, Rule *rule)
{
    if(checkIfEmptyMatch(str, rule))
        return NULL;

    return rule;
}
