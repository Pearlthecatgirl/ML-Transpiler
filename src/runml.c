#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char **util_loadScriptFromPath(char *fullpath);

char **
util_loadScriptFromPath(char *fullpath) {
	FILE *fptr;
	if (!(fptr=fopen(fullpath, "r"))) {
		fprintf(stderr, "fopen: ml file could not be opened. %d\n", 1);
		goto util_Load_Script_Fail;
	}
	// Determine how many lines are in the file
	int lines=0;
	char current;
	size_t *bytecount=calloc(1, sizeof(size_t)*1); // Calloc only used to save 1 line to set the first element to 0
	while (current!=EOF) {
		current=fgetc(fptr);
		if (current=='\n') {	
			lines++;
			// reallocate bytecount 
			size_t *result=realloc(bytecount, sizeof(size_t)*lines+1);
			if (!result) {
				fprintf(stderr, "Realloc: failed to reallocate byte counter"); 
				goto util_Load_clean_bytecounter;
			} bytecount=result;
			bytecount[lines]=0;
		} else bytecount[lines]+=(sizeof(size_t)*1);
	}
	fseek(fptr, 0, SEEK_SET);

	// Allocate according to the lines and bytes per line from previous..
	char **output=malloc(sizeof(char *)*lines);
	for (int i=0;i<lines;i++) {
		//printf("sizeof line: %d, line: %d\n", (int)(bytecount[i]), i);
		output[i]=malloc(sizeof(char)*bytecount[i]);
		// If there is an error, we want to free all previous elements before freeing the array itself
		if (!output[i]) {
			for(int j=i;j>0;j--) free(output[j]);
			goto util_Load_clean_output;
		}
		// Copy from file
		fgets(output[i],sizeof(bytecount[i]), fptr);
	}
	
	for(int i=0;i<lines;i++) {
		printf("%s", output[i]);
	
	}

	free(bytecount);
	return output;	

	util_Load_clean_output:
	free(output);
	util_Load_clean_bytecounter:
	free(bytecount);
	util_Load_Script_Fail:	
	return NULL;	
} 

int
main(int argc, char **argv) {
	if (argc<2) goto usage;

	//printf("%d\n", (int)sizeof(char));
	util_loadScriptFromPath("../test/generic.txt");

	return 0;
	usage:
	fprintf(stderr,"Usage : runml file.ml\n");
}
