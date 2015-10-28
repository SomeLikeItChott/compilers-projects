CC=gcc

build: parse_main.c parser.c lexical_analyzer.c lex_errors.c lex_machines.c 
	$(CC) -o parse parse_main.c parser.c lexical_analyzer.c lex_errors.c lex_machines.c -I.

debug: parse_main.c parser.c lexical_analyzer.c lex_errors.c lex_machines.c 
	$(CC) -g -o parse parse_main.c parser.c lexical_analyzer.c lex_errors.c lex_machines.c -I.
