#include "cranelib.h"

#define MAX_BUFFER 25

int main(int argc, char * argv[])
{
  int msg_size, Return, fd_socket;
	const char *LOG_PROCESS = "READER"; 
	char buffer[MAX_BUFFER]; 
	msg_t msg; 
	
	FILE *log;
	int logfd;

	if (argc < 3)
  {
		fprintf(stderr,"READER: Not enough arguments\n");
		exit(1);
	}

	//open the log file
	logfd = atoi(argv[1]);
	if((log = fdopen(logfd, "w"))==(FILE*)NULL){
		perror("READER: file opened wrong");
	}
	fd_socket = atoi(argv[2]);
	
	msg_init(&msg); msg_size = msg_getsize(); // initialize message

	fprintf(log, "%s: starting\n", LOG_PROCESS); fflush(log);
	
	//Wait command
	while (msg_getstatus(msg) != EXIT)
	{	
		bzero(buffer, MAX_BUFFER);

		//Logic to read the message
		if ((Return = read(fd_socket, msg, msg_size)) < 0)
			error("READER: reading from socket", Return);

		//Write the crane status in the buffer
		switch (msg_getstatus(msg)) 
    {
			case UP:
				sprintf(buffer, "Crane up");
				break;
			case DOWN:
				sprintf(buffer, "Crane going down");
				break;
			case STOP:
				sprintf(buffer, "Crane stopped");
				break;
			case TOP:
				sprintf(buffer, "Maximal height");
				break;
			case BOTTOM:
				sprintf(buffer, "Minimal height");
				break;
			case EXIT:
				sprintf(buffer, "Exiting");
				break;
		}
		printf("height: %3d cm\t status: %s\n", msg_getheight(msg), buffer); fflush(stdout);
	}

	fprintf(log, "%s: exiting\n", LOG_PROCESS); fflush(log);

	msg_free(msg);

	close(logfd);
	close(fd_socket);

  exit (0);
}