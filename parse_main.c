#include "lexical_analyzer.h"
#include "parser.h"

int main(int argc, char *argv[]){
	char *fileName, *tokenName, *listingName, *reservedName, *addressesName;
	if (argc > 1)
		fileName = argv[1];
	else
		fileName = "file";
		
	tokenName = "tokens";
	listingName = "listing";
	reservedName = "reserved";
	addressesName = "addresses";


	readInReserved(reservedName);
	openFiles(tokenName, listingName, fileName, addressesName);

	parse();

	closeFiles();
}