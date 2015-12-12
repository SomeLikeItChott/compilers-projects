CC=gcc

build: parse_main.c parser.c lexical_analyzer.c lex_errors.c lex_machines.c helpers.c
	$(CC) -o parse parse_main.c parser.c lexical_analyzer.c lex_errors.c lex_machines.c helpers.c -I.
