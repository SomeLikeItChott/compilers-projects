#ifndef HELPERS
#define HELPERS

#include "string.h"
#include "stdio.h"
#include "stdlib.h"

//pointer to symbol table
extern struct linkedNode *symbolTable;
extern struct symbolNode *eye;
extern struct stackNode *greenStack;

//token file
extern FILE *tokenFile;
//listing file
extern FILE *listingFile;
//source file
extern FILE *srcFile;
//source file, for printing to listing file
extern FILE *srcLineFile;



struct linkedNode {
	char* lexeme;
	int type;
	long attr;
	struct linkedNode *next;
	int varType;
};

//attr has to be long to be able to hold pointers
//machine (or compiler?) is 64-bit
struct token {
	int type;
	long attr;
};

struct stackNode {
	struct symbolNode *greenNode;
	struct stackNode *next;
};

//the following defines and structs are for the symbol table
#define BLUE_COLOR 	1
#define GREEN_COLOR	2

struct symbolNode {
	char* lexeme;
	int varType;
	int color;
	int returnType;
	int numParams;
	struct symbolNode *firstChild;
	struct symbolNode *parentOrPrevSibling;
	struct symbolNode *nextSibling;
};


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

//defining types for type-checking
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
#define UNDECL_TYPE	13
#define PGMPARM_TYPE	14
//I may need to also define types for function parameters?
//TODO

int varTypesAreFPEquivalent(int type1, int type2);
int checkAddGreenNode(char *lexeme, int varType);
int addToGreenNode(char *lexeme, int returnType, int numParams);
int checkAddBlueNode(char *lexeme, int varType);
//void setVarType(int varType, long attr, struct linkedNode *table);
//int getVarType(long attr, struct linkedNode *table);
int getVarType(long attr);
struct symbolNode* getFunctionNode(long attr);
int getReturnType(long attr, int numParams);
void addToGreenStack(struct symbolNode *symNode);
void popGreenStack();
int isNumVarType(int varType);
void getTextFromType(char *text, int type);
void getPlaintext(char *text, int type, long attr);
void getCatchallPlaintext(char *text, long attr);
void getVarTypePlaintext(char *text, int varType);
int tokenEquals(struct token tok, int type, long attr);



#endif