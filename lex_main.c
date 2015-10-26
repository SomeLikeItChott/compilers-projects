#include "lexical_analyzer.h"

int main(int argc, char *argv[]){
	char *fileName, *tokenName, *listingName, *reservedName;
	if (argc > 1)
		fileName = argv[1];
	else
		fileName = "file";
	if (argc > 2)
		tokenName = argv[2];
	else
		tokenName = "tokens";
	if (argc > 3)
		listingName = argv[3];
	else
		listingName = "listing";
	if (argc > 4)
		reservedName = argv[4];
	else
		reservedName = "reserved";

	readInReserved(reservedName);
	openFiles(tokenName, listingName, fileName);

	struct token toke;

	do{
		toke = getToken();
	}while(toke.type != EOF_TYPE);

	closeFiles();
}