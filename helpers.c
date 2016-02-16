#include "helpers.h"

struct linkedNode *symbolTable;
struct symbolNode *eye;
struct stackNode *greenStack;

FILE *tokenFile;
FILE *listingFile;
FILE *srcFile;
FILE *srcLineFile;
FILE *addressesFile;

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

int varTypesAreFPEquivalent(int type1, int type2){
	if((type1 == INT_TYPE || type1 == FPINT_TYPE) && (type2 == INT_TYPE || type2 == FPINT_TYPE))
		return INT_TYPE;
	else if((type1 == REAL_TYPE || type1 == FPREAL_TYPE) && (type2 == REAL_TYPE || type2 == FPREAL_TYPE))
		return REAL_TYPE;
	else if((type1 == AINT_TYPE || type1 == FPAINT_TYPE) && (type2 == AINT_TYPE || type2 == FPAINT_TYPE))
		return AINT_TYPE;
	else if((type1 == AREAL_TYPE || type1 == FPAREAL_TYPE) && (type2 == AREAL_TYPE || type2 == FPAREAL_TYPE))
		return AREAL_TYPE;
	else
		return 0;
}

int checkAddGreenNode(char *lexeme, int varType){
	printf("adding green node called %s with type %d\n", lexeme, varType);
	struct symbolNode *temp = eye;
	while(temp != NULL){
		if(strcmp(lexeme, temp->lexeme) == 0){
			//you have found a thing with the same lexeme
			fprintf(listingFile, "SEMERR: Variable %s already exists in this scope\n", lexeme);
			return -1;
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

	fprintf(addressesFile, "\n%s:\n", lexeme);

	printf("now adding node with lexeme %s\n", newNode->lexeme);
	printf("address of new node is %p\n", newNode);

	if(eye != NULL)
		eye->nextSibling = newNode;
	eye = newNode;
	addToGreenStack(newNode);
	printf("eye is now %s\n", eye->lexeme);
	return varType;
}

int addToGreenNode(char *lexeme, int returnType, int numParams){
	printf("adding returnType %d and numParams %d to greenNode %s\n", returnType, numParams, lexeme);
	struct symbolNode *temp = eye;
	while(temp != NULL){
		if(strcmp(lexeme, temp->lexeme) == 0){
			//you have found a thing with the same lexeme
			temp->returnType = returnType;
			temp->numParams = numParams;
			return returnType;
		}
		temp = temp->parentOrPrevSibling;
	}
	//there are no nodes above eye with the same name as new green node
	//struct symbolNode a = {.lexeme = lexeme, .parentOrPrevSibling = eye, .color = GREEN_COLOR, .varType = varType};
	printf("couldn't find greennode\n");
	return ERR_TYPE;
}

int checkAddBlueNode(char *lexeme, int varType){
	printf("the current eye is %s\n", eye->lexeme);
	printf("adding blue node called %s with type %d\n", lexeme, varType);
	struct symbolNode *temp = eye;
	while(temp != NULL){
		if(strcmp(lexeme, temp->lexeme) == 0){
			//you have found a thing with the same lexeme
			fprintf(listingFile, "SEMERR: Variable %s already exists in this scope\n", lexeme);
			return -1;
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
		printf("%s\n", temp->lexeme);
		if(strcmp(lexeme, temp->lexeme) == 0){
			//you have found a thing with the same lexeme
			printf("the type is %d\n", temp->varType);
			return temp->varType;
		}
		temp = temp->parentOrPrevSibling;
	}
	printf("the type is %d\n", UNDECL_TYPE);
	fprintf(listingFile, "SEMERR: Variable %s is undeclared in this scope\n", lexeme);
	return UNDECL_TYPE;
}

struct symbolNode* getFunctionNode(long attr){
	struct symbolNode *temp = eye;
	struct linkedNode *prgmNode = (struct linkedNode*)attr;
	char *lexeme = prgmNode->lexeme;
	printf("lexeme of the function about to be called is %s\n", lexeme);
	while(temp != NULL){
		printf("%s\n", temp->lexeme);
		if(strcmp(lexeme, temp->lexeme) == 0){
			//you have found a thing with the same lexeme
			return temp;
		}
		temp = temp->parentOrPrevSibling;
	}
	fprintf(listingFile, "SEMERR: Function %s is undeclared in this scope\n", lexeme);
	struct symbolNode *newNode = (struct symbolNode*)malloc(sizeof(struct symbolNode));
	newNode->varType = UNDECL_TYPE;
	return newNode;
}

int getReturnType(long attr, int numParams){
	struct symbolNode *temp = eye;
	struct linkedNode *prgmNode = (struct linkedNode*)attr;
	char *lexeme = prgmNode->lexeme;
	printf("looking for the returnType of %s\n", lexeme);
	while(temp != NULL){
		printf("%s\n", temp->lexeme);
		if(strcmp(lexeme, temp->lexeme) == 0){
			if(temp->varType == FNAME_TYPE){
			//you have found a thing with the same lexeme
				if(numParams == temp->numParams || numParams == -2){
					printf("the returnType is %d\n", temp->returnType);
					return temp->returnType;
				}else{
					printf("wrong # of params\n");
					fprintf(listingFile, "SEMERR: Function %s is being called with the wrong number of params \n", lexeme);
					return ERR_TYPE;
				}
			} else {
				printf("OH SHIT YOU TRIED TO CALL GETRETURNTYPE ON A NONFUNCTION\n");
				return ERR_TYPE;
			}
		}
		temp = temp->parentOrPrevSibling;
	}
	printf("the returnType is %d\n", UNDECL_TYPE);
	//fprintf(listingFile, "SEMERR: Variable %s is undeclared in this scope\n", lexeme);
	return UNDECL_TYPE;
}


void addToGreenStack(struct symbolNode *symNode){
	//struct stackNode nNode = {.greenNode = symNode};
	//struct stackNode *newNode = &nNode;

	struct stackNode *newNode = (struct stackNode*)malloc(sizeof(struct stackNode));
	newNode->greenNode = symNode;

	if(greenStack != NULL)
		newNode->next = greenStack;

	greenStack = newNode;

	printf("address of thing is %p\n", greenStack);
	printf("address of thing is %p\n", greenStack->greenNode);

}

void popGreenStack(){
	printf("popping green stack\n");
	if(greenStack->next != NULL){
		eye = greenStack->greenNode;
		printf("green stack is now pointing to %s\n", (greenStack->greenNode)->lexeme);
		greenStack = greenStack->next;
	} else{
		printf("just made no changes to green stack\n");
	} 
	printf("eye is now %s\n", eye->lexeme);
}

int isNumVarType(int varType){
	if(varType == INT_TYPE || varType == REAL_TYPE || varType == FPINT_TYPE || varType == FPREAL_TYPE)
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
		case UNDECL_TYPE: strcpy(text, "undeclared"); break;
		case PGMPARM_TYPE: strcpy(text, "prgm param"); break;
		default: strcpy(text, "unknown"); break;
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