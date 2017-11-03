#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define	max(a, b)	( (a) > (b) ? (a):(b) )
#define	min(a, b)	( (a) < (b) ? (a):(b) )

#define	TRUE		1
#define	FALSE		0

int	MyRandom(int, int);
int	Usage(char *app_name);

int	main(int argc, char **argv)
{
	FILE	*InputHandle;
	char	InputFilename[512],
		DoPerColumnNormaliseInputs = FALSE,
		DoPerColumnNormaliseOutputs= FALSE,
		flag = TRUE,
		NormaliseInputs = FALSE, NormaliseOutputs = FALSE,
		QuantiseInputs = FALSE, Randomised = FALSE;
	double	Number, MaxNum, MinNum, MaxQ, QNumber;
	int	NumInputs = -1, NumOutputs = -1, Numbers, NumbersToRead, LinesToRead,
		i, j, MaxNumLines = -1, Seed = (int )time(0);
	int	NumOutputClasses = 0, LinesThereAre, vector;
	double	QuantisationStep = 0.0, FirstClassAt = 0.0, LastClassAt = 0.0, Sx, **InputVectors, **OutputVectors,
		MinNormaliseInputs=-1, MaxNormaliseInputs=-1, MinNormaliseOutputs=-1, MaxNormaliseOutputs=-1,
		MinNumInp, MaxNumInp, MinNumOut, MaxNumOut, scaleNormaliseInputs, scaleNormaliseOutputs,
		*MinValuesPerColumnInput, *MaxValuesPerColumnInput,
		*MinValuesPerColumnOutput, *MaxValuesPerColumnOutput, dummy,
		*scalePerColumnNormaliseInputs, *scalePerColumnNormaliseOutputs;

	InputFilename[0] = '\0';
	/* Get the commands from the command line */
	for(i=1;(i<argc)&&flag;i++){
		if( !strncmp(argv[i], "-file", 4) ){
			if( ++i < argc ) strcpy(InputFilename, argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-normalise_inputs", 16) ){
			if( ++i < argc ) MinNormaliseInputs = atof(argv[i]);
			else { flag = Usage(argv[0]); continue; }
			if( ++i < argc ) MaxNormaliseInputs = atof(argv[i]);
			else flag = Usage(argv[0]);
			if( MaxNormaliseInputs <= MinNormaliseInputs ){ flag = Usage(argv[0]); continue; }
			continue;
		}
		if( !strncmp(argv[i], "-per_column_normalise_inputs", 27) ){
			if( ++i < argc ) MinNormaliseInputs = atof(argv[i]);
			else { flag = Usage(argv[0]); continue; }
			if( ++i < argc ) MaxNormaliseInputs = atof(argv[i]);
			else flag = Usage(argv[0]);
			if( MaxNormaliseInputs <= MinNormaliseInputs ){ flag = Usage(argv[0]); continue; }
			DoPerColumnNormaliseInputs = TRUE;
			continue;
		}
		if( !strncmp(argv[i], "-normalise_output", 16) ){
			if( ++i < argc ) MinNormaliseOutputs = atof(argv[i]);
			else { flag = Usage(argv[0]); continue; }
			if( ++i < argc ) MaxNormaliseOutputs = atof(argv[i]);
			else flag = Usage(argv[0]);
			if( MaxNormaliseOutputs <= MinNormaliseOutputs ){ flag = Usage(argv[0]); continue; }
			continue;
		}
		if( !strncmp(argv[i], "-per_column_normalise_output", 27) ){
			if( ++i < argc ) MinNormaliseOutputs = atof(argv[i]);
			else { flag = Usage(argv[0]); continue; }
			if( ++i < argc ) MaxNormaliseOutputs = atof(argv[i]);
			else flag = Usage(argv[0]);
			if( MaxNormaliseOutputs <= MinNormaliseOutputs ){ flag = Usage(argv[0]); continue; }
			DoPerColumnNormaliseOutputs = TRUE;
			continue;
		}
		if( !strncmp(argv[i], "-inputs", 2) ){
			if( ++i < argc ) NumInputs = atoi(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-outputs", 2) ){
			if( ++i < argc ) NumOutputs = atoi(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-lines", 3) ){
			if( ++i < argc ) MaxNumLines = atoi(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-num_output_classes", 2) ){
			if( ++i < argc ) NumOutputClasses = atoi(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-first_class_at", 4) ){
			if( ++i < argc )  FirstClassAt = atof(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-last_class_at", 3) ){
			if( ++i < argc ) LastClassAt = atof(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-quantise_inputs", 2) ){
			QuantiseInputs = TRUE;
			continue;
		}
		if( !strncmp(argv[i], "-seed", 2) ){
			if( ++i < argc ){
				if( (j=atoi(argv[i])) > 0 ) Seed = j;
				srand(Seed);
				Randomised = TRUE;
			} else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-usage", 2) || !strncmp(argv[i], "-help", 2) ){
			flag = Usage(argv[0]);
			continue;
		}
	}
	if( (NumInputs<=0) || (NumOutputs<=0) || (MaxNumLines<=0) || !flag || (InputFilename[0]=='\0') ){
		if( flag ) Usage(argv[0]);
		exit(1);
	}

	if( NumOutputClasses > 1 ){
		if( LastClassAt <= FirstClassAt ){
			fprintf(stderr, "%s: Some wrong with the output classes.\a\n", argv[0]);
			exit(1);
		}
		QuantisationStep = (LastClassAt - FirstClassAt) / (NumOutputClasses-1);
	}

	if( (InputHandle=fopen(InputFilename, "r")) == NULL ){
		fprintf(stderr, "Error %s: Could not open file '<%s'.\a\n", argv[0], InputFilename);
		exit(1);
	}

	if( (MinValuesPerColumnInput=(double *)malloc(NumInputs*sizeof(double))) == NULL ){
		fprintf(stderr, "%s : could not allocate %d doubles for MinValuesPerColumnInput (input file '%s')\n", argv[0], NumInputs, InputFilename);
		exit(1);
	}
	if( (MaxValuesPerColumnInput=(double *)malloc(NumInputs*sizeof(double))) == NULL ){
		fprintf(stderr, "%s : could not allocate %d doubles for MaxValuesPerColumnInput (input file '%s')\n", argv[0], NumInputs, InputFilename);
		exit(1);
	}
	if( (scalePerColumnNormaliseInputs=(double *)malloc(NumInputs*sizeof(double))) == NULL ){
		fprintf(stderr, "%s : could not allocate %d doubles for scalePerColumnNormaliseInputs (input file '%s')\n", argv[0], NumInputs, InputFilename);
		exit(1);
	}
	if( (MinValuesPerColumnOutput=(double *)malloc(NumOutputs*sizeof(double))) == NULL ){
		fprintf(stderr, "%s : could not allocate %d doubles for MinValuesPerColumnOutput (input file '%s')\n", argv[0], NumOutputs, InputFilename);
		exit(1);
	}
	if( (MaxValuesPerColumnOutput=(double *)malloc(NumOutputs*sizeof(double))) == NULL ){
		fprintf(stderr, "%s : could not allocate %d doubles for MaxValuesPerColumnOutput (input file '%s')\n", argv[0], NumOutputs, InputFilename);
		exit(1);
	}		
	if( (scalePerColumnNormaliseOutputs=(double *)malloc(NumOutputs*sizeof(double))) == NULL ){
		fprintf(stderr, "%s : could not allocate %d doubles for scalePerColumnNormaliseOutputs (input file '%s')\n", argv[0], NumInputs, InputFilename);
		exit(1);
	}

	Numbers = 0; flag = TRUE;
	while( 1 ){
		for(i=0;i<NumInputs;i++){
			fscanf(InputHandle, "%lf", &Number);
			if( feof(InputHandle) ) break;
			if( Numbers == 0 ) MaxNumInp = MinNumInp = Number;
			else {
				MaxNumInp = max(MaxNumInp, Number);
				MinNumInp = min(MinNumInp, Number);
			}
			// are we in the first line?
			if( flag ) MinValuesPerColumnInput[i] = MaxValuesPerColumnInput[i] = Number;
			else {
				MaxValuesPerColumnInput[i] = max(MaxValuesPerColumnInput[i], Number);
				MinValuesPerColumnInput[i] = min(MinValuesPerColumnInput[i], Number);
			}				
			Numbers++;
		}
		for(i=0;i<NumOutputs;i++){
			fscanf(InputHandle, "%lf", &Number);
			if( feof(InputHandle) ) break;
			if( Numbers == NumInputs ) MaxNumOut = MinNumOut = Number;
			else {
				MaxNumOut = max(MaxNumOut, Number);
				MinNumOut = min(MinNumOut, Number);
			}
			if( flag ) MinValuesPerColumnOutput[i] = MaxValuesPerColumnOutput[i] = Number;
			else {
				MaxValuesPerColumnOutput[i] = max(MaxValuesPerColumnOutput[i], Number);
				MinValuesPerColumnOutput[i] = min(MinValuesPerColumnOutput[i], Number);
			}				
			Numbers++;
		}
		flag = FALSE; // not the first line any more
		if( feof(InputHandle) ) break;
	}
/*	for(i=0;i<NumInputs;i++){
		printf("(%f,%f) ", MinValuesPerColumnInput[i], MaxValuesPerColumnInput[i]);
	}
	printf("\n");
	exit(1);
*/	rewind(InputHandle);
	if( MaxNumInp == MinNumInp ){
		fprintf(stderr, "%s: The input data does not seem to have much variation. Its minimum and maximum values are the same!\a\n", argv[0]);
		fclose(InputHandle);
		exit(1);
	}
	if( MaxNumOut == MinNumOut ){
		fprintf(stderr, "%s: The output data does not seem to have much variation. Its minimum and maximum values are the same!\a\n", argv[0]);
		fclose(InputHandle);
		exit(1);
	}
	if( QuantiseInputs ){
		MinNum = max(MinNumOut, MinNumInp);
		MaxNum = max(MaxNumOut, MaxNumInp);
	} else {
		MinNum = MinNumOut;
		MaxNum = MaxNumOut;
	}
	if( MaxNormaliseInputs > MinNormaliseInputs ){
		NormaliseInputs = TRUE;
		scaleNormaliseInputs = (MaxNormaliseInputs - MinNormaliseInputs) / (MaxNumInp - MinNumInp);
		for(i=0;i<NumInputs;i++){
			if( (dummy=(MaxValuesPerColumnInput[i] - MinValuesPerColumnInput[i])) == 0.0 ) scalePerColumnNormaliseInputs[i] = 0.0;
			else scalePerColumnNormaliseInputs[i] = (MaxNormaliseInputs - MinNormaliseInputs) / dummy;
		}
	}
	if( (MaxNormaliseOutputs > MinNormaliseOutputs) && !QuantiseInputs ){
		NormaliseOutputs = TRUE;
		scaleNormaliseOutputs = (MaxNormaliseOutputs - MinNormaliseOutputs) / (MaxNumOut - MinNumOut);
		for(i=0;i<NumOutputs;i++){
			if( (dummy=(MaxValuesPerColumnOutput[i] - MinValuesPerColumnOutput[i])) == 0.0 ) scalePerColumnNormaliseOutputs[i] = 0.0;
			else scalePerColumnNormaliseOutputs[i] = (MaxNormaliseOutputs - MinNormaliseOutputs) / dummy;
		}
	}

//	printf("(%f,%f, %f) ", MinValuesPerColumnInput[49], MaxValuesPerColumnInput[49], scalePerColumnNormaliseInputs[49]);
//	exit(1);
	if( (NumbersToRead=(NumInputs+NumOutputs) * (Numbers / (NumInputs+NumOutputs))) == 0 ){
		fprintf(stderr, "Error %s: There are not enough numbers in the file.\a\n", argv[0]);
		fclose(InputHandle);
		exit(1);
	}
	LinesThereAre = NumbersToRead / (NumInputs+NumOutputs);
	LinesToRead = min(MaxNumLines, NumbersToRead / (NumInputs+NumOutputs));
	/* Allocate Memory for the input and output vectors */
	if( (InputVectors=(double **)malloc(LinesThereAre*sizeof(double *))) == NULL ){
		fprintf(stderr, "%s: Memory Allocation Error (1), %d of double *.\a\n", argv[0], LinesThereAre);
		fclose(InputHandle);
		exit(1);
	}
	if( (OutputVectors=(double **)malloc(LinesThereAre*sizeof(double *))) == NULL ){
		fprintf(stderr, "%s: Memory Allocation Error (2), %d of double *.\a\n", argv[0], LinesThereAre);
		free(InputVectors);
		fclose(InputHandle);
		exit(1);
	}
	for(i=0;i<LinesThereAre;i++){
		if( (InputVectors[i]=(double *)malloc(NumInputs*sizeof(double))) == NULL ){
			fprintf(stderr, "%s: Memory Allocation Error (3, %d), %d of double.\a\n", argv[0], i, NumInputs);
			for(j=0;j<i;j++){ free(InputVectors[j]); free(OutputVectors[j]);}
			free(InputVectors); free(OutputVectors);
			fclose(InputHandle);
			exit(1);
		}
		if( (OutputVectors[i]=(double *)malloc(NumOutputs*sizeof(double))) == NULL ){
			fprintf(stderr, "%s: Memory Allocation Error (4, %d), %d of double.\a\n", argv[0], i, NumOutputs);
			for(j=0;j<i;j++){ free(InputVectors[j]); free(OutputVectors[j]);}
			free(InputVectors); free(OutputVectors);
			fclose(InputHandle);
			exit(1);
		}
	}
fprintf(stderr, "num inputs=%d, outs=%d\n", NumInputs, NumOutputs);
	/* Fill in the input and output vectors */
	for(i=0;i<LinesThereAre;i++){
		for(j=0;j<NumInputs;j++) fscanf(InputHandle, "%lf", &(InputVectors[i][j]));
		for(j=0;j<NumOutputs;j++) fscanf(InputHandle, "%lf", &(OutputVectors[i][j]));
	}
	fclose(InputHandle);

	/* For the sake of efficiency double-write code */
	if( NumOutputClasses > 0 ){	/* Quantise the output as specified */
		Sx = (MaxNum - MinNum) / NumOutputClasses;
		MaxQ = (double )(NumOutputClasses-1) * QuantisationStep + FirstClassAt;
		if( QuantiseInputs ){
			for(i=0;i<LinesToRead;i++){
				vector = ( (Randomised == TRUE) ? MyRandom(0, LinesThereAre) : (i) );
				j = 0;
				if( NumInputs > 1 ){
					for(j=0;j<NumInputs-1;j++){
						QNumber = min(MaxQ, ((double )((int )((InputVectors[vector][j]-MinNum)/Sx))) * QuantisationStep + FirstClassAt);
						printf("%lf ", QNumber);
					}
				}
				QNumber = min(MaxQ, ((double )((int )((InputVectors[vector][j]-MinNum)/Sx))) * QuantisationStep + FirstClassAt);
				printf("%lf\t", QNumber);
				j = 0;
				if( NumOutputs > 1 ){
					for(j=0;j<NumOutputs-1;j++){
						QNumber = min(MaxQ, ((double )((int )((OutputVectors[vector][j]-MinNum)/Sx))) * QuantisationStep + FirstClassAt);
						printf("%lf ", QNumber);
					}
				}
				if( NumOutputs > 0 ){
					QNumber = min(MaxQ, ((double )((int )((OutputVectors[vector][j]-MinNum)/Sx))) * QuantisationStep + FirstClassAt);
					printf("%lf\n", QNumber);
				} else {
					printf("\n");
				}
			}
		} else { // not quantised
			for(i=0;i<LinesToRead;i++){
				vector = ( (Randomised == TRUE) ? MyRandom(0, LinesThereAre) : (i) );
				j = 0;
				if( NumInputs > 1 ){
					if( NormaliseInputs ){
						if( DoPerColumnNormaliseInputs ){
							for(j=0;j<NumInputs-1;j++){
								printf("%lf ", (InputVectors[vector][j]-MinValuesPerColumnInput[j]) * scalePerColumnNormaliseInputs[j] + MinNormaliseInputs);
							}
						} else {
							for(j=0;j<NumInputs-1;j++)
								printf("%lf ", (InputVectors[vector][j]-MinNumInp) * scaleNormaliseInputs + MinNormaliseInputs);
						}
					} else {
						for(j=0;j<NumInputs-1;j++)
							printf("%lf ", InputVectors[vector][j]);
					}
				}
				if( NormaliseInputs ){
					if( DoPerColumnNormaliseInputs ){
						printf("%lf\t", (InputVectors[vector][j]-MinValuesPerColumnInput[j]) * scalePerColumnNormaliseInputs[j] + MinNormaliseInputs);
					} else {
						printf("%lf\t", (InputVectors[vector][j]-MinNumInp) * scaleNormaliseInputs + MinNormaliseInputs);
					}
				} else printf("%lf\t", InputVectors[vector][j]);
				j = 0;
				if( NumOutputs > 1 )
					for(j=0;j<NumOutputs-1;j++){
						QNumber = min(MaxQ, ((double )((int )((OutputVectors[vector][j]-MinNum)/Sx))) * QuantisationStep + FirstClassAt);
						printf("%lf ", QNumber);
					}
				if( NumOutputs > 0 ){
					QNumber = min(MaxQ, ((double )((int )((OutputVectors[vector][j]-MinNum)/Sx))) * QuantisationStep + FirstClassAt);
					printf("%lf\n", QNumber);
				} else printf("\n");
			}
		}
	} else {		/* Normal Output */
		for(i=0;i<LinesToRead;i++){
			vector = ( (Randomised == TRUE) ? MyRandom(0, LinesThereAre) : (i) );
			if( NumInputs > 1 ){
				if( NormaliseInputs ){
					if( DoPerColumnNormaliseInputs ){
						for(j=0;j<NumInputs-1;j++)
							printf("%lf ", (InputVectors[vector][j]-MinValuesPerColumnInput[j]) * scalePerColumnNormaliseInputs[j] + MinNormaliseInputs);
					} else {
						for(j=0;j<NumInputs-1;j++)
							printf("%lf ", (InputVectors[vector][j]-MinNumInp) * scaleNormaliseInputs + MinNormaliseInputs);
					}
				} else {
					for(j=0;j<NumInputs-1;j++)
						printf("%lf ", InputVectors[vector][j]);
				}
			}
			if( NormaliseInputs ){
				if( DoPerColumnNormaliseInputs ){
					printf("%lf ", (InputVectors[vector][j]-MinValuesPerColumnInput[j]) * scalePerColumnNormaliseInputs[j] + MinNormaliseInputs);
				} else {
					printf("%lf ", (InputVectors[vector][j]-MinNumInp) * scaleNormaliseInputs + MinNormaliseInputs);
				}
			} else {
				printf("%lf ", InputVectors[vector][j]);
			}
			j = 0;
			if( NumOutputs > 1 ){
				if( NormaliseOutputs ){
					if( DoPerColumnNormaliseOutputs ){
						printf("%lf ", (OutputVectors[vector][j]-MinValuesPerColumnOutput[j]) * scalePerColumnNormaliseOutputs[j] + MinNormaliseOutputs);
					} else {
						for(j=0;j<NumOutputs-1;j++)
							printf("%lf ", (OutputVectors[vector][j]-MinNumOut) * scaleNormaliseOutputs + MinNormaliseOutputs);
					}
				} else {
					for(j=0;j<NumOutputs-1;j++)
						printf("%lf ", OutputVectors[vector][j]);
				}
			}
			if( NumOutputs > 0 ){
				if( NormaliseOutputs ){
					if( DoPerColumnNormaliseOutputs ){
						printf("%lf ", (OutputVectors[vector][j]-MinValuesPerColumnOutput[j]) * scalePerColumnNormaliseOutputs[j] + MinNormaliseOutputs);
					} else {
						printf("%lf\t", (OutputVectors[vector][j]-MinNumOut) * scaleNormaliseOutputs + MinNormaliseOutputs);
					}
				} else printf("%lf\n", OutputVectors[vector][j]);
			} else printf("\n");
		}
	}

	fclose(InputHandle);

	free(MinValuesPerColumnOutput);
	free(MaxValuesPerColumnOutput);
	free(MinValuesPerColumnInput);
	free(MaxValuesPerColumnInput);
	free(scalePerColumnNormaliseInputs);
	free(scalePerColumnNormaliseOutputs);
	for(i=0;i<LinesThereAre;i++){
		free(InputVectors[i]);
		free(OutputVectors[i]);
	}
	free(InputVectors);
	free(OutputVectors);

	exit(0);
}


int	MyRandom(int MinV, int MaxV)
{
	int	difference = MaxV - MinV - 1;

	if( difference == 0 )
		return( MinV );
	else
		return( MinV + (int )(lrand48() % ((long )difference)) );
}

int	Usage(char *app_name)
{
	fprintf(stderr, "Usage: %s options...\n", app_name);
	fprintf(stderr, "Options are (squarish brackets, [], denote optional parameter):\n");
	fprintf(stderr, "  -file  filename    The name of the file holding the data to format.\n");
	fprintf(stderr, "                     It contains numbers separated by white space.\n");

	fprintf(stderr, "  -inputs  INP       The number of inputs, the size of the input vector.\n");
	fprintf(stderr, "  -outputs OUT       The number of outputs, the size of the output vector.\n");
	fprintf(stderr, "  -lines   L         How many lines of output (input+output vectors) to\n");
	fprintf(stderr, "                     produce?\n");
	fprintf(stderr, " [-num_output_classes  C] If you want the output to be quantised to C classes.\n");
	fprintf(stderr, " [-first_class_at F] When (see above) C > 0, the first class will be at F.\n");
	fprintf(stderr, " [-last_class_at LA] ... and the last one at LA. All the others will be\n");
	fprintf(stderr, "                     spaced evenly between the two.\n");
	fprintf(stderr, " [-quantise_inputs]  Quantisation can be applied to the inputs as well with\n");
	fprintf(stderr, "                     the same first and last and number of classes as the output.\n");
	fprintf(stderr, " [-normalise_inputs A B] Normalise the inputs between the values (real) of A and B - all input columns will be treated as a unified set, therefore normalisation will be over ALL inputs (and not per column, see -per_column... options following)\n");
	fprintf(stderr, " [-normalise_outputs A B] Normalise the outputs between the values (real) of A and B - all output columns will be treated as a unified set, therefore normalisation will be over ALL outputs (and not per column, see -per_column... options following)\n");
	fprintf(stderr, " [-per_column_normalise_inputs A B] Normalise the inputs between the values (real) of A and B PER COLUMN, i.e. each column's numbers will be normalised according to this column's min and max values.\n");
	fprintf(stderr, " [-per_column_normalise_outputs A B] Normalise the outputs between the values (real) of A and B PER COLUMN, i.e. each column's numbers will be normalised according to this column's min and max values.\n");

	fprintf(stderr, " [-seed [S]]         Should you want to draw vectors at random from the input file. If you do not specify this option, the first 'L' lines (specified with -lines option) will be printed at the output. The rest will be ignored.\n");
	fprintf(stderr, "                     then use this option. If S is negative or zero then the seed will be\n");
	fprintf(stderr, "                     taken from a call to `(int )time(0)' (e.g. the current second).\n");

	fprintf(stderr, "Program by A.Hadjiprocopis, (C) Noodle Woman Software.\n");
	fprintf(stderr, "Bugs and suggestions to the author, livantes@soi.city.ac.uk\n");
	fprintf(stderr, "Free to modify, plagiarise, delete and use this program for non-commercial\n");
	fprintf(stderr, "institutions and individuals.\n");

        return FALSE;

}
