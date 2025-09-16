#include "stdio2.h"

#define	MAXUTEX	10
#define MAXBUMP	10

VOID	(*usertext[MAXUTEX])() = {NULL},
	(*bumping [MAXBUMP])() = {NULL};

