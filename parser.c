#include "lexical_analyzer.h"
#include "parser.h"

struct token tok;

struct token tempToken(int type, long attr){
	struct token temp = {type, attr};
	return temp;
}

int tokensEq(struct token t1, struct token t2){
	if(t1.type == t2.type && t1.attr == t2.attr)
		return 1;
	else
		return 0;
}

void parse(){
	tok = getToken();
	prgm();
	match(EOF_TYPE, NO_ATTR);
}

void match(int expType, long expAttr){
	//printf("exptype: %d, expattr: %ld, tok.type: %d, tok.attr: %ld\n", expType, expAttr, tok.type, tok.attr);
	//if the expected attribute is -1, we should only match the type (for ids only)
	if(expAttr == -1){
		match(expType, tok.attr);
	} else{
		if(tokenEquals(tok, expType, expAttr) && !tokenEquals(tok, EOF_TYPE, NO_ATTR))
			tok = getToken();
		else if(tokenEquals(tok, expType, expAttr) && tokenEquals(tok, EOF_TYPE, NO_ATTR))
			printf("PARSE COMPLETE\n");
		else if(!tokenEquals(tok, expType, expAttr))
			printf("SYNTAX ERROR, need more hrer\n");
		else
			printf("I fell through match this is a problem\n");
	}
}

void prgm(){
	if(tokenEquals(tok, PROG_TYPE, NO_ATTR)){
		match(PROG_TYPE, NO_ATTR);
		match(ID_TYPE, -1);
		match(CATCHALL_TYPE, OPENPAREN_ATTR);
		idlist();
		match(CATCHALL_TYPE, CLOSEPAREN_ATTR);
		match(CATCHALL_TYPE, SEMICOLON_ATTR);
		prgmtail();
	}
	else
		printf("prgm error\n");
}

void prgmtail(){
	if(tokenEquals(tok, BEGIN_TYPE, NO_ATTR)){
		cmpdstmt();
		match(CATCHALL_TYPE, PERIOD_ATTR);
	} else if (tokenEquals(tok, FUNC_TYPE, NO_ATTR)){
		subprgmdecs();
		cmpdstmt();
		match(CATCHALL_TYPE, PERIOD_ATTR);
	} else if(tokenEquals(tok, VAR_TYPE, NO_ATTR)){
		decs();
		prgmtailtail();
	} else{
		printf("prgmtail error\n");
	}
}

void prgmtailtail(){
	if(tokenEquals(tok, BEGIN_TYPE, NO_ATTR)){
		cmpdstmt();
		match(CATCHALL_TYPE, PERIOD_ATTR);
	} else if (tokenEquals(tok, FUNC_TYPE, NO_ATTR)){
		subprgmdecs();
		cmpdstmt();
		match(CATCHALL_TYPE, PERIOD_ATTR);
	} else{
		printf("prgmtailtail error\n");
	}
}

void idlist(){
	if(tokenEquals(tok, ID_TYPE, -1)){
		match(ID_TYPE, -1);
		idlisttail();
	} else{
		printf("idlist error\n");
	}
}

void idlisttail(){
	if(tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR)){
		match(CATCHALL_TYPE, COMMA_ATTR);
		match(ID_TYPE, -1);
		idlisttail();
	} else if(tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR)){
		return;
	} else{
		printf("idlisttail error\n");
	}
}

void decs(){
	if(tokenEquals(tok, VAR_TYPE, NO_ATTR)){
		match(VAR_TYPE, NO_ATTR);
		match(ID_TYPE, -1);
		match(CATCHALL_TYPE, COLON_ATTR);
		type();
		match(CATCHALL_TYPE, SEMICOLON_ATTR);
		decstail();
	} else{
		printf("decs error\n");
	}
}

void decstail(){
	if(tokenEquals(tok, VAR_TYPE, NO_ATTR)){
		match(VAR_TYPE, NO_ATTR);
		match(ID_TYPE, -1);
		match(CATCHALL_TYPE, COLON_ATTR);
		type();
		match(CATCHALL_TYPE, SEMICOLON_ATTR);
		decstail();
	} else if (tokenEquals(tok, FUNC_TYPE, NO_ATTR)){
		return;
	} else if (tokenEquals(tok, BEGIN_TYPE, NO_ATTR)){
		return;
	} else{
		printf("decstail error\n");
	}
}

void type(){
	if(tokenEquals(tok, ARRAY_TYPE, NO_ATTR)){
		match(ARRAY_TYPE, NO_ATTR);
		match(CATCHALL_TYPE, OPENBRACKET_ATTR);
		match(INT_TYPE, NO_ATTR);
		match(CATCHALL_TYPE, PERIOD_ATTR);
		match(CATCHALL_TYPE, PERIOD_ATTR);
		match(INT_TYPE, NO_ATTR);
		match(CATCHALL_TYPE, CLOSEBRACKET_ATTR);
		match(OF_TYPE, NO_ATTR);
		stndtype();
	} else if(tokenEquals(tok, REALWORD_TYPE, NO_ATTR)){
		stndtype();
	} else if(tokenEquals(tok, INTWORD_TYPE, NO_ATTR)){
		stndtype();
	} else{
		printf("type error\n");
	}
}

void stndtype(){
	if(tokenEquals(tok, REALWORD_TYPE, NO_ATTR)){
		match(REALWORD_TYPE, NO_ATTR);
	} else if(tokenEquals(tok, INTWORD_TYPE, NO_ATTR)){
		match(INTWORD_TYPE, NO_ATTR);
	} else{
		printf("stndtype error\n");
	}
}

void subprgmdecs(){
	if(tokenEquals(tok, FUNC_TYPE, NO_ATTR)){
		subprgmdec();
		match(CATCHALL_TYPE, SEMICOLON_ATTR);
		subprgmdecstail();
	} else{
		printf("subprgmdecs error\n");
	}
}

void subprgmdecstail(){
	if(tokenEquals(tok, FUNC_TYPE, NO_ATTR)){
		subprgmdec();
		match(CATCHALL_TYPE, SEMICOLON_ATTR);
		subprgmdecstail();
	} else if(tokenEquals(tok, BEGIN_TYPE, NO_ATTR)){
		return;
	} else{
		printf("subprgmdecstail error\n");
	}
}

void subprgmdec(){
	if(tokenEquals(tok, FUNC_TYPE, NO_ATTR)){
		subprgmhead();
		subprgmdectail();
	} else{
		printf("subprgmdec error\n");
	}
}

void subprgmdectail(){
	if(tokenEquals(tok, FUNC_TYPE, NO_ATTR)){
		subprgmdecs();
		cmpdstmt();
	} else if(tokenEquals(tok, BEGIN_TYPE, NO_ATTR)){
		cmpdstmt();
	} else if(tokenEquals(tok, VAR_TYPE, NO_ATTR)){
		decs();
		subprgmdectailtail();
	} else{
		printf("subprgmdec error\n");
	}
}

void subprgmdectailtail(){
	if(tokenEquals(tok, FUNC_TYPE, NO_ATTR)){
		subprgmdecs();
		cmpdstmt();
	} else if(tokenEquals(tok, BEGIN_TYPE, NO_ATTR)){
		cmpdstmt();
	} else{
		printf("subprgmdec error\n");
	}
}

void subprgmhead(){
	if(tokenEquals(tok, FUNC_TYPE, NO_ATTR)){
		match(FUNC_TYPE, NO_ATTR);
		match(ID_TYPE, -1);
		subprgmheadtail();
	} else{
		printf("subprgmhead error\n");
	}
}

void subprgmheadtail(){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR)){
		args();
		match(CATCHALL_TYPE, COLON_ATTR);
		stndtype();
		match(CATCHALL_TYPE, SEMICOLON_ATTR);
	} else if(tokenEquals(tok, CATCHALL_TYPE, COLON_ATTR)){
		match(CATCHALL_TYPE, COLON_ATTR);
		stndtype();
		match(CATCHALL_TYPE, SEMICOLON_ATTR);
	} else {
		printf("subprgmheadtail error\n");
	}
}

void args(){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR)){
		match(CATCHALL_TYPE, OPENPAREN_ATTR);
		paramlist();
		match(CATCHALL_TYPE, CLOSEPAREN_ATTR);
	}else{
		printf("args error\n");
	}
}

void paramlist(){
	if(tokenEquals(tok, ID_TYPE, -1)){
		match(ID_TYPE, -1);
		match(CATCHALL_TYPE, COLON_ATTR);
		type();
		paramlisttail();
	} else{
		printf("paramlist error\n");
	}
}

void paramlisttail(){
	if(tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR)){
		return;
	} else if(tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR)){
		match(CATCHALL_TYPE, SEMICOLON_ATTR);
		match(ID_TYPE, -1);
		match(CATCHALL_TYPE, COLON_ATTR);
		type();
		paramlisttail();
	} else{
		printf("paramlisttail error\n");
	}
}

void cmpdstmt(){
	if(tokenEquals(tok, BEGIN_TYPE, NO_ATTR)){
		match(BEGIN_TYPE, NO_ATTR);
		cmpdstmttail();
	} else{
		printf("cmpdstmt error\n");
	}
}

void cmpdstmttail(){
	if (tokenEquals(tok, BEGIN_TYPE, NO_ATTR) ||
		tokenEquals(tok, ID_TYPE, -1) ||
		tokenEquals(tok, IF_TYPE, NO_ATTR) ||
		tokenEquals(tok, WHILE_TYPE, NO_ATTR)){
		optnlstmts();
		match(END_TYPE, NO_ATTR);
	} else if (tokenEquals(tok, END_TYPE, NO_ATTR)){
		match(END_TYPE, NO_ATTR);
	} else{
		printf("cmpdstmttail error\n");
	}
}

void optnlstmts(){
	if(tokenEquals(tok, BEGIN_TYPE, NO_ATTR) || 
		tokenEquals(tok, ID_TYPE, -1) ||
		tokenEquals(tok, IF_TYPE, NO_ATTR) ||
		tokenEquals(tok, WHILE_TYPE, NO_ATTR)){
		stmtlist();
	} else{
		printf("optnlstmts error\n");
	}
}

void stmtlist(){
	if(tokenEquals(tok, BEGIN_TYPE, NO_ATTR) || 
		tokenEquals(tok, ID_TYPE, -1) ||
		tokenEquals(tok, IF_TYPE, NO_ATTR) ||
		tokenEquals(tok, WHILE_TYPE, NO_ATTR)){
		stmt();
		stmtlisttail();
	} else{
		printf("stmtlist error\n");
	}
}

void stmtlisttail(){
	if(tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR)){
		match(CATCHALL_TYPE, SEMICOLON_ATTR);
		stmt();
		stmtlisttail();
	} else if(tokenEquals(tok, END_TYPE, NO_ATTR)){
		return;
	} else{
		printf("stmtlisttail error\n");
	}
}

void stmt(){
	if(tokenEquals(tok, BEGIN_TYPE, NO_ATTR)){
		cmpdstmt();
	} else if(tokenEquals(tok, ID_TYPE, -1)){
		variable();
		match(ASSIGNOP_TYPE, NO_ATTR);
		expr();
	} else if(tokenEquals(tok, IF_TYPE, NO_ATTR)){
		match(IF_TYPE, NO_ATTR);
		expr();
		match(THEN_TYPE, NO_ATTR);
		stmt();
		stmttail();
	} else if(tokenEquals(tok, WHILE_TYPE, NO_ATTR)){
		match(WHILE_TYPE, NO_ATTR);
		expr();
		match(DO_TYPE, NO_ATTR);
		stmt();
	} else{
		printf("stmt error\n");
	}
}

void stmttail(){
	if(tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) || tokenEquals(tok, END_TYPE, NO_ATTR)){
		return;
	} else if(tokenEquals(tok, ELSE_TYPE, NO_ATTR)){
		match(ELSE_TYPE, NO_ATTR);
		stmt();
	} else{
		printf("stmttail error\n");
	}
}

void variable(){
	if(tokenEquals(tok, ID_TYPE, -1)){
		match(ID_TYPE, -1);
		variabletail();
	} else{
		printf("variable error\n");
	}
}

void variabletail(){
	if(tokenEquals(tok, ASSIGNOP_TYPE, NO_ATTR)){
		return;
	} else if(tokenEquals(tok, CATCHALL_TYPE, OPENBRACKET_ATTR)){
		match(CATCHALL_TYPE, OPENBRACKET_ATTR);
		expr();
		match(CATCHALL_TYPE, CLOSEBRACKET_ATTR);
	} else{
		printf("variabletail error\n");
	}
}

void exprlist(){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR) ||
		tokenEquals(tok, CATCHALL_TYPE, PLUS_ATTR) ||
		tokenEquals(tok, CATCHALL_TYPE, MINUS_ATTR) ||
		tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) ||
		tokenEquals(tok, ID_TYPE, -1) ||
		tokenEquals(tok, NOT_TYPE, NO_ATTR) ||
		tokenEquals(tok, INT_TYPE, NO_ATTR) ||
		tokenEquals(tok, REAL_TYPE, NO_ATTR)){
		expr();
		exprlisttail();
	} else{
		printf("exprlist error\n");
	}
}

void exprlisttail(){
	if(tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR)){
		return;
	} else if (tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR)) {
		match(CATCHALL_TYPE, COMMA_ATTR);
		expr();
		exprlisttail();
	} else{
		printf("exprlisttail error\n");
	}
}

void expr(){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR) || tokenEquals(tok, CATCHALL_TYPE, PLUS_ATTR) || tokenEquals(tok, CATCHALL_TYPE, MINUS_ATTR) || tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) || tokenEquals(tok, ID_TYPE, -1) || tokenEquals(tok, NOT_TYPE, NO_ATTR) || tokenEquals(tok, INT_TYPE, NO_ATTR) || tokenEquals(tok, REAL_TYPE, NO_ATTR)){
		smplexpr();
		exprtail();
	} else {
		printf("expr error\n");
	}
}

void exprtail(){
	if(tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR) || tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) || tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) || tokenEquals(tok, CATCHALL_TYPE, CLOSEBRACKET_ATTR) || tokenEquals(tok, DO_TYPE, NO_ATTR) || tokenEquals(tok, ELSE_TYPE, NO_ATTR) || tokenEquals(tok, END_TYPE, NO_ATTR) || tokenEquals(tok, THEN_TYPE, NO_ATTR)){
		return;
	} else if (tokenEquals(tok, RELOP_TYPE, -1)){
		match(RELOP_TYPE, -1);
		smplexpr();
	} else{
		printf("exprtail error\n");
	}
}

void smplexpr(){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR) || tokenEquals(tok, ID_TYPE, -1) || tokenEquals(tok, NOT_TYPE, NO_ATTR) || tokenEquals(tok, INT_TYPE, NO_ATTR) || tokenEquals(tok, REAL_TYPE, NO_ATTR)){
		term();
		smplexprtail();
	} else if (tokenEquals(tok, CATCHALL_TYPE, MINUS_ATTR) || tokenEquals(tok, CATCHALL_TYPE, PLUS_ATTR)){
		sign();
		term();
		smplexprtail();
	} else{
		printf("smplexpr error\n");
	}
}

void smplexprtail(){
	if(tokenEquals(tok, RELOP_TYPE, -1) || tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR) || tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) || tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) || tokenEquals(tok, CATCHALL_TYPE, CLOSEBRACKET_ATTR) || tokenEquals(tok, DO_TYPE, NO_ATTR) || tokenEquals(tok, ELSE_TYPE, NO_ATTR) || tokenEquals(tok, END_TYPE, NO_ATTR) || tokenEquals(tok, THEN_TYPE, NO_ATTR)){
		return;
	} else if(tokenEquals(tok, ADDOP_TYPE, -1)){
		match(ADDOP_TYPE, -1);
		term();
		smplexprtail();
	} else{
		printf("smplexprtail error\n");
	}
}

void term(){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR) || tokenEquals(tok, ID_TYPE, -1) || tokenEquals(tok, NOT_TYPE, NO_ATTR) || tokenEquals(tok, INT_TYPE, NO_ATTR) || tokenEquals(tok, REAL_TYPE, NO_ATTR)){
		factor();
		termtail();
	} else {
		printf("term error\n");
	}
}

void termtail(){
	if(tokenEquals(tok, ADDOP_TYPE, -1) || tokenEquals(tok, RELOP_TYPE, -1) || tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR) || tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) || tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) || tokenEquals(tok, CATCHALL_TYPE, CLOSEBRACKET_ATTR) || tokenEquals(tok, DO_TYPE, NO_ATTR) || tokenEquals(tok, ELSE_TYPE, NO_ATTR) || tokenEquals(tok, END_TYPE, NO_ATTR) || tokenEquals(tok, THEN_TYPE, NO_ATTR)){
		return;
	} else if(tokenEquals(tok, MULOP_TYPE, -1)){
		match(MULOP_TYPE, -1);
		factor();
		termtail();
	} else {
		printf("termtail error\n");
	}
}

void factor(){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR)){
		match(CATCHALL_TYPE, OPENPAREN_ATTR);
		expr();
		match(CATCHALL_TYPE, CLOSEPAREN_ATTR);
	} else if(tokenEquals(tok, ID_TYPE, -1)){
		match(ID_TYPE, -1);
		factortail();
	} else if(tokenEquals(tok, NOT_TYPE, NO_ATTR)){
		match(NOT_TYPE, NO_ATTR);
		factor();
	} else if(tokenEquals(tok, INT_TYPE, NO_ATTR)){
		match(INT_TYPE, NO_ATTR);
	} else if(tokenEquals(tok, REAL_TYPE, NO_ATTR)){
		match(REAL_TYPE, NO_ATTR);
	} else{
		printf("factor error\n");
	}
}

void factortail(){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR)){
		match(CATCHALL_TYPE, OPENPAREN_ATTR);
		exprlist();
		match(CATCHALL_TYPE, CLOSEPAREN_ATTR);
	} else if(tokenEquals(tok, CATCHALL_TYPE, OPENBRACKET_ATTR)){
		match(CATCHALL_TYPE, OPENBRACKET_ATTR);
		expr();
		match(CATCHALL_TYPE, CLOSEBRACKET_ATTR);
	} else if(tokenEquals(tok, MULOP_TYPE, -1) || tokenEquals(tok, ADDOP_TYPE, -1) || tokenEquals(tok, RELOP_TYPE, -1) || tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR) || tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) || tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) || tokenEquals(tok, CATCHALL_TYPE, CLOSEBRACKET_ATTR) || tokenEquals(tok, DO_TYPE, NO_ATTR) || tokenEquals(tok, ELSE_TYPE, NO_ATTR) || tokenEquals(tok, END_TYPE, NO_ATTR) || tokenEquals(tok, THEN_TYPE, NO_ATTR)){
		return;
	} else{
		printf("factortail error\n");
	}
}

void sign(){
	if(tokenEquals(tok, CATCHALL_TYPE, PLUS_ATTR)){
		match(CATCHALL_TYPE, PLUS_ATTR);
	} else if (tokenEquals(tok, CATCHALL_TYPE, MINUS_ATTR)){
		match(CATCHALL_TYPE, MINUS_ATTR);
	} else{
		printf("sign error\n");
	}
}

/*
//defining token types
#define WS_TYPE 		0
#define ID_TYPE 		1
#define INT_TYPE 		2
#define REAL_TYPE 		3
#define LONGREAL_TYPE 	4
#define RELOP_TYPE		5
#define ADDOP_TYPE		6
#define MULOP_TYPE		7
#define ASSIGNOP_TYPE 	8
#define CATCHALL_TYPE	9
//these ones are defined in reserved
#define PROG_TYPE		10
#define FUNC_TYPE		11
#define PROC_TYPE		12
#define ARRAY_TYPE		13
#define OF_TYPE			14
#define BEGIN_TYPE		15
#define END_TYPE		16
#define IF_TYPE			17
#define THEN_TYPE		18
#define ELSE_TYPE		19
#define WHILE_TYPE		20
#define DO_TYPE			21
#define VAR_TYPE		22
#define INTWORD_TYPE	23
#define REALWORD_TYPE	24
#define NOT_TYPE		25
//end types defined in reserved
#define EOF_TYPE		50
#define LEXERR_TYPE		99

//ws attr
#define SPACE_ATTR 		1
#define NEWLINE_ATTR	2
#define TAB_ATTR		3
#define OTHER_WS_ATTR	4

//relop attr
#define EQUAL_ATTR		1
#define GTEQUAL_ATTR	2
#define GT_ATTR 		3
#define LT_ATTR 		4
#define LTEQUAL_ATTR	5
#define NOTEQUAL_ATTR 	6

//addop attr
#define PLUS_ATTR		1
#define MINUS_ATTR		2
#define OR_ATTR			3

//mulop attr
#define TIMES_ATTR		1
#define DIVIDE_ATTR		2
#define DIV_ATTR 		3
#define MOD_ATTR		4
#define AND_ATTR		5

//catchall attr
#define COLON_ATTR		1
#define SEMICOLON_ATTR 	2
#define OPENBRACKET_ATTR 3
#define CLOSEBRACKET_ATTR 4
#define OPENPAREN_ATTR 	5
#define CLOSEPAREN_ATTR 6
#define OPENCURLYBRACE_ATTR 7
#define CLOSECURLYBRACE_ATTR 8
#define COMMA_ATTR 		9
#define PERIOD_ATTR		10

//lexerr attr
#define EXTRA_LONG_INT_ATTR		1
#define EXTRA_LONG_ID_ATTR		2
#define LEADING_ZEROES_ATTR		3
#define XX_TOO_LONG_ATTR		4
#define YY_TOO_LONG_ATTR		5
#define TRAILING_ZEROES_ATTR	6
#define UNRECOGNIZED_SYMBOL_ATTR 7
#define ZZ_TOO_LONG_ATTR		8

//misc attr
#define NO_ATTR		0
*/