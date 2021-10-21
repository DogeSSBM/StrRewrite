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

// _H$DD(_A$dee_B$)_T

// head DD(h dee t) tail

uint numStrsTermList
(Term *term)
{
    uint ret = 0;
    while(term != NULL){
        ret += term->type == T_STR;
        term = term->next;
    }
    return ret;
}

Term* matchStrsTermList
(const char *str, Term *term)
{
    const uint strTerms = numStrsTermList(term);
    char *pos = str;
    for(Term *l = term; l != NULL; l = l->next){
        if(l->type == T_VAR)
            continue;
        if(pos = strstr(pos, l->text))
            return NULL;
        pos+=strlen(l->text);
}

Rule* matchRule
(char *str, Rule *rule)
{

    return rule;
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
