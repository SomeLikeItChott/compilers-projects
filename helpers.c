#include "helpers.h"

struct linkedNode *symbolTable;

void setVarType(int varType, long attr, struct linkedNode *table){
	struct linkedNode *temp = table;
	while(temp != NULL){
		if(attr == temp->attr){
			temp->varType = varType;
			return;
		}
		temp = temp->next;
	}
}

int getVarType(long attr, struct linkedNode *table){
	struct linkedNode *temp = table;
	while(temp != NULL){
		if(attr == temp->attr){
			return temp->varType;
		}
		temp = temp->next;
	}
	return -1;
}

/*
struct token isInTable(char *string, struct linkedNode *table){
	struct linkedNode *temp = table;
	while(temp != NULL){
		if(strcmp(string, temp->lexeme) == 0){
			struct token tableItem;
			tableItem.type = temp->type;
			tableItem.attr = temp->attr;
			return tableItem;
		}
		temp = temp->next;
	}
	struct token a = {.type = -1, .attr = -1};
	return a;
}
*/

void getTextFromType(char *text, int type){
	switch (type){
		case WS_TYPE: strcpy(text, "(WS)"); break;
		case ID_TYPE: strcpy(text, "(ID)"); break;
		case INT_TYPE: strcpy(text, "(INT)"); break;
		case REAL_TYPE: strcpy(text, "(REAL)"); break;
		case LONGREAL_TYPE: strcpy(text, "(LREAL)"); break;
		case RELOP_TYPE: strcpy(text, "(RELOP)"); break;
		case ADDOP_TYPE: strcpy(text, "(ADDOP)"); break;
		case MULOP_TYPE: strcpy(text, "(MULOP)"); break;
		case ASSIGNOP_TYPE: strcpy(text, "(ASGNOP)"); break;
		case CATCHALL_TYPE: strcpy(text, "(CTCHAL)"); break;
		case PROG_TYPE: strcpy(text, "(PROGRM)"); break;
		case FUNC_TYPE: strcpy(text, "(FUNCTN)"); break;
		case PROC_TYPE: strcpy(text, "(PROC)"); break;
		case ARRAY_TYPE: strcpy(text, "(ARRAY)"); break;
		case OF_TYPE: strcpy(text, "(OF)"); break;
		case BEGIN_TYPE: strcpy(text, "(BEGIN)"); break;
		case END_TYPE: strcpy(text, "(END)"); break;
		case IF_TYPE: strcpy(text, "(IF)"); break;
		case THEN_TYPE: strcpy(text, "(THEN)"); break;
		case ELSE_TYPE: strcpy(text, "(ELSE)"); break;
		case WHILE_TYPE: strcpy(text, "(WHILE)"); break;
		case DO_TYPE: strcpy(text, "(DO)"); break;
		case VAR_TYPE: strcpy(text, "(VAR)"); break;
		case INTWORD_TYPE: strcpy(text, "(INTWRD)"); break;
		case REALWORD_TYPE: strcpy(text, "(REALWD)"); break;
		case NOT_TYPE: strcpy(text, "(NOT)"); break;
		case EOF_TYPE: strcpy(text, "(EOF)"); break;
		case LEXERR_TYPE: strcpy(text, "(LEXERR)"); break;
		default: strcpy(text, "(UNKNWN)"); break;
	}
}

void getPlaintext(char *text, int type, long attr){
	switch (type){
		case WS_TYPE: strcpy(text, "whitespace"); break;
		case ID_TYPE: strcpy(text, "identifier"); break;
		case INT_TYPE: strcpy(text, "int (num)"); break;
		case REAL_TYPE: strcpy(text, "real (num)"); break;
		case LONGREAL_TYPE: strcpy(text, "longreal"); break;
		case RELOP_TYPE: strcpy(text, "relop"); break;
		case ADDOP_TYPE: strcpy(text, "addop"); break;
		case MULOP_TYPE: strcpy(text, "mulop"); break;
		case ASSIGNOP_TYPE: strcpy(text, ":="); break;
		case CATCHALL_TYPE: getCatchallPlaintext(text, attr); break;
		case PROG_TYPE: strcpy(text, "program"); break;
		case FUNC_TYPE: strcpy(text, "function"); break;
		case PROC_TYPE: strcpy(text, "procedure"); break;
		case ARRAY_TYPE: strcpy(text, "array"); break;
		case OF_TYPE: strcpy(text, "of"); break;
		case BEGIN_TYPE: strcpy(text, "begin"); break;
		case END_TYPE: strcpy(text, "end"); break;
		case IF_TYPE: strcpy(text, "if"); break;
		case THEN_TYPE: strcpy(text, "then"); break;
		case ELSE_TYPE: strcpy(text, "else"); break;
		case WHILE_TYPE: strcpy(text, "while"); break;
		case DO_TYPE: strcpy(text, "do"); break;
		case VAR_TYPE: strcpy(text, "var"); break;
		case INTWORD_TYPE: strcpy(text, "int (word)"); break;
		case REALWORD_TYPE: strcpy(text, "real (word)"); break;
		case NOT_TYPE: strcpy(text, "not"); break;
		case EOF_TYPE: strcpy(text, "EOF"); break;
		case LEXERR_TYPE: strcpy(text, "LEXERR"); break;
		default: strcpy(text, "(UNKNWN)"); break;
	}
}

void getCatchallPlaintext(char *text, long attr){
	switch (attr){
		case COLON_ATTR: strcpy(text, ":"); break;
		case SEMICOLON_ATTR: strcpy(text, ";"); break;
		case OPENBRACKET_ATTR: strcpy(text, "["); break;
		case CLOSEBRACKET_ATTR: strcpy(text, "]"); break;
		case OPENPAREN_ATTR: strcpy(text, "("); break;
		case CLOSEPAREN_ATTR: strcpy(text, ")"); break;
		case OPENCURLYBRACE_ATTR: strcpy(text, "{"); break;
		case CLOSECURLYBRACE_ATTR: strcpy(text, "}"); break;
		case COMMA_ATTR: strcpy(text, ","); break;
		case PERIOD_ATTR: strcpy(text, "."); break;
		default: strcpy(text, "(UNKNWN)"); break;
	}
}

int tokenEquals(struct token toke, int type, long attr){
	if(attr == -1){
		if(toke.type == type)
			return 1;
		else
			return 0;
	}else{
		if(toke.type == type && toke.attr == attr)
			return 1;
		else
			return 0;
	}
}