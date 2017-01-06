/***************************************************************/
/*          Librairie Socket : TCPLIB.C                        */
/*          Mastrolilli Bob                                    */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>			/* pour exit */
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>	
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include "TCPLIB.h"

int connectClient(char* nommachine, int port)
{
	int hSocket;
	struct hostent* infosHost;
	struct in_addr adresseIP;
	struct sockaddr_in adresseSocket;
	unsigned int tailleSockaddr_in;
	int ret;

	/*Création de la socket*/
	hSocket = socket(AF_INET,SOCK_STREAM,0);
	if (hSocket == -1)
	{
		printf("Erreur de création de la socket %d\n", errno);
		exit(1);
	}
	else printf("Creation de la socket OK\n");

	/*Acquisition des informations sur l'ordinateur distant*/
	if((infosHost = gethostbyname(nommachine)) == 0 )
	{
		printf("Erreur d'acquisition d'infos sur le host %d\n",errno);
		exit(1);
	}
	else printf("Acquisition info host OK\n");
	memcpy(&adresseIP, infosHost->h_addr, infosHost->h_length);

	/*Préparation de la structure sockaddr_in*/
	memset(&adresseSocket, 0, sizeof(struct sockaddr_in));
	adresseSocket.sin_family = AF_INET;
	adresseSocket.sin_port = htons(port);
	memcpy(&adresseSocket.sin_addr, infosHost->h_addr, infosHost->h_length);

	/*Tentative de connexion*/
	tailleSockaddr_in = sizeof(struct sockaddr_in);
	if (( ret = connect(hSocket, (struct sockaddr *)&adresseSocket, tailleSockaddr_in) ) == -1)
	{
		printf("Erreur sur connect de la socket %d\n", errno);
		close(hSocket); 
		exit(1);
	}
	else printf("Connect socket OK\n");

	return hSocket;
}


struct sockaddr_in connectServeur(int* hSocket, char* nommachine, int port)
{
	struct hostent* infosHost;			/* handle de la socket	*/
	struct in_addr adresseIP;			/* infos sur le host	*/
	struct sockaddr_in adresseSocket;	/*	Structure de type sockaddr contenant les infos adresses */

	/*Création de la socket*/
	*hSocket = socket(AF_INET,SOCK_STREAM,0);
	if (*hSocket == -1)
	{
		printf("Erreur de création de la socket %d\n", errno);
		adresseSocket.sin_port = -1;
		return adresseSocket;
	}
	else printf("Creation de la socket OK\n");

	/*Acquisition des informations sur l'ordinateur local*/
	if((infosHost = gethostbyname(nommachine)) == 0 )
	{
		printf("Erreur d'acquisition d'infos sur le host %d\n",errno);
		adresseSocket.sin_port = -1;
		return adresseSocket;
	}
	else printf("Acquisition info host OK\n");
	memcpy(&adresseIP, infosHost->h_addr, infosHost->h_length);

	/*Préparation de la structure sockaddr_in*/
	memset(&adresseSocket, 0, sizeof(struct sockaddr_in));
	adresseSocket.sin_family = AF_INET;
	adresseSocket.sin_port = htons(port);
	memcpy(&adresseSocket.sin_addr, infosHost->h_addr, infosHost->h_length);

	/*Le système prend connaissance de l'adresse et du port de la socket */
	if(bind(*hSocket, (struct sockaddr*)&adresseSocket, sizeof(struct sockaddr_in)) == -1)
	{
		printf("Erreur sur le bind de la socket %d\n", errno);
		adresseSocket.sin_port = -1;
		return adresseSocket;
	}
	else printf("Bind adresse et port socket OK\n");

	/*Mise en écoute */
	if (listen(*hSocket, SOMAXCONN) == -1)
	{
		printf("Ereur sur le listen de la socket %d\n", errno);
		close(*hSocket);		
		adresseSocket.sin_port = -1;
		return adresseSocket;
	}
	else printf("Listen socket OK\n");

	return adresseSocket;
}

int acceptConnection(int hSocketEcoute, struct sockaddr_in adresseSocket)
{
	int hSocketService;
	int tailleSockaddr_in;

	tailleSockaddr_in = sizeof(struct sockaddr_in);
	if ((hSocketService = accept(hSocketEcoute, (struct sockaddr*)&adresseSocket, &tailleSockaddr_in)) == -1)
	{
		printf("Erreur sur l'accept de la socket %d\n",errno);
		close(hSocketEcoute);
		exit(1);
	}
	else printf("Accept socket OK\n");

	return hSocketService;
}

int closeSocket(int hSocket)
{
	/*Fermeture de la socket*/
	close(hSocket);
	printf("Socket fermee\n");
}

int sendMessageTCP(int hSocket, void *Requete, int length)
{
	int nbreEnv = 0;

	if ((nbreEnv = send(hSocket, Requete, length, 0)) == -1)
	{
		printf("Erreur sur le send de la socket %d\n", errno);
		close(hSocket);
		exit(1);
	}
	else printf("Message TCP sent !\n ");
	return nbreEnv;
}

int receiveMessageTCP(int hSocket, void *Requete,int length)
{	
	char msgTmp[MAXSTRING];
	int tailleMsgRecu = 0;
	int nbreBytesRecus = 0;
	int finDetectee = 0;
	
		if((nbreBytesRecus = recv(hSocket, Requete, length, 0)) == -1)
		{
			printf("Erreur sur le recv de la socket %d\n", errno);
			close(hSocket);
			exit(1);
		}
		else 
		{
			printf("Message TCP received ! \n");
		}
	
	return nbreBytesRecus;
}
