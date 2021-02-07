#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/wait.h>
#include "cranelib.h"

int main(int argc, char *argv[])
{
	const char *LOG_PROCESS = "SERVER";
	struct sockaddr_in serv_addr, cli_addr;
	
  int fd_socket, newsockfd, Port_No, clilen;
  char STATUS = BOTTOM;
  unsigned long sec = 1, usec = 0;
	int Return; 
	struct timeval tv;
	char input = 0;

	msg_t msg; 

	int s2h[2], h2s[2];
	pid_t crane;

  int msg_size;
	fd_set in_fd;

	char send = 0; 
  char error_msg[256];
	int r_height = 0;

	FILE *log;
	int  logfd;
	char *log_file = "./server_log.txt";

//check number of arguments provided
	if (argc < 2) {
		fprintf(stderr,"ERROR\n");
		exit(1);
	}
	fprintf(log, "%s: starting\n", LOG_PROCESS); fflush(log);

	//open the log file
	if((log = fopen(log_file, "w"))==(FILE*)NULL)
  {
		perror("Log file open from");
		exit(1);
	}
	logfd = fileno(log);	

	//open the Server - crane pipe 
	if((Return = pipe(s2h))<0)
  {
		error("ERROR Server - Crane pipe", Return);
	}

  	//open the socket
	if ((fd_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error("ERROR", fd_socket);
	fprintf(log, "%s: socket opened\n", LOG_PROCESS); fflush(log);

	//open the Crane - Server pipe
	if((Return = pipe(h2s))<0)
  {
		error("ERROR Crane - Server pipe", Return);
	}
	fprintf(log, "%s: pipes opened\n", LOG_PROCESS); fflush(log);
	
	msg_init(&msg); msg_size = msg_getsize(); 

	//Fill server
	bzero((char *) &serv_addr, sizeof(serv_addr));
	Port_No = atoi(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(Port_No);

  //Spawn the crane
	if((crane = spawn("./crane", logfd, s2h, h2s, 0))<0) error("ERROR Crane generation", crane);
	close(s2h[0]);
	close(h2s[1]);
	fprintf(log, "%s: Crane spawned\n", LOG_PROCESS); fflush(log);
	
	//Merge socket
	if ((Return = bind(fd_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) < 0)
  {
		error("ERROR on binding", Return);
		close(fd_socket);
	}

	listen(fd_socket,5);
	clilen = sizeof(cli_addr);
	fprintf(log, "%s: listening to socket\n", LOG_PROCESS); fflush(log);

	//Client connection
	if ((newsockfd = accept(fd_socket, (struct sockaddr *) &cli_addr, &clilen)) < 0)
		error("ERROR on accept", newsockfd);
	fprintf(log, "%s: communication started\n", LOG_PROCESS); fflush(log);
	
  while(input != EXIT)
  {
		FD_ZERO(&in_fd);
		FD_SET(newsockfd, &in_fd);
		tv.tv_sec = sec; tv.tv_usec = usec;
		input = 0;	

		if((Return=select(newsockfd+1, &in_fd, NULL, NULL, &tv)) < 0)
    {
      error("Select on socket", Return); 
    }

		if (FD_ISSET(newsockfd, &in_fd)){
			if ((Return = read(newsockfd,&input,1)) < 0) error("ERROR reading from socket", Return);
			if ((input == UP && STATUS != TOP) || (input == DOWN && STATUS != BOTTOM) || (input == EXIT) || (input == STOP))
				STATUS = input;
		}

		//Crane commands
		switch (STATUS)
    {
			case UP:	 
				send = '+';
				break;
			case DOWN: 	
				send = '-';
				break;
			case STOP: 
				send = 0;
				break;
			case TOP:
				send = 0;
				break;
			case BOTTOM:
				send = 0;
				break;
			case EXIT:	
				send = EXIT;
				break;
		}

		//Logic to send command to Crane
		if ((Return = write(s2h[1],&send,1))<0) error("ERROR writing on Server - Crane pipe", Return);

		//Read the height from the crane
		if ((Return = read(h2s[0],&r_height,sizeof(r_height)))<0) error("ERROR reading from Crane - Server pipe", Return);

		if (r_height>=MAX_HEIGHT && STATUS != EXIT) STATUS = TOP;	
		if (r_height<=MIN_HEIGHT && STATUS != EXIT) STATUS = BOTTOM;	
		if (Return == 0) STATUS = EXIT;	

		msg_setheight(msg, r_height); msg_setstatus(msg, STATUS);
		
		if ((Return = write(newsockfd,msg,msg_size))<0) error("ERROR writing on socket", Return);
		select(1, NULL, NULL, NULL, &tv);
	}													
	fprintf(log, "%s: communication terminated\n", LOG_PROCESS); fflush(log);

	//Wait crane action to finish
    waitpid(crane, &Return, 0);
	if (!WIFEXITED(Return))
  {
		sprintf(error_msg, "Crane terminated with an error %d %d\n", WIFSIGNALED(Return), WTERMSIG(Return));
		perror(error_msg);
	}
	else
		printf("Crane exited with value %d\n", WEXITSTATUS(Return)); fflush(stdout);

	fprintf(log, "%s: exiting\n", LOG_PROCESS); fflush(log);

	msg_free(msg);
	
	close(s2h[1]);
	close(h2s[0]);
	close(logfd);
	close(newsockfd);

	return 0;
}