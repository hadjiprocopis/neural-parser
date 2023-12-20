#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define	MAX(a, b)	( (a)>(b) ? (a):(b) )
#define	MIN(a, b)	( (a)<(b) ? (a):(b) )
#define	ABS(a)		( (a)<0   ? (-(a)):(a) )
#define	TRUE		1
#define	FALSE		0

int	Usage(char *app_name);

int	main(int argc, char **argv)
{
	FILE	*file;
	int	Values, flag = TRUE, StandardiseFlag = FALSE,
		StatisticsFlag = FALSE, i;
	double	Value, Maximum, Minimum, MaxValue = 1.0, MinValue = 0.0, l,
		RemoveSpikesPercent = 0.0, Average, NormalisedValue,
		RSPmin, RSPmax, sq_sum, StDev, a;
	char	InputFilename[256];

	InputFilename[0] = '\0';

	/* Get the commands from the command line */
	for(i=1;(i<argc)&&flag;i++){
		if( !strncmp(argv[i], "-input", 2) ){
			if( ++i < argc ) strcpy(InputFilename, argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-min", 4) ){
			if( ++i < argc ) MinValue = (double )atof(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-max", 4) ){
			if( ++i < argc ) MaxValue = (double )atof(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-clip", 2) ){
			if( ++i < argc ) RemoveSpikesPercent = (double )atof(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-standardise", 5) ){
			StandardiseFlag = TRUE;
			continue;
		}
		if( !strncmp(argv[i], "-statistics", 5) ){
			StatisticsFlag = TRUE;
			continue;
		}
	}

	if( !flag ) exit(1);
	/* Check if the parameters make sense */
	if( (InputFilename[0]=='\0') || (MinValue>=MaxValue) || (RemoveSpikesPercent<0.0) ){
		Usage(argv[0]);
		exit(1);
	}

	if( (file=fopen(InputFilename, "r")) == NULL ){
		fprintf(stderr, "%s: Error, could not open file '%s'.\n", argv[0], InputFilename);
		exit(1);
	}

	Maximum = -100000.0;
	Minimum =  100000.0;
	sq_sum = Average = 0.0; Values = 0;
	while( 1 ){
		fscanf(file, "%lf", &Value);
		if( feof(file) ) break;
		Maximum = MAX( Maximum, Value );
		Minimum = MIN( Minimum, Value );
		Average += Value;
		sq_sum += (Value * Value);
		Values++;
	}
	rewind(file);
	Average /= Values;
	StDev = sqrt( sq_sum / Values - Average*Average );

	if( StatisticsFlag ){
		printf("# Mean\t\tStandard Deviation\tMinimum\t\tMaximum\n  %lf\t%lf\t\t%lf\t%lf\n", Average, StDev, Minimum, Maximum);
		fclose(file);
		exit(0);
	}
	if( StandardiseFlag ){
		while( 1 ){
			fscanf(file, "%lf", &Value);
			if( feof(file) ) break;
			printf("%lf\n", (Value-Average)/StDev);
		}
		fclose(file);
		exit(0);
	}
	if( RemoveSpikesPercent > 0 ){
		a = (RemoveSpikesPercent * StDev) / 100.0;
		RSPmax = Average + a; RSPmin = Average - a;
		Maximum = MIN(Maximum, RSPmax);
		Minimum = MAX(Minimum, RSPmin);
	}
	l = (MaxValue - MinValue) / (Maximum - Minimum);
	/*fprintf(stderr, "%lf %lf %lf %lf %lf\n", l, MaxValue, MinValue, Maximum, Minimum);*/
	if( Maximum == Minimum ){
		fprintf(stderr, "%s: Error data file contains atonic (standard deviation is zero!) data.\a\n", argv[0]);
		fclose(file);
		exit(1);
	}

	if( RemoveSpikesPercent > 0 ){
		for(i=0;i<Values;i++){
			fscanf(file, "%lf", &Value);
			Value = MAX(RSPmin, MIN(RSPmax, Value));
			NormalisedValue = l * (Value - Minimum) + MinValue;
			printf("%lf\n", NormalisedValue);
		}
		fprintf(stderr, "%s: Removed everything not in the range of %lf to %lf.\n", argv[0], RSPmin, RSPmax);
	} else {
		for(i=0;i<Values;i++){
			fscanf(file, "%lf", &Value);
			NormalisedValue = l * (Value - Minimum) + MinValue;
			printf("%lf\n", NormalisedValue);
		}
	}

	fclose(file);
	exit(0);
}

int	Usage(char *app_name)
{
	fprintf(stderr, "Usage: %s options...\n", app_name);
	fprintf(stderr, "Options are:\n");
	fprintf(stderr, "  -input filename       The name of the file holding the data\n");
	fprintf(stderr, "                        to be normalised/standardised.\n");
	fprintf(stderr, " [-min MIN]\n");
	fprintf(stderr, " [-max MAX]             Normalised data will be within the range MIN-MAX.\n");
	fprintf(stderr, "                        If either of the limits is not given, 0 for min and 1\n");
	fprintf(stderr, "                        for max will be used as default values.\n");
	fprintf(stderr, " [-clip N]              N is a number representing the allowable deviation from\n");
	fprintf(stderr, "                        the mean as a percentage of the standard deviation.\n");
	fprintf(stderr, "                        Data below or above the mean by N %% of the standard deviation\n");
	fprintf(stderr, "                        will be clipped to MIN, MAX values respectively.\n");
	fprintf(stderr, " [-standardise]         Perform only standardisation of the input data. This process\n");
	fprintf(stderr, "                        consists of shifting the data to zero mean and unit standard\n");
	fprintf(stderr, "                        deviation.\n");
	fprintf(stderr, " [-statistics]          It will display the mean and standard deviation of the data.\n");
	fprintf(stderr, "\n\nThis program will read data as a single column. It will return 0 if something\n");
	fprintf(stderr, "went wrong, or 1 if successful. The output will be sent to standard output.\n");
	fprintf(stderr, "A.Hadjiprocopis (c) 1998\n");

	return FALSE;
}
