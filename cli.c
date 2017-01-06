#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <sys/types.h>
#include <sys/socket.h> /* pour les types de socket */ 
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h> /* memset */
#include <unistd.h> /* close */
#include "TCPLIB.h"
#include "DMMPlib.h"
#include <stdbool.h>
#define EOC "END_OF_CONNEXION"
#include <signal.h>

void handlerSigint(int sig);

int hSocket; /* handle de la socket */
struct trameDMMP trameQuit;
void main() 
{

	struct hostent * infosHost; /* pour gethostbyname */
	struct in_addr adresseIP; /* Adresse Internet au format reseau */ 
	struct sockaddr_in adresseSocket;
	/* Structure de type sockaddr contenant les infos adresses - ici, cas de TCP */ 
	unsigned int tailleSockaddr_in;
	int finConnexion,cpt=0,ret,choix;
	char buff[MAXSTRING];char *msg = NULL;
	char buffer_port[10];
	
	strcpy(buffer_port, Get_properties("PORT_SERVICE"));
	sprintf(buffer_port, "%d", PORT_SERVICE);
	hSocket= connectClient("127.0.0.1",70000);
	
	/*Etat du client*/
	int STATE=0;
	int test=0;
	/*Armement sur le signal SIGINT */
	struct sigaction act;	
	struct trameDMMP trameLogin;
	struct trameDMMP trameLoginReponse;
	struct trameDMMP trameQuit;
	act.sa_handler = handlerSigint;
	sigemptyset(&act.sa_mask);
	sigaction(SIGINT, &act, 0);
	
		do
		{		
			if(STATE==0)
			{
				memset(&trameLogin, 0, sizeof trameLogin);
				printf("\nNom d'utilisateur: ");
				fflush(stdin);
				gets(buff);
				signal(SIGINT,handlerSigint);
				strcpy(trameLogin.user, buff);
				printf("\nMot de passe: ");
				fflush(stdin);
				gets(buff);
				strcpy(trameLogin.password, buff);
				strcpy(trameLogin.type, "LOGIN");
				sendMessageTCP(hSocket,&trameLogin,sizeof(struct trameDMMP ));
				receiveMessageTCP(hSocket, &trameLoginReponse,sizeof(struct trameDMMP));
				printf("Reponse = %s\n", trameLoginReponse.msg);
				if(strcmp(trameLoginReponse.msg,"OUI") ==0)
				{
					printf("connexion reussie! \n");
					STATE=1;
				}
				else
				{
					printf("Login or password incorrect!!!\n");
					STATE = 0;
				}
				cpt++;
			}
			else
			{
				struct trameDMMP trameInputDevice1;
				struct trameDMMP trameInputDevice2;
				struct trameDMMP trameGetDelivery1;
				struct trameDMMP trameGetDelivery2;
				struct trameDMMP trameGetDeliveryEnd;
				
					do
					{
						
						printf("\nQue voulez-vous faire?\n");
						printf("1) INPUT_DEVICES1.\n");
						printf("2) INPUT-DEVICE2.\n");
						printf("3) GET-DELIVERY-1.\n");
						printf("4) GET-DELIVERY-2.\n");
						printf("5) GET-DELIVERY-END.\n");
						printf("6) DECONNECT.\n");
						printf("choix :  ");scanf("%d", &choix);
						if( (choix ==1 && STATE ==1)  || (choix ==2 && STATE ==2) || (choix ==3 && STATE ==1) || (choix ==4 && STATE ==3) || (choix ==5 && (STATE ==3 || STATE==4) ) || (choix ==6 && STATE !=0))
							test=1;
						else
							test=0;
						
						
					 }while( test != 1 );
					
						
					char temp[MAXSTRING];
					int tempInt;
					switch(choix)
					{
						case 1:  	
						memset(&trameInputDevice1, 0, sizeof trameInputDevice1);
						printf("Nom fournisseur : ");
						fflush(stdin);
						scanf("%s",&temp);
						strcpy(trameInputDevice1.nomFournisseur, temp);
						strcpy(trameInputDevice1.type, "INPUT_DEVICE1");
						fflush(stdin);
						sendMessageTCP(hSocket,&trameInputDevice1,sizeof(struct trameDMMP ));
						receiveMessageTCP(hSocket,  &trameInputDevice1,sizeof(struct trameDMMP));
						
						printf("Reponse du serveur conteneur = %s", trameInputDevice1.msg);
						if(strcmp(trameInputDevice1.msg,"OUI") ==0)
						{
							printf("\nPassage vers INPUT2! \n");
							STATE= 2;
						}

						/* ENVOIE COMMANDE INPUTDEVICE2 */
						memset(&trameInputDevice2, 0, sizeof trameInputDevice2);

							break;
						case 2:    
					    printf("Combien avez vous de sorte d'appareil? ");
						fflush(stdin);
						scanf("%d",&tempInt);
						int nbAppareil;
						nbAppareil=tempInt;
						trameInputDevice2.quantiteAppDiff = nbAppareil;
						fflush(stdin);
						int i ;
						
						for (i=0; i < nbAppareil; i++)
						{
							printf("Num Appareil %d svp ? : ",i);
							scanf("%s",&temp);
							strcpy(trameInputDevice2.idAppareil, temp);
							printf("Quantité appareil %d ? : ",i);
							scanf("%d",&tempInt);
							printf("\n");
							trameInputDevice2.quantite=tempInt;
							strcpy(trameInputDevice2.nomFournisseur,trameInputDevice1.nomFournisseur);
							strcpy(trameInputDevice2.type, "INPUT_DEVICE2");
							sendMessageTCP(hSocket,&trameInputDevice2,sizeof(struct trameDMMP ));
							memset(&trameInputDevice2, 0, sizeof trameInputDevice2);
							receiveMessageTCP(hSocket,  &trameInputDevice2,sizeof(struct trameDMMP));
							printf("Reponse du serveur conteneur = %s", trameInputDevice2.coord);
							fflush(stdin);
						}
					
							STATE = 1;
							break;
						case 3:
							printf("Num entreprise du véhicule svp ? : ");
							scanf("%s",&temp);
							strcpy(trameGetDelivery1.numEntreprise, temp);
							printf("Zone de destination (x,y) : ");
							scanf("%s",&temp);
							strcpy(trameGetDelivery1.coord, temp);
							strcpy(trameGetDelivery1.type, "GET_DELIVERY1");
							sendMessageTCP(hSocket,&trameGetDelivery1,sizeof(struct trameDMMP ));
							memset(&trameGetDelivery1, 0, sizeof trameGetDelivery1);
							receiveMessageTCP(hSocket,  &trameGetDelivery1,sizeof(struct trameDMMP));
							printf("Reponse du serveur conteneur = %s", trameGetDelivery1.msg);
							if(trameGetDelivery1.response == 0) // SI NON --> rien pour cette zone
							{
								STATE = 1 ;
							}
							else
							{
								STATE = 3  ;  // SI OUI, --> attente get_delivery2 ou get_delivery_end
							}
							break;
						case 4:
								do
								{
									printf("1) Id-suivant ? .\n");
									printf("2) Véhicule rempli.\n");
									printf("choix :  ");scanf("%d", &choix);
								}while(choix < 1 ||  choix >2 );
							
							if(choix == 1)
							{
								strcpy(trameGetDelivery2.coord, trameGetDelivery1.coord); // les coordonnées de l'emplacement n'ont pas changé
								strcpy(trameGetDelivery2.msg, "NEXT");
								strcpy(trameGetDelivery2.type, "GET_DELIVERY2");
								sendMessageTCP(hSocket,&trameGetDelivery2,sizeof(struct trameDMMP ));
								memset(&trameGetDelivery2, 0, sizeof trameGetDelivery1);
								receiveMessageTCP(hSocket,  &trameGetDelivery2,sizeof(struct trameDMMP));
								if(trameGetDelivery2.response == 0) //si le serveur lui répond NON, c'est qu'il n'y a plus rien à prendre donc --> get_delivery_end
								{
									STATE = 4;
								}
								else  //le serveur a répondu OUI, le client peut refaire un get_delivery2 (l'état en change pas)
								{
									STATE = 3;
								}
								
							}
							else if (choix ==2) // signifie que le camion est plein
							{
								strcpy(trameGetDelivery2.msg, "FULL");
								strcpy(trameGetDelivery2.type, "GET_DELIVERY2");
								sendMessageTCP(hSocket,&trameGetDelivery2,sizeof(struct trameDMMP ));
								memset(&trameGetDelivery2, 0, sizeof trameGetDelivery1);
								receiveMessageTCP(hSocket,  &trameGetDelivery2,sizeof(struct trameDMMP));
								STATE =4 ;
							}
							printf("Reponse du serveur conteneur = %s", trameGetDelivery2.msg);
							

							break;
						case 5: 
							strcpy(trameGetDeliveryEnd.type, "GET_DELIVERY_END");
							sendMessageTCP(hSocket,&trameGetDeliveryEnd,sizeof(struct trameDMMP ));
							memset(&trameGetDeliveryEnd, 0, sizeof trameGetDeliveryEnd);
							receiveMessageTCP(hSocket,  &trameGetDeliveryEnd,sizeof(struct trameDMMP));
							printf("Reponse du serveur conteneur = %s", trameGetDeliveryEnd.msg);
							if(trameGetDeliveryEnd.response == 1)
							{
								STATE = 1;
							}
							break;
						case 6:
							STATE = 6;
							strcpy(trameQuit.type, "EOC");
							sendMessageTCP(hSocket,&trameQuit,sizeof(struct trameDMMP ));
							printf("Message envoye = %s\n", trameQuit.msg);
							close(hSocket);
							break;
						default:   // exit(1);
							break;
					 }
			}	
		}
		while (strcmp(trameLogin.msg, EOC) && STATE !=6);
		
		close(hSocket); /* Fermeture de la socket */ 
		printf("Socket client fermee\n");
		printf("%d messages envoyes !", cpt);
}
void handlerSigint(int sig)
{
	
	strcpy(trameQuit.type, "EOC");
	puts("Passage dans Sigint");
	sendMessageTCP(hSocket,&trameQuit,sizeof(struct trameDMMP ));
	printf("Message envoye = %s\n", trameQuit.msg);
	close(hSocket);
	exit(0);
}

