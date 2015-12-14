#include "helpers.h"

struct linkedNode *symbolTable;
struct symbolNode *eye;
struct stackNode *greenStack;

/*
void addToTable(char *lexeme, int type, int attr, struct linkedNode **table){
	struct linkedNode *temp = (struct linkedNode*)malloc(sizeof(struct linkedNode));
	temp->lexeme = malloc(sizeof(lexeme));
	strcpy(temp->lexeme, lexeme);
	temp->type = type;
	temp->attr = attr;
	temp->next = *table;
	*table = temp;
}
*/

int checkAddGreenNode(char *lexeme, int varType){
	printf("adding green node called %s with type %d\n", lexeme, varType);
	struct symbolNode *temp = eye;
	while(temp != NULL){
		if(strcmp(lexeme, temp->lexeme) == 0){
			//you have found a thing with the same lexeme
			return ERR_TYPE;
		}
		temp = temp->parentOrPrevSibling;
	}
	//there are no nodes above eye with the same name as new green node
	//struct symbolNode a = {.lexeme = lexeme, .parentOrPrevSibling = eye, .color = GREEN_COLOR, .varType = varType};
	struct symbolNode *newNode = (struct symbolNode*)malloc(sizeof(struct symbolNode));
	newNode->lexeme = malloc(sizeof(lexeme));
	strcpy(newNode->lexeme, lexeme);
	newNode->varType = varType;
	newNode->parentOrPrevSibling = eye;
	newNode->color = GREEN_COLOR;

	printf("now adding node with lexeme %s\n", newNode->lexeme);

	if(eye != NULL)
		eye->nextSibling = newNode;
	eye = newNode;
	addToGreenStack(newNode);
	printf("eye is now %s\n", eye->lexeme);
	return varType;
}

int checkAddBlueNode(char *lexeme, int varType){
	printf("adding blue node called %s with type %d\n", lexeme, varType);
	printf("the current eye is %s\n", eye->lexeme);
	struct symbolNode *temp = eye;
	while(temp != NULL){
		if(strcmp(lexeme, temp->lexeme) == 0){
			//you have found a thing with the same lexeme
			return ERR_TYPE;
		}
		if(temp->color == GREEN_COLOR)
			break;
		temp = temp->parentOrPrevSibling;
	}
	//there are no nodes between eye and next green node upwards
	//with the same name as new blue node
	//struct symbolNode a = {.lexeme = lexeme, .parentOrPrevSibling = eye, .color = BLUE_COLOR, .varType = varType};
	struct symbolNode *newNode = (struct symbolNode*)malloc(sizeof(struct symbolNode));
	newNode->lexeme = malloc(sizeof(lexeme));
	strcpy(newNode->lexeme, lexeme);
	newNode->varType = varType;
	newNode->parentOrPrevSibling = eye;
	newNode->color = BLUE_COLOR;

	if(eye->color == BLUE_COLOR)
		eye->nextSibling = newNode;
	else
		eye->firstChild = newNode;
	eye = newNode;
	return varType;
}

int getVarType(long attr){
	struct symbolNode *temp = eye;
	struct linkedNode *prgmNode = (struct linkedNode*)attr;
	char *lexeme = prgmNode->lexeme;
	printf("looking for the type of %s\n", lexeme);
	while(temp != NULL){
		if(strcmp(lexeme, temp->lexeme) == 0){
			//you have found a thing with the same lexeme
			printf("the type is %d\n", temp->varType);
			return temp->varType;
		}
		temp = temp->parentOrPrevSibling;
	}
	printf("the type is %d\n", temp->varType);
	return ERR_TYPE;
}


void addToGreenStack(struct symbolNode *symNode){
	struct stackNode nNode = {.greenNode = symNode};
	struct stackNode *newNode = &nNode;
	if(greenStack == NULL)
		greenStack = newNode;
	else{
		newNode->next = greenStack;
		greenStack = newNode;
	}
}

void popGreenStack(){
	eye = (greenStack->greenNode)->parentOrPrevSibling;
	greenStack = greenStack->next;
	printf("eye is now %s\n", eye->lexeme);
}

/*
//is actually unnecessary i think?
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
*/

int isNumVarType(int varType){
	if(varType == INT_TYPE || varType == REAL_TYPE)
		return 1;
	else
		return 0;
}

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

void getVarTypePlaintext(char *text, int varType){
	switch (varType){
		case ERR_TYPE: strcpy(text, "ERR"); break;
		case ERRSTAR_TYPE: strcpy(text, "ERR*"); break;
		case INT_TYPE: strcpy(text, "integer"); break;
		case REAL_TYPE: strcpy(text, "real"); break;
		case BOOL_TYPE: strcpy(text, "boolean"); break;
		case FNAME_TYPE: strcpy(text, "func name"); break;
		case AINT_TYPE: strcpy(text, "int array"); break;
		case AREAL_TYPE: strcpy(text, "real array"); break;
		case FPINT_TYPE: strcpy(text, "FP int"); break;
		case FPREAL_TYPE: strcpy(text, "FP real"); break;
		case PROG_TYPE: strcpy(text, "prgm name"); break;
		case FPAINT_TYPE: strcpy(text, "FP int array"); break;
		case FPAREAL_TYPE: strcpy(text, "FP real array"); break;
		default: strcpy(text, "unknown"); break;
	}
}

/*
#define ERR_TYPE	0
#define ERRSTAR_TYPE	1
//these types are literal types, and luckily their numbering works out!
//INT_TYPE 			2
//REAL_TYPE 		3
#define BOOL_TYPE	4
#define FNAME_TYPE	5
#define AINT_TYPE	6
#define AREAL_TYPE	7
#define FPINT_TYPE	8
#define FPREAL_TYPE	9
//PROG_TYPE			10
#define FPAINT_TYPE	11
#define FPAREAL_TYPE	12
*/

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