#include <stdlib.h>
#include "xtcrules.h"

sRules* _RULES;

sRules* sRules_get(void){
    if (_RULES == NULL){
        _RULES = malloc(sizeof(sRules));
        memcpy(_RULES, &DEFAULT_RULES , sizeof(sRules));
    }
    return _RULES;
}
sRules* sRules_set(sRules newRules){
    memcpy(_RULES, &newRules , sizeof(sRules));
    return _RULES;
}