#ifndef SENETXTC_RULES_H
#define SENETXTC_RULES_H

typedef struct _SenetRules {
    char addmoves[5];//1-5, for empty positions use 0
} sRules;
const sRules DEFAULT_RULES = {.addmoves = {1,4,5,0,0}};

/*
 * return current sRules*
 * if rules has not been set,
 * initialize it from DEFAULT_RULES
 */
sRules* sRules_get(void);
sRules* sRules_set(sRules);
#endif