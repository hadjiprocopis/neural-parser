#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

#define MAX(a, b)	( (a)>(b) ? (a):(b) )
#define MIN(a, b)	( (a)<(b) ? (a):(b) )
#define	TRUE		1
#define	FALSE		0
#define ABS(a)		( (a)<0 ? (-(a)):(a) )

#define MAX_RANDOM_NUMBER	1073741824

int	MyRandom(int MinV, int MaxV);

int	main(int argc, char **argv)
{

	int	MinimumNumberOfInputsPerFirstLayerFFNN, MaximumNumberOfInputsPerFirstLayerFFNN,
		NumberOfInputs, *InputsToFirstLayerFFNNs, NumberOfFirstLayerFFNNs,
		total_inputs, BreakFlag, i, these_inputs, j, reverse, Seed, Eunits;

	if( argc < 6 ){
		fprintf(stderr, "Usage: %s NumberOfInputs Entity|Single MinimumNumberOfInputsPerFirstLayerFFNN MaximumNumberOfInputsPerFirstLayerFFNN Seed[Any|positive integer]\n", argv[0]);
		exit(1);
	}

	if( (NumberOfInputs=atoi(argv[1])) <= 1 ){
		fprintf(stderr, "Number of inputs must be a positive integer > 1.\n");
		exit(1);
	}
	if( toupper(argv[2][0]) == 'S' ){
		printf("O1 = ");
		for(i=1;i<NumberOfInputs;i++)
			printf("X%d, ", i);
		printf("X%d ; Y : FFNN_1\n", i);
		exit(0);
	}
	if( (MinimumNumberOfInputsPerFirstLayerFFNN=atoi(argv[3])) <= 0 ){
		fprintf(stderr, "The minimum number of inputs per first layer FFNN must be a positive integer.\n");
		exit(1);
	}
	if( (MaximumNumberOfInputsPerFirstLayerFFNN=atoi(argv[4])) <= 0 ){
		fprintf(stderr, "The maximum number of inputs per first layer FFNN must be a positive integer.\n");
		exit(1);
	}
	if( MaximumNumberOfInputsPerFirstLayerFFNN < MinimumNumberOfInputsPerFirstLayerFFNN ){
		fprintf(stderr, "The maximum number of inputs per first layer FFNN must be greater than the minimum.\n");
		exit(1);
	}
	if( toupper(argv[5][0]) == 'A' ){
		Seed = (int )time(0);
	} else {
		Seed = ABS(atoi(argv[5]));
	}
		
	printf("# Command line:"); 
	for(i=0;i<argc;i++)
		printf(" %s", argv[i]); 
	printf("\n\n");


	if( (InputsToFirstLayerFFNNs=(int *)calloc(NumberOfInputs+10, sizeof(int))) == NULL ){
		fprintf(stderr, "Memory Allocation Error: %d of int for 'InputsToFirstLayerFFNNs'.\n", NumberOfInputs);
		exit(1);
	}

	srand48(Seed);
	NumberOfFirstLayerFFNNs = 0;
	total_inputs = 0;
	BreakFlag = FALSE;
	while( !BreakFlag ){
		these_inputs = MyRandom(MinimumNumberOfInputsPerFirstLayerFFNN, MaximumNumberOfInputsPerFirstLayerFFNN);
		if( (total_inputs+these_inputs) <= NumberOfInputs ){
			InputsToFirstLayerFFNNs[NumberOfFirstLayerFFNNs++] = these_inputs;
			if( (total_inputs+=these_inputs) == NumberOfInputs )
				BreakFlag = TRUE;
		} else {
			these_inputs = NumberOfInputs - total_inputs;
			if( these_inputs == 1 ){
				for(i=0;i<NumberOfFirstLayerFFNNs;i++){
					if( InputsToFirstLayerFFNNs[i] > MinimumNumberOfInputsPerFirstLayerFFNN ){
						InputsToFirstLayerFFNNs[i]--;
						these_inputs++;
						break;
					}
				}
			}
			InputsToFirstLayerFFNNs[NumberOfFirstLayerFFNNs++] = these_inputs;
			BreakFlag = TRUE;
		}
	}

	fprintf(stderr, "Number of FFNNs\n\tHorizontal: %d\n\tVertical: %d\n\tTotal: %d + 1(last) = %d\n", NumberOfFirstLayerFFNNs, NumberOfFirstLayerFFNNs-1, 2*NumberOfFirstLayerFFNNs-1, 2*NumberOfFirstLayerFFNNs);
	/* The First Layer FFNNs */
	these_inputs = 1; Eunits = 0;
	for(i=0;i<NumberOfFirstLayerFFNNs;i++){
		printf("O%d=([", i+1);
		for(j=0;j<InputsToFirstLayerFFNNs[i]-1;j++){
			printf("%d,", these_inputs);
			these_inputs++;
		}
		if( i == 0 ){
			printf("%d]),(Y[1]),FFNN_%d,(%d,1),1,1;\n", these_inputs, i+1, InputsToFirstLayerFFNNs[i]);
			/* E1 = O1 - Y1, here you can insert any kind of discrepancy function like abs(..) or (..)*(..) etc. */
			printf("E1=O1[1]-Y[1];\n");
			Eunits++;
		} else {
			printf("%d]:E%d[1]),(Y[1]),FFNN_%d,(%d,1),%d,1;\n", these_inputs, i, i+1, InputsToFirstLayerFFNNs[i]+1,i+1);
			if( i < (NumberOfFirstLayerFFNNs-1) ){
				/* E1 = O1 - Y1, here you can insert any kind of discrepancy function like abs(..) or (..)*(..) etc. */
				printf("E%d=O%d[1]-Y[1];\n", i+1, i+1, i);
				Eunits++;
			}
		}
		these_inputs++;
	}
	/* Enhancement to Class Four script: The output of all O units is sent to a last O unit. */
	printf("O%d=(", i+1);
	for(i=0;i<NumberOfFirstLayerFFNNs-1;i++)
		printf("O%d[1]:", i+1);
	printf("O%d[1]),(Y[1]),FFNN_%d,(%d,1),%d,1;\n", i+1, NumberOfFirstLayerFFNNs+1, NumberOfFirstLayerFFNNs, NumberOfFirstLayerFFNNs+1);


	/* Now delete all the E units */
	printf("DEL(");
	for(i=0;i<Eunits-1;i++) printf("E%d,", i+1);
	printf("E%d);\n", i+1);
	/* ... and all the O units, apart from the last one */
	printf("DEL(");
	for(i=0;i<NumberOfFirstLayerFFNNs-1;i++) printf("O%d,", i+1);
	printf("O%d);\n$\n", i+1);

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

