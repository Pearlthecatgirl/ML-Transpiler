#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct arg {
	char **id_namev;
	size_t *id_sizev;
	int *id_typev;
	int idc;
	
	char *script;
} arg;

int
util_parseMath(const char script) {


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
util_loadScript(const char *fullpath, char *script) {
	FILE *fptr;
	if (access(fullpath, R_OK==-1)) {
		fprintf(stderr, "access: file (%s) either does not exist, or you do not have the permission to read from it. Code: %d\n", fullpath, 1);
		return 0;
	}
	if (!(fptr=fopen(fullpath, "r"))) {
		fprintf(stderr, "fopen: file (%s) could not be opened Code: %d\n", fullpath, 2);
		return 0;	
	}
	// Get the size of the file for malloc
	fseek(fptr, 0, SEEK_END);
	size_t size_of_file=ftell(fptr);
	fseek(fptr, 0, SEEK_SET);
	
	char *result=realloc(script, size_of_file * sizeof(char));
	if (!result) {
		return 0;
	} script=result;

	if (fread(script, size_of_file, 1, fptr)!=1) {
		fprintf(stderr, "fread: size read from is different from size requested. Code: %d\n", 3);
		goto Util_loadScript_clean_script;
	}
	fclose(fptr);
	return 1;

	Util_loadScript_clean_script:
	free(script);
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
	args->script=malloc(sizeof(char));

	if (!util_loadScript(fullpath, args->script)) {
		goto clean_args_script;
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
	free(args->script);
}
