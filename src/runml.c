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
		args->code[i]=malloc(args->bcounter[i]);
		//printf("lines: %d\n", i);

		// If there is an error, we want to free all previous elements before freeing the array itself
		if (!args->code[i]) {
			for(int j=i;j>0;j--) free(args->code[j]);
			return 0;
		}
		// Copy from file
		fgets(args->code[i],args->bcounter[i], fptr);
		//printf("%s", args->code[i]);
	}
	
	return 1;	
} 

int  
util_parseMath(struct arg *args) {
	// Loop through every line
	char **output;
	int resize_ret_code=0;
	// Registering the name of the identifiers
	char **idname=malloc(sizeof(char *)*1);
	idname[0]="EOA";
	short *idtype; //0->variable 1->function
	int idcount=1;
	size_t *idsize=calloc(1,sizeof(size_t));

	// for (int i=0;i<args->lines;i++) {
	// 	printf("%ld\n", args->bcounter[i]);
	// }

	for (int i=0;i<args->lines;i++) {
		// First word in the line 
		char *current=strtok(args->code[i], " ");
		if (!strcmp(current, "function")) {
			current=strtok(NULL, " ");
			// check if the current identifier is already created
			for (int i;i<idcount;i++) {
				if (!strcmp(current, idname[i])) {
					fprintf(stderr, "! runml: previous declaration of identifier\n");
				} 
			}
			// Add to identifier registery
		resize_ret_code=1; // Sets the return code so the program knows to return here on function
		goto Util_Parse_Math_AddToCount; // Reallocates the size of each, and then returns to this line
		Util_Parse_math_Return_Function:
		idsize[idcount-1]=strlen(current);
		idname[idcount-1]=malloc(idsize[idcount-1]);
		strncpy(idname[idcount-1], current, idsize[idcount-1]);
		idtype[idcount-1]=1;

		} else if (!strcmp(strtok(args->code[i], " "), "print")) {
			current=strtok(NULL, " ");
			// check if the current identifier is already created
			for (int i;i<idcount;i++) {
				if (!strcmp(current, idname[i])) goto identifier_found;
			} 
			
			identifier_found:
		
		} else if (!strcmp(strtok(args->code[i], " "), "#")) {
			// Line starts with a comment

		} else {
			// Create new variable identifier or call identifier
			current=strtok(NULL, " ");
			// check if the current identifier is already created
			for (int i;i<idcount;i++) {
				if (!strcmp(current, idname[i])) {
					fprintf(stderr, "! runml: previous declaration of identifier\n");
				}
			}
			resize_ret_code=0;
			goto Util_Parse_Math_AddToCount;
			Util_Parse_math_Return_Variable:
			idsize[idcount-1]=strlen(current);
			idname[idcount-1]=malloc(idsize[idcount-1]);
			strncpy(idname[idcount-1], current, idsize[idcount-1]);
			idtype[idcount-1]=0;
		}


	}
	return 1;
	// Clean out all mallocs made here
	Util_Parse_Math_Clean_Identifier_Name_Register:
	for (int i=idcount;i>0;i--) free(idname[i]);
	Util_Parse_Math_Clean_Identifier_Name:
	free(idname);
	Util_Parse_Math_Clean_Identifier_Size:
	free(idsize);
	idcount=0;

	return 0;

	Util_Parse_Math_AddToCount:
		idcount++;
		char **tmp_reallocRes=realloc(idname, sizeof(char *) *(idcount+1));
		if (!tmp_reallocRes) {
			fprintf(stderr, "Realloc: failed to reallocate identifier registry. Code: %d\n", 2); 
			// TODO: handle this later	
		} idname=tmp_reallocRes;
		size_t *tmp_reallocRes2=realloc(idsize, sizeof(size_t) * (idcount+1));
		if (!tmp_reallocRes2) {
			fprintf(stderr, "Realloc: failed to reallocate identifier size registry. Code: %d\n", 2); 
			// TODO: handle this later	
		} idsize=tmp_reallocRes2;
		short *tmp_reallocRes3=realloc(idtype, sizeof(short) * (idcount+1));
		if (!tmp_reallocRes3) {
			fprintf(stderr, "Realloc: failed to reallocate identifier type registry. Code: %d\n", 2); 
			// TODO: handle this later	
		} idtype=tmp_reallocRes3;
		switch (resize_ret_code) {
			case 1:
				goto Util_Parse_math_Return_Function;
				break;
			case 0:
				goto Util_Parse_math_Return_Variable;
				break;
			default:
				break;
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
