

#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <sys/types.h>
#include <sys/socket.h> /* pour les types de socket */ 
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>



void main()
{
	float borne_minimale = 0;
	float borne_maximale = 20;
	float n=0; ;
	int i;
	srand(time(NULL));
	int res;
	for (i = 0; i<100; i++)
	{
		n = rand();
		res = (int)(n * (borne_maximale + 1 - borne_minimale) / RAND_MAX + borne_minimale);
	}
	
	printf("NB = %d",res);
	getchar();
	
}