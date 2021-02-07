#include "cranelib.h"

void error(char *msg, int Return) 
{
  perror(msg);
  exit(Return); 
}

// Server - Reader message structure
struct message
{
	int crane_height;
	char status; 
};

int spawn(char* ex_name, int logfd, int *in_fd, int *out_fd, char new_shell) 
{
	int Return;
	pid_t child_pid = fork();
	char *args[7];
	int i = 0;

	if (child_pid != 0)
	{
		/*File closed by father*/
		return child_pid;
	}

	else
	{
		char tmp_log[5] = "", tmp_in[5]="", tmp_out[5]="";
		sprintf(tmp_log, "%d", logfd);

    if (out_fd != NULL)	
    {
      close(out_fd[0]); sprintf(tmp_out, "%d", out_fd[1]);
    }
		if (in_fd != NULL)	
    {
      close(in_fd[1]); sprintf(tmp_in, "%d", in_fd[0]);
    }
		if (new_shell)
    {
			args[i+1] = "/usr/bin/konsole";
			args[i+1] = "-e";
		}
		args[i+1] = ex_name;
		args[i+1] = tmp_log;
		args[i+1] = tmp_in;
		args[i+1] = tmp_out;
		args[i+1] = (char*)NULL;

		Return = execvp(args[0], args);
		perror("FAIL");
		exit(Return);
	}
}

void height_tryup (int *crane_height)
{
	*crane_height = *crane_height>=MAX_HEIGHT? MAX_HEIGHT:*crane_height + SPEED;
}
void height_trydown	(int *crane_height)
{
	*crane_height = *crane_height<=MIN_HEIGHT? MIN_HEIGHT:*crane_height - SPEED;
}

/*Message functions*/
void msg_init		(msg_t *pp_msg)
{
	(*pp_msg) = (msg_t)malloc(sizeof(struct message));
	(*pp_msg)->crane_height = 0;
	(*pp_msg)->status = BOTTOM;
}
/**/
void msg_free(msg_t p_msg)
{
	free(p_msg);
}
/**/
void msg_setheight(msg_t p_msg, int crane_height)
{
	p_msg->crane_height = crane_height;
}
/**/
void msg_setstatus(msg_t p_msg, char status)
{
	p_msg->status = status;
}
/**/
int  msg_getheight(msg_t p_msg)
{
	return p_msg->crane_height;
}
/**/
char msg_getstatus(msg_t p_msg)
{
	return p_msg->status;
}
/**/
int  msg_getsize()
{
	return sizeof(struct message);
}