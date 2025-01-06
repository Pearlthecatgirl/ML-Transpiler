#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

int
util_parseMath(char *script, int script_size) {
	enum nodeType {Expression, Function, Assignment} nodeType;


	// while (!reached_Bottom_of_Tree) {
	// 	nodeType=Expression;
	// 	switch (nodeType) {
	// 		case Expression:
	// 			// Determine if there are parenthesis

	// 			break;
	// 		case Function:
	// 			// read until out of scope (\t)
	// 			break;
	// 		case Assignment:
	// 			// Make sure left side only has 1 variable
	// 			break;
	// 		default: 
	// 			break;	
	// 	}
	// }
	
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

	//if (!util_parseMath(args->script, args->script_size)) {
	//	return 1;
	//}

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
