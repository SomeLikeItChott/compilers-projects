#include "parser.h"

struct token tok;

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
		else if(!tokenEquals(tok, expType, expAttr)){
			char expText[12];
			getPlaintext(expText, expType, expAttr);
			char tokText[12];
			getPlaintext(tokText, tok.type, tok.attr);
			printf("SYNERR (match): Expecting %s, received %s\n", expText, tokText);
			fprintf(listingFile, "SYNERR: Expecting %s, received %s\n", expText, tokText);
			tok = getToken();
		}else
		printf("fell through match(), aborting\n");
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
	else{
		printf("prgm error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting program, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR)){
			tok = getToken();
		}
	}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of var, function, begin, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of function, begin, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR)){
			tok = getToken();
		}
	}
}

void idlist(){
	if(tokenEquals(tok, ID_TYPE, -1)){
		match(ID_TYPE, -1);
		idlisttail();
	} else{
		printf("idlist error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting identifier, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting , or ), received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting var, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, FUNC_TYPE, NO_ATTR) && 
			!tokenEquals(tok, BEGIN_TYPE, NO_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of var, function, begin, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, FUNC_TYPE, NO_ATTR) && 
			!tokenEquals(tok, BEGIN_TYPE, NO_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of array, real (word), int (word), received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR)){
			tok = getToken();
		}
	}
}

void stndtype(){
	if(tokenEquals(tok, REALWORD_TYPE, NO_ATTR)){
		match(REALWORD_TYPE, NO_ATTR);
	} else if(tokenEquals(tok, INTWORD_TYPE, NO_ATTR)){
		match(INTWORD_TYPE, NO_ATTR);
	} else{
		printf("stndtype error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of real (word), int (word), received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR)){
			tok = getToken();
		}
	}
}

void subprgmdecs(){
	if(tokenEquals(tok, FUNC_TYPE, NO_ATTR)){
		subprgmdec();
		match(CATCHALL_TYPE, SEMICOLON_ATTR);
		subprgmdecstail();
	} else{
		printf("subprgmdecs error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting function, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, BEGIN_TYPE, NO_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of function, begin, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, BEGIN_TYPE, NO_ATTR)){
			tok = getToken();
		}
	}
}

void subprgmdec(){
	if(tokenEquals(tok, FUNC_TYPE, NO_ATTR)){
		subprgmhead();
		subprgmdectail();
	} else{
		printf("subprgmdec error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting function, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of function, var, begin, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of function, begin, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR)){
			tok = getToken();
		}
	}
}

void subprgmhead(){
	if(tokenEquals(tok, FUNC_TYPE, NO_ATTR)){
		match(FUNC_TYPE, NO_ATTR);
		match(ID_TYPE, -1);
		subprgmheadtail();
	} else{
		printf("subprgmhead error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting function, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, FUNC_TYPE, NO_ATTR) && 
			!tokenEquals(tok, BEGIN_TYPE, NO_ATTR) && 
			!tokenEquals(tok, VAR_TYPE, NO_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of (, :, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, FUNC_TYPE, NO_ATTR) && 
			!tokenEquals(tok, BEGIN_TYPE, NO_ATTR) && 
			!tokenEquals(tok, VAR_TYPE, NO_ATTR)){
			tok = getToken();
		}
	}
}

void args(){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR)){
		match(CATCHALL_TYPE, OPENPAREN_ATTR);
		paramlist();
		match(CATCHALL_TYPE, CLOSEPAREN_ATTR);
	}else{
		printf("args error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting (, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, COLON_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting identifier, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of ), ;, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR)){
			tok = getToken();
		}
	}
}

void cmpdstmt(){
	if(tokenEquals(tok, BEGIN_TYPE, NO_ATTR)){
		match(BEGIN_TYPE, NO_ATTR);
		cmpdstmttail();
	} else{
		printf("cmpdstmt error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting begin, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, END_TYPE, NO_ATTR) && 
			!tokenEquals(tok, ELSE_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, PERIOD_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of begin, identifier, if, while, end, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, END_TYPE, NO_ATTR) && 
			!tokenEquals(tok, ELSE_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, PERIOD_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of begin, identifier, if, while, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, END_TYPE, NO_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of begin, identifier, if, while, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, END_TYPE, NO_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of ;, end, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, END_TYPE, NO_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of identifier, begin, if, while, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, END_TYPE, NO_ATTR) &&
			!tokenEquals(tok, ELSE_TYPE, NO_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of ;, end, else, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, END_TYPE, NO_ATTR) &&
			!tokenEquals(tok, ELSE_TYPE, NO_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR)){
			tok = getToken();
		}
	}
}

void variable(){
	if(tokenEquals(tok, ID_TYPE, -1)){
		match(ID_TYPE, -1);
		variabletail();
	} else{
		printf("variable error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting identifier, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, ASSIGNOP_TYPE, NO_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of :=, [, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, ASSIGNOP_TYPE, NO_ATTR)){
			tok = getToken();
		}
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
		tokenEquals(tok, REAL_TYPE, NO_ATTR) ||
		tokenEquals(tok, LONGREAL_TYPE, NO_ATTR)){
		expr();
		exprlisttail();
	} else{
		printf("exprlist error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of num, (, not, identifier, +, -, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR)){
			tok = getToken();
		}
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
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of comma, ), received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR)){
			tok = getToken();
		}
	}
}

void expr(){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, PLUS_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, MINUS_ATTR) ||
		tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) || 
		tokenEquals(tok, ID_TYPE, -1) || 
		tokenEquals(tok, NOT_TYPE, NO_ATTR) || 
		tokenEquals(tok, INT_TYPE, NO_ATTR) || 
		tokenEquals(tok, REAL_TYPE, NO_ATTR)|| 
		tokenEquals(tok, LONGREAL_TYPE, NO_ATTR)){
		smplexpr();
		exprtail();
	} else {
		printf("expr error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of comma, num, (, not, identifier, +, -, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, END_TYPE, NO_ATTR) &&
			!tokenEquals(tok, THEN_TYPE, NO_ATTR) &&
			!tokenEquals(tok, DO_TYPE, NO_ATTR) &&
			!tokenEquals(tok, ELSE_TYPE, NO_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEBRACKET_ATTR)){
			tok = getToken();
		}
	}
}

void exprtail(){
	if(tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, CLOSEBRACKET_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) ||
		tokenEquals(tok, DO_TYPE, NO_ATTR) || 
		tokenEquals(tok, ELSE_TYPE, NO_ATTR) || 
		tokenEquals(tok, END_TYPE, NO_ATTR) || 
		tokenEquals(tok, THEN_TYPE, NO_ATTR)){
		return;
	} else if (tokenEquals(tok, RELOP_TYPE, -1)){
		match(RELOP_TYPE, -1);
		smplexpr();
	} else{
		printf("exprtail error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of ), ;, ], do, else, end, then, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, END_TYPE, NO_ATTR) &&
			!tokenEquals(tok, THEN_TYPE, NO_ATTR) &&
			!tokenEquals(tok, DO_TYPE, NO_ATTR) &&
			!tokenEquals(tok, ELSE_TYPE, NO_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEBRACKET_ATTR)){
			tok = getToken();
		}
	}
}

void smplexpr(){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR) || 
		tokenEquals(tok, ID_TYPE, -1) || 
		tokenEquals(tok, NOT_TYPE, NO_ATTR) || 
		tokenEquals(tok, INT_TYPE, NO_ATTR) || 
		tokenEquals(tok, REAL_TYPE, NO_ATTR) || 
		tokenEquals(tok, LONGREAL_TYPE, NO_ATTR)){
		term();
		smplexprtail();
	} else if (tokenEquals(tok, CATCHALL_TYPE, MINUS_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, PLUS_ATTR)){
		sign();
		term();
		smplexprtail();
	} else{
		printf("smplexpr error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of num, (, not, identifier, +, -, received %s\n", tokText);
		while(!tokenEquals(tok, RELOP_TYPE, -1) &&
			!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, END_TYPE, NO_ATTR) &&
			!tokenEquals(tok, THEN_TYPE, NO_ATTR) &&
			!tokenEquals(tok, DO_TYPE, NO_ATTR) &&
			!tokenEquals(tok, ELSE_TYPE, NO_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEBRACKET_ATTR)){
			tok = getToken();
		}
	}
}

void smplexprtail(){
	if(tokenEquals(tok, RELOP_TYPE, -1) || 
		tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, CLOSEBRACKET_ATTR) || 
		tokenEquals(tok, DO_TYPE, NO_ATTR) || 
		tokenEquals(tok, ELSE_TYPE, NO_ATTR) || 
		tokenEquals(tok, END_TYPE, NO_ATTR) || 
		tokenEquals(tok, THEN_TYPE, NO_ATTR)){
		return;
	} else if(tokenEquals(tok, ADDOP_TYPE, -1)){
		match(ADDOP_TYPE, -1);
		term();
		smplexprtail();
	} else{
		printf("smplexprtail error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of relop, ), comma, ;, ], do, else, end, then, addop, received %s\n", tokText);
		while(!tokenEquals(tok, RELOP_TYPE, -1) &&
			!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, END_TYPE, NO_ATTR) &&
			!tokenEquals(tok, THEN_TYPE, NO_ATTR) &&
			!tokenEquals(tok, DO_TYPE, NO_ATTR) &&
			!tokenEquals(tok, ELSE_TYPE, NO_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEBRACKET_ATTR)){
			tok = getToken();
		}
	}
}

void term(){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR) || 
		tokenEquals(tok, ID_TYPE, -1) || 
		tokenEquals(tok, NOT_TYPE, NO_ATTR) || 
		tokenEquals(tok, INT_TYPE, NO_ATTR) || 
		tokenEquals(tok, REAL_TYPE, NO_ATTR) || 
		tokenEquals(tok, LONGREAL_TYPE, NO_ATTR)){
		factor();
		termtail();
	} else {
		printf("term error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of (, identifier, not, num, received %s\n", tokText);
		while(!tokenEquals(tok, ADDOP_TYPE, -1) &&
			!tokenEquals(tok, RELOP_TYPE, -1) &&
			!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, END_TYPE, NO_ATTR) &&
			!tokenEquals(tok, THEN_TYPE, NO_ATTR) &&
			!tokenEquals(tok, DO_TYPE, NO_ATTR) &&
			!tokenEquals(tok, ELSE_TYPE, NO_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEBRACKET_ATTR)){
			tok = getToken();
		}
	}
}

void termtail(){
	if(tokenEquals(tok, ADDOP_TYPE, -1) || 
		tokenEquals(tok, RELOP_TYPE, -1) || 
		tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, CLOSEBRACKET_ATTR) || 
		tokenEquals(tok, DO_TYPE, NO_ATTR) || 
		tokenEquals(tok, ELSE_TYPE, NO_ATTR) || 
		tokenEquals(tok, END_TYPE, NO_ATTR) || 
		tokenEquals(tok, THEN_TYPE, NO_ATTR)){
		return;
	} else if(tokenEquals(tok, MULOP_TYPE, -1)){
		match(MULOP_TYPE, -1);
		factor();
		termtail();
	} else {
		printf("termtail error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of addop, relop, ), comma, ;, ], do, else, end, then, mulop, received %s\n", tokText);
		while(!tokenEquals(tok, ADDOP_TYPE, -1) &&
			!tokenEquals(tok, RELOP_TYPE, -1) &&
			!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, END_TYPE, NO_ATTR) &&
			!tokenEquals(tok, THEN_TYPE, NO_ATTR) &&
			!tokenEquals(tok, DO_TYPE, NO_ATTR) &&
			!tokenEquals(tok, ELSE_TYPE, NO_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEBRACKET_ATTR)){
			tok = getToken();

		}
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
	} else if(tokenEquals(tok, LONGREAL_TYPE, NO_ATTR)){
		match(LONGREAL_TYPE, NO_ATTR);
	} else{
		printf("factor error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of (, identifier, not, num, received %s\n", tokText);
		while(!tokenEquals(tok, MULOP_TYPE, -1) &&
			!tokenEquals(tok, ADDOP_TYPE, -1) &&
			!tokenEquals(tok, RELOP_TYPE, -1) &&
			!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, END_TYPE, NO_ATTR) &&
			!tokenEquals(tok, THEN_TYPE, NO_ATTR) &&
			!tokenEquals(tok, DO_TYPE, NO_ATTR) &&
			!tokenEquals(tok, ELSE_TYPE, NO_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEBRACKET_ATTR)){
			tok = getToken();
		}
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
	} else if(tokenEquals(tok, MULOP_TYPE, -1) || 
		tokenEquals(tok, ADDOP_TYPE, -1) || 
		tokenEquals(tok, RELOP_TYPE, -1) || 
		tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, CLOSEBRACKET_ATTR) || 
		tokenEquals(tok, DO_TYPE, NO_ATTR) || 
		tokenEquals(tok, ELSE_TYPE, NO_ATTR) || 
		tokenEquals(tok, END_TYPE, NO_ATTR) || 
		tokenEquals(tok, THEN_TYPE, NO_ATTR)){
		return;
	} else{
		printf("factortail error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of (, [, mulop, addop, relop, ), comma, ;, ], do, else, end, then, received %s\n", tokText);
		while(!tokenEquals(tok, MULOP_TYPE, -1) &&
			!tokenEquals(tok, ADDOP_TYPE, -1) &&
			!tokenEquals(tok, RELOP_TYPE, -1) &&
			!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, END_TYPE, NO_ATTR) &&
			!tokenEquals(tok, THEN_TYPE, NO_ATTR) &&
			!tokenEquals(tok, DO_TYPE, NO_ATTR) &&
			!tokenEquals(tok, ELSE_TYPE, NO_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEBRACKET_ATTR)){
			tok = getToken();
		}
	}
}

void sign(){
	if(tokenEquals(tok, CATCHALL_TYPE, PLUS_ATTR)){
		match(CATCHALL_TYPE, PLUS_ATTR);
	} else if (tokenEquals(tok, CATCHALL_TYPE, MINUS_ATTR)){
		match(CATCHALL_TYPE, MINUS_ATTR);
	} else{
		printf("sign error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of +, -, received %s\n", tokText);
		while(!tokenEquals(tok, ID_TYPE, -1) &&
			!tokenEquals(tok, NOT_TYPE, NO_ATTR) &&
			!tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR) &&
			!tokenEquals(tok, INT_TYPE, NO_ATTR) &&
			!tokenEquals(tok, REAL_TYPE, NO_ATTR) &&
			!tokenEquals(tok, LONGREAL_TYPE, NO_ATTR)){
			tok = getToken();
		}
	}
}