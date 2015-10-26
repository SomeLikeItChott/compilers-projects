CC=gcc

build: lex_main.c lexical_analyzer.c lex_errors.c lex_machines.c 
	$(CC) -o lex lex_main.c lexical_analyzer.c lex_errors.c lex_machines.c -I.
