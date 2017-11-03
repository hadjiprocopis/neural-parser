#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define	TRUE	1
#define	FALSE	0

#define	MAX_LINE_LENGTH	20000

int	main(int argc, char **argv)
{
	FILE	**Handles;
	int	NumberOfFiles = argc - 1, flag = TRUE, i, j;
	char	*Line, *buffer;

	if( argc < 2 ){
		fprintf(stderr, "Usage %s: file1 [file2 ... fileN]\a\n", argv[0]);
		fprintf(stderr, "This program will take the contents of the files 1 to N\n");
		fprintf(stderr, "and dump them one next to the other, to the standard output.\n");
		fprintf(stderr, "It returns 1 on failure, 0 on success. A.Hadjiprocopis (c) 1998\n");
		exit(1);
	}

	if( (buffer=(char *)calloc((int )MAX_LINE_LENGTH+5, sizeof(char *))) == NULL ){
		fprintf(stderr, "%s: Memory allocation error for %d of char.\a\n", argv[0], (int )MAX_LINE_LENGTH);
		exit(1);
	}
	if( (Handles=(FILE **)calloc(NumberOfFiles, sizeof(FILE *))) == NULL ){
		fprintf(stderr, "%s: Memory allocation error for %d of FILE *.\a\n", argv[0], NumberOfFiles);
		free(buffer);
		exit(1);
	}
	if( (Line=(char *)calloc(MAX_LINE_LENGTH * NumberOfFiles, sizeof(char))) == NULL) {
		fprintf(stderr, "%s: Memory allocation error for %d chars.\a\n", argv[0], MAX_LINE_LENGTH * NumberOfFiles);
		free(buffer); free(Handles);
		exit(1);
	}
	Line[0] = '\0';

	for(i=0;i<NumberOfFiles;i++){
		if( (Handles[i]=fopen(argv[i+1], "r")) == NULL ){
			fprintf(stderr, "%s: Failed to open file '%s' for reading (%d in the row). Perhaps the problem is with too many open files?\a\n", argv[0], argv[i+1], i+1);
			for(j=0;j<i;j++) fclose(Handles[j]);
			free(buffer); free(Handles); free(Line);
			exit(1);
		}
	}

	while( 1 ){
		for(i=0;i<NumberOfFiles;i++){
			fgets(buffer, MAX_LINE_LENGTH, Handles[i]);
			if( feof(Handles[i]) ) {flag = FALSE; break;}
			buffer[strlen(buffer)-1] = ' ';
			strcat(Line, buffer);
		}
		if( !flag ) break;
		Line[strlen(Line)-1] = '\0';
		puts(Line);
		Line[0] = '\0';
	}

	for(i=0;i<NumberOfFiles-1;i++) fclose(Handles[i]);
	free(buffer); free(Handles); free(Line);

	exit(0);
}
