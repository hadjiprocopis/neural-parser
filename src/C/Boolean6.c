#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#define	BINARY_VALUE(a)	( ((a)==0) ? OFFValue : ONValue )
#define	SGN(a)		( ((a)<=0.5) ? 0 : 1 )
#define	TRUE	1
#define	FALSE	0
#define	NUM_OPERATORS	6
#define LOG_2E	1.95
#define M_PI2	6.283185307
#define	DEFAULT_SEQUENCE_LENGTH	5

static int	AND(int,int);
static int	OR(int,int);
static int	NOR(int,int);
static int	XOR(int,int);
static int	NAND(int,int);
static int	RANDOM(int,int);
void		printRANDOM(void);

int	SequenceLength, *RandomSequence;

double	GaussianNoise(double Mean, double StDeviation);
int	Usage(char *app_name);
char	KnownOperators[NUM_OPERATORS][10] = {"AND", "OR", "NOR", "XOR", "NAND", "RANDOM"};
int	(((*OperatorFunctions[NUM_OPERATORS])(int, int))) = {*AND, *OR, *NOR, *XOR, *NAND, *RANDOM};
void	DecimalToBinary(long int in, char **out, int num);
long	power2(int n);

char	ONValue[20], OFFValue[20];

int	main(int argc, char **argv)
{

	int			i, j, flag = TRUE, Seed = (int )time(0), k, NoiseFlag = FALSE,
				NumInputs = 0, NumLines = 0, NumOperator, Result, next,
				AllFlag = FALSE, DecimalFlag = FALSE;
   	char			Operator[255] = {""}, *Buffer;
	void			*FunctionOperator;
	double			NoiseMean, NoiseStDeviation;
	long			il;

	strcpy(ONValue, "1"); strcpy(OFFValue, "0");
	SequenceLength = DEFAULT_SEQUENCE_LENGTH;;
	/* Get the commands from the command line */
	for(i=1;(i<argc)&&flag;i++){
		if( !strncmp(argv[i], "-inputs", 2) ){
			if( ++i < argc ) NumInputs = atoi(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-lines", 2) ){
			if( ++i < argc ) NumLines = atoi(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-seed", 2) ){
			if( ++i < argc ){
				if( (j=atoi(argv[i])) > 0 ) Seed = j;
			} else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-operator", 2) ){
			if( ++i < argc ){
				j = 0;
				while( (Operator[j]=toupper(argv[i][j])) ) j++;
				if( !strcmp(Operator, "RANDOM") ){
					if( (++i<argc) && (argv[i][0]!='-') )
						SequenceLength = atoi(argv[i]) * 2 + 1;
				}
			}
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-noise", 2) ){
			if( ++i < argc ) NoiseMean = atof(argv[i]);
			else { flag = Usage(argv[0]); continue; }
			if( ++i < argc ) NoiseStDeviation = atof(argv[i]);
			else flag = Usage(argv[0]);
			NoiseFlag = TRUE;
			continue;
		}
		if( !strncmp(argv[i], "-all", 2) ){
			AllFlag = TRUE;
			if( (i+1 < argc) && (argv[i+1][0]!='-') && ++i ){
				if( !strncmp(argv[i], "decimal", 1) ) DecimalFlag = TRUE;
			}
			continue;
		}
		if( !strncmp(argv[i], "-values", 2) ){
			if( ++i < argc ) strcpy(OFFValue, argv[i]);
			else flag = Usage(argv[0]);
			if( ++i < argc ) strcpy(ONValue, argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-usage", 2) || !strncmp(argv[i], "-help", 2) ){
			flag = Usage(argv[0]);
			continue;
		}
	}

	for(i=0,NumOperator=-1;i<NUM_OPERATORS;i++)
		if( !strcmp(Operator, KnownOperators[i]) ){ NumOperator = i; break; }
	if( NumOperator < 0 ) Operator[0] = '\0';

	if( (NumInputs<=1) || ((NumLines<=0)&&(!AllFlag)) || (Operator[0]=='\0') || !flag){
		if( flag ) Usage(argv[0]);
		exit(1);
	}
	srandom((unsigned int )Seed); srand48((long int )Seed);

	if( !strcmp(Operator, "RANDOM") ){
		if( !(SequenceLength % 2) ) SequenceLength++;
		if( (RandomSequence=(int *)calloc(SequenceLength, sizeof(int))) == NULL ){
			fprintf(stderr, "%s: Memory allocation error for %d int.\a\n", argv[0], SequenceLength);
			exit(1);
		}
		RandomSequence[0] = random()&01;
		/* Use only AND/OR hence (2) * drand ... */
		for(i=1;i<SequenceLength;i++) RandomSequence[i] = ( (i%2) ? (random()&01):((int )((2)*drand48())) );
		fprintf(stderr, "%s: RANDOM FUNCTION IS ", argv[0]);
		printRANDOM();

	}
		

	if( AllFlag ){
		if( (Buffer=(char *)calloc(512, sizeof(char))) == NULL ){
			fprintf(stderr, "%s: Memory allocation error for 512 char.\a\n", argv[0]);
			exit(1);
		}
		for(il=0;il<power2(NumInputs);il++){
			DecimalToBinary(il, &Buffer, NumInputs);
			Result = Buffer[0] - '0';
			for(i=1;i<NumInputs;i++){
				next = Buffer[i] - '0';
				Result = (int )(*OperatorFunctions[NumOperator])(Result, next);
			}
			if( DecimalFlag ) printf("%ld\t%d\n", il, Result);
			else{
				for(j=0;j<NumInputs;j++) printf("%c ", Buffer[j]);
				printf("\t%d\n", Result);
			}
		}
		free(Buffer); exit(0);
	}
	if( NoiseFlag ){
		for(i=0;i<NumLines;i++){
			Result = random()&01; /* Produce a binary random value */
			printf("%.2lf ", (double )Result + GaussianNoise(NoiseMean, NoiseStDeviation));
			for(j=1;j<NumInputs;j++){
				next = random()&01;
				printf("%.2lf ", (double )next + GaussianNoise(NoiseMean, NoiseStDeviation));
				Result = (int )(*OperatorFunctions[NumOperator])(Result, next);
			}
			printf("\t%.2lf\n", (double )Result + GaussianNoise(NoiseMean, NoiseStDeviation));
		}
	} else {
		for(i=0;i<NumLines;i++){
			Result = random()&01; /* Produce a binary random value */
			printf("%s ", BINARY_VALUE(Result));
			for(j=1;j<NumInputs;j++){
				next = random()&01;
				printf("%s ", BINARY_VALUE(next));
				Result = (int )(*OperatorFunctions[NumOperator])(Result, next);
			}
			printf("\t%s\n", BINARY_VALUE(Result));
		}
	}

	exit(0);
}

double  GaussianNoise(double Mean, double StDeviation)
{
	double  r1, r2, rsq;

	do{
		r1 = drand48();
		r2 = drand48();
	} while( !r1 );
	   
	return( sqrt(-log(r1)*LOG_2E*StDeviation*StDeviation) * cos(M_PI2 * StDeviation * StDeviation * r2) + Mean );
}

int	Usage(char *app_name)
{
	int	i;

	fprintf(stderr, "Usage: %s options...\n", app_name);
	fprintf(stderr, "Options are(squarish brackets, [], denote optional parameter):\n");
	fprintf(stderr, "   -inputs   N    the number of input variables.\n");
	fprintf(stderr, "   -lines    L    the number of lines of output to produce.\n");
	fprintf(stderr, "   -operator O    the boolean operator to use. It must be one of:\n");
	fprintf(stderr, "                           ");
	for(i=0;i<NUM_OPERATORS;i++)
	fprintf(stderr, "%s ", KnownOperators[i]);
	fprintf(stderr, "\n");
	fprintf(stderr, "                  The `RANDOM' operator causes the program to create\n");
	fprintf(stderr, "                  an arbitrary boolean function. The size of this function\n");
	fprintf(stderr, "                  is defined by an optional integer following the RANDOM\n");
	fprintf(stderr, "                  keyword. The default length is %d.\n", SequenceLength);
	fprintf(stderr, "  [-seed     S]   the seed to start creating the variables.\n");
	fprintf(stderr, "                  Do not use it for creating the seed from time().\n");
	fprintf(stderr, "  [-values A B]   Instead of using 0 and 1 for OFF and ON values,\n");
	fprintf(stderr, "                  use STRINGS A and B. e.g. A='-1' and B='+1' or\n");
	fprintf(stderr, "                  or A='hello' and B='goodbye'. Default are 0 and 1.\n");
        fprintf(stderr, "  [-noise  M D]   gaussian noise with M as mean and D as standard\n");
        fprintf(stderr, "                  deviation will be added to the inputs and outputs.\n");
	fprintf(stderr, "  [-all [dec]]    Print the truth table of the specified function.\n");
	fprintf(stderr, "                  If the word `dec' follows the `-all' option, then\n");
	fprintf(stderr, "                  the input will be in decimal notation rather than binary\n");
	fprintf(stderr, "\nThis program calculates a boolean expression, given the operator and the\n");
	fprintf(stderr, "number of variables to use. The variables are instantiated randomly to 0/1.\n");
	fprintf(stderr, "The result is : X[1] {OPERATOR} X[2] {OPERATOR} ... X[n].\n");
	fprintf(stderr, "\nProgram by A.Hadjiprocopis (livantes@soi.city.ac.uk)\n");
	fprintf(stderr, "Noodle Woman Software (c) 1998.\n");

	return FALSE;
}

int	AND(int a, int b){return(a&b);}
int	OR(int a, int b){return(a|b);}
int	NOR(int a, int b){return(!OR(a,b));}
int	XOR(int a, int b){return(a^b);}
int	NAND(int a, int b){return(!AND(a,b));}
int	RANDOM(int a, int b)
{
	int	result, operand, i;

	result = (int )(*OperatorFunctions[RandomSequence[0]])(a, b);
	for(i=1;i<SequenceLength;i+=2){
		operand = ((RandomSequence[i]==0) ? (a):(b));
		result = (int )(*OperatorFunctions[RandomSequence[i+1]])(result, operand);
	}
	return(result);
}
void	printRANDOM(void)
{
	int	result, operand, i;


	for(i=SequenceLength-1;i>0;i-=2){
		printf("%s(", KnownOperators[RandomSequence[i]]);
		printf("%c,", ((RandomSequence[i-1]==0) ? ('a'):('b')));
	}
	printf("%c", ((RandomSequence[0]==0) ? ('a'):('b')));
	for(i=1;i<SequenceLength;i+=2) printf(")");
	printf("\n");
}
void	DecimalToBinary(long in, char **out, int num)
{
	int	i;
	char	*p = *out;

	for(i=0;i<num;i++) p[i] = '0';

	i = num-1;
	while( in ){
		p[i] = ( (in % 2) ? '1' : '0' );
		in /= 2;
		i--;
	}
}
		
long	power2(int n)
{
	register long	i = 1;

	while(n--){i *= 2;}

	return i;
}
