#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

struct arg {
	char **code;
	int lines;
	size_t *bcounter;
} arg;

void util_loadScriptFromPath(char *fullpath, struct arg *args);
void util_parseMath(struct arg *args);

void
util_loadScriptFromPath(char *fullpath, struct arg *args) {
	FILE *fptr;
	if (access(fullpath, F_OK)==-1) goto util_Load_Script_Fail; 
	if (!(fptr=fopen(fullpath, "r"))) {
		fprintf(stderr, "fopen: ml file could not be opened. %d\n", 1);
		goto util_Load_Script_Fail;
	}
	// Determine how many lines are in the file
	args->lines=0;
	char current;
	args->bcounter=calloc(1, sizeof(size_t)*1); // Calloc only used to save 1 line to set the first element to 0
	while (current!=EOF) {
		current=fgetc(fptr);
		if (current=='\n') {	
			args->lines++;
			// reallocate args->bcounter 
			size_t *result=realloc(args->bcounter, sizeof(size_t)*args->lines+1);
			if (!result) {
				fprintf(stderr, "Realloc: failed to reallocate byte counter"); 
				goto util_Load_clean_bcounter;
			} args->bcounter=result;
			args->bcounter[args->lines]=0;
		} else args->bcounter[args->lines]+=(sizeof(size_t)*1);
	}

	fseek(fptr, 0, SEEK_SET);

	char **result=realloc(args->code, sizeof(char *)*args->lines);
	//printf("lines: %d\n", args->lines);
	if (!result) {
		fprintf(stderr, "Realloc: failed to reallocate byte counter"); 
		goto util_Load_clean_bcounter;
	} args->code=result;

	for (int i=0;i<args->lines;i++) {
		//printf("sizeof line: %d, line: %d\n", (int)(args->bcounter[i]), i);
		args->code[i]=malloc(sizeof(char)*args->bcounter[i]);
		//printf("lines: %d\n", i);

		// If there is an error, we want to free all previous elements before freeing the array itself
		if (!args->code[i]) {
			for(int j=i;j>0;j--) free(args->code[j]);
			goto util_Load_clean_code;
		}
		// Copy from file
		fgets(args->code[i],sizeof(args->bcounter[i]), fptr);
	}
	
	return;	

	util_Load_clean_code:
	free(args->code);
	util_Load_clean_bcounter:
	free(args->bcounter);
	util_Load_Script_Fail:	
	free(args);
	return;	
} 

void 
util_parseMath(struct arg *args) {
	for (int i=0;i<args->lines;i++) {
		printf("lines: ");
	}
}

int 
checkContains(const char *string, const char *contents) {
	char *occurrence;
	if ((occurrence=strrchr(string,*contents))!=NULL) {
		if (!strcmp(occurrence, contents)) {
			// Returns correct
			return 1;	
		}
	}
	// does not exist
	return 0;
}

int
main(int argc, char **argv) {
	char *outputName;
	char fullpath[1024];
	if (argc==0) goto usage;
	for (int i=1;i<argc;i++) {
		if (argv[i][0]=='-') {
			if (!strcmp(argv[i], "-h")) {
				goto usage;
			} else if (!strcmp(argv[i], "-o")) {
				// Create a temporary buffer for storing the output name before memory allocation
				char tmpbuffer[1024];
				strncpy(tmpbuffer, argv[++i], sizeof(char)*1024);
				//outputLength=strlen(tmpbuffer);
				outputName=malloc(sizeof(char)*strlen(tmpbuffer));
				strncpy(outputName, tmpbuffer, sizeof(char)*strlen(tmpbuffer));
			} else {
			fprintf(stderr,"unrecognised option :\"%s\"\n", argv[i]);
			goto usage;
			}
		} else if (checkContains(argv[i], ".ml")) {
			strncpy(fullpath, argv[i], sizeof(char)*strlen(argv[i]));
		} else fprintf(stderr,"unrecognised option :\"%s\"\n", argv[i]);
	}

	struct arg *args=malloc(sizeof(arg));
	util_loadScriptFromPath(fullpath, args);
	// Check to see if util_loadScriptFromPath fails.
	if (!args->code || !args->bcounter || !args->lines) return 1;

	// TODO: PARSE EACH LINE	

	// TODO: TRANSLATE TO C
	// TODO: COMPILE AND RUN


	return 0;
	usage:
	fprintf(stderr,"Usage : runml file.ml\n");
}
