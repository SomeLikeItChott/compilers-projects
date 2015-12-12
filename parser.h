#ifndef PARSER
#define PARSER

#include "lexical_analyzer.h"
#include "helpers.h"

extern struct token tok;

struct token tempToken(int type, long attr);
int tokensEq(struct token t1, struct token t2);
void parse();
void match(int expType, long expAttr);
void prgm();
void prgmtail();
void prgmtailtail();
void idlist();
void idlisttail();
void decs();
void decstail();
void type();
void stndtype();
void subprgmdecs();
void subprgmdecstail();
void subprgmdec();
void subprgmdectail();
void subprgmdectailtail();
void subprgmhead();
void subprgmheadtail();
void args();
void paramlist();
void paramlisttail();
void cmpdstmt();
void cmpdstmttail();
void optnlstmts();
void stmtlist();
void stmtlisttail();
void stmt();
void stmttail();
void variable();
void variabletail();
void exprlist();
void exprlisttail();
void expr();
void exprtail();
void smplexpr();
void smplexprtail();
void term();
void termtail();
void factor();
void factortail();
void sign();


#endif