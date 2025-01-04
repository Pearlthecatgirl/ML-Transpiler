#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

struct arg {
	char **code;
	size_t *bcounter;
	int lines;

	// Identifier value, name and sizes for both
	char **idval;
	char **idname;
	int *idnamesize;
	int *idvalsize;
	int idvalnum, idnamenum;
} arg;

int util_loadScriptFromPath(char *fullpath, struct arg *args);
int util_parseMath(struct arg *args);
int util_checkContains(const char *string, const char *contents);
void util_cleanArgs(struct arg *args);

int 
util_loadScriptFromPath(char *fullpath, struct arg *args) {
	FILE *fptr;
	if (access(fullpath, F_OK)==-1) return 0; 
	if (!(fptr=fopen(fullpath, "r"))) {
		fprintf(stderr, "fopen: ml file could not be opened. Code: %d\n", 1);
		return 0;
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
				fprintf(stderr, "Realloc: failed to reallocate byte counter. Code: %d\n", 2); 
				return 0;
			} args->bcounter=result;
			args->bcounter[args->lines]=0;
		} else args->bcounter[args->lines]+=(sizeof(size_t)*1);
		//printf("%c : %ld\n",current, args->bcounter[args->lines]);
	}

	fseek(fptr, 0, SEEK_SET);

	char **result=realloc(args->code, sizeof(char *)*args->lines);
	//printf("lines: %d\n", args->lines);
	if (!result) {
		fprintf(stderr, "Realloc: failed to reallocate byte counter. Code: %d\n", 2); 
		return 0;
	} args->code=result;

	for (int i=0;i<args->lines;i++) {
		//printf("sizeof line: %ld, line: %d\n", args->bcounter[i], i);
		args->code[i]=malloc(sizeof(char)*args->bcounter[i]);
		//printf("lines: %d\n", i);

		// If there is an error, we want to free all previous elements before freeing the array itself
		if (!args->code[i]) {
			for(int j=i;j>0;j--) free(args->code[j]);
			return 0;
		}
		// Copy from file
		fgets(args->code[i],sizeof(char)*args->bcounter[i], fptr);
		//printf("%s", args->code[i]);
	}
	
	return 1;	
} 

int  
util_parseMath(struct arg *args) {
	// Loop through every line
	char **output;

	// Registering the name of the identifiers
	char **idname=malloc(sizeof(char *)*1);
	idname[0]="EOA";
	int *idtype;
	int idcount=1;
	size_t *idsize;

	char **current;

	for (int i=0;i<args->lines;i++) {
		char **result=realloc(current, sizeof(char)*args->bcounter[i]);
		if (!result) {
			fprintf(stderr, "Realloc: failed to reallocate byte counter. Code: %d\n", 2); 
			// TODO: handle this later
		} current=result;
		// First word in the line 
		if (!strcmp(strtok(args->code[i], " "), "function")) {
			// check if the current identifier is already created
			for (int i;i<idcount;i++) {
				if (!strcmp(args->code[i+1], idname[i])) {
					fprintf(stderr, "! runml: previous declaration of identifier\n");
				}
			}

		} else if (!strcmp(strtok(args->code[i], " "), "print")) {
		
		} else if (!strcmp(strtok(args->code[i], " "), "#")) {

		} else {
		// Create new identifier or call identifier
		
		}


	}
	return 0;
}

int 
util_checkContains(const char *string, const char *contents) {
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
	if (argc<2) goto usage;
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
			fprintf(stderr,"unrecognised option :\"%s\". Code: %d\n", argv[i], -1);
			goto usage;
			}
		} else if (util_checkContains(argv[i], ".ml")) {
			strncpy(fullpath, argv[i], sizeof(char)*strlen(argv[i]));
		} else {
			fprintf(stderr,"unrecognised option :\"%s\". Code: %d\n", argv[i], -1);
			goto usage;	
		}
	}

	struct arg *args=malloc(sizeof(arg));
	args->lines=0;
	if (!util_loadScriptFromPath(fullpath, args)) return 1;	

	// TODO: PARSE EACH LINE	
	if (!util_parseMath(args)) {
		if (!args->code) goto clean_args_code_and_bcounter;
		if (!args->bcounter) goto clean_args_code_and_bcounter;
	}

	// TODO: TRANSLATE TO C
	// TODO: COMPILE AND RUN

	return 0;

	usage:
	fprintf(stderr,"Usage : runml [-hv] [file.ml] [-o output_name]\nProgram returns 0 on success, 1 on fail. Read the stderr code for issue.\nCode: %d\n", 0);
	return 1;
	clean_args_code_contents:
	for (int i=args->lines;i>0;i--) free(args->code[i]);
	clean_args_code_and_bcounter:
	if (!args->code) free(args->code);
	if (!args->bcounter) free(args->bcounter);
	clean_args:
	free(args);
	return 1;
}
