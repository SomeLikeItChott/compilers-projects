#ifndef LEXICAL_ANALYZER
#define LEXICAL_ANALYZER

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "lex_errors.h"
#include "lex_machines.h"
#include "helpers.h"

//how big the buffer is, for checking if we've moved off of it
#define BUFFER_SIZE	80

//pointer to reserved word table
extern struct linkedNode *reservedTable;

//token file
extern FILE *tokenFile;
//listing file
extern FILE *listingFile;
//source file
extern FILE *srcFile;
//source file, for printing to listing file
extern FILE *srcLineFile;

//buffer that holds chars that make the tokens
extern char buffer[];
//forward pointer
extern char *forw;
//back pointer
extern char *back;


//line num, for printing listing file
extern int lineNum;

struct token getToken();
char nextChar();
struct token generateToken(int tokenType, int tokenAttr, char *attrName);
void retract(int num);
void readInReserved();
struct token isInTable(char *string, struct linkedNode *table);
void addToTable(char *lexeme, int type, int attr, struct linkedNode **table);
void openFiles(char *tokenName, char *listingName, char *fileName);
void closeFiles();
void getTextFromType(char *text, int type);
void printLexErr(int attr, char *lexeme);
void initLex();
char* getLexerrAttrName(int attr);
char* getReservedAttrName(int type, int attr, char *attrName);

#endif