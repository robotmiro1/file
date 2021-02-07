#include <sys/socket.h>
#include <netinet/in.h>
#include "cranelib.h"

#define TRUE 1
#define FALSE 0

int main(int argc, char *argv[])
{
	const char *LOG_PROCESS = "CRANE"; //process name for the log file
  char input = 0, Return;
	int crane_height = 0, in_fd, out_fd;	

	FILE *log;
	int logfd;

	//Logic to open the file
	logfd = atoi(argv[1]);
	if((log = fdopen(logfd, "w")) == (FILE*)NULL){
		perror("CRANE: Log file open");
	}

  	//Check arguments provided
	if (argc < 4){
		fprintf(stderr,"Not enough arguments\n");
		exit(TRUE);
	}

	//Logic for pipe
	in_fd = atoi(argv[2]);
	out_fd = atoi(argv[3]);
	fprintf(log, "%s: starting\n", LOG_PROCESS); fflush(log);

	//Wait for command
	while(input != EXIT)
  {
		input = 0;

		//Commands +  and -
		if((Return = read(in_fd, &input, 1)) < 0) error("ERROR from Server", Return);

		//move the crane (or don't move it)
		switch (input)
    {
			case '+':	 
				height_tryup(&crane_height);
				break;
			case '-':
				height_trydown(&crane_height);
				break;
		}
		//Logic tosend the actual height to server
		if((Return = write(out_fd, &crane_height, sizeof(int))) < 0) error("ERROR", Return);
	}
	fprintf(log, "%s: exiting\n", LOG_PROCESS); fflush(log);

	close(logfd);
	close(in_fd);
	close(out_fd);

	return 0;
}