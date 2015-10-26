#ifndef LEX_ERRORS
#define LEX_ERRORS

#include "lexical_analyzer.h"

struct token checkidErrors(char *lexeme);
struct token checkintErrors(char *lexeme);
struct token checkrealErrors(char *lexeme);
struct token checklongrealErrors(char *lexeme);

#endif