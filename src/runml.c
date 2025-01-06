#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum nodeType {Expression, Function, Assignment} nodeType;

struct arg {
	char **id_namev;
	size_t *id_sizev;
	int *id_typev;
	int idc;
	
	char **script;
	size_t *script_sizev;
	int script_len;
} arg;

int util_checkContains(const char *string, const char *contents);
int util_checkContains_r(const char *string, const char letter);

int
util_parseMath(struct arg *args) {
	char *current;
	// Outer loop
	for (int i=0;i<args->script_len;i++) {
		//current=strtok(args->script[i], " ");
		if (args->script[i][0]==35) continue; 

			//fprintf(stdout, "%s, %d", args->script[i], util_checkContains_r(args->script[i], ' '));
		// Check for a space first before trying to tokenize
		if (util_checkContains_r(args->script[i], ' ')) {
			current=strtok(args->script[i], " ");
			if (!strcmp(current, "function")) {
					current=strtok(NULL, " ");
					// Check if it is already an identifier
			}
		// there is no space. just compare it
		} else {
				if (!strncmp(args->script[i], "function", strlen("function"))) {
						if (strlen(args->script[i])>strlen("function")) {
							fprintf(stderr, "! runml: Error: spotted \"function\" with more letters behind it. Are you missing a space? \n");
							return 1;
						} else {
							fprintf(stderr, "! runml: Error: spotted \"function\" with no identifier behind it. Is this an identifier? \n");
							return 1;
						}
				}
		}

		/* Only skip the line if the first character is a #. Otherwise we can have such cases:
		 * function args # comment <- such line would be skipped
		 * */

		
		while (1)	{
				// Access current here, before reassigning via strtok
				//fprintf(stdout,"%s\n", current);

				if (util_checkContains_r(current, '\n')) break;
				current=strtok(NULL, " ");
		}
	}

	return 1;
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
util_checkContains_r(const char *string, const char letter) {
		for (int i=0;i<strlen(string);i++) {
				if (string[i]==letter) return 1;
		}
		return 0;
}

int 
util_loadScript(const char *fullpath, struct arg *args) {
	FILE *fptr;
	// Check File existence
	if (access(fullpath, R_OK==-1)) {
		fprintf(stderr, "access: file (%s) either does not exist, or you do not have the permission to read from it. Code: %d\n", fullpath, 1);
		return 0;
	}
	if (!(fptr=fopen(fullpath, "r"))) {
		fprintf(stderr, "fopen: file (%s) could not be opened Code: %d\n", fullpath, 2);
		return 0;	
	}
	// Count number of lines
	char cursor;
	args->script_len=0;

	cursor=fgetc(fptr);
	while (1) {
		if (feof(fptr)) break;
		if (cursor=='\n') args->script_len++; 
		cursor=fgetc(fptr);
	}

	fseek(fptr, 0, SEEK_SET);
	char **res=realloc(args->script, sizeof(char *) *args->script_len);
	if (!res) {
		fprintf(stderr, "realloc: script resize failed. Code %d\n", 3);
		goto Util_loadScript_Ret_fail;
	} args->script=res;
	size_t *res2=realloc(args->script_sizev, sizeof(size_t)*args->script_len);
	if (!res2) {
		fprintf(stderr, "realloc: script_size vector resize failed. Code: %d\n", 4);
		goto Util_loadScript_Ret_fail;
	} args->script_sizev=res2;

	#define BUFMAX 256
	char buf[BUFMAX];
	for (int currentLine=0;currentLine<args->script_len;currentLine++) {
		if (!fgets(buf, BUFMAX, fptr)) {
			fprintf(stderr, "fgets: failed to get line. Cleaning script vector. Code: %d\n", 5);
			goto Util_loadScript_clean_script_contents;
		}
		int currentlen=strlen(buf);
		args->script[currentLine]=malloc(sizeof(char)*currentlen);
		if (!args->script[currentLine]) {
			fprintf(stderr, "malloc: failed to initialise current line in the script vector. Code: %d\n", 6);
			goto Util_loadScript_clean_script_contents;	
		}
		args->script_sizev[currentLine]=sizeof(size_t)*currentlen;
		if (!strncpy(args->script[currentLine], buf, currentlen)) {
			goto Util_loadScript_clean_script_contents;
		}

		if (0) {
			Util_loadScript_clean_script_contents:
			for (int currentLineRev=currentLine;currentLineRev>0;currentLineRev--) {
				free(args->script[currentLineRev]);
				args->script_sizev[currentLineRev]=(size_t)0;
				args->script_len--;
			}
			return 0;
		}
	}
	//#undef BUFMAX

	fclose(fptr);
	return 1;

	Util_loadScript_Ret_fail:
	return 0;
}

int
main(int argc, char **argv) {
	char *fullpath;
	if (argc<2) goto usage;	
	for (int i=1;i<argc;i++) {
		if (argv[i][0]=='-') {
			if (!strcmp(argv[i], "-h")) {
				goto usage;
			} else {
				fprintf(stderr,"unrecognised option :\"%s\". Code: %d\n", argv[i], -1);
				goto usage;
			}
		} else if (util_checkContains(argv[i], ".ml")) {
			fullpath=malloc(sizeof(char)*strlen(argv[i]));
			strncpy(fullpath, argv[i], sizeof(char)*strlen(argv[i]));
		} else {
			fprintf(stderr,"unrecognised option :\"%s\". Code: %d\n", argv[i], -1);
			goto usage;	
		}
	}
	struct arg *args=malloc(sizeof(arg));
	args->script_len=1;
	args->script=malloc(sizeof(char *) * args->script_len);
	args->script_sizev=malloc(sizeof(size_t) * args->script_len);

	if (!util_loadScript(fullpath, args)) goto clean_args_script;
	
	//for (int i=0;i<args->script_len;i++) {
	//printf("%s", args->script[i]);
	//}

	if (!util_parseMath(args)) {
		return 1;
	}

	// TODO: read from file
	// TODO: parse file
	// TODO: generate c file
	// TODO: compile and run

return 0;

	usage:
	fprintf(stderr,"Usage : runml [-hv] [file.ml] [-o output_name]\nProgram returns 0 on success, 1 on fail. Read the stderr code for issue.\nCode: %d\n", 0);
	return 1;

	clean_args_script:
	free(args);
	return 1;
}

//Nothing to see here :3
