#include <sys/socket.h> //gain access to the data arrays in the ancillary data associated with a message header
#include <sys/wait.h> // define the macros for analysis of process status
#include <netinet/in.h> //defines the sockaddr_in structure
#include <netdb.h>  //defines the netent structure
#include "cranelib.h"

#define SIZE_ERROR 100
#define TRUE 1
#define FALSE 0

/* Log file must be opened with fopen from the master process, retrieving the file descriptor */

int main(int argc, char *argv[])
{
	  const char *LOG_PROCESS = "CLIENT";
    int fd_socket, Port_No, n;

    struct sockaddr_in serv_addr; //library netinet/in.h
    struct hostent *serv;
    int Return; 
    char output = FALSE; 
	  char error_msg[SIZE_ERROR];

	FILE *log;
	int  logfd;
	char *log_file = "./client_log.txt";

	//Check arguments provided
    if (argc < 3) {
       fprintf(stderr,"%s port\n", argv[0]);
       exit(FALSE);
    }
    Port_No = atoi(argv[2]);

	//Logic to open the file
	if((log = fopen(log_file, "w"))==(FILE*)NULL){
		perror("");
		exit(TRUE);
	}
	logfd = fileno(log);
	fprintf(log, "%s: starting\n", LOG_PROCESS); fflush(log);

	
	//Logic to get the host name
    if ((serv = gethostbyname(argv[1])) == NULL) {
        fprintf(stderr,"ERROR, host doesn't exist\n");
        exit(FALSE);
    }
    
	//Logic to open the socket
    if ((fd_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("Could not open socket", fd_socket);
	fprintf(log, "%s: socket done creating\n", LOG_PROCESS); fflush(log);
	
	//Logic to fill the server
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)serv->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         serv->h_length);
    serv_addr.sin_port = htons(Port_No);
	
	//Logic to connect with server
    if ((Return = connect(fd_socket,(struct sockaddr *)&serv_addr,sizeof(serv_addr))) < 0)
        error("ERROR in connection", Return);
	fprintf(log, "%s: connected\n", LOG_PROCESS); fflush(log);

	//wait the exit command
    while (output != EXIT) {
		printf("Enter command:\n");
	  printf("<U> UP\t<D> DOWN\t<S> STOP\t<E> EXIT\n");
      scanf(" %c", &output);

	//Open a new shell to see the results separated
	int tmp_socket[2] = {fd_socket, -1};
    int child_pid = spawn("./reader", logfd, tmp_socket, NULL, 1);
	fprintf(log, "%s: reader spawned\n", LOG_PROCESS); fflush(log);

		//Logic for user commands
      if ((output != UP) && (output != DOWN) && (output != EXIT) && (output != STOP))
      {
        printf("Command doesn't exist, try again\n");
      }
		//Send command to socket
      if ((Return = write(fd_socket, &output, 1)) < 0)
        error("ERROR writing to socket", Return);
    }
	fprintf(log, "%s: Finish\n", LOG_PROCESS); fflush(log);

	if (!WIFEXITED(Return)){
		sprintf(error_msg, "Error in %d %d\n", WIFSIGNALED(Return), WTERMSIG(Return));
		perror(error_msg);
	}
	else
		printf("Reader value: %d\n", WEXITSTATUS(Return)); fflush(stdout);

	fprintf(log, "%s: exiting\n", LOG_PROCESS); fflush(log);
	//close socket and log file descriptors before exiting
	close(logfd);
	close(fd_socket);
    return 0;
}