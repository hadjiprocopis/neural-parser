#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define	TRUE	1
#define	FALSE	0

/* 'Notify.c' sends one byte (#) to the specified address, 'Address', at the specified port, 'Port'. */
/* If the optional 3rd parameter is c (i.e. check), then notify will send one byte of anything but a '#'
   in order to establish whether there is somebody listening to that port (exit value of 1) or nobody is
   listening (exit value of -1). Use this option when you are using 2 listeners at the same host, then
   obviously you will need 2 ports */

int	Usage(char *app_name);

int	main(int argc, char **argv)
{
	int			i, flag = TRUE, socket_number, Port = -1;
	struct	sockaddr_in	foreign_host;
	struct	hostent		*foreign_host_data;
	char			data[10], Address[128];

	strcpy(data, "#"); strcpy(Address, "");

	/* Get the commands from the command line */
	for(i=1;(i<argc)&&flag;i++){
		if( !strncmp(argv[i], "-address", 2) ){
			if( ++i < argc ) strcpy(Address, argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-probe", 3) ){
			strcpy(data, "!");
			continue;
		}
		if( !strncmp(argv[i], "-port", 2) ){
			if( ++i < argc ) Port = atoi(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-usage", 2) || !strncmp(argv[i], "-help", 2) ){
			flag = Usage(argv[0]);
			continue;
		}
	}

	if( (Port <= 0) || (Address[0]=='\0') ){
		Usage(argv[0]);
		exit(1);
	}

	if( (foreign_host_data=gethostbyname(Address)) == 0 ){
		perror("Can not look up given address");
		exit(1);
	}
	if( (socket_number=socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
		perror("opening socket");
		exit(1);
	}

	foreign_host.sin_family = AF_INET;
	foreign_host.sin_addr.s_addr = INADDR_ANY;
	foreign_host.sin_port = htons(Port);

	bcopy(foreign_host_data->h_addr, &foreign_host.sin_addr, foreign_host_data->h_length);
/*	strncpy(foreign_host.sin_addr, foreign_host_data->h_addr, (size_t )foreign_host_data->h_length);*/

 	if( connect(socket_number, (struct sockaddr *)(&foreign_host), sizeof(foreign_host)) < 0 ){
		perror("connect");
		close(socket_number);
		exit(1);
	}

	if( write(socket_number, data, 1) < 0 ){
		perror("writing data");
		close(socket_number);
		exit(1);
	}

	close(socket_number);

	exit(0);
}

int	Usage(char *app_name)
{
	fprintf(stderr, "Usage: %s options...\n", app_name);
	fprintf(stderr, "Options are:\n");
	fprintf(stderr, "   -address inet_address   It will notify the listener set at\n");
	fprintf(stderr, "                           the given address\n");
	fprintf(stderr, "   -port port_number       The port at the remote machine where the\n");
	fprintf(stderr, "                           listener is set\n");
	fprintf(stderr, "  [-probe]                 Probe whether there is a listener set at\n");
	fprintf(stderr, "                           the given address and port. If there is a\n");
	fprintf(stderr, "                           listener then it will respond appropriately.\n");
	fprintf(stderr, "\nThis program sends a byte to the specified port of the specified\n");
	fprintf(stderr, "address. Useful for sending notification about something. See `Listen.c'.\n");
	fprintf(stderr, "It will return 0 on finding a listener or 1 on failed connection.\n");
	fprintf(stderr, "\nProgram by A.Hadjiprocopis (livantes@soi.city.ac.uk) (c) 1997.\n");
	return FALSE;
}
