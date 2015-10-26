#include "lex_errors.h"

struct token checkidErrors(char *lexeme){
	//already checked for too-long id (in machine)
	//check if id is already in reserved, then symbol table
	struct token tok = isInTable(lexeme, reservedTable);
	if(tok.type != -1 && tok.attr != -1){
		return tok;
	} else{
		tok = isInTable(lexeme, symbolTable);
		if(tok.type != -1 && tok.attr != -1){
			return tok;
		} else {
			//add new symbol to symbol table
			addToTable(lexeme, ID_TYPE, NO_ATTR, &symbolTable);
			(*symbolTable).attr = (long)&(*symbolTable);
			struct token a = {.type = ID_TYPE, .attr = (*symbolTable).attr};
			return a;
		}
	}
}

int hasLeadingZeroes(char *lexeme){
	char *charpointer = lexeme;
	if(*charpointer == '0'){
		int length = 0;
		while(*charpointer != '\0'){
			length++;
			charpointer += sizeof(char);
			if(length == 1 && *charpointer == '.')
				return 0;
		}
		if(length > 1)
			return 1;
	}
	return 0;
}

int hasTrailingZeroesAfterDecimal(char *lexeme){
	char *charpointer = lexeme;
	while(*charpointer != '.')
		charpointer += sizeof(char);
	charpointer += sizeof(char);
	while(isdigit(*charpointer))
		charpointer += sizeof(char);
	charpointer -= sizeof(char);
	//now pointing to last digit after decimal point
	if(*charpointer == '0'){
		charpointer -= sizeof(char);
		if(*charpointer == '.')
			return 0;
		else
			return 1;
	} else
		return 0;
}

int hasTrailingZeroesAtEnd(char *lexeme){
	char *charpointer = lexeme + sizeof(lexeme)/sizeof(char);
	//charpoiner is pointing at the end of the lexeme, including nulls
	while(!isdigit(*charpointer))
		charpointer -= sizeof(char);
	//now pointing to last digit
	if(*charpointer == '0'){
		charpointer -= sizeof(char);
		if(*charpointer == '.')
			return 0;
		else
			return 1;
	} else
		return 0;
}

int digitLengthBeforeChar(char *lexeme, char before){
	char *charpointer = lexeme;
	int length = 0;
	while(*charpointer != before){
		charpointer += sizeof(char);
		length++;
	}
	return length;
}

int digitLengthAfterChar(char *lexeme, char after){
	char *charpointer = lexeme;
	int length = 0;
	while(*charpointer != after)
		charpointer += sizeof(char);
	//now at desired char
	charpointer += sizeof(char);
	while(isdigit(*charpointer)){
		charpointer += sizeof(char);
		length++;
	}
	return length;
}

int digitLengthAfterCharIgnoreSigns(char *lexeme, char after){
	char *charpointer = lexeme;
	int length = 0;
	while(*charpointer != after)
		charpointer += sizeof(char);
	//now at desired char
	charpointer += sizeof(char);
	while(isdigit(*charpointer) || *charpointer == '+' || *charpointer == '-'){
		if(isdigit(*charpointer))
			length++;
		charpointer += sizeof(char);
	}
	return length;
}

struct token checkintErrors(char *lexeme){
	if(hasLeadingZeroes(lexeme)){
		struct token a = {.type = LEXERR_TYPE, .attr = LEADING_ZEROES_ATTR};
		return a;
	} else {
		struct token a = {.type = INT_TYPE, .attr = NO_ATTR};
		return a;
	}
}

struct token checkrealErrors(char *lexeme){
	if(hasLeadingZeroes(lexeme)){
		struct token a = {.type = LEXERR_TYPE, .attr = LEADING_ZEROES_ATTR};
		return a;
	} else if(hasTrailingZeroesAfterDecimal(lexeme)){
		struct token a = {.type = LEXERR_TYPE, .attr = TRAILING_ZEROES_ATTR};
		return a;
	} else if(digitLengthBeforeChar(lexeme, '.') > 5){
		struct token a = {.type = LEXERR_TYPE, .attr = XX_TOO_LONG_ATTR};
		return a;
	} else if(digitLengthAfterChar(lexeme, '.') > 5){
		struct token a = {.type = LEXERR_TYPE, .attr = YY_TOO_LONG_ATTR};
		return a;
	}
	struct token a = {.type = REAL_TYPE, .attr = NO_ATTR};
	return a;
}

struct token checklongrealErrors(char *lexeme){
	if(hasLeadingZeroes(lexeme)){
		struct token a = {.type = LEXERR_TYPE, .attr = LEADING_ZEROES_ATTR};
		return a;
	} else if(hasTrailingZeroesAtEnd(lexeme)){
		struct token a = {.type = LEXERR_TYPE, .attr = TRAILING_ZEROES_ATTR};
		return a;
	} else if(digitLengthBeforeChar(lexeme, '.') > 5){
		struct token a = {.type = LEXERR_TYPE, .attr = XX_TOO_LONG_ATTR};
		return a;
	} else if(digitLengthAfterChar(lexeme, '.') > 5){
		struct token a = {.type = LEXERR_TYPE, .attr = YY_TOO_LONG_ATTR};
		return a;
	} else if(digitLengthAfterCharIgnoreSigns(lexeme, 'E') > 2){
		struct token a = {.type = LEXERR_TYPE, .attr = ZZ_TOO_LONG_ATTR};
		return a;
	}
	struct token a = {.type = LONGREAL_TYPE, .attr = NO_ATTR};
	return a;
}