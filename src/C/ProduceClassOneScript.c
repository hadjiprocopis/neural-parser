#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h> 

#define	CEIL(a)		( ((a)-(int )(a))>=0.5 ? ((a)+1.0):(a) )
#define	ROUND(a)	( ((float )(a)) < 0.5  ?  0:1 )
#define MAX(a, b)	( (a)>(b) ? (a):(b) )
#define MIN(a, b)	( (a)<(b) ? (a):(b) )
#define	TRUE		1
#define	FALSE		0
#define	ABS(a)		( (a)<0 ? (-(a)):(a) )
#ifndef M_LN10
#define	M_LN10		2.30258509299404568401
#endif

#define	MAX_LAYERS		50
#define	MAX_INPUTS_PER_FFNN	16
#define MAX_RANDOM_NUMBER	1073741824
#define	ARCH_WORD_LENGTH	16

int	Usage(char *app_name);
double	RanD(double MinV, double MaxV);
void	ResetInputsArray(int *inputs_array, int *inputs_stack, int inputs_number);
int	RetrieveInputElement(int *inputs_array, int *inputs_stack, int inputs_number, int *elements_in_stack);
int	RetrieveOutputElement(int *outputs_array, int *outputs_stack, int inputs_number, int *elements_in_stack);
void	PadMe(int max_num, int num, char *str);

int	main(int argc, char **argv)
{

	int	NumberOfElementsInInputsStack = 0, MinNumberOfInputsPerFFNN, MaxNumberOfInputsPerFFNN,
		*IndFirstLayerInputsArray, *IndOtherLayerInputsArray, *InputsArray, *InputsStack,
		n1, n2, n3, elem, BreakFlag, index, NumberOfOutputs, *UsedOutputs, LastLayerNumberOfInputs,
		Layer[MAX_LAYERS], CumulativeLayer[MAX_LAYERS], lm, i, j, k, l, decision, Iterations, lengthD,
		lengthO, index_del, NumberOfFFNN, flag = TRUE, SingleFlag = FALSE, Seed = (int )time(0);
	char	****Architecture, *env_string, dummy[200], *buffer, *ArchD, *ArchO, *ToBeDeleted, **Deleted,
		UnitBaseName[64];
	double	C1, C2, A, B, x, fat = 0.0;

	strcpy(UnitBaseName, "FFNN"); /* default unit name */
	Layer[0] = MinNumberOfInputsPerFFNN = MaxNumberOfInputsPerFFNN = -1;

        /* Get the commands from the command line */
	for(i=1;(i<argc)&&flag;i++){
		if( !strncmp(argv[i], "-inputs", 2) ){
			if( ++i < argc ) Layer[0] = atoi(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-name", 2) ){
			if( ++i < argc ) strcpy(UnitBaseName, argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-single", 3) ){
			SingleFlag = TRUE;
			continue;
		}
		if( !strncmp(argv[i], "-mininputs", 3) ){
			if( ++i < argc ) MinNumberOfInputsPerFFNN = atoi(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-maxinputs", 3) ){
			if( ++i < argc ) MaxNumberOfInputsPerFFNN = atoi(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-seed", 3) ){
			if( ++i < argc ){
				if( (j=atoi(argv[i])) > 0 ) Seed = j;
			} else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-fat", 4) ){
			if( ++i < argc ){
				if( (x=atof(argv[i])) ) fat = x;
			} else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-c", 2) ){
			if( ++i < argc ) C1 = atof(argv[i]);
			else flag = Usage(argv[0]);
			if( ++i < argc ) C2 = atof(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-usage", 2) || !strncmp(argv[i], "-help", 2) ){
			flag = Usage(argv[0]);
			continue;
		}
	}

	/* Check if parameters are valid */
	if( (Layer[0]<=1) || (MinNumberOfInputsPerFFNN<=0) || (MaxNumberOfInputsPerFFNN<=0) || (MaxNumberOfInputsPerFFNN<MinNumberOfInputsPerFFNN) || !flag || (UnitBaseName[0]=='\0') ){
		if( flag ) Usage(argv[0]);
		exit(1);
	}
	if( Layer[0] < MaxNumberOfInputsPerFFNN ) MaxNumberOfInputsPerFFNN = Layer[0];

	/* if single specified, just do it and exit gracefully */
	if( SingleFlag ){
		printf("O1 = ");
		for(i=1;i<Layer[0];i++)
			printf("X%d, ", i);
		printf("X%d ; Y : %s_01\n", i, UnitBaseName);
		exit(0);
	}

	/* ...otherwise... */
	if( C1 <= 0 ){
		fprintf(stderr, "%s: Using default value for parameter C1 (the number of first layer units).\n", argv[0]);
		C1 = 2 * Layer[0]/(MaxNumberOfInputsPerFFNN+MinNumberOfInputsPerFFNN);
	}
	if( C2 <= 0 ){
		fprintf(stderr, "%s: Using default value for parameter C2 (the number of layers).\n", argv[0]);
		C2 = 5;
	}

	if( fat > 0.0 ){
		/* Exponential Decay Fit with user-specified shape */
		lm =  (int )C2;
		C2 = (double )lm;
		A = log(1.0/C1) / (C2-1);
		for(x=0;x<C2;x+=1.0){
			Layer[(int )x+1] = MAX(1, (int )(C1*exp(A*(x*x+x)/fat)));
			if( x == 0 )
				CumulativeLayer[(int )x] = Layer[(int )x+1];
			else
				CumulativeLayer[(int )x] = CumulativeLayer[(int )x-1] + Layer[(int )x+1];
		}
		if( Layer[lm] != 1 ) Layer[lm] = 1;
		if( Layer[lm-1] == 1 ){
			if( (x=(Layer[lm-2]/2)) > 1 )
				Layer[lm-1] = (int )x;
			else
				lm--;
		}
		fprintf(stderr, "%s : you can see the fitness function using gnuplot: plot %.0lf*exp(%lf*(x*x+x)/%lf)\n", argv[0], C1, A, fat);
	} else {
		/* Exponential Decay Fit */
		lm =  (int )C2;
		C2 = (double )lm;
		A = log(1.0/C1) / (C2-1);
		for(x=0;x<C2;x+=1.0){
			Layer[(int )x+1] = MAX(1, (int )(C1*exp(A*x)));
			if( x == 0 )
				CumulativeLayer[(int )x] = Layer[(int )x+1];
			else
				CumulativeLayer[(int )x] = CumulativeLayer[(int )x-1] + Layer[(int )x+1];
		}
		if( Layer[lm] != 1 ) Layer[lm] = 1;
		if( Layer[lm-1] == 1 ){
			if( (x=(Layer[lm-2]/2)) > 1 )
				Layer[lm-1] = (int )x;
			else
				lm--;
		}
		fprintf(stderr, "%s : you can see the fitness function using gnuplot:plot %lf*exp(%lf*x)\n", argv[0], C1, A);
	}
	fprintf(stderr, "%s: Using C1 = %lf, C2 = %lf\n", argv[0], C1, C2);
	fprintf(stderr, "%s: Architecture = ", argv[0]);
	index = 0;
	for(i=1;i<lm+1;i++){
		fprintf(stderr, "%d ", Layer[i]);
		index += Layer[i];
	}
	fprintf(stderr, "\n%s: Number of inputs(X) = %d, Total number of units = %d\n", argv[0], Layer[0], index);
	NumberOfFFNN = NumberOfOutputs = index;
	printf("# Command line:");
	for(i=0;i<argc;i++)
		printf(" %s", argv[i]);
	printf("\n");
	printf("# total number of units = %d\n\n", index);

	LastLayerNumberOfInputs = -1;
	if( (MaxNumberOfInputsPerFFNN*Layer[lm]) < Layer[lm-1] ){
		LastLayerNumberOfInputs = (Layer[lm-1]+3) / Layer[lm];
		fprintf(stderr, "%s: Increasing the Max Number of Inputs from %d to %d, for the last layer of units in order to accomodate all the outputs of the previous layer.\n", argv[0], MaxNumberOfInputsPerFFNN, LastLayerNumberOfInputs);
	}

	/* Alloc */
	if( (buffer=(char *)calloc(25, sizeof(char))) == NULL ){
		fprintf(stderr, "%s: Memory Allocation Error: 25 of char.\n", argv[0]);
		exit(1);
	}
	if( (InputsArray=(int *)calloc(Layer[0]+1, sizeof(int))) == NULL ){
		fprintf(stderr, "%s: Memory Allocation Error: %d of int.\n", argv[0], Layer[0]);
		exit(1);
	}
	if( (InputsStack=(int *)calloc(Layer[0]+1, sizeof(int))) == NULL ){
		fprintf(stderr, "%s: Memory Allocation Error: %d of int.\n", argv[0], Layer[0]);
		exit(1);
	}
	if( (IndFirstLayerInputsArray=(int *)calloc(Layer[0]+2, sizeof(int))) == NULL ){
		fprintf(stderr, "%s: Memory Allocation Error: %d of int.\n", argv[0], Layer[0]);
		exit(1);
	}
	if( (Architecture=(char ****)calloc(lm+1, sizeof(char ***))) == NULL ){
		fprintf(stderr, "%s: Memory Allocation Error: %d of char ****.\n", argv[0], lm);
 		exit(1);
	}
	n1 = MaxNumberOfInputsPerFFNN;
	for(i=1;i<=lm;i++){
		if( (Architecture[i-1]=(char ***)calloc(Layer[i]+4, sizeof(char **))) == NULL ){
			fprintf(stderr, "%s: Memory Allocation Error: %d of char ***.\n", argv[0], Layer[i]);
			exit(1);
		}
		if( (i==lm) && (LastLayerNumberOfInputs>0) )
			n1 = LastLayerNumberOfInputs;
		for(j=0;j<Layer[i];j++){
			if( (Architecture[i-1][j]=(char **)calloc(n1+2, sizeof(char *))) == NULL ){
				fprintf(stderr, "%s: Memory Allocation Error: %d of char **.\n", argv[0], n1);
				exit(1);
			}
			for(k=0;k<n1+2;k++){
/*				printf("Alloc (%d,%d,%d,8)\n", i-1, j, k);*/
				if( (Architecture[i-1][j][k]=(char *)calloc(ARCH_WORD_LENGTH, sizeof(char))) == NULL ){
					fprintf(stderr, "%s: Memory Allocation Error: %d of char *.\n", argv[0], (int )ARCH_WORD_LENGTH);
					exit(1);
				}
			}
		}
	}
	if( (IndOtherLayerInputsArray=(int *)calloc(n1+2, sizeof(int))) == NULL ){
		fprintf(stderr, "%s: Memory Allocation Error: %d of int.\n", argv[0], n1);
		exit(1);
	}
	if( (ArchD=(char *)calloc((MaxNumberOfInputsPerFFNN+2)*ARCH_WORD_LENGTH, sizeof(char))) == NULL ){
		fprintf(stderr, "%s: Memory Allocation Error: %d of char *.\n", argv[0], (MaxNumberOfInputsPerFFNN+2)*ARCH_WORD_LENGTH);
		exit(1);
	}
	if( (ArchO=(char *)calloc((MaxNumberOfInputsPerFFNN+2)*ARCH_WORD_LENGTH, sizeof(char))) == NULL ){
		fprintf(stderr, "%s: Memory Allocation Error: %d of char *.\n", argv[0], (MaxNumberOfInputsPerFFNN+2)*ARCH_WORD_LENGTH);
		exit(1);
	}
	if( (Deleted=(char **)calloc(NumberOfOutputs+1, sizeof(char *))) == NULL ){
		fprintf(stderr, "%s: Memory Allocation Error: %d of char *.\n", argv[0], NumberOfOutputs);
		exit(1);
	}
	for(i=0;i<NumberOfOutputs;i++){
		if( (Deleted[i]=(char *)calloc(ARCH_WORD_LENGTH, sizeof(char))) == NULL ){
			fprintf(stderr, "%s: Memory Allocation Error: %d of char .\n", argv[0], ARCH_WORD_LENGTH);
			exit(1);
		}
	}
	if( (ToBeDeleted=(char *)calloc(NumberOfOutputs*(ARCH_WORD_LENGTH+3), sizeof(char))) == NULL ){
		fprintf(stderr, "%s: Memory Allocation Error: %d of char .\n", argv[0], NumberOfOutputs*(ARCH_WORD_LENGTH+3));
		exit(1);
	}

	/* Init */
	srand48(Seed);
	NumberOfElementsInInputsStack = Layer[0];
	ResetInputsArray(InputsArray, InputsStack, Layer[0]);

	/* Produce the script */
	/* First Layer */
	for(i=0;i<Layer[1];i++){
		n1 = (int )RanD(MinNumberOfInputsPerFFNN, MaxNumberOfInputsPerFFNN);
		sprintf(Architecture[0][i][0], "%d", n1);
		n2 = 0;
		for(j=0;j<MaxNumberOfInputsPerFFNN;j++)
			IndFirstLayerInputsArray[j] = -1;
		for(j=1;j<=n1;j++){
			BreakFlag = FALSE;
			while( !BreakFlag ){
				elem = RetrieveInputElement(InputsArray, InputsStack, Layer[0], &NumberOfElementsInInputsStack);
				BreakFlag = TRUE;
				for(k=0;k<n2;k++){
					if( IndFirstLayerInputsArray[k] == elem ){
						BreakFlag = FALSE;
						break;
					}
				}
			}
			IndFirstLayerInputsArray[n2++] = elem;
			sprintf(Architecture[0][i][j], "%d", elem);
		}
	}

	/* The Middle Layers */
	for(l=2;l<lm;l++){
		for(i=0;i<Layer[l];i++){
			n1 = (int )RanD(MinNumberOfInputsPerFFNN, MaxNumberOfInputsPerFFNN);
			sprintf(Architecture[l-1][i][0], "%d", n1);
			n2 = 0; n3 = 0;
			for(j=0;j<MaxNumberOfInputsPerFFNN;j++){
				IndFirstLayerInputsArray[j] = -1;
				IndOtherLayerInputsArray[j] = -1;
			}
			for(j=1;j<=n1;j++){
				BreakFlag = FALSE;
				/* Which layer is this output coming from? */
				/* 45% from previous layer, 20% from X layer, 35% from O layer */
				decision = RanD(0.0, 100.0);
				if( l == 2 ) decision = RanD(35.0, 100.0); /* ie from previous O or X */
				if( decision >= 55.0 ){ /* From the previous layer */
					Iterations = 0;
					while( (!BreakFlag) && (Iterations++<=4*Layer[l-1]) ){
						if( l == 2 )
							elem = (int )RanD(-0.9, Layer[l-1]);
						else
							elem = (int )RanD(-0.9, Layer[l-1]);
						BreakFlag = TRUE;
						for(k=0;k<n3;k++){
							if( IndOtherLayerInputsArray[k] == elem ){
								BreakFlag = FALSE;
								break;
							}
						}
					}
					if( BreakFlag == FALSE ){
						j--;
						continue;
					} else {
						IndOtherLayerInputsArray[n3++] = elem;
						PadMe(NumberOfFFNN, elem+1, buffer);
						sprintf(Architecture[l-1][i][j], "O%s", buffer);
					}
				} else {
					if( decision >= 35.0 ){ /* from X layer */
						Iterations = 0;
						while( (!BreakFlag) && (Iterations++<=4*Layer[0]) ){
							elem = RetrieveInputElement(InputsArray, InputsStack, Layer[0], &NumberOfElementsInInputsStack);
							BreakFlag = TRUE;
							for(k=0;k<n2;k++){
								if( IndFirstLayerInputsArray[k] == elem ){
									BreakFlag = FALSE;
									break;
								}
							}
						}
						if( BreakFlag == FALSE ){
							j--;
							continue;
						} else {
							IndFirstLayerInputsArray[n2++] = elem;
							sprintf(Architecture[l-1][i][j], "%d", elem);
						}
					} else { /* From any O layer */
						Iterations = 0;
						while( (!BreakFlag) && (Iterations++<=4*CumulativeLayer[l-3]) ){
							elem = (int )RanD(-0.9, CumulativeLayer[l-3]-0.1);
							BreakFlag = TRUE;
							for(k=0;k<n3;k++){
								if( IndOtherLayerInputsArray[k] == elem ){
									BreakFlag = FALSE;
									break;
								}
							}
						}
						if( BreakFlag == FALSE ){
							j--;
							continue;
						} else {
							IndOtherLayerInputsArray[n3++] = elem;
							PadMe(NumberOfFFNN, elem+1, buffer);
							sprintf(Architecture[l-1][i][j], "O%s", buffer);
						}
					}
				}
			}
		}
	}

	/* The Last Layer */
	for(i=0;i<Layer[lm];i++){
		n1 = ( (LastLayerNumberOfInputs>0) ? (LastLayerNumberOfInputs):((int )RanD(MinNumberOfInputsPerFFNN, MaxNumberOfInputsPerFFNN)) );
		sprintf(Architecture[lm-1][i][0], "%d", n1);
		n2 = 0; n3 = 0;
		for(j=0;j<MaxNumberOfInputsPerFFNN;j++){
			IndFirstLayerInputsArray[j] = -1;
			IndOtherLayerInputsArray[j] = -1;
		}
		for(j=1;j<=n1;j++){
			BreakFlag = FALSE;
			/* Which layer is this output coming from? */
			/* 85% from previous layer, 5% from X layer, 10% from O layer */
			decision = RanD(0.0, 100.0);
			if( lm == 2 ) decision = RanD(10.0, 100.0); /* ie from Previous O and X */
			if( decision >= 95.0 ){ /* from X layer */
				Iterations = 0;
				while( (!BreakFlag) && (Iterations++<=4*Layer[0]) ){
					elem = RetrieveInputElement(InputsArray, InputsStack, Layer[0], &NumberOfElementsInInputsStack);
					BreakFlag = TRUE;
					for(k=0;k<n2;k++){
						if( IndFirstLayerInputsArray[k] == elem ){
							BreakFlag = FALSE;
							break;
						}
					}
				}
				if( BreakFlag == FALSE ){
					j--;
					continue;
				} else {
					IndFirstLayerInputsArray[n2++] = elem;
					sprintf(Architecture[lm-1][i][j], "%d", elem);
				}
			} else {
				if( decision >= 10.0 ){ /* From the previous layer */
					Iterations = 0;
					while( (!BreakFlag) && (Iterations++<=4*Layer[lm-1]) ){
						if( lm == 2 )
							elem = (int )RanD(-0.9, CumulativeLayer[lm-1]);
						else
							elem = (int )RanD(CumulativeLayer[lm-3]+0.1, CumulativeLayer[lm-2]-0.1);
						BreakFlag = TRUE;
						for(k=0;k<n3;k++){
							if( IndOtherLayerInputsArray[k] == elem ){
								BreakFlag = FALSE;
								break;
							}
						}
					}
					if( BreakFlag == FALSE ){
						j--;
						continue;
					} else {
						IndOtherLayerInputsArray[n3++] = elem;
						PadMe(NumberOfFFNN, elem+1, buffer);
						sprintf(Architecture[lm-1][i][j], "O%s", buffer);
					}
				} else { /* From any O layer */
					Iterations = 0;
					while( (!BreakFlag) && (Iterations++<=4*NumberOfOutputs) ){
						elem = (int )RanD(-0.9, CumulativeLayer[lm-2]-0.1);
						BreakFlag = TRUE;
						for(k=0;k<n3;k++){
							if( IndOtherLayerInputsArray[k] == elem ){
								BreakFlag = FALSE;
								break;
							}
						}
					}
					if( BreakFlag == FALSE ){
						j--;
						continue;
					} else {
						IndOtherLayerInputsArray[n3++] = elem;
						PadMe(NumberOfFFNN, elem+1, buffer);
						sprintf(Architecture[lm-1][i][j], "O%s", buffer);
					}
				}
			}
		}
	}

	if( (UsedOutputs=(int *)calloc(NumberOfOutputs+5, sizeof(int))) == NULL ){
		fprintf(stderr, "%s: Memory Allocation Error: %d of int.\n", argv[0], NumberOfOutputs);
		exit(1);
	}
	for(i=0;i<NumberOfOutputs;i++)
		UsedOutputs[i] = 0;
	for(i=1;i<=lm;i++){
		for(j=0;j<Layer[i];j++){
			n1 = atoi(Architecture[i-1][j][0]);
			for(k=1;k<=n1;k++){
				if( Architecture[i-1][j][k][0] == 'O' ){
					strcpy(dummy, &(Architecture[i-1][j][k][1]));
					UsedOutputs[atoi(dummy)-1]++;
				}
			}
		}
	}
	/* We have to format the architecture so that we have all O%d[1] and [%d...]*/
	index = 1;index_del = 0;
	for(i=1;i<=lm;i++){
		if( i == lm ) printf("# output layer starts with O%d and follows below\n", index);
		for(j=0;j<Layer[i];j++){
			n1 = atoi(Architecture[i-1][j][0]);
			PadMe(NumberOfFFNN, index, buffer);
			printf("O%s=(", buffer);
			strcpy(ArchO, "");
			strcpy(ArchD, "[");
			for(k=1;k<=n1;k++){
				if( Architecture[i-1][j][k][0] == 'O' ){
					strcat(ArchO, Architecture[i-1][j][k]);
					strcat(ArchO, "[1]:");
				} else {
					strcat(ArchD, Architecture[i-1][j][k]);
					strcat(ArchD, ",");
				}
			}
			lengthD = strlen(ArchD); lengthO = strlen(ArchO);
			if( ArchD[lengthD-1] == ',' ){
				ArchD[lengthD-1] = ']';
				printf("%s", ArchD);
			}
			if( ArchO[lengthO-1] == ':' ){
				ArchO[lengthO-1] = '\0';
				if( ArchD[lengthD-1] == ']' ){
					printf(":%s", ArchO);
				} else {
					printf("%s", ArchO);
				}
			}
			printf("),(Y[1]),%s_%s,(%d,1),%d,%d;\n", UnitBaseName, buffer, n1, i, Layer[i]);
			ToBeDeleted[0] = '\0';
			for(k=1;k<=n1;k++){
				if( Architecture[i-1][j][k][0] == 'O' ){
					strcpy(dummy, &(Architecture[i-1][j][k][1]));
					if( UsedOutputs[atoi(dummy)-1] == 0 )
						fprintf(stderr, "%s: This should not be happening '%s'.\n\a", argv[0], Architecture[i-1][j][k]);
					UsedOutputs[atoi(dummy)-1]--;
					if( UsedOutputs[atoi(dummy)-1] == 0 ){
						strcat(ToBeDeleted, Architecture[i-1][j][k]);
						strcat(ToBeDeleted, ",");
						/*printf("DEL(%s);\n", Architecture[i-1][j][k]);*/
						strcpy(Deleted[index_del++], Architecture[i-1][j][k]);
					}
				}
			}
			if( ToBeDeleted[0] ){
				ToBeDeleted[strlen(ToBeDeleted)-1] = '\0';
				printf("DEL(%s);\n", ToBeDeleted);
			}
			index++;
		}
	}

	
	ToBeDeleted[0] = '\0';
	for(i=1;i<index;i++){
		BreakFlag = FALSE;
		for(j=0;(j<index_del)&&(!BreakFlag);j++){
			PadMe(NumberOfFFNN, i, buffer);
			sprintf(dummy, "O%s", buffer);
			if( !strcmp(Deleted[j], dummy) )
				BreakFlag = TRUE;
		}
		if( !BreakFlag ){
			strcat(dummy, ",");
			strcat(ToBeDeleted, dummy);
		}
	}

	if( ToBeDeleted[0] ){
		ToBeDeleted[strlen(ToBeDeleted)-1] = '\0';
		printf("DEL(%s);\n", ToBeDeleted);
	}

	printf("$\n");

	/* Free */
	n1 = MaxNumberOfInputsPerFFNN;
	for(i=1;i<=lm;i++){
		if( (i==lm) && (LastLayerNumberOfInputs>0) )
			n1 = LastLayerNumberOfInputs;
		for(j=0;j<Layer[i];j++){
			for(k=0;k<n1;k++)
				free(Architecture[i-1][j][k]);
			free(Architecture[i-1][j]);
		}
  		free(Architecture[i-1]);
	}
	for(i=0;i<NumberOfOutputs;i++)
		free(Deleted[i]);

	free(Deleted); free(ToBeDeleted);
	free(Architecture);
	free(InputsArray);
	free(InputsStack);
	free(IndFirstLayerInputsArray);
	free(IndOtherLayerInputsArray);
	free(UsedOutputs);
	free(ArchD); free(ArchO);
	free(buffer);

	exit(0);
}


void	ResetInputsArray(int *inputs_array, int *inputs_stack, int inputs_number)
{
	int	i;

	for(i=0;i<inputs_number;i++)
		inputs_stack[i] = inputs_array[i] = i+1;
}

int	RetrieveInputElement(int *inputs_array, int *inputs_stack, int inputs_number, int *elements_in_stack)
{
	int	num_elem, elem;

	if( *elements_in_stack <= 0 )
		return( inputs_array[(int )RanD(0, inputs_number)] );
	else {
		/* pop element from stack */
		*elements_in_stack -= 1;
		num_elem = (int )RanD(0.0, (double )(*elements_in_stack));
		elem = inputs_stack[num_elem];
		inputs_stack[num_elem] = inputs_stack[*elements_in_stack];
		return(elem);
	}
}

double	RanD(double MinV, double MaxV)
{
	long	ran;
	double	ret;

	while( (ran=lrand48()) > MAX_RANDOM_NUMBER );

	ret = (MaxV-MinV) * ( ((double )ran) / ((double )MAX_RANDOM_NUMBER) ) + MinV;
	return(ret);
}

/* for example max_num = 1021 and num = 23 then str = "0023" ok? */
char	_str2[500];
void	PadMe(int max_num, int num, char *str)
{
	int	max_digits, num_digits, i, ori_num = num, I;

	sprintf(_str2, "%d", max_num);
	max_digits = strlen(_str2);
	sprintf(_str2, "%d", num);
	num_digits = strlen(_str2);

	I = max_digits - num_digits;
	for(i=0;i<I;i++) _str2[i] = '0'; _str2[i] = 0;
	sprintf(str, "%s%d", _str2, ori_num);

//	printf("max_num=%d(%d), num=%d(%d), I=%d, and result='%s'\n", max_num, max_digits, num, num_digits, I, str);
}
void	PadMeBuggy(int max_num, int num, char *str)
{
	int	max_digits, num_digits, i, ori_num = num, I;


// 	bugs with this approach : floor(log(1000.0)/log(10)) gives 2 instead of 3 !!!!
	if( max_num <= 1 ) max_digits = 1;
	else max_digits = (int )(log(max_num)/M_LN10)+1;

	if( num <= 1 ) num_digits = 1;
	else num_digits = (int )(log(num)/M_LN10)+1;

	strcpy(str, "");
	I = max_digits - num_digits;
	for(i=0;i<I;i++) _str2[i] = '0'; _str2[i] = 0;
	sprintf(str, "%s%d", _str2, ori_num);

	printf("max_num=%d(%d), num=%d(%d), I=%d, and result='%s'\n", max_num, max_digits, num, num_digits, I, str);
}

int	Usage(char *app_name)
{

	fprintf(stderr, "Usage: %s options...\n", app_name);
	fprintf(stderr, "Options are (squarish brackets, [], denote optional parameter):\n");
	fprintf(stderr, "  -inputs  N      The number of data columns to be processed by the\n");
	fprintf(stderr, "                  entity, e.g. the number of inputs to the entity.\n");

	fprintf(stderr, "  -mininputs M    The minimum number of inputs to each of the units\n");
	fprintf(stderr, "                  of the entity.\n");
	fprintf(stderr, "  -maxinputs M    The maximum number of inputs to each of the units\n");
	fprintf(stderr, "                  of the entity. The number of a unit's inputs is determined\n");
	fprintf(stderr, "                  randomly. Use the same number of min/max units\n");
	fprintf(stderr, "                  to denote exactly so many inputs.\n");

	fprintf(stderr, " [-name N]        Use the string N (no spaces, use quotes if neeeded)\n");
	fprintf(stderr, "                  as the base name of each of the  units in the output file\n");

	fprintf(stderr, " [-single]        If you want to produce a configuration file for\n");
	fprintf(stderr, "                  a single network, use this flag. By default an entity\n");
	fprintf(stderr, "                  will be produced.\n");

	fprintf(stderr, " [-seed   S]      Use S as a seed to the random number generator. If\n");
	fprintf(stderr, "                  this parameter is not specified, the seed is taken\n");
	fprintf(stderr, "                  via a call to time(). Which sort of quarantees random\n");
	fprintf(stderr, "                  initial conditions any time you run this program.\n");

	fprintf(stderr, " [-c  C1   C2]    C1 and C2 are two parameters denoting the number of\n");
	fprintf(stderr, "                  first layer units and the total number of layers,\n");
	fprintf(stderr, "                  respectively.\n");

	fprintf(stderr, " [-fat f]         f, a number between 0.0 and 100.0 specifies the shape\n");
	fprintf(stderr, "                  of the logarithmic decay fit function which defines the\n");
	fprintf(stderr, "                  number of units per layer. A value of 0.0 will produce less\n");
	fprintf(stderr, "                  units than a value of 100.0. In any event, this is highly\n");
	fprintf(stderr, "                  empirical process so plot the function given during runtime\n");
	fprintf(stderr, "                  to see exactly the shape of the fit function.\n");
	fprintf(stderr, "                  If 0.0 <= fat <= 100.0 then the fit function will be the following\n");
	fprintf(stderr, "                  UnitsOfLayer[x] = MAX(1, (int )(C1*exp(A*(x*x+x)/fat)))\n");
	fprintf(stderr, "                  where A = - ln(1/C1) / C2\n");
	fprintf(stderr, "                  On the contrary, if fat is not specified, the function will be\n");
	fprintf(stderr, "                  UnitsOfLayer[x] = MAX(1, (int )(C1*exp(A*x))\n");

	fprintf(stderr, "Program by A.Hadjiprocopis, (C) Noodle Woman Software.\n");
	fprintf(stderr, "Bugs and suggestions to the author, livantes@soi.city.ac.uk\n");
	fprintf(stderr, "Free to modify, plagiarise, delete and use this program for non-commercial\n");
	fprintf(stderr, "institutions and individuals.\n");

	return FALSE;
}
