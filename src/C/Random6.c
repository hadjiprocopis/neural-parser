#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define	TRUE			1
#define	FALSE			0
#define	MAX(a, b)		( (a) < (b) ? (b):(a) )
#define	CEIL(a)			( ((a)-(int )(a))>=0.5 ? ((a)+1.0):(a) )
#define	M_PI2			6.283185307
#define ABS(a)			( (a)<0 ? (-(a)):(a) )
#define	MAX_RANDOM_NUMBER	1073741824

double	MyRandom(double MinV, double MaxV);

/************************************************************************
 *									*
 * 	Random: An implementation of a random function generator	*
 *									*
 *				by					*
 *									*
 *			Andreas Hadjiprocopis				*
 *			(livantes@soi.city.ac.uk)			*
 *									*
 ************************************************************************/ 

int	Usage(char *);

int	main(int argc, char **argv)
{
	int		NumInputs = -1, NumLines = -1, NumProducts, flag = TRUE,
			*a, *b, *c, i, j, RandomNumbers, Seed = (int )time(0);
	double		*x, y, *RandomNumbersArray, RangeMin = 0.0, RangeMax = 1.0;

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
		if( !strncmp(argv[i], "-range", 2) ){
			if( ++i < argc ) RangeMin = atoi(argv[i]);
			else flag = Usage(argv[0]);
			if( ++i < argc ) RangeMax = atoi(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-seed", 2) ){
			if( ++i < argc ){
				if( (j=atoi(argv[i])) > 0 ) Seed = j;
			} else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-usage", 2) || !strncmp(argv[i], "-help", 2) ){
			flag = Usage(argv[0]);
			continue;
		}
	}

	if( (NumInputs<0) || (NumLines<0) ){
		Usage(argv[0]);
		exit(1);
	}

	if( (x=(double *)calloc(NumInputs+3, sizeof(double))) == NULL ){
		fprintf(stderr, "Memory Allocation Error %s: 'x' (%d).\n", argv[0], NumInputs);
		exit(1);
	}
	RandomNumbers = NumLines * NumInputs;
	if( (RandomNumbersArray=(double *)calloc(RandomNumbers+10, sizeof(double))) == NULL ){
		fprintf(stderr, "Memory Allocation Error %s: 'RandomNumbersArray' (%d).\n", argv[0], RandomNumbers);
		free(x);
		exit(1);
	}

	srand48(1974);
	NumProducts = (int )(lrand48() % (long )( NumInputs - (int )CEIL((float )NumInputs/2.0) )) + (int )CEIL((float )NumInputs/2.0);

	if( (a=(int *)calloc(NumProducts+3, sizeof(int))) == NULL ){
		fprintf(stderr, "Memory Allocation Error %s: 'a' (%d).\n", argv[0], NumProducts);
		free(x);free(RandomNumbersArray);
		exit(1);
	}
	if( (b=(int *)calloc(NumProducts+3, sizeof(int))) == NULL ){
		fprintf(stderr, "Memory Allocation Error %s: 'b' (%d).\n", argv[0], NumProducts);
		free(x);free(RandomNumbersArray); free(a);
		exit(1);
	}
	if( (c=(int *)calloc(NumProducts+3, sizeof(int))) == NULL ){
		fprintf(stderr, "Memory Allocation Error %s: 'c' (%d).\n", argv[0], NumProducts);
		free(x);free(RandomNumbersArray); free(a); free(b);
		exit(1);
	}
	for(i=0;i<NumProducts;i++){
		a[i] = (int )(lrand48() % (long )NumInputs);
		b[i] = (int )(lrand48() % (long )NumInputs);
		c[i] = (int )(lrand48() % 6L);
	}

	srand48(Seed);
	for(i=0;i<RandomNumbers;i++)
		RandomNumbersArray[i] = MyRandom(RangeMin, RangeMax);

	for(i=0;i<NumLines;i++){
		for(j=0;j<NumInputs;j++){
			x[j] = RandomNumbersArray[NumInputs*i+j];
			printf("%lf ", x[j]);
		}
		y = 0.0;
		for(j=0;j<NumProducts;j++){
			switch( c[j] ){
				case 0:	y += sin(M_PI2 * x[a[j]])*cos(M_PI2 * x[a[j]]);break;
				case 1:	y += cos(M_PI2 * x[a[j]])*sin(M_PI2 * x[b[j]]);break;
				case 2:	y += sin(M_PI2 * x[b[j]])*sin(M_PI2 * x[a[j]]);break;
				case 3:	y += sin(M_PI2 * x[a[j]])*cos(M_PI2 * x[b[j]]);break;
				case 4:	y += cos(M_PI2 * x[a[j]])*cos(M_PI2 * x[b[j]]);break;
				case 5:	y += sin(M_PI2 * x[b[j]])*cos(M_PI2 * x[b[j]]);break;
			}
		}
		printf("\t%lf\n", 3.0*y/((double )NumProducts));
	}

	free(x);free(RandomNumbersArray);free(a); free(b); free(c);
	exit(0);
}

double	MyRandom(double MinV, double MaxV)
{
	long	ran;
	double	ret;

	while( (ran=lrand48()) > MAX_RANDOM_NUMBER );

	ret = (MaxV-MinV) * ( ((double )ran) / ((double )MAX_RANDOM_NUMBER) ) + MinV;
	return(ret);
}

int     Usage(char *app_name)
{
	int     i;

	fprintf(stderr, "Usage: %s options...\n", app_name);
	fprintf(stderr, "Options are:\n");
	fprintf(stderr, "   -inputs N   the number of input variables.\n");
	fprintf(stderr, "   -lines  L   the number of lines of output to produce.\n");
	fprintf(stderr, "   -seed   S   the seed to start creating the variables.\n");
	fprintf(stderr, "               Do not use it for creating the seed from time().\n");
        fprintf(stderr, "  [-range P R] The range of the input (random) numbers will be\n");
	fprintf(stderr, "               between P and R. Default is the range [0.0,1.0).\n");
	fprintf(stderr, "  [-gauss M D] The input numbers have a gaussian distribution\n");
	fprintf(stderr, "               centred around M and with standard deviation D.\n");
	fprintf(stderr, "\nProgram by A.Hadjiprocopis (livantes@soi.city.ac.uk)\n");
	fprintf(stderr, "Noodle Woman Software (c) 1998.\n");

	return FALSE;
}

