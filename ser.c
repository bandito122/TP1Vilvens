#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <sys/types.h>
#include <sys/socket.h> /* pour les types de socket */ 
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <string.h> /* memset */
#define EOC "END_OF_CONNEXION"
#include "DMMPlib.h"
#include "TCPLIB.h"


/*Fonctions */

int test_end_connexion(char msg[]);
void handlerfin(int sig);
int hSocketEcoute,hSocketService;
struct sigaction act;	
int rand_number();



/* Variables de configuration GLOBALES */



void main() 
{
	struct hostent * infosHost;  
	struct in_addr adresseIP;
	struct sockaddr_in adresseSocket; 
	int tailleSockaddr_in,nbreRecv;
	char msgClient[MAXSTRING], msgServeur[MAXSTRING]; 
	struct trameDMMP trame;
	int CONNECTED_STATE=0,INPUT_DEVICE1_STATE=0,INPUT_DONE_STATE=0,STATE=0;
	int count_quantity_serveur = 0 ; // quantité servant pour compter le nombre d'appareil du client et remettre l'état du compteur à 1.
	char buffer_port[10];
	
	strcpy(buffer_port, Get_properties("PORT_SERVICE"));
	sprintf(buffer_port, "%d", PORT_SERVICE);
	adresseSocket = connectServeur(&hSocketEcoute,"127.0.0.1",70000);
	hSocketService=acceptConnection(hSocketEcoute,adresseSocket);
	if ( hSocketService == -1 )
    perror("CreateSockets") ;
	else
    printf(" CreateSockets : %d \n",hSocketService) ;
	
	Create_EMPLACEMENT_File();
	act.sa_handler = handlerfin;
	sigemptyset(&act.sa_mask);
	sigaction(SIGQUIT, &act, 0);

	do
	{
		printf("***** En attente d'un message client *****\n");
		memset(&trame, 0, sizeof trame);
		nbreRecv= receiveMessageTCP(hSocketService,&trame,sizeof(struct trameDMMP));
		if ( nbreRecv == -1 )
			perror("ReceiveDatagram") ;
		test_end_connexion(trame.type);
		
		if(STATE == 0) //si le client n'est pas encore authentifié
		{
			/*SI REQUETE LOGIN */
			if(!strcmp(trame.type,"LOGIN"))
			{
				STATE=Verify_Login(trame.user,trame.password);
				printf("Ceci est une requete LOGIN\n");
			}
			if(STATE==1) //si login ok
			{
				printf("LOGIN OK");
				memset(trame.msg, 0, strlen(trame.msg)*sizeof(char) + 1);
				sprintf(trame.msg,"OUI"); 
				STATE=1;
				sendMessageTCP(hSocketService,&trame,sizeof(struct trameDMMP ));
				printf("\nEnvoie accuse reception. \n");	
			}
			else if(STATE==0) //si login pas ok, juste accuser pour qu'il aille se faire *****
			{	
				printf("LOGIN NOT OK");
				memset(trame.msg, 0, strlen(trame.msg)*sizeof(char) + 1);
				sprintf(trame.msg,"NON"); 
				sendMessageTCP(hSocketService,&trame,sizeof(struct trameDMMP ));
				if ( nbreRecv == -1 )
				printf("Envoie accuse reception. \n");
			}
		}
		else if (STATE > 0) // le client chercher à faire un INPUT-DEVICE1 ou GET_DELIVERY1
		{
	
			if(strcmp(trame.type,"INPUT_DEVICE1")==0 && STATE ==1)
			{
				printf("***Received INPUT-DEVICE1 request***\n");
				int test = 0;
				test=Verify_Fournisseur(trame.nomFournisseur);
				memset(trame.msg, 0, strlen(trame.msg)*sizeof(char) + 1);
				if(test)
				{
					STATE=2;
					sprintf(trame.msg,"OUI"); 
					sendMessageTCP(hSocketService,&trame,sizeof(struct trameDMMP ));
					printf("\nEnvoie accuse reception. \n");	
				}
				else
				{
				STATE = 1;
				sprintf(trame.msg,"NON"); 
				sendMessageTCP(hSocketService,&trame,sizeof(struct trameDMMP ));
				printf("\nEnvoie accuse reception. \n");	
				}
			}
			if(strcmp(trame.type,"INPUT_DEVICE2")==0 && STATE == 2)
			{
					memset(trame.msg, 0, strlen(trame.msg)*sizeof(char) + 1);
					printf("***Received INPUT_DEVICE2 request***\n");
					int i=0 ;
					int n1=0,n2=0;
					char coordonnees[20];
					do // RECHERCHE D'UN EMPLACEMENT VIDE
					{
						
						/*Generation d'un nombre aléatoire */
						n1=0;
						n2=0;
					
						n1=(int) rand_number();
						n2=(int) rand_number();

						stpcpy(coordonnees,"("); // pourra dire non grace au server Trafic !
						char buffer [15];
						sprintf(buffer, "%d", n1);
						strcat(coordonnees,buffer); // emplacement aléatoire, au préalabre utilier rand()
						strcat(coordonnees,",");
						char buffer2 [15];
						sprintf(buffer, "%d", n2);
						strcat(coordonnees,buffer); // emplacement aléatoire, au préalabre utilier rand()
						strcat(coordonnees,")");
						printf("Message à renvoyer : %s",trame.coord);
						i++;
						strcpy(trame.coord,coordonnees);
					}while(existDelivery(trame.coord));
					
					
					Enregistrement_APPAREIL_EMPLACEMENT(trame.idAppareil,trame.quantite,trame.coord,trame.nomFournisseur);
					
					List_File_EMPLACEMENT();
					count_quantity_serveur ++;
					if((trame.quantiteAppDiff)== (count_quantity_serveur ))
					{
						STATE = 1;
						count_quantity_serveur=0;
					}
					
					sendMessageTCP(hSocketService,&trame,sizeof(struct trameDMMP ));
					printf("\nEnvoie accuse reception. \n");
				
			}
			if(strcmp(trame.type,"GET_DELIVERY1")==0 && STATE ==1)
			{
				printf("***Received GET_DELIVERY1 request***\n");
				int test = 0;
				test=existDelivery(trame.coord);
				char idApp[30];
				strcpy(idApp,getIdFromEmplacement(trame.coord));
				//memset(trame.msg, 0, strlen(trame.msg)*sizeof(char) + 1);
				if(test == 2) // appareil dispo
				{
					STATE=3;
					DecrementeQuantite(trame.coord);
					trame.response = 1 ;
					sprintf(trame.msg,"OUI + id premier appareil à charger : %s", idApp); 
					sendMessageTCP(hSocketService,&trame,sizeof(struct trameDMMP ));
					printf("\nEnvoie accuse reception. \n");	
				}
				else if(test==1) // plus d'appareil
				{
					STATE = 1;
					trame.response = 0;
					sprintf(trame.msg,"NON + PLUS RIEN POUR CETTE ZONE"); 
					sendMessageTCP(hSocketService,&trame,sizeof(struct trameDMMP ));
					printf("\nEnvoie accuse reception. \n");	
				}
				else if (test==0) // commande invalide
				{
					STATE = 1;
					trame.response = 0 ;
					sprintf(trame.msg,"NON + COMMANDE INVALIDE"); 
					sendMessageTCP(hSocketService,&trame,sizeof(struct trameDMMP ));
					printf("\nEnvoie accuse reception. \n");
				}
			}
			if(strcmp(trame.type,"GET_DELIVERY2")==0 && STATE ==3)
			{
				printf("***Received GET_DELIVERY2 request***\n");
				
				if (strcmp(trame.msg,"FULL")==0)
				{
					printf("Reception message camion plein... \n");
					STATE = 4 ;
					trame.response = 1 ;
					sprintf(trame.msg,"OK + attente de la commande GET_DELIVERY_END ! "); 
					sendMessageTCP(hSocketService,&trame,sizeof(struct trameDMMP ));
					printf("\nEnvoie accuse reception. \n");
				}
				else // app suivant
				{
					int test = 0;
					test=existDelivery(trame.coord);
					char idApp[30];
					strcpy(idApp,getIdFromEmplacement(trame.coord));
					//memset(trame.msg, 0, strlen(trame.msg)*sizeof(char) + 1);
					if(test == 2) // appareil dispo
					{
						STATE=3;
						DecrementeQuantite(trame.coord);
						trame.response = 1 ;
						sprintf(trame.msg,"OUI + id appareil suivant à charger : %s", idApp); 
						sendMessageTCP(hSocketService,&trame,sizeof(struct trameDMMP ));
						printf("\nEnvoie accuse reception. \n");	
					}
					else if(test==1) // plus d'appareil + le serveur attend get_delivery_end
					{
						STATE = 4;
						trame.response = 0 ;
						sprintf(trame.msg,"NON + PLUS RIEN POUR CETTE ZONE"); 
						sendMessageTCP(hSocketService,&trame,sizeof(struct trameDMMP ));
						printf("\nEnvoie accuse reception. \n");	
					}
					else if (test==0) // commande invalide
					{
						STATE = 1;
						trame.response =  0 ;
						sprintf(trame.msg,"NON + COMMANDE INVALIDE"); 
						sendMessageTCP(hSocketService,&trame,sizeof(struct trameDMMP ));
						printf("\nEnvoie accuse reception. \n");
					}
				}
				
			}
			if(strcmp(trame.type,"GET_DELIVERY_END")==0 && (STATE ==3 || STATE ==4 ))
			{
				trame.response = 1 ;
				sprintf(trame.msg,"OUI + notification déjà enregistrée... "); 
				sendMessageTCP(hSocketService,&trame,sizeof(struct trameDMMP ));
				printf("\nEnvoie accuse reception. \n");
				STATE=1;
			}
				
		}
	}while(1); // ça s'arrête jamais
}


int test_end_connexion(char msg[])
{
	/*Armement sur le signal SIGKILL */
	if (strcmp(msg,"EOC")==0)
	{
		raise(SIGQUIT);
		puts("INTERRUPTION CLIENT!ç!!!!!!!!!!");
	}
}
void handlerfin(int sig)
{
	puts("Passage dans Sigusr");
	    /*Fermeture des sockets */
		close(hSocketService); /* Fermeture de la socket */ 
		printf("Socket connectee au client fermee\n"); 
		close(hSocketEcoute); /* Fermeture de la socket */ 
	printf("Socket serveur fermee\n");
	exit(0);
}
int rand_number()
{
	float borne_minimale = 0;
	float borne_maximale = 20;
	float n=0; ;
	int i;
	//srand(time(NULL));
	int res;
	for (i = 0; i<100; i++)
	{
		n = rand();
		res = (int)(n * (borne_maximale + 1 - borne_minimale) / RAND_MAX + borne_minimale);
	}
	return res;
}
