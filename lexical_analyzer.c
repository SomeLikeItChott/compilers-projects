#include "lexical_analyzer.h"

struct linkedNode *reservedTable;
struct linkedNode *symbolTable;
FILE *tokenFile;
FILE *listingFile;
FILE *srcFile;
FILE *srcLineFile;
char buffer[BUFFER_SIZE];
char *forw;
char *back;
int lineNum = 1;

struct token getToken(){
	if(forw == NULL)
		initLex();

	struct token t;
	int state = 0;
	while(1){
		switch(state){
			case 0: t = wsMachine(); break;
			case 1: t = idMachine(); break;
			case 2: t = longrealMachine(); break;
			case 3: t = realMachine(); break;
			case 4: t = intMachine(); break;
			case 5: t = relopMachine(); break;
			case 6: t = catchallMachine(); break;
			default: 
			printf("%s\n", "fatal error: fell through to the bottom of getToken");
			exit(1);
		}
		if (!tokenEquals(t, -1, -1))
			return t;
		else
			state++;
	}
}

void initLex(){
	char c;
	int bufferIndex = 0;
	do{
		c = fgetc(srcFile);
		buffer[bufferIndex++] = c;
	}while(c != EOF && c != '\n');
	forw = &buffer[0] - sizeof(char);
	back = &buffer[0] - sizeof(char);
}

void openFiles(char *tokenName, char *listingName, char *fileName){
	tokenFile = fopen(tokenName, "w");
	if(tokenFile == NULL){
		printf("Cannot open token file, exiting\n");
		exit(1);
	}
	fprintf(tokenFile, "Line# Lexeme      Token Type  Attribute\n");
	listingFile = fopen(listingName, "w");
	if(listingFile == NULL){
		printf("Cannot open listing file, exiting\n");
		exit(1);
	}
	srcFile = fopen(fileName, "r");
	if(srcFile == NULL){
		printf("Cannot open source file, exiting\n");
		exit(1);
	}
	srcLineFile = fopen(fileName, "r");
	if(srcFile == NULL){
		printf("Cannot open source file, exiting\n");
		exit(1);
	}
	char buffer[74];
	fgets(buffer, 73, srcLineFile);
	fprintf(listingFile, "%-8d%s", 1, buffer);
}

void closeFiles(){
	fclose(tokenFile);
	fclose(listingFile);
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

char nextChar(){
	forw += sizeof(char);
	return *forw;
}

void retract(int num){
	forw -= sizeof(char) * num;
}

struct token generateToken(int tokenType, int tokenAttr, char *attrName){
	//gets lexeme, returns token
	int lexemeLength = (forw - back) / sizeof(char);
	char lexeme[lexemeLength + 1];
	strncpy(lexeme, back + sizeof(char), lexemeLength * sizeof(char));
	//strncpy doesn't want to add null terminal, do it manually
	lexeme[lexemeLength] = '\0';
	back = forw;
	struct token a = {.type = tokenType, .attr = tokenAttr};
	switch (tokenType){
		case ID_TYPE: a = checkidErrors(lexeme); break;
		case INT_TYPE: a = checkintErrors(lexeme); break;
		case REAL_TYPE: a = checkrealErrors(lexeme); break;
		case LONGREAL_TYPE: a = checklongrealErrors(lexeme); break;
	}
	if(a.type == LEXERR_TYPE){
		printLexErr(a.attr, lexeme);
		attrName = getLexerrAttrName(a.attr);
	}else if(a.type == ADDOP_TYPE || a.type == MULOP_TYPE){
		//check if its a reserved word thing
		attrName = getReservedAttrName(a.type, a.attr, attrName);
	}
	if(a.type != WS_TYPE){
		char typeText[10];
		getTextFromType(typeText,a.type);
		//reserved words don't get attrs
		if(a.type < 50 && a.type > 9)
			attrName = "(NULL)";

		fprintf(tokenFile, "%-5d %-11s %-3d%-8s %-15ld %-20s\n", lineNum, lexeme, a.type, typeText, a.attr, attrName);
	} else if(a.attr == NEWLINE_ATTR){
		//if newline, print line to listing file
		lineNum++;
		char listBuffer[100];
		//copy over the buffer, unless we're at EOF
		if(fgets(listBuffer, 73, srcLineFile) == NULL)
			memset(listBuffer, 0, sizeof(listBuffer));
		//make sure the line prints with a newline char, to make listing file prettier
		int hasNewLine = 0;
		char *ptr = listBuffer;
		while(*ptr != '\0'){
			if(*ptr == '\n'){
				hasNewLine = 1;
				break;
			}
			ptr += sizeof(char);
		}
		if(!hasNewLine)
			*ptr = '\n';
		//now every line will print with a newline
		fprintf(listingFile, "%-8d%s", lineNum, listBuffer);
		//set up a new line buffer for the tokens to be read off of
		//first, clear current buffer
		memset(buffer, 0, sizeof(buffer));
		//then, populate buffer
		char c;
		int bufferIndex = 0;
		do{
			c = fgetc(srcFile);
			buffer[bufferIndex++] = c;
		}while(c != EOF && c != '\n' && bufferIndex != 72);
		//put pointers at start of new buffer
		forw = &buffer[0] - sizeof(char);
		back = &buffer[0] - sizeof(char);
	} 


	//parser doesn't want any whitespace tokens
	if(a.type == WS_TYPE)
		return getToken();

	printf("token get: (%d %ld)\n", a.type, a.attr);

	return a;
}

void addToTable(char *lexeme, int type, int attr, struct linkedNode **table){
	struct linkedNode *temp = (struct linkedNode*)malloc(sizeof(struct linkedNode));
	temp->lexeme = malloc(sizeof(lexeme));
	strcpy(temp->lexeme, lexeme);
	temp->type = type;
	temp->attr = attr;
	temp->next = *table;
	*table = temp;
}

void readInReserved(char *reservedName){
	FILE *infile;
	char lexeme[11];
	int type, attr;
	infile = fopen(reservedName, "r");
	if(infile == NULL)
		printf("%s\n", "No reserved word file found");
	while(fscanf(infile, "%s %d %d", lexeme, &type, &attr) != EOF){
		addToTable(lexeme, type, attr, &reservedTable);
	}
	fclose(infile);
}

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

char* getReservedAttrName(int type, int attr, char *attrName){
	if(type == ADDOP_TYPE && attr == OR_ATTR)
		return "(Or)";
	if(type == MULOP_TYPE){
		if(attr == DIV_ATTR)
			return "(Div)";
		else if (attr == MOD_ATTR)
			return "(Mod)";
		else if (attr == AND_ATTR)
			return "(And)";
	}

	return attrName;
}

char* getLexerrAttrName(int attr){
	switch (attr){
		case EXTRA_LONG_INT_ATTR: return "(Extra long int)"; break;
		case EXTRA_LONG_ID_ATTR: return "(Extra long id)"; break;
		case LEADING_ZEROES_ATTR: return "(Leading zeroes)"; break;
		case XX_TOO_LONG_ATTR: return "(XX too long)"; break;
		case YY_TOO_LONG_ATTR: return "(YY too long)"; break;
		case TRAILING_ZEROES_ATTR: return "(Trailing zeroes)"; break;
		case UNRECOGNIZED_SYMBOL_ATTR: return "(Unrecognized symbol)"; break;
		case ZZ_TOO_LONG_ATTR: return "(ZZ too long)"; break;
	}
	return "(Unrecognized Error)";
}

void printLexErr(int attr, char *lexeme){
	switch (attr){
		case EXTRA_LONG_INT_ATTR: fprintf(listingFile, "%-10s%-40s %s\n", "LEXERR:", "Extra long int(max is 10):", lexeme); break;
		case EXTRA_LONG_ID_ATTR: fprintf(listingFile, "%-10s%-40s %s\n", "LEXERR:", "Extra long id(max is 10):", lexeme); break;
		case LEADING_ZEROES_ATTR: fprintf(listingFile, "%-10s%-40s %s\n", "LEXERR:", "Leading zeroes:", lexeme); break;
		case XX_TOO_LONG_ATTR: fprintf(listingFile, "%-10s%-40s %s\n", "LEXERR:", "Extra long integer-part(max is 5):", lexeme); break;
		case YY_TOO_LONG_ATTR: fprintf(listingFile, "%-10s%-40s %s\n", "LEXERR:", "Extra long fractional-part(max is 5):", lexeme); break;
		case TRAILING_ZEROES_ATTR: fprintf(listingFile, "%-10s%-40s %s\n", "LEXERR:", "Trailing zeroes:", lexeme); break;
		case UNRECOGNIZED_SYMBOL_ATTR: fprintf(listingFile, "%-10s%-40s %s\n", "LEXERR:", "Unrecognized symbol:", lexeme); break;
		case ZZ_TOO_LONG_ATTR: fprintf(listingFile, "%-10s%-40s %s\n", "LEXERR:", "Extra long exponent-part(max is 2):", lexeme); break; 
	}
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