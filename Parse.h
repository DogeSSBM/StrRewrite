#pragma once

char *readFile
(char *filePath)
{
    char *buffer = 0;
    long length;
    File *f = NULL;
    if((f = fopen(filePath, "rb")) == NULL){
        printf("Unable to open file \"%s\"\n", filePath);
        exit(-1);
    }

    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);
    buffer = calloc(length+1, sizeof(char));
    fread(buffer, 1, length, f);
    buffer[length] = '\0';
    fclose(f);
    return buffer;
}

bool isDelim
(char c)
{
    return c == '-' || c == '\n' || c == '<' || c == '@' || c == '_' || c == '$';
}

bool isDupeRule
(Rule *rule1, Rule *rule2)
{
    if(rule1 == NULL || rule2 == NULL || rule1->text == NULL || rule1->text == NULL)
    if(
        rule1 == rule2
    )
        return true;
    return strcmp(rule1->text, rule2->text) != 0;
}

uint getLen
(char *pos)
{
    if(*pos != '@' && *pos != '_' && *pos != '$'){
        printf("Dont know how to parse len of string beginning at %s\n", pos);
        exit(-1);
    }
    uint len = 0;
    pos++;
    while(!isDelim(*pos)){
        pos++;
        len++;
    }
    return len;
}

char *getEndOfRuleSet
(char *pos)
{
    char *end = strchr(pos, '@');
    if(end==NULL)
        end = strchr(pos, '\0');
    while(end>pos && isspace(*end))
        end--;
    return end;
}

void printTermListNoDelim
(Term *term)
{
    while(term!=NULL){
        puts(term->type==T_STR?term->value:term->name);
        term = term->next;
    }
}

void printTermList
(Term *term)
{
    while(term!=NULL){
        printf("%c%s", term->type==T_STR?'$':'_', term->type==T_STR?term->value:term->name);
        term = term->next;
    }
}

void printTermListLn
(Term *term)
{
    printTermList(term);
    putchar('\n');
}

Term *appendTerm
(Term *list, Term *term)
{
    if(list == NULL)
        return term;
    Term *current = list;
    while(current->next != NULL)
        current = current->next;
    current->next = term;
    return list;
}

Term *currentTerm
(Term *list)
{
    if(list == NULL){
        printf("Cannot get current Term from NULL list\n");
        exit(-1);
    }
    while(list->next != NULL)
        list = list->next;
    return list;
}

Term *parseTerm
(char *pos)
{
    if(*pos!='$' && *pos!='_'){
        if(*pos!='-' && *pos!='<'){
            printf("unexpected char where -$_< expected\n%s\n", pos);
            exit(-1);
        }
        return NULL;
    }

    Term *term = calloc(1, sizeof(Term));
    term->type = *pos=='$'?T_STR:T_VAR;
    const uint len = getLen(pos);
    pos++;

    char **label = term->type==T_STR ? &(term->value) : &(term->name);
    *label = calloc(len+1, 1);
    memcpy(*label, pos, len);

    return term;
}

Term *dupTerm
(Term *term)
{
    if(term == NULL)
        return NULL;
    Term *dup = calloc(1, sizeof(Term));
    dup->type = term->type;
    char **label = term->type==T_STR ? &(term->value) : &(term->name);
    const uint len = strlen(*label);
    char **duplabel = dup->type==T_STR ? &(dup->value) : &(dup->name);
    *duplabel = calloc(len+1, 1);
    memcpy(*duplabel, *label, len);
    return dup;
}

Term *freeTerm
(Term *term)
{
    if(term==NULL)
        return NULL;
    free(term->name);
    free(term->value);
    Term *next = term->next;
    free(term);
    return next;
}

void printRule
(Rule *rule)
{
    printf("\t");
    printTermList(rule->l);
    printf("->");
    printTermList(rule->r);
    printf("\n");
}

bool validTermSeq
(Term *terms)
{
    if(terms == NULL){
        printf("NULL term sequence is not valid\n");
        exit(-1);
    }
    if(terms->next == NULL)
        return true;
    Term *start = terms;
    TermType prev = terms->type;
    terms = terms->next;
    uint i = 1;
    while(terms != NULL){
        if(terms->type == T_VAR && prev == terms->type){
            printf("Error\n");
            printf("Term %u and %u of in rule: ", i, i+1);
            printTermList(start);
            printf("Rules must not contain sequences of variables\n"
            );
            exit(-1);
        }
        i++;
    }
    return true;
}

bool validRule
(Rule *rule)
{
    Term *term = rule->l;
    if(term == NULL){
        printf("Rules must contain at least 1 Lterm");
        exit(-1);
    }

    bool wasvar = term->type == T_VAR;
    term = term->next;

    while(term != NULL){
        if(wasvar && term->type == T_VAR){
            printf("Lterm variables must be seperated by 1 or more Lterm string(s)\n");
            printf("%s\n", term->name);
            printRule(rule);
            return false;
        }
        wasvar = term->type == T_VAR;
        term = term->next;
    }

    return true;
}

Rule *freeRule
(Rule *rule)
{
    if(rule==NULL)
        return NULL;
    free(rule->text);
    while((rule->l = freeTerm(rule->l)) != NULL);
    while((rule->r = freeTerm(rule->r)) != NULL);
    Rule *next = rule->next;
    free(rule);
    return next;
}

Rule *getLastRule
(Rule *list)
{
    if(list == NULL){
        printf("Cannot get current Rule from NULL list\n");
        exit(-1);
    }

    while(list->next != NULL)
        list = list->next;

    return list;
}

Rule *appendRule
(Rule *list, Rule *rule)
{
    if(list == NULL)
        return rule;

    getLastRule(list)->next = rule;
    return list;
}

Rule *dupRule
(Rule *rule)
{
    if(rule == NULL)
        return NULL;

    Rule *dup = calloc(1, sizeof(Rule));
    Term *term = rule->r;
    while(term != NULL){
        dup->r = appendTerm(dup->r, dupTerm(term));
        term = term->next;
    }

    term = rule->l;
    while(term != NULL){
        dup->l = appendTerm(dup->l, dupTerm(term));
        term = term->next;
    }

    return dup;
}

Rule *reverseRule
(Rule *rule)
{
    if(rule == NULL)
        return NULL;

    Rule *rev = calloc(1, sizeof(Rule));
    Term *term = rule->r;
    while(term != NULL){
        rev->l = appendTerm(rev->l, dupTerm(term));
        term = term->next;
    }

    term = rule->l;
    while(term != NULL){
        rev->r = appendTerm(rev->r, dupTerm(term));
        term = term->next;
    }

    return rev;
}

Rule *parseRule
(char *pos)
{
    // static uint count = 0;
    // printf("Parsing rule %u\n", count++);
    if(*pos!='$' && *pos!='_'){
        printf("no $/_ ? %s", pos);
        exit(-1);
    }

    char *end = strchr(pos, '\n');
    char *arrow = strstr(pos, "->");
    const uint textlen = end-pos;
    bool doubleArrow = *(arrow-1) == '<';
    arrow-=doubleArrow;

    Rule *rule = calloc(1, sizeof(Rule));
    rule->text = calloc(textlen+1, 1);
    memcpy(rule->text, pos, textlen);

    Term *term = NULL;
    while(pos<arrow){
        term = parseTerm(pos);
        rule->l = appendTerm(rule->l, term);
        pos++;
        pos = strpbrk(pos, "$_");
    }

    term = NULL;
    while(pos != NULL && pos < end){
        term = parseTerm(pos);
        rule->r = appendTerm(rule->r, term);
        pos++;
        pos = strpbrk(pos, "<-$_");
    }

    // if(!validTermSeq(rule->l)){
    //     freeRule(rule);
    //     return NULL;
    // }
    if(!validRule(rule))
        exit(-1);

    if(doubleArrow)
        return appendRule(rule, reverseRule(rule));

    return rule;
}

void printRuleSet
(RuleSet *rs)
{
    while(rs != NULL){
        printf("RuleSet \"@%s\" -\n", rs->name);
        Rule *rule = rs->rules;

        while(rule != NULL){
            printRule(rule);
            rule = rule->next;
        }

        rs = rs->next;
        printf("\n");
    }
}

RuleSet *appendRuleSet
(RuleSet *list, RuleSet *ruleset)
{
    if(list == NULL)
        return ruleset;

    RuleSet *current = list;

    while(current->next != NULL)
        current = current->next;

    current->next = ruleset;
    return list;
}

RuleSet *currentRuleSet
(RuleSet *list)
{
    if(list == NULL){
        printf("Cannot get current RuleSet from NULL list\n");
        exit(-1);
    }

    while(list->next != NULL)
        list = list->next;

    return list;
}

RuleSet *parseRuleSet
(char *pos)
{
    if(*pos!='@'){
        printf("no @? %s\n", pos);
        exit(-1);
    }

    const uint len = getLen(pos);
    RuleSet *rs = calloc(1, sizeof(RuleSet));
    rs->name = calloc(len+1, 1);
    pos++;
    char *end = getEndOfRuleSet(pos);
    memcpy(rs->name, pos, len);
    Rule *rule = NULL;

    while(pos != NULL && pos<end){
        pos = strchr(pos, '\n');
        pos = strpbrk(pos, "$_");
        if(pos == NULL || pos > end)
            return rs;
        rule = parseRule(pos);
        rs->rules = appendRule(rs->rules, rule);
    }

    return rs;
}

RuleSet *freeRuleSet
(RuleSet *rs)
{
    if(rs==NULL)
        return NULL;
    free(rs->name);
    while((rs->rules=freeRule(rs->rules)) != NULL);
    RuleSet *next = rs->next;
    free(rs);
    return next;
}

RuleSet *parseAll
(char *pos)
{
    if(*pos!='@'){
        printf("no @? %s\n", pos);
        exit(-1);
    }
    RuleSet *rs = NULL;
    do{
        rs = appendRuleSet(rs, parseRuleSet(pos));
    }while((pos = strchr(pos+1, '@'))!=NULL);
    return rs;
}

void freeAll
(RuleSet *rs)
{
    while((rs = freeRuleSet(rs)) != NULL);
}
