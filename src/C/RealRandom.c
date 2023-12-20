#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define	TRUE	1
#define FALSE	0
#define	ABS(a)		( ((a) < 0.0) ? (-(a)) : (a) )
#define	INC(a)		( ((a) < 0.0) ? ((a)-1) : ((a)+1) )
#define	ROUND(a)	( (ABS((a)-(int )(a))<0.5) ? ((int )(INC(a))) : ((int )(a)) )

int	Usage(char *app_name);

int	main(int argc, char **argv)
{
	int			i, flag = TRUE, intFlag = FALSE,
				N = 1;
	long			Seed = time(0);
	float			Max = 1.0, Min = 0.0;
	double			Result;


	/* Get the commands from the command line */
	for(i=1;(i<argc)&&flag;i++){
		if( !strncmp(argv[i], "-seed", 2) ){
			if( ++i < argc ) Seed = (long )atoi(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-max", 3) ){
			if( ++i < argc ) Max = atof(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-min", 3) ){
			if( ++i < argc ) Min = atof(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-n", 2) ){
			if( ++i < argc ) N = atoi(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-integer", 2) ){
			intFlag = TRUE;
			continue;
		}
		if( !strncmp(argv[i], "-usage", 2) || !strncmp(argv[i], "-help", 2) ){
			flag = Usage(argv[0]);
			continue;
		}
	}

	if( (Seed <= 0) || (Max<=Min) || (N<=0) ){
		Usage(argv[0]);
		exit(1);
	}

	srand48(Seed);
	if( intFlag ) {
		for(i=0;i<N;i++){
			Result = Min + (Max-Min) * drand48();
			printf("%d\n", ROUND(Result));
		}
	} else {
		for(i=0;i<N;i++){
			Result = Min + (Max-Min) * drand48();
			printf("%lf\n", Result);
		}
	}

	exit(0);
}

int	Usage(char *app_name)
{
	fprintf(stderr, "Usage: %s options...\n", app_name);
	fprintf(stderr, "Options are:\n");
	fprintf(stderr, "  [-seed S]                Use the the integer 'S' as the seed\n");
	fprintf(stderr, "                           to the random number generator instead\n");
	fprintf(stderr, "                           of constructing one based on the time\n");
	fprintf(stderr, "                           the program is ran. The latter is\n");
	fprintf(stderr, "                           more or less non-deterministic.\n");
	fprintf(stderr, "  [-max M]                 The biggest number to output. Default is 1.0\n");
	fprintf(stderr, "  [-min M]                 The smallest number to output. Default is 0.0\n");
	fprintf(stderr, "  [-integer]               Output integers rather than floats.\n");
	fprintf(stderr, "  [-n N]                   Output N random numbers rather than just one.\n");
	fprintf(stderr, "\nThis program prints some random numbers, integers or floats to the standard\n");
	fprintf(stderr, "output in the range specified. It returns 0 on success and 1 on failure.\n");
	fprintf(stderr, "\nProgram by A.Hadjiprocopis (livantes@soi.city.ac.uk) (c) 1997.\n");
	return FALSE;
}
