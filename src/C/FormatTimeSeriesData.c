#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define	max(a, b)	( (a) > (b) ? (a):(b) )
#define	min(a, b)	( (a) < (b) ? (a):(b) )

#define	TRUE		1
#define	FALSE		0

/* If you add a command line parameter then increment this constant, if you removed one then decrement. */
/* It is the minimum number of arguments required (including the command itself - argv[0]). */
/* It is IMPORTANT to modify it otherwise the input/output time points will not be correct. */
/* To check the input/output time points, uncomment the following DEBUG define */
/*#define	DEBUG		1*/

int     MyRandom(int MinV, int MaxV);
int	Usage(char *app_name, char *error);

int	main(int argc, char **argv)
{
	FILE	*InputHandle;
	int	NumInputs = 0, NumOutputs = 0, Numbers,
		i, j, MaxNumLines = -1, *InputTimePoints, *OutputTimePoints, MaxTimePoint = 0,
		NumOutputClasses = 0.0, QuantiseInputs = FALSE, limit, offset, Randomised = FALSE,
		HideOutput = FALSE, TimeOffset = 0, flag = TRUE,
		StartInputTimePoints=0, StopInputTimePoints=0,
		StartOutputTimePoints=0, StopOutputTimePoints=0;
	double	Number, MaxNum, MinNum, MaxQ, QNumber,
		QuantisationStep = 0.0, FirstClassAt = 0.0, LastClassAt = 0.0, Sx, *TimeSeries;
	char	InputFilename[512];

	InputFilename[0] = '\0';
	for(i=1;(i<argc)&&flag;i++){
		if( !strncmp(argv[i], "-file", 4) ){
			if( ++i < argc ) strcpy(InputFilename, argv[i]);
			else flag = Usage(argv[0], "-file");
			continue;
		}
		if( !strncmp(argv[i], "-inputs", 4) ){
			if( ++i < argc ) NumInputs = atoi(argv[i]);
			else flag = Usage(argv[0], "-inputs");
			continue;
		}
		if( !strncmp(argv[i], "-outputs", 5) ){
			if( ++i < argc ) NumOutputs = atoi(argv[i]);
			else flag = Usage(argv[0], "-outputs");
			continue;
		}
		if( !strncmp(argv[i], "-lines", 3) ){
			if( ++i < argc ) MaxNumLines = atoi(argv[i]);
			else flag = Usage(argv[0], "-lines");
			continue;
		}
		if( !strncmp(argv[i], "-num_output_classes", 2) ){
			if( ++i < argc ) NumOutputClasses = atoi(argv[i]);
			else flag = Usage(argv[0], "-num_output_classes");
			continue;
		}
		if( !strncmp(argv[i], "-first_class_at", 4) ){
			if( ++i < argc )  FirstClassAt = atof(argv[i]);
			else flag = Usage(argv[0], "-first_class_at");
			continue;
		}
		if( !strncmp(argv[i], "-last_class_at", 3) ){
			if( ++i < argc ) LastClassAt = atof(argv[i]);
			else flag = Usage(argv[0], "-last_class_at");
			continue;
		}
		if( !strncmp(argv[i], "-quantise_inputs", 2) ){
			QuantiseInputs = TRUE;
			continue;
		}
		if( !strncmp(argv[i], "-seed", 2) ){
			if( ++i < argc ){
				if( (j=atoi(argv[i])) > 0 ) srand48(j);
				else srand48((int )time(0));
			} else flag = Usage(argv[0], "");
			Randomised = TRUE;
			continue;
		}
		if( !strncmp(argv[i], "-offset", 3) ){
			if( ++i < argc ) TimeOffset = atoi(argv[i]);
			else flag = Usage(argv[0], "-offset");
			continue;
		}
		if( !strncmp(argv[i], "-hide", 3) ){
			HideOutput = TRUE;
			continue;
		}
		if( !strncmp(argv[i], "-intime", 4) ){
			StartInputTimePoints = ++i;
			for(StopInputTimePoints=i;(StopInputTimePoints<argc)&&(strncmp(argv[StopInputTimePoints],"-",1));StopInputTimePoints++);
		}
		if( !strncmp(argv[i], "-outime", 5) ){
			StartOutputTimePoints = ++i;
			for(StopOutputTimePoints=i;(StopOutputTimePoints<argc)&&(strncmp(argv[StopOutputTimePoints],"-",1));StopOutputTimePoints++);
		}
		if( !strncmp(argv[i], "-usage", 2) || !strncmp(argv[i], "-help", 3) ){
			flag = Usage(argv[0], NULL);
			continue;
		}
	}

	if( (NumInputs<0) || (NumOutputs<0) || (TimeOffset<0) ){
		fprintf(stderr, "%s: Error, illegal values (e.g. < 0) for one or more of the `-inputs'(=%d), `-outputs'(=%d) or `-offset'(=%d) parameters.\a\n", argv[0], NumInputs, NumOutputs, TimeOffset);
		exit(1);
	}
	if( MaxNumLines < 0 ){
		fprintf(stderr, "%s: Error, illegal value (e.g. < 0) for the `-lines'(=%d) parameter.\a\n", argv[0], MaxNumLines);
		exit(1);
	}
	if( InputFilename[0] == '\0' ){
		fprintf(stderr, "%s: Error, input file name was not defined!\a\n", argv[0]);
		exit(1);
	}
	if( (StopInputTimePoints-StartInputTimePoints) != NumInputs ){
		fprintf(stderr, "%s: Error, number of input points (value of `-inputs'(=%d)) does not agree with the actual number of input time points (`-intime'(=%d)).\a\n", argv[0], NumInputs, (StopInputTimePoints-StartInputTimePoints));
		exit(1);
	}
	if( (StopOutputTimePoints-StartOutputTimePoints) != NumOutputs ){
		fprintf(stderr, "%s: Error, number of output points (value of `-outputs'(=%d)) does not agree with the actual number of output time points (`-outime'(=%d)).\a\n", argv[0], NumOutputs, (StopOutputTimePoints-StartOutputTimePoints));
		exit(1);
	}
	if( !flag ) {
		Usage(argv[0], NULL);
		exit(1);
	}

	if( (InputTimePoints=(int *)calloc(NumInputs+2, sizeof(int))) == NULL ){
		fprintf(stderr, "%s: Memory Allocation Error 'InputTimePoints' (%d).\a\n", argv[0], NumInputs);
		exit(1);
	}
	if( (OutputTimePoints=(int *)calloc(NumOutputs+2, sizeof(int))) == NULL ){
		fprintf(stderr, "%s: Memory Allocation Error 'OutputTimePoints' (%d).\a\n", argv[0], NumOutputs);
		exit(1);
	}
	for(i=0;i<NumInputs;i++){
		if( (InputTimePoints[i]=atoi(argv[StartInputTimePoints+i])) <= 0 ){
			fprintf(stderr, "%s: Error Time points must be >= 1.\a\n", argv[0]);
			free(InputTimePoints); free(OutputTimePoints);
			exit(1);
		}
#ifdef DEBUG
		fprintf(stderr, "%d ", InputTimePoints[i]);
#endif
		MaxTimePoint = max(MaxTimePoint, InputTimePoints[i]);
	}
#ifdef DEBUG
		fprintf(stderr, "\n");
#endif
	for(i=0;i<NumOutputs;i++){
		if( (OutputTimePoints[i]=atoi(argv[StartOutputTimePoints+i])) <= 0 ){
			fprintf(stderr, "%s: Error Time points must be >= 1.\a\n", argv[0]);
			free(InputTimePoints); free(OutputTimePoints);
			exit(1);
		}
#ifdef DEBUG
		fprintf(stderr, "%d ", OutputTimePoints[i]);
#endif
		MaxTimePoint = max(MaxTimePoint, OutputTimePoints[i]);
	}
#ifdef DEBUG
		fprintf(stderr, "\n");
#endif
	/* The Input file */
	if( (InputHandle=fopen(InputFilename, "r")) == NULL ){
		fprintf(stderr, "%s: Error Could not open file '<%s'.\a\n", argv[0], InputFilename);
		free(InputTimePoints); free(OutputTimePoints);
		exit(1);
	}

	fscanf(InputHandle, "%lf", &Number);
	MaxNum = MinNum = Number;
	Numbers = 1;
	while( 1 ){
		fscanf(InputHandle, "%lf", &Number);
		if( feof(InputHandle) ) break;
		MaxNum = max(MaxNum, Number);
		MinNum = min(MinNum, Number);
		Numbers++;
	}
	if( MaxNum == MinNum ){
		fprintf(stderr, "%s: The time series does not seem to have much variation. Its minimum and maximum values are the same!\a\n", argv[0]);
		free(InputTimePoints); free(OutputTimePoints);
		fclose(InputHandle);
		exit(1);
	}
	/* Rewind back the input file and then go ahead by the time offset */
	rewind(InputHandle);
	for(i=0;i<TimeOffset;i++){
		fscanf(InputHandle, "%lf", &Number);
	}
	Numbers -= TimeOffset;

	/* Is there enough data? */
	/* We need at max MaxNumLines each spanning from 0 to MaxTimePoint */
	/* Do we have enough for a single line ? */
	if( ((MaxTimePoint+1) * (Numbers / (MaxTimePoint+1))) == 0 ){
		fprintf(stderr, "%s: Error There are not enough numbers in the file.\a\n", argv[0]);
		free(InputTimePoints); free(OutputTimePoints);
		fclose(InputHandle);
		exit(1);
	}

	if( (TimeSeries=(double *)calloc(Numbers+2, sizeof(double))) == NULL ){
		fprintf(stderr, "%s: Memory Allocation Error 'TimeSeries' (%d).\a\n", argv[0], Numbers);
		free(InputTimePoints); free(OutputTimePoints);
		fclose(InputHandle);
		exit(1);
	}

	for(i=0;i<Numbers;i++) fscanf(InputHandle, "%lf", &(TimeSeries[i]));
	fclose(InputHandle);

	limit = Numbers-MaxTimePoint-1;

	/* Initialise offset. There are 2 cases either we want to draw time sub-sequences at random (Seed > 0)
	   or just go over all time points (and therefore have a restriction to the number of output lines) (Seed <= 0) */
	offset = ( (Randomised == TRUE) ? MyRandom(0, limit) : (0) );
	if( NumOutputClasses > 0 ){
		MaxQ = (double )(NumOutputClasses-1) * QuantisationStep + FirstClassAt;
		Sx = (MaxNum - MinNum) / NumOutputClasses;
		if( QuantiseInputs ){
			for(i=0;i<MaxNumLines;i++){
				for(j=0;j<NumInputs-1;j++){
					QNumber = min(MaxQ, ((double )((int )((TimeSeries[InputTimePoints[j]+offset-1]-MinNum)/Sx))) * QuantisationStep + FirstClassAt);
					printf("%lf ", QNumber);
				}
				QNumber = min(MaxQ, ((double )((int )((TimeSeries[InputTimePoints[NumInputs-1]+offset-1]-MinNum)/Sx))) * QuantisationStep + FirstClassAt);
				if( (NumOutputs>0) && (!HideOutput) ){
					printf("%lf\t", QNumber);
					for(j=0;j<NumOutputs-1;j++){
						QNumber = min(MaxQ, ((double )((int )((TimeSeries[OutputTimePoints[j]+offset-1]-MinNum)/Sx))) * QuantisationStep + FirstClassAt);
						printf("%lf ", QNumber);
					}
					QNumber = min(MaxQ, ((double )((int )((TimeSeries[OutputTimePoints[NumOutputs-1]+offset-1]-MinNum)/Sx))) * QuantisationStep + FirstClassAt);
					printf("%lf\n", QNumber);
				} else {
					printf("%lf\n", QNumber);
				}
				offset = ( (Randomised == TRUE) ? MyRandom(0, limit) : (offset+1) );
			}
		} else {
			for(i=0;i<MaxNumLines;i++){
				for(j=0;j<NumInputs-1;j++){
					printf("%lf ", TimeSeries[InputTimePoints[j]+offset-1]);
				}
				if( (NumOutputs>0) && (!HideOutput) ){
					printf("%lf\t", TimeSeries[InputTimePoints[j]+offset-1]);
					for(j=0;j<NumOutputs-1;j++){
						QNumber = min(MaxQ, ((double )((int )((TimeSeries[OutputTimePoints[j]+offset-1]-MinNum)/Sx))) * QuantisationStep + FirstClassAt);
						printf("%lf ", QNumber);
					}
					QNumber = min(MaxQ, ((double )((int )((TimeSeries[OutputTimePoints[NumOutputs-1]+offset-1]-MinNum)/Sx))) * QuantisationStep + FirstClassAt);
					printf("%lf\n", QNumber);
				} else {
					printf("%lf\n", TimeSeries[InputTimePoints[NumInputs-1]+offset-1]);
				}
				offset = ( (Randomised == TRUE) ? MyRandom(0, limit) : (offset+1) );
			}
		}
	} else {
		for(i=0;(i<MaxNumLines)&&(offset<=limit);i++){
			for(j=0;j<NumInputs-1;j++) printf("%lf ", TimeSeries[InputTimePoints[j]+offset-1]);
			if( (NumOutputs>0) && (!HideOutput) ){
				printf("%lf\t", TimeSeries[InputTimePoints[NumInputs-1]+offset-1]);
				for(j=0;j<NumOutputs-1;j++){
					printf("%lf ", TimeSeries[OutputTimePoints[j]+offset-1]);
				}
				printf("%lf\n", TimeSeries[OutputTimePoints[NumOutputs-1]+offset-1]);
			} else {
				printf("%lf\n", TimeSeries[InputTimePoints[NumInputs-1]+offset-1]);
			}
			offset = ( (Randomised == TRUE) ? MyRandom(0, limit) : (offset+1) );
		}
	}

	free(InputTimePoints);
	free(OutputTimePoints);
	free(TimeSeries);

	exit(0);
}


int	MyRandom(int MinV, int MaxV)
{
	int	difference = MaxV - MinV + 1;

	if( difference == 0 )
		return( MinV );
	else
		return( MinV + (int )(lrand48() % ((long )difference)) );
}

int	Usage(char *app_name, char *error)
{
	if( error ) fprintf(stderr, "Error with the `%s' option.\a\n", error);

	fprintf(stderr, "This program will take an input file containing numbers\n");
	fprintf(stderr, "separated by white space and format it as a time series\n");
	fprintf(stderr, "using the input and output time points supplied.\n");
	fprintf(stderr, "     *** All time points start from ONE *** \n\n");
	fprintf(stderr, "Usage: %s options...\n", app_name);
	fprintf(stderr, "Options are (squarish brackets, [], denote optional parameter):\n");
	fprintf(stderr, "  -file  filename    The name of the file holding the data to format.\n");
	fprintf(stderr, "                     It contains numbers separated by white space.\n");

	fprintf(stderr, "  -inputs  INP       The number of inputs, the size of the input vector.\n");
	fprintf(stderr, "  -outputs OUT       The number of outputs, the size of the output vector.\n");
	fprintf(stderr, "  -lines   L         How many lines of output (input+output vectors) to\n");
	fprintf(stderr, "                     produce?\n");
	fprintf(stderr, "  -intime P1 P2 ...  The input time points. There number must be equal to `INP'\n");
	fprintf(stderr, "                     above and they must start from 1.\n");
	fprintf(stderr, "  -outime P1 P2 ...  ditto as above but for the output points (`OUT').\n");
	
	fprintf(stderr, " [-num_output_classes  C] If you want the output to be quantised to C classes.\n");
	fprintf(stderr, " [-first_class_at F] When (see above) C > 0, the first class will be at F.\n");
	fprintf(stderr, " [-last_class_at LA] ... and the last one at LA. All the others will be\n");
	fprintf(stderr, "                     spaced evenly between the two.\n");
	fprintf(stderr, " [-quantise_inputs]  Quantisation can be applied to the inputs as well with\n");
	fprintf(stderr, "                     the same first and last and number of classes as the output.\n");

	fprintf(stderr, " [-seed  S]          Should you want to draw vectors at random from the input file\n");
	fprintf(stderr, "                     then use this option. If S is negative or zero then the seed will be\n");
	fprintf(stderr, "                     taken from a call to `(int )time(0)' (e.g. the current second).\n");

	fprintf(stderr, " [-hide]             Use this flag if you do not want the output time points\n");
	fprintf(stderr, "                     to be displayed (e.g. for test-phase data sets).\n");
	fprintf(stderr, " [-offset T]         Specify T as a positive integer so that the output starts\n");
	fprintf(stderr, "                     from time point T rather than from the beginning (time point 1).\n");

	fprintf(stderr, "Program by A.Hadjiprocopis, (C) Noodle Woman Software.\n");
	fprintf(stderr, "Bugs and suggestions to the author, livantes@soi.city.ac.uk\n");
	fprintf(stderr, "Free to modify, plagiarise, delete and use this program for non-commercial\n");
	fprintf(stderr, "institutions and individuals.\n");

	return FALSE;

}

