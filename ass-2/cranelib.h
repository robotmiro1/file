#define _CRANELIB_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>

#define MAX_HEIGHT 200
#define MIN_HEIGHT 0
#define SPEED 5 

// Server - Reader message structure
typedef struct message* msg_t;

// Crane status
enum status 
  {
  STOP = 'S', UP = 'U', DOWN = 'D', EXIT = 'E', TOP = 'T', BOTTOM = 'B'
  };

/*Decrease the crane height*/
void height_trydown(int *crane_height);

/*Increase the crane height*/
void height_tryup(int *crane_height);

/*Spawn a child process*/
void error(char *msg, int Return);

/*Message status*/
void msg_setstatus(msg_t p_msg, char status);

/*Message height*/
void msg_setheight(msg_t p_msg, int crane_height);

/*Get message status*/
char msg_getstatus(msg_t p_msg);

void msg_init(msg_t *pp_msg);

/*Get message of crane height*/
int  msg_getheight(msg_t p_msg);

/*Size of message*/
int  msg_getsize();

/*Free the memory space*/
void msg_free(msg_t p_msg);