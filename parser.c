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
		struct linkedNode *prgmNode = (struct linkedNode*)tok.attr;
		match(ID_TYPE, -1);
		printf("the lex of the program is %s\n", prgmNode->lexeme);
		checkAddGreenNode(prgmNode->lexeme, PROG_TYPE);
		printf("eye is now %s\n", eye->lexeme);
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
		struct linkedNode *idNode = (struct linkedNode*)tok.attr;
		checkAddBlueNode(idNode->lexeme, PGMPARM_TYPE);
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
		struct linkedNode *idNode = (struct linkedNode*)tok.attr;
		checkAddBlueNode(idNode->lexeme, PGMPARM_TYPE);
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
		struct linkedNode *idNode = (struct linkedNode*)tok.attr;
		match(ID_TYPE, -1);
		match(CATCHALL_TYPE, COLON_ATTR);
		int varType = type();
		match(CATCHALL_TYPE, SEMICOLON_ATTR);
		checkAddBlueNode(idNode->lexeme, varType);
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
		struct linkedNode *idNode = (struct linkedNode*)tok.attr;
		match(ID_TYPE, -1);
		match(CATCHALL_TYPE, COLON_ATTR);
		int varType = type();
		match(CATCHALL_TYPE, SEMICOLON_ATTR);
		checkAddBlueNode(idNode->lexeme, varType);
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

int type(){
	if(tokenEquals(tok, ARRAY_TYPE, NO_ATTR)){
		match(ARRAY_TYPE, NO_ATTR);
		match(CATCHALL_TYPE, OPENBRACKET_ATTR);
		match(INT_TYPE, NO_ATTR);
		match(CATCHALL_TYPE, PERIOD_ATTR);
		match(CATCHALL_TYPE, PERIOD_ATTR);
		match(INT_TYPE, NO_ATTR);
		match(CATCHALL_TYPE, CLOSEBRACKET_ATTR);
		match(OF_TYPE, NO_ATTR);
		int ofType = stndtype();
		if(ofType == INT_TYPE){
			return AINT_TYPE;
		} else if(ofType == REAL_TYPE){
			return AREAL_TYPE;
		} else{
			return ERR_TYPE;
		}
	} else if(tokenEquals(tok, REALWORD_TYPE, NO_ATTR) || tokenEquals(tok, INTWORD_TYPE, NO_ATTR)){
		return stndtype();
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
		return ERR_TYPE;
	}
}

int stndtype(){
	if(tokenEquals(tok, REALWORD_TYPE, NO_ATTR)){
		match(REALWORD_TYPE, NO_ATTR);
		return REAL_TYPE;
	} else if(tokenEquals(tok, INTWORD_TYPE, NO_ATTR)){
		match(INTWORD_TYPE, NO_ATTR);
		return INT_TYPE;
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
		return ERR_TYPE;
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
		struct linkedNode *prgmNode = (struct linkedNode*)tok.attr;
		printf("the lex of the function is %s\n", prgmNode->lexeme);
		match(ID_TYPE, -1);
		subprgmheadtail(prgmNode->lexeme);
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


void subprgmheadtail(char *lexeme){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR)){
		checkAddGreenNode(lexeme, FNAME_TYPE);
		int numArgs = args();
		match(CATCHALL_TYPE, COLON_ATTR);
		int returnType = stndtype();
		addToGreenNode(lexeme, returnType, numArgs);
		//checkAddGreenNode(lexeme, FNAME_TYPE, returnType, numArgs);
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

int args(){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR)){
		match(CATCHALL_TYPE, OPENPAREN_ATTR);
		int numParams = paramlist();
		match(CATCHALL_TYPE, CLOSEPAREN_ATTR);
		return numParams;
	}else{
		printf("args error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting (, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, COLON_ATTR)){
			tok = getToken();
		}
		return 0;
	}
}

int paramlist(){
	if(tokenEquals(tok, ID_TYPE, -1)){
		struct linkedNode *idNode = (struct linkedNode*)tok.attr;
		match(ID_TYPE, -1);
		match(CATCHALL_TYPE, COLON_ATTR);
		int varType = type();
		if(varType == INT_TYPE)
			varType = FPINT_TYPE;
		else if(varType == REAL_TYPE)
			varType = FPREAL_TYPE;
		else if(varType == AINT_TYPE)
			varType = FPAINT_TYPE;
		else if(varType == AREAL_TYPE)
			varType = FPAREAL_TYPE;
		else
			varType = ERR_TYPE;
		checkAddBlueNode(idNode->lexeme, varType);
		int numParams = paramlisttail();
		return numParams + 1;
	} else{
		printf("paramlist error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting identifier, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR)){
			tok = getToken();
		}
		return 0;
	}
}

int paramlisttail(){
	if(tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR)){
		return 0;
	} else if(tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR)){
		match(CATCHALL_TYPE, SEMICOLON_ATTR);
		struct linkedNode *idNode = (struct linkedNode*)tok.attr;
		match(ID_TYPE, -1);
		match(CATCHALL_TYPE, COLON_ATTR);
		int varType = type();
		if(varType == INT_TYPE)
			varType = FPINT_TYPE;
		else if(varType == REAL_TYPE)
			varType = FPREAL_TYPE;
		else if(varType == AINT_TYPE)
			varType = FPAINT_TYPE;
		else if(varType == AREAL_TYPE)
			varType = FPAREAL_TYPE;
		else
			varType = ERR_TYPE;
		checkAddBlueNode(idNode->lexeme, varType);
		int numParams = paramlisttail();
		return numParams + 1;
	} else{
		printf("paramlisttail error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of ), ;, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR)){
			tok = getToken();
		}
		return 0;
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
		popGreenStack();
	} else if (tokenEquals(tok, END_TYPE, NO_ATTR)){
		match(END_TYPE, NO_ATTR);
		popGreenStack();
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
		long attr = tok.attr;
		int varType = variable();
		match(ASSIGNOP_TYPE, NO_ATTR);
		int exprType = expr();
		if(varType == FNAME_TYPE){
			varType = getReturnType(attr, -2);
		}
		if(varTypesAreFPEquivalent(varType, exprType)){
			printf("assignop ok\n");
		} else{
			printf("BAD ASSIGNOP\n");
			char varText[15];
			getVarTypePlaintext(varText, varType);
			char exprText[15];
			getVarTypePlaintext(exprText, exprType);
			fprintf(listingFile, "SEMERR: Mismatched types on assignop statement, you have %s and %s\n", varText, exprText);
		}
	} else if(tokenEquals(tok, IF_TYPE, NO_ATTR)){
		match(IF_TYPE, NO_ATTR);
		int exprType = expr();
		match(THEN_TYPE, NO_ATTR);
		stmt();
		stmttail();
		if(exprType == BOOL_TYPE){
			printf("if statement has bool\n");
		} else{
			printf("BAD IF STATEMENT\n");
			char exprText[15];
			getVarTypePlaintext(exprText, exprType);
			fprintf(listingFile, "SEMERR: Nonboolean on if statement, it is %s\n", exprText);
		}
	} else if(tokenEquals(tok, WHILE_TYPE, NO_ATTR)){
		match(WHILE_TYPE, NO_ATTR);
		int exprType = expr();
		match(DO_TYPE, NO_ATTR);
		stmt();
		if(exprType == BOOL_TYPE){
			printf("while statement has bool\n");
		} else{
			printf("BAD IF STATEMENT");
			char exprText[15];
			getVarTypePlaintext(exprText, exprType);
			fprintf(listingFile, "SEMERR: Nonboolean on while statement, it is %s\n", exprText);
		}
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

int variable(){
	if(tokenEquals(tok, ID_TYPE, -1)){
		int varType = getVarType(tok.attr);
		match(ID_TYPE, -1);
		return variabletail(varType);
	} else{
		printf("variable error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting identifier, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, ASSIGNOP_TYPE, NO_ATTR)){
			tok = getToken();
		}
		return ERR_TYPE;
	}
}

int variabletail(int inheritedType){
	if(tokenEquals(tok, ASSIGNOP_TYPE, NO_ATTR)){
		return inheritedType;
	} else if(tokenEquals(tok, CATCHALL_TYPE, OPENBRACKET_ATTR)){
		match(CATCHALL_TYPE, OPENBRACKET_ATTR);
		int exprType = expr();
		match(CATCHALL_TYPE, CLOSEBRACKET_ATTR);
		if(exprType == INT_TYPE){
			if(inheritedType == AINT_TYPE){
				return INT_TYPE;
			} else if (inheritedType == AREAL_TYPE){
				return REAL_TYPE;
			} else{
				return ERR_TYPE;
			}
		} else{
			return ERR_TYPE;
		}
	} else{
		printf("variabletail error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of :=, [, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, ASSIGNOP_TYPE, NO_ATTR)){
			tok = getToken();
		}
		return ERR_TYPE;
	}
}

int exprlist(struct symbolNode *childNode){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR) ||
		tokenEquals(tok, ADDOP_TYPE, PLUS_ATTR) ||
		tokenEquals(tok, ADDOP_TYPE, MINUS_ATTR) ||
		tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) ||
		tokenEquals(tok, ID_TYPE, -1) ||
		tokenEquals(tok, NOT_TYPE, NO_ATTR) ||
		tokenEquals(tok, INT_TYPE, NO_ATTR) ||
		tokenEquals(tok, REAL_TYPE, NO_ATTR) ||
		tokenEquals(tok, LONGREAL_TYPE, NO_ATTR)){
		int exprVarType = expr();
		//TODO give more specific error message?
		if(!(childNode->varType == FPINT_TYPE || 
			childNode->varType == FPREAL_TYPE || 
			childNode->varType == FPAINT_TYPE || 
			childNode->varType == FPAREAL_TYPE)){
			fprintf(listingFile, "SEMERR: Too many parameters in function call\n" );
		}
		else if(!varTypesAreFPEquivalent(childNode->varType, exprVarType)){
			fprintf(listingFile, "SEMERR: Actual parameter type doesn't match formal parameter type\n" );
		}
		int numExprs = exprlisttail(childNode->nextSibling);
		return numExprs + 1;
	} else{
		printf("exprlist error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of num, (, not, identifier, +, -, received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR)){
			tok = getToken();
		}
		return 0;
	}
}

int exprlisttail(struct symbolNode *childNode){
	if(tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR)){
		return 0;
	} else if (tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR)) {
		match(CATCHALL_TYPE, COMMA_ATTR);
		int exprVarType = expr();
		//TODO give more specific error message?
		if(!(childNode->varType == FPINT_TYPE || 
			childNode->varType == FPREAL_TYPE || 
			childNode->varType == FPAINT_TYPE || 
			childNode->varType == FPAREAL_TYPE)){
			fprintf(listingFile, "SEMERR: Too many parameters in function call\n" );
		}
		else if(!varTypesAreFPEquivalent(childNode->varType, exprVarType)){
			fprintf(listingFile, "SEMERR: Actual parameter type doesn't match formal parameter type\n" );
		}
		int numExprs = exprlisttail(childNode->nextSibling);
		//TODO this might result in a segfault, if there's no more nextSibling?
		return numExprs + 1;
	} else{
		printf("exprlisttail error\n");
		char tokText[12];
		getPlaintext(tokText, tok.type, tok.attr);
		fprintf(listingFile, "SYNERR: Expecting one of comma, ), received %s\n", tokText);
		while(!tokenEquals(tok, EOF_TYPE, NO_ATTR) && 
			!tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR)){
			tok = getToken();
		}
		return 0;
	}
}

int expr(){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR) || 
		tokenEquals(tok, ADDOP_TYPE, PLUS_ATTR) || 
		tokenEquals(tok, ADDOP_TYPE, MINUS_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) || 
		tokenEquals(tok, ID_TYPE, -1) || 
		tokenEquals(tok, NOT_TYPE, NO_ATTR) || 
		tokenEquals(tok, INT_TYPE, NO_ATTR) || 
		tokenEquals(tok, REAL_TYPE, NO_ATTR)|| 
		tokenEquals(tok, LONGREAL_TYPE, NO_ATTR)){
		int smplexprType = smplexpr();
		return exprtail(smplexprType);
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
		return ERR_TYPE;
	}
}

int exprtail(int inheritedType){
	if(tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, CLOSEBRACKET_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) ||
		tokenEquals(tok, DO_TYPE, NO_ATTR) || 
		tokenEquals(tok, ELSE_TYPE, NO_ATTR) || 
		tokenEquals(tok, END_TYPE, NO_ATTR) || 
		tokenEquals(tok, THEN_TYPE, NO_ATTR)){
		return inheritedType;
	} else if (tokenEquals(tok, RELOP_TYPE, -1)){
		match(RELOP_TYPE, -1);
		int smplexprType = smplexpr(inheritedType);
		if(isNumVarType(smplexprType) && isNumVarType(inheritedType)){
			return BOOL_TYPE;
		} else if(smplexprType == inheritedType){
			return BOOL_TYPE;
		} else {
			return ERR_TYPE;
		}
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
		return ERR_TYPE;
	}
}

int smplexpr(){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR) || 
		tokenEquals(tok, ID_TYPE, -1) || 
		tokenEquals(tok, NOT_TYPE, NO_ATTR) || 
		tokenEquals(tok, INT_TYPE, NO_ATTR) || 
		tokenEquals(tok, REAL_TYPE, NO_ATTR) || 
		tokenEquals(tok, LONGREAL_TYPE, NO_ATTR)){
		int termType = term();
		return smplexprtail(termType);
	} else if (tokenEquals(tok, ADDOP_TYPE, MINUS_ATTR) || tokenEquals(tok, ADDOP_TYPE, PLUS_ATTR)){
		sign();
		int termType = term();
		return smplexprtail(termType);
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
		return ERR_TYPE;
	}
}

int smplexprtail(int inheritedType){
	if(tokenEquals(tok, RELOP_TYPE, -1) || 
		tokenEquals(tok, CATCHALL_TYPE, CLOSEPAREN_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, COMMA_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, SEMICOLON_ATTR) || 
		tokenEquals(tok, CATCHALL_TYPE, CLOSEBRACKET_ATTR) || 
		tokenEquals(tok, DO_TYPE, NO_ATTR) || 
		tokenEquals(tok, ELSE_TYPE, NO_ATTR) || 
		tokenEquals(tok, END_TYPE, NO_ATTR) || 
		tokenEquals(tok, THEN_TYPE, NO_ATTR)){
		return inheritedType;
	} else if(tokenEquals(tok, ADDOP_TYPE, -1)){
		long addopAttr = tok.attr;
		match(ADDOP_TYPE, -1);
		int termType = term();
		int smplexprtailType;
		if(addopAttr == PLUS_ATTR || addopAttr == MINUS_ATTR){
			if(inheritedType == INT_TYPE && termType == INT_TYPE)
				smplexprtailType = INT_TYPE;
			else if (isNumVarType(inheritedType) && isNumVarType(termType))
				smplexprtailType = REAL_TYPE;
			else
				smplexprtailType = ERR_TYPE;
		} else if (addopAttr == OR_ATTR){
			if(inheritedType == BOOL_TYPE && termType == BOOL_TYPE)
				smplexprtailType = BOOL_TYPE;
			else
				smplexprtailType = ERR_TYPE;
		}
		return smplexprtail(smplexprtailType);
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
		return ERR_TYPE;
	}
}

int term(){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR) || 
		tokenEquals(tok, ID_TYPE, -1) || 
		tokenEquals(tok, NOT_TYPE, NO_ATTR) || 
		tokenEquals(tok, INT_TYPE, NO_ATTR) || 
		tokenEquals(tok, REAL_TYPE, NO_ATTR) || 
		tokenEquals(tok, LONGREAL_TYPE, NO_ATTR)){
		int termInh = factor();
		return termtail(termInh);
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
		return ERR_TYPE;
	}
}

int termtail(int inheritedType){
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
		return inheritedType;
	} else if(tokenEquals(tok, MULOP_TYPE, -1)){
		long mulopAttr = tok.attr;
		match(MULOP_TYPE, -1);
		int factorType = factor();
		int termtailType;
		if(mulopAttr == TIMES_ATTR || mulopAttr == MOD_ATTR){
			//if(inheritedType == INT_TYPE && factorType == INT_TYPE)
			if(varTypesAreFPEquivalent(inheritedType, factorType) == INT_TYPE)
				termtailType = INT_TYPE;
			else
				termtailType = ERR_TYPE;
		} else if(mulopAttr == DIVIDE_ATTR){
			if (isNumVarType(inheritedType) && isNumVarType(factorType))
				termtailType = REAL_TYPE;
			else
				termtailType = ERR_TYPE;
		} else if(mulopAttr == DIV_ATTR){
			if (isNumVarType(inheritedType) && isNumVarType(factorType))
				termtailType = INT_TYPE;
			else
				termtailType = ERR_TYPE;
		} else if(mulopAttr == AND_ATTR){
			if(inheritedType == BOOL_TYPE && factorType == BOOL_TYPE)
				termtailType = BOOL_TYPE;
			else
				termtailType = ERR_TYPE;
		} else{
			termtailType = ERR_TYPE;
		}
		return termtail(termtailType);
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
		return ERR_TYPE;
	}
}

int factor(){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR)){
		match(CATCHALL_TYPE, OPENPAREN_ATTR);
		int exprType = expr();
		match(CATCHALL_TYPE, CLOSEPAREN_ATTR);
		return exprType;
	} else if(tokenEquals(tok, ID_TYPE, -1)){
		int fin = getVarType(tok.attr);
		long attr = tok.attr;
		match(ID_TYPE, -1);
		return factortail(fin, attr);
	} else if(tokenEquals(tok, NOT_TYPE, NO_ATTR)){
		match(NOT_TYPE, NO_ATTR);
		int ftype = factor();
		if(ftype == BOOL_TYPE)
			return BOOL_TYPE;
		else
			return ERR_TYPE;
	} else if(tokenEquals(tok, INT_TYPE, NO_ATTR)){
		match(INT_TYPE, NO_ATTR);
		return INT_TYPE;
	} else if(tokenEquals(tok, REAL_TYPE, NO_ATTR) || tokenEquals(tok, LONGREAL_TYPE, NO_ATTR)){
		match(REAL_TYPE, NO_ATTR);
		return REAL_TYPE;
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
		return ERR_TYPE;
	}
}

int factortail(int inheritedType, long attr){
	if(tokenEquals(tok, CATCHALL_TYPE, OPENPAREN_ATTR)){
		match(CATCHALL_TYPE, OPENPAREN_ATTR);
		struct symbolNode *funcNode = getFunctionNode(attr);
		struct symbolNode *childNode;
		if(funcNode->varType != UNDECL_TYPE) {
			if(funcNode->firstChild != NULL){
				childNode = funcNode->firstChild;
				printf("the first child is %s\n", childNode->lexeme);
			}else{
				printf("TODO also here\n");
			}
		} else {
			//there is no function called that! might as well abort, i guess
			struct linkedNode *prgmNode = (struct linkedNode*)attr;
			char *lexeme = prgmNode->lexeme;
			//fprintf(listingFile, "SEMERR: Function %s is not declared in this scope\n", lexeme);
			return ERR_TYPE;
		}
		int numParams = exprlist(childNode);
		match(CATCHALL_TYPE, CLOSEPAREN_ATTR);
		return getReturnType(attr, numParams);
	} else if(tokenEquals(tok, CATCHALL_TYPE, OPENBRACKET_ATTR)){
		match(CATCHALL_TYPE, OPENBRACKET_ATTR);
		int exprType = expr();
		match(CATCHALL_TYPE, CLOSEBRACKET_ATTR);
		// is this right? it has aint but no areal
		//TODO
		if(exprType == INT_TYPE)
			return AINT_TYPE;
		else if(exprType == INT_TYPE){
			if(inheritedType == AINT_TYPE)
				return INT_TYPE;
			else if(inheritedType == REAL_TYPE)
				return REAL_TYPE;
			else
				return ERR_TYPE;
		} else if (exprType == ERR_TYPE || inheritedType == ERR_TYPE)
			return ERR_TYPE;
		else
			return ERR_TYPE;
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
		return inheritedType;
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
		return ERR_TYPE;
	}
}

void sign(){
	if(tokenEquals(tok, ADDOP_TYPE, PLUS_ATTR)){
		match(ADDOP_TYPE, PLUS_ATTR);
	} else if (tokenEquals(tok, ADDOP_TYPE, MINUS_ATTR)){
		match(ADDOP_TYPE, MINUS_ATTR);
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