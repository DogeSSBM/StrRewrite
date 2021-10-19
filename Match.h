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

Rule* matchRule
(char *str, Rule *rule)
{
    Rule *ret = dupRule(rule);
    for(Term *l = ret->l; l != NULL; l = l->next){

    }
    
    return freeRule(ret);
}

// Returns a list of all possible ways a rule matches a string
Rule* matchRuleAll
(char *str, Rule *rule)
{
    if(checkIfEmptyMatch(str, rule))
        return NULL;

    // for()

    return rule;
}
