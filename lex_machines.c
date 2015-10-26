#include "lex_machines.h"

struct token fail(){
	forw = back;
	struct token a = {-1, -1};
	return a;
}

struct token catchallMachine(){
	int state = 0;
	char c;
	while(1){
		//ws: 0, id: 1, int: 4
		switch(state){
			case 0:
			c = nextChar();
			if (c == ':') state = 1;
			else if (c == ';') {return generateToken(CATCHALL_TYPE, SEMICOLON_ATTR, "(Semicolon)"); break;}
			else if (c == '[') {return generateToken(CATCHALL_TYPE, OPENBRACKET_ATTR, "(Open bracket)"); break;}
			else if (c == ']') {return generateToken(CATCHALL_TYPE, CLOSEBRACKET_ATTR, "(Close bracket)"); break;}
			else if (c == '(') {return generateToken(CATCHALL_TYPE, OPENPAREN_ATTR, "(Open parenthesis)"); break;}
			else if (c == ')') {return generateToken(CATCHALL_TYPE, CLOSEPAREN_ATTR, "(Close parenthesis)"); break;}
			else if (c == '{') {return generateToken(CATCHALL_TYPE, OPENCURLYBRACE_ATTR, "(Open curly brace)"); break;}
			else if (c == '}') {return generateToken(CATCHALL_TYPE, CLOSECURLYBRACE_ATTR, "(Close curly brace)"); break;}
			else if (c == '+') {return generateToken(ADDOP_TYPE, PLUS_ATTR, "(Plus sign)"); break;}
			else if (c == '-') {return generateToken(ADDOP_TYPE, MINUS_ATTR, "(Minus sign)"); break;}
			else if (c == '*') {return generateToken(MULOP_TYPE, TIMES_ATTR, "(Multiply sign)"); break;}
			else if (c == '/') {return generateToken(MULOP_TYPE, DIVIDE_ATTR, "(Division sign)"); break;}
			else if (c == ',') {return generateToken(CATCHALL_TYPE, COMMA_ATTR, "(Comma)"); break;}
			else if (c == '.') {return generateToken(CATCHALL_TYPE, PERIOD_ATTR, "(Period)"); break;}
			else if (c == EOF) {return generateToken(EOF_TYPE, NO_ATTR, "(End of file)"); break;}
			else state = 4;
			break;

			case 1:
			c = nextChar();
			if(c == '=') state = 2;
			else state = 3;
			break;

			case 2:
			return generateToken(ASSIGNOP_TYPE, NO_ATTR, "(Assignment sign)");
			break;

			case 3:
			retract(1);
			return generateToken(CATCHALL_TYPE, COLON_ATTR, "(Colon)");
			break;

			case 4:
			return generateToken(LEXERR_TYPE, UNRECOGNIZED_SYMBOL_ATTR, "(Unrecognized Symbol)");
			break;
		}
	}
}

struct token relopMachine(){
	int state = 0;
	char c;
	while(1){
		//ws: 0, id: 1, int: 4
		switch(state){
			case 0:
			c = nextChar();
			if (c == '=') state = 1;
			else if (c == '>') state = 2;
			else if (c == '<') state = 5;
			else return fail();
			break;

			case 1:
			return generateToken(RELOP_TYPE, EQUAL_ATTR, "(Equals)");
			break;

			case 2:
			c = nextChar();
			if (c == '=') state = 3;
			else state = 4;
			break;

			case 3:
			return generateToken(RELOP_TYPE, GTEQUAL_ATTR, "(Greater than or equals)");
			break;

			case 4:
			retract(1);
			return generateToken(RELOP_TYPE, GT_ATTR, "(Greater than)");
			break;

			case 5:
			c = nextChar();
			if (c == '>') state = 6;
			else if (c == '=') state = 7;
			else state = 8;
			break;

			case 6:
			return generateToken(RELOP_TYPE, NOTEQUAL_ATTR, "(Not equals)");
			break;

			case 7:
			return generateToken(RELOP_TYPE, LTEQUAL_ATTR, "(Less than or equals)");
			break;

			case 8:
			retract(1);
			return generateToken(RELOP_TYPE, LT_ATTR, "(Less than)");
			break;		
		}
	}
}

struct token realMachine(){
	int state = 0;
	char c;
	while(1){
		//ws: 0, id: 1, int: 4
		switch(state){
			case 0:
			c = nextChar();
			if (isdigit(c)) state = 1;
			else return fail();
			break;

			case 1:
			c = nextChar();
			if (isdigit(c)) state = 1;
			else if(c == '.') state = 2;
			else return fail();
			break;

			case 2:
			c = nextChar();
			if (isdigit(c)) state = 3;
			else return fail();
			break;

			case 3:
			c = nextChar();
			if (isdigit(c)) state = 3;
			else state = 4;
			break;

			case 4: retract(1);
			return generateToken(REAL_TYPE, NO_ATTR, "(Real number)");
			break;
		}
	}
}

struct token longrealMachine(){
	int state = 0;
	char c;
	while(1){
		//ws: 0, id: 1, int: 4
		switch(state){
			case 0:
			c = nextChar();
			if (isdigit(c)) state = 1;
			else return fail();
			break;

			case 1:
			c = nextChar();
			if (isdigit(c)) state = 1;
			else if(c == '.') state = 2;
			else return fail();
			break;

			case 2:
			c = nextChar();
			if (isdigit(c)) state = 3;
			else return fail();
			break;

			case 3:	
			c = nextChar();
			if (isdigit(c)) state = 3;
			else if (c == 'E') state = 4;
			else return fail();
			break;

			case 4:
			c = nextChar();
			if (isdigit(c)) state = 6;
			else if (c == '-' || c == '+') state = 5;
			else return fail();
			break;

			case 5:
			c = nextChar();
			if (isdigit(c)) state = 6;
			else return fail();
			break;

			case 6:
			c = nextChar();
			if (isdigit(c)) state = 6;
			else state = 7;
			break;

			case 7:
			retract(1);
			return generateToken(LONGREAL_TYPE, NO_ATTR, "(Long real number)");
			break;
		}
	}
}

struct token wsMachine(){
	int state = 0;
	char c;
	while(1){
		switch(state){
			case 0:
			//eats up whitespace
			c = nextChar();
			if(c == '\n') return generateToken(WS_TYPE, NEWLINE_ATTR, "(Newline)");
			if(c == ' ') return generateToken(WS_TYPE, SPACE_ATTR, "(Space)");
			if(c == '	') return generateToken(WS_TYPE, TAB_ATTR, "(Tab)");
			if (isspace(c)) generateToken(WS_TYPE, OTHER_WS_ATTR, "(Other WS)");
			else return fail();
			break;
		}
	}
}

struct token intMachine(){
	int state = 0;
	char c;
	while(1){
		//ws: 0, id: 1, int: 4
		switch(state){
			case 0: 
			c = nextChar();
			if(isdigit(c)) state = 1;
			else return fail();
			break;

			case 1: 
			c = nextChar();
			if(isdigit(c)) state = 1;
			else state = 2;
			break;

			case 2: retract(1);
			if ((forw - back) > 10)
				return generateToken(LEXERR_TYPE, EXTRA_LONG_INT_ATTR, "(Extra long int)");
			else
				return generateToken(INT_TYPE, NO_ATTR, "(Integer)");
			break;
		}
	}
}

struct token idMachine(){
	int state = 0;
	char c;
	while(1){
		//ws: 0, id: 1, int: 4
		switch(state){
			case 0:
			c = nextChar();
			if (isalpha(c)) state = 1;
			else return fail();
			break;

			case 1:
			c = nextChar();
			if (isalpha(c) || isdigit(c)) state = 1;
			else state = 2;
			break;

			case 2: retract(1);
			if ((forw - back) > 10)
				return generateToken(LEXERR_TYPE, EXTRA_LONG_ID_ATTR, "(Extra long ID)");
			else
				return generateToken(ID_TYPE, NO_ATTR, "(Ptr to symbol table)");
			break;
		}
	}
}
