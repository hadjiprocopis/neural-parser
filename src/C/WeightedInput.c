#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define	TRUE		1
#define	FALSE		0

int	Usage(char *app_name);

int	main(int argc, char **argv)
{
	FILE	*InputsFile, *WeightsFile;
	int	Rows, Columns, flag = TRUE, i, j;
	char	InputsFilename[256], WeightsFilename[256];
	double	Data, *Weights, *Thresholds;

	WeightsFilename[0] = InputsFilename[0] = '\0';

	/* Get the commands from the command line */
	for(i=1;(i<argc)&&flag;i++){
		if( !strncmp(argv[i], "-input", 2) ){
			if( ++i < argc ) strcpy(InputsFilename, argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-weights", 2) ){
			if( ++i < argc ) strcpy(WeightsFilename, argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-size", 2) ){
			if( ++i < argc ) Columns = atoi(argv[i]);
			else flag = Usage(argv[0]);
			if( ++i < argc ) Rows = atoi(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
	}

	/* Check if the parameters make sense */
	if( (InputsFilename=='\0') || (WeightsFilename=='\0') || (Columns<=0) || (Rows<=0) ){
		Usage(argv[0]);
		exit(1);
	}

	/* Allocate memory */
	if( (Weights=(double *)calloc(Columns, sizeof(double))) == NULL ){
		fprintf(stderr, "%s: Memory allocation error for %d doubles(1).\a\n", argv[0], Columns);
		exit(1);
	}
	if( (Thresholds=(double *)calloc(Columns, sizeof(double))) == NULL ){
		fprintf(stderr, "%s: Memory allocation error for %d doubles(2).\a\n", argv[0], Columns);
		free(Weights);
		exit(1);
	}
	/* Open the files for reading the input and weight values */
	if( (InputsFile=fopen(InputsFilename, "r")) == NULL ){
		fprintf(stderr, "%s: Error, could not open file '%s'.\n", argv[0], InputsFilename);
		free(Weights); free(Thresholds);
		exit(1);
	}
	if( (WeightsFile=fopen(WeightsFilename, "r")) == NULL ){
		fprintf(stderr, "%s: Error, could not open file '%s'.\n", argv[0], WeightsFilename);
		fclose(InputsFile);
		free(Weights); free(Thresholds);
		exit(1);
	}

	for(i=0;i<Columns;i++) fscanf(WeightsFile, "%lf", &(Weights[i]));
	for(i=0;i<Columns;i++) fscanf(WeightsFile, "%lf", &(Thresholds[i]));
	fclose(WeightsFile);
	for(i=0;i<Rows;i++){
		for(j=0;j<Columns;j++){
			fscanf(InputsFile, "%lf", &Data);
			printf("%lf ", Data * Weights[j] + Thresholds[j]);

		}
		printf("\n");
	}
	free(Weights); free(Thresholds);
	fclose(InputsFile);
	exit(0);
}

int	Usage(char *app_name)
{
	fprintf(stderr, "Usage: %s options...\n", app_name);
	fprintf(stderr, "Options are:\n");
	fprintf(stderr, "  -input filename       The name of the file holding the input data as reals\n");
	fprintf(stderr, "                        separated by white space.\n");
	fprintf(stderr, "  -weights filename     The name of the file holding the weights data as reals\n");
	fprintf(stderr, "                        separated by white space.\n");
	fprintf(stderr, "  -size C R             The size of the input file as rows(R) and columns(C).\n");
	fprintf(stderr, "                        Note that the weights file must contain not only C weight values\n");
	fprintf(stderr, "                        but also C bias values, corresponding to each weight.\n");
	fprintf(stderr, "                        The weighted output for a given input is then Input*Weight + Bias.\n");
	fprintf(stderr, "                        The bias values FOLLOW the weight values.\n");
	fprintf(stderr, "\n\nThis program will multiply every column of the input file (for all its rows)\n");
	fprintf(stderr, "with the corresponding column of the weight file. It will return 0 on success\n");
	fprintf(stderr, "and 1 on failure, A.Hadjiprocopis (c) 1998\n");

	return FALSE;
}
