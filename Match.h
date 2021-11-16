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

void printStrsTermList
(Term *list)
{
    Term *term = list;
    while(term != NULL){
        if(term->type == T_STR && term->match == NULL)
            return;
        term = term->next;
    }
    term = list;
    while(term != NULL){
        if(term->type == T_STR && term->match != NULL)
            printf("%s at %s\n", term->value, term->match);
        term = term->next;
    }
    putchar('\n');
}

Term* clearStrsTermList
(Term *list)
{
    Term *term = list;
    while(term != NULL){
        if(term->type == T_STR)
            term->match = NULL;
        term = term->next;
    }
    return list;
}

Term* matchStrsTermList
(char *str, Term *list)
{
    // const uint strTerms = numStrsTermList(list);
    char *pos = str;
    for(Term *l = list; l != NULL; l = l->next){
        if(l->type == T_VAR)
            continue;
        char *matchpos = strstr(pos, l->value);
        if(matchpos != NULL){
            pos = matchpos;
            l->match = pos;
        }else{
            return clearStrsTermList(list);
            //printf("Could not find \"%s\" in \"%s\"\n", l->value, pos);
        }
        pos += l->value == NULL ? 0 : strlen(l->value);
    }
    return list;
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
