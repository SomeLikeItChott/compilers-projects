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
int type();
int stndtype();
void subprgmdecs();
void subprgmdecstail();
void subprgmdec();
void subprgmdectail();
void subprgmdectailtail();
void subprgmhead();
void subprgmheadtail();
int args();
int paramlist();
int paramlisttail();
void cmpdstmt();
void cmpdstmttail();
void optnlstmts();
void stmtlist();
void stmtlisttail();
void stmt();
void stmttail();
int variable();
int variabletail();
int exprlist();
int exprlisttail();
int expr();
int exprtail();
int smplexpr();
int smplexprtail();
int term();
int termtail();
int factor();
int factortail();
void sign();


#endif