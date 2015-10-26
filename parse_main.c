#include "lexical_analyzer.h"

int main(int argc, char *argv[]){
	char *fileName, *tokenName, *listingName, *reservedName;
	if (argc > 1)
		fileName = argv[1];
	else
		fileName = "file";
		
	tokenName = "tokens";
	listingName = "listing";
	reservedName = "reserved";

	readInReserved(reservedName);
	openFiles(tokenName, listingName, fileName);

	parse();

	/*
	struct token toke;
	do{
		toke = getToken();
	}while(toke.type != EOF_TYPE);
	*/

	closeFiles();
}