#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>         

#define TRUE    1
#define FALSE   0

/* 'Listen.c' is a program which listens to the specified port, 'Port', until it receives 'NotificationsExpected' bytes (#). */

int     Usage(char *app_name);

int	main(int argc, char **argv)
{
	int			socket_number, l, i, flag = TRUE, Port = -1,
				connected_socket, NotificationsExpected = -1,
				NotificationsReceivedSoFar;
	struct	sockaddr_in	foreign_host;
	struct	sockaddr	foreign_host_address;
	char			buffer[10];

	/* Get the commands from the command line */
	for(i=1;(i<argc)&&flag;i++){	
		if( !strncmp(argv[i], "-port", 2) ){
			if( ++i < argc ) Port = atoi(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-notifications", 2) ){
			if( ++i < argc ) NotificationsExpected = atoi(argv[i]);
			else flag = Usage(argv[0]);
			continue;
		}
		if( !strncmp(argv[i], "-usage", 2) || !strncmp(argv[i], "-help", 2) ){
			flag = Usage(argv[0]);
			continue;
		}
	}
	if( (NotificationsExpected<=0) || (Port<=0) ){
		Usage(argv[0]);
		exit(1);
	}

	if( (socket_number=socket(AF_INET, SOCK_STREAM, 0)) < 0 ){
		perror("opening socket");
		exit(1);
	}

	foreign_host.sin_family = AF_INET;
	foreign_host.sin_addr.s_addr = INADDR_ANY;
	foreign_host.sin_port = htons(Port);

	if( bind(socket_number, (struct sockaddr *)(&foreign_host), sizeof(foreign_host)) == -1 ){
		perror("bind");
		close(socket_number);
		exit(1);
	}
	
	
	l = sizeof(foreign_host);
	if( getsockname(socket_number, (struct sockaddr *)(&foreign_host), &l) == -1 ){
		perror("getsockname");
		close(socket_number);
		exit(1);
	}

	if( listen(socket_number, 1) == -1 ){
		perror("listen");
		close(socket_number);
		exit(1);
	}

	l = sizeof(foreign_host_address);
	NotificationsReceivedSoFar = 0;
	for(;;){
		if( (connected_socket=accept(socket_number, &foreign_host_address, &l)) == -1 ){
			perror("accept");
			close(socket_number);
			exit(1);
		}
		if( (l=read(connected_socket, buffer, 1)) < 0 ){
			perror("read");
			close(socket_number);
			exit(1);
		}
		if( buffer[0] == '#' ){
			if( ++NotificationsReceivedSoFar == NotificationsExpected ) break;
		} else {
			/*perror("Received unknown signal. Ignoring...\a");*/
			/* we received something not a '#' so it must be a check to see whether somebody
			   is listening to this port or a stray byte from sunspot activity... */
			continue;
		}
	}

	close(socket_number);

	exit(0);
}

int	Usage(char *app_name)
{
	fprintf(stderr, "Usage: %s options...\n", app_name);
	fprintf(stderr, "Options are:\n");
	fprintf(stderr, "   -port port_number       The port to listen at for notifications\n");
	fprintf(stderr, "   -notifications N        The number of notifications to expect before exiting\n");
	fprintf(stderr, "\nThis program listens at the specified port for N bytes ('#').\n");
	fprintf(stderr, "When it has received these, it will exit. If it receives the probe\n");
	fprintf(stderr, "character '!', it will send back a message that it is alive and listening\n");
	fprintf(stderr, "-- it does not count towards N. This program is useful we have some\n");
	fprintf(stderr, "processes running on several processors and we want to be notified when\n");
	fprintf(stderr, "It will return 0 on success, 1 on connection failure.\n");
	fprintf(stderr, "they are finished. See the program `Notify.c'.\n\nProgram by A.Hadjiprocopis (livantes@soi.city.ac.uk) (c) 1997.\n");

	return FALSE;
}
