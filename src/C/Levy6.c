#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define	TRUE			1
#define	FALSE			0
#define	CEIL(a)			( ((a)-(int )(a))>=0.5 ? ((a)+1.0):(a) )
#define	M_PI2			6.283185307
#define	M_PI3			8,424777961
#define	M_2PI			9.8696044
#define	M_2PI9			88.8264396
#define	LOG_2E			1.95
#define	MAX_RANDOM_NUMBER	1073741824
#define ABS(a)			( (a)<0 ? (-(a)):(a) )
double	MyRandom(double MinV, double MaxV);
double	GaussianNoise(double Mean, double StDeviation);

/************************************************************************
 *									*
 * 	Levy6: An implementation of the Levy6 function			*
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
	int		NumInputs = -1, NumLines = -1, NoiseFlag = FALSE,
			i, j, Seed = (int )time(0), flag = TRUE, AdjustedFlag = FALSE,
			ShowInputsFlag = TRUE, ShowOutputsFlag = TRUE;
	double		*x, y, sum, NoiseMean = 0.0, NoiseStDeviation = 1.0,
			RangeMin = 0.0, RangeMax = 1.0;
  
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
		if( !strncmp(argv[i], "-noise", 2) ){
			if( ++i < argc ) NoiseMean = atof(argv[i]);
			else { flag = Usage(argv[0]); continue; }
			if( ++i < argc ) NoiseStDeviation = atof(argv[i]);
			else flag = Usage(argv[0]);
			NoiseFlag = TRUE;
			continue;
		}
		if( !strncmp(argv[i], "-adjusted", 2) ){
			AdjustedFlag = TRUE;
			continue;
		}
		if( !strncmp(argv[i], "-only_inputs", 7) ){
			ShowOutputsFlag = FALSE;
			continue;
		}
		if( !strncmp(argv[i], "-only_outputs", 7) ){
			ShowInputsFlag = FALSE;
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

	srand48(Seed);

	if( AdjustedFlag ){
		if( NoiseFlag ){
			for(i=0;i<NumLines;i++){
				for(sum=0,j=0;j<NumInputs;j++){
					x[j] = MyRandom(RangeMin, RangeMax);
					if( ShowInputsFlag ) printf("%lf ", x[j]);
				}
				for(sum=0,j=0;j<NumInputs-1;j++)
					sum += (2*x[j]-2.0)*(2*x[j]-2.0)*sin(M_2PI9*(2*x[j+1]-1)*(2*x[j+1]-1));
				y = sin(M_2PI9*(2*x[0]-1)*(2*x[0]-1))/10.0 + sum/(1.2*sqrt(((double )NumInputs))) + (2*x[NumInputs-1]-2.0)*(1.0+sin(M_PI2*(2*x[NumInputs-1]-1)))/10.0 + GaussianNoise(NoiseMean, NoiseStDeviation);
				if( ShowOutputsFlag ) printf("\t%lf\n", y);
				else printf("\n");
			}
		} else {
			for(i=0;i<NumLines;i++){
				for(sum=0,j=0;j<NumInputs;j++){
					x[j] = MyRandom(RangeMin, RangeMax);
					if( ShowInputsFlag ) printf("%lf ", x[j]);
				}
				for(sum=0,j=0;j<NumInputs-1;j++)
					sum += (2*x[j]-2.0)*(2*x[j]-2.0)*sin(M_2PI9*(2*x[j+1]-1)*(2*x[j+1]-1));
				y = sin(M_2PI9*(2*x[0]-1)*(2*x[0]-1))/10.0 + sum/(1.2*sqrt(((double )NumInputs))) + (2*x[NumInputs-1]-2.0)*(1.0+sin(M_PI2*(2*x[NumInputs-1]-1)))/10.0 + GaussianNoise(NoiseMean, NoiseStDeviation);
				y /= (0.083942857*((double )NumInputs)-1.1371428571);
/*				y /= 2*(0.00324459333333333 * ((double )NumInputs) + 0.210765333333333);*/
				if( ShowOutputsFlag ) printf("\t%lf\n", y);
				else printf("\n");
			}
		}
	} else {
		if( NoiseFlag ){
			for(i=0;i<NumLines;i++){
				for(sum=0,j=0;j<NumInputs;j++){
					x[j] = MyRandom(RangeMin, RangeMax);
					if( ShowInputsFlag ) printf("%lf ", x[j]);
				}
				for(sum=0,j=0;j<NumInputs-1;j++)
					sum += (x[j]-1.0)*(x[j]-1.0)*sin(M_2PI9*x[j+1]*x[j+1]);
				y = sin(M_2PI9*x[0]*x[0])/10.0 + sum/(1.2*sqrt(((double )NumInputs))) + (x[NumInputs-1]-1.0)*(1.0+sin(M_PI2*x[NumInputs-1]))/10.0 + GaussianNoise(NoiseMean, NoiseStDeviation);
				if( ShowOutputsFlag ) printf("\t%lf\n", y);
				else printf("\n");
			}
		} else {
			for(i=0;i<NumLines;i++){
				for(sum=0,j=0;j<NumInputs;j++){
					x[j] = MyRandom(RangeMin, RangeMax);
					if( ShowInputsFlag ) printf("%lf ", x[j]);
				}
				for(sum=0,j=0;j<NumInputs-1;j++)
					sum += (x[j]-1.0)*(x[j]-1.0)*sin(M_2PI9*x[j+1]*x[j+1]);
				y = sin(M_2PI9*x[0]*x[0])/10.0 + sum/(1.2*sqrt(((double )NumInputs))) + (x[NumInputs-1]-1.0)*(1.0+sin(M_PI2*x[NumInputs-1]))/10.0;
				y = y/(0.000730865 * ((double )NumInputs) + 0.02762575) - 1.0;
				if( ShowOutputsFlag ) printf("\t%lf\n", y);
				else printf("\n");
			}
		}
	}
	free(x);

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

	fprintf(stderr, "Usage: %s options...\n", app_name);
	fprintf(stderr, "Options are(squarish brackets, [], denote optional parameter):\n");
	fprintf(stderr, "  -inputs N     the number of input variables.\n");
	fprintf(stderr, "  -lines  L     the number of lines of output to produce.\n");
	fprintf(stderr, " [-range P R]   The range of the input (random) numbers will be\n");
        fprintf(stderr, "                between P and R. Default is the range [0.0,1.0).\n");
	fprintf(stderr, " [-noise M D]   Gaussian noise with M as mean and D as standard\n");
	fprintf(stderr, "                deviation will be added to the OUTPUT only.\n");
	fprintf(stderr, " [-seed   S]    the seed to start creating the variables.\n");
	fprintf(stderr, "                If the `-seed' flag is not present, then\n");
	fprintf(stderr, "                the result of the call to `time(0)' will\n");
	fprintf(stderr, "                seed the random number generator.\n");
	fprintf(stderr, " [-adjusted]    The original levy function was accepting inputs\n");
	fprintf(stderr, "                within the range -1<x<1. This modified version\n");
	fprintf(stderr, "                normalises the output but also restricts the\n");
	fprintf(stderr, "                inputs in the range 0<x<1. If you want to compensate\n");
	fprintf(stderr, "                for this restriction, then use this flag which will\n");
	fprintf(stderr, "                cause the transformation x_new = (2*x-1). So, still,\n");
	fprintf(stderr, "                the range of the function is 0<x<1 but x is transformed.\n");
	fprintf(stderr, " [-only_inputs] Show only inputs, skip any output.\n");
	fprintf(stderr, " [-only_outputs]Show only outputs, skip any inputs.\n");
	fprintf(stderr, "\nProgram by A.Hadjiprocopis (livantes@soi.city.ac.uk)\n");
	fprintf(stderr, "Noodle Woman Software (c) 1998.\n");

	return FALSE;
}
