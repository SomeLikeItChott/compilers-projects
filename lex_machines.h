#ifndef LEX_MACHINES
#define LEX_MACHINES

#include <ctype.h>
#include "lexical_analyzer.h"

struct token fail();
struct token wsMachine();
struct token idMachine();
struct token intMachine();
struct token realMachine();
struct token longrealMachine();
struct token relopMachine();
struct token catchallMachine();


#endif