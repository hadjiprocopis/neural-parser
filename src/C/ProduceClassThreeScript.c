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
int     PadMe(int max_num, int num, char *str);

int	main(int argc, char **argv)
{

	int	MinimumNumberOfInputsPerFirstLayerFFNN, MaximumNumberOfInputsPerFirstLayerFFNN,
		NumberOfInputs, *InputsToFirstLayerFFNNs, NumberOfFirstLayerFFNNs,
		total_inputs, BreakFlag, i, these_inputs, j, reverse, Seed, NumberOfFFNN;
	char	*buffer;

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

	/* Alloc */
	if( (buffer=(char *)calloc(25, sizeof(char))) == NULL ){
		fprintf(stderr, "%s: Memory Allocation Error: 25 of char.\n", argv[0]);
		exit(1);
	}
	if( (InputsToFirstLayerFFNNs=(int *)calloc(NumberOfInputs+10, sizeof(int))) == NULL ){
		fprintf(stderr, "Memory Allocation Error: %d of int for 'InputsToFirstLayerFFNNs'.\n", NumberOfInputs);
		free(buffer);
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

	NumberOfFFNN = 2*NumberOfFirstLayerFFNNs-1;
	fprintf(stderr, "Number of FFNNs\n\tHorizontal: %d\n\tVertical: %d\n\tTotal: %d\n", NumberOfFirstLayerFFNNs, NumberOfFirstLayerFFNNs-1, NumberOfFFNN);
	/* The First Layer FFNNs */
	these_inputs = 1;
	for(i=0;i<NumberOfFirstLayerFFNNs;i++){
		printf("O%d=([", i+1);
		for(j=0;j<InputsToFirstLayerFFNNs[i]-1;j++){
			printf("%d,", these_inputs);
			these_inputs++;
		}
		if( !PadMe(NumberOfFFNN, i+1, buffer) ){
			fprintf(stderr, "%s: PadMe has failed.\a\n", argv[0]);
			free(buffer); free(InputsToFirstLayerFFNNs);
			exit(1);
		}
		if( i == 0 ){
			printf("%d]),(Y[1]),FFNN_%s,(%d,1),1,1;\n", these_inputs, buffer, InputsToFirstLayerFFNNs[i]);
			/* E1 = O1 - Y1, here you can insert any kind of discrepancy function like abs(..) or (..)*(..) etc. */
			printf("E1=O1[1]-Y[1];\n");
		} else {
			printf("%d]),(E%d[1]),FFNN_%s,(%d,1),%d,1;\n", these_inputs, i, buffer, InputsToFirstLayerFFNNs[i], i+1);
			if( i < (NumberOfFirstLayerFFNNs-1) )
				/* E1 = O1 - Y1, here you can insert any kind of discrepancy function like abs(..) or (..)*(..) etc. */
				printf("E%d=O%d[1]-E%d[1];\n", i+1, i+1, i);
			else	printf("DEL(E%d);\n", i);
		}
		these_inputs++;
	}

	reverse = these_inputs = i;
	/* The Vertical Layer FFNNs. There number is NumberOfFirstLayerFFNNs-1 */
	/* We have to start from the left down side */
	for(i=these_inputs;i<NumberOfFirstLayerFFNNs+these_inputs-2;i++){
		if( !PadMe(NumberOfFFNN, i+1, buffer) ){
			fprintf(stderr, "%s: PadMe has failed.\a\n", argv[0]);
			free(buffer); free(InputsToFirstLayerFFNNs);
			exit(1);
		}
		printf("O%d=(O%d[1]:O%d[1]),(E%d[1]),FFNN_%s,(2,1),%d,1;\n", i+1, i, reverse-1, reverse-2, buffer, i-these_inputs+1);
		printf("DEL(O%d,O%d,E%d);\n", i, reverse-1, reverse-2);
		reverse--;
	}
	if( !PadMe(NumberOfFFNN, i+1, buffer) ){
		fprintf(stderr, "%s: PadMe has failed.\a\n", argv[0]);
		free(buffer); free(InputsToFirstLayerFFNNs);
		exit(1);
	}
	printf("O%d=(O%d[1]:O%d[1]),(Y[1]),FFNN_%s,(2,1),%d,1;\n", i+1, i, reverse-1, buffer, i-these_inputs+1);
	printf("DEL(O1,O%d);\n$\n", i);

	free(buffer); free(InputsToFirstLayerFFNNs);
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

/* for example max_num = 1021 and num = 23 then str = "0023" ok? */
int     PadMe(int max_num, int num, char *str)
{
	int     max_digits = 2, digits = 1, i, ori_num = num;
	char    *str2;
			
	while( (max_num/10)>0 ){ max_num /= 10; max_digits++; }
	while( (num/10)>0 ){ num /= 10; digits++; }

	strcpy(str, "");
	if( (str2=(char *)calloc(max_digits-digits+2, sizeof(char))) == NULL ){
		fprintf(stderr, "Malloc\n\a");
		return FALSE;
	}
	for(i=0;i<max_digits-digits;i++) strcat(str2, "0");
	sprintf(str, "%s%d", str2, ori_num);
	free(str2);
	return TRUE;
}

