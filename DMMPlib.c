#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <sys/types.h>
#include <sys/socket.h> /* pour les types de socket */ 
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h> /* memset */
#include <unistd.h> /* close */
#define EOC "END_OF_CONNEXION"
#include "DMMPlib.h"
char* Get_properties(char cle[])
{
	FILE * pFile;
	char mystring[100];
	char * token = NULL;
	char * token2 = NULL;
	const char s[2] = "=";
	char mdp[100];
	char tmp[50];
	pFile = fopen("serv_mouv.conf", "r");

	if (pFile == NULL)
	{
		perror("Error opening file");
	}
	else
	{
		while (fgets(mystring, 100, pFile) != NULL) // la ligne
		{
			/* get the first token */
			token = strtok(mystring, s);

			if (strcmp(token, cle) == 0)
			{
				if (token != NULL)
				{
					token2 = strtok(NULL, s);
					strcpy(tmp, token2);
					size_t ln = strlen(tmp) - 1;
					if (tmp[ln] == '\n')
					{
						tmp[ln] = '\0';
					}
					puts(tmp);
					break;
							
				}	
			}
			
		}
		return tmp;
	}
	fclose(pFile);
}

void Enregistrement_APPAREIL_EMPLACEMENT(char NumImm[],int quantite,char emplacement[],char nomFournisseur[])
{
    struct PARC trans;
    FILE *ptF;

    printf("----------SAVE INTO FILE--------------------\n");
    
	
    if((ptF=fopen("EMPLACEMENT.dat","r+b"))==NULL)
    {
        perror("Erreur de lecture de EMPLACEMENT.dat");
    }
    else
    {
		strcpy(trans.coord,emplacement); // généré aléatoirement : changer
		int i;

			strcpy(trans.idAppareil,NumImm);
			printf("%s ",&trans.idAppareil);
			strcpy(trans.flag,"1");//1 pris, 0 pas pris
			trans.quantite = quantite;
			strcpy(trans.nomFournisseur,nomFournisseur);
			strcpy(trans.dateArrivee,getDateSystem()); 
			strcpy(trans.poids,"100kg");
			fflush(stdin);
			fseek(ptF,0,SEEK_END);
			if(!fwrite(&trans,sizeof(struct PARC),1,ptF))
			{
				perror("Erreur de fwrite");
			}
			else
				printf("J'ai écrit dans le fichier EMPLACEMENT\n");
			
    } 
    fclose(ptF);
	printf("Fichier EMPLACEMENT fermé\n");
    
}
void List_File_EMPLACEMENT()
{
    struct PARC trans;
    FILE *ptF;
    int i=1;
    
    ptF=fopen("EMPLACEMENT.dat","r");
	
	printf("\n ************VOICI CE QU'IL Y DANS FICHIER EMPLACEMENT*****************\n");
    while(fread(&trans,1,sizeof(struct PARC),ptF))
    {
		printf("---------------------RECORD %d--------------------\n",i);
		printf("Coordon\ées de l'emplacement : %s\n",trans.coord);
		printf("Id appareil : %s\n",&trans.idAppareil);
		printf("Quantite : %d\n",trans.quantite);
		printf("Flag : %s\n",trans.flag);
		printf("Nom Fournisseur : %s\n",&trans.nomFournisseur);
		printf("Date de stockage : %s\n",trans.dateArrivee);
		printf("Poids : %s\n",trans.poids);
        i++;
    }
	printf("\n ************FIN DU FICH_PARC*****************\n");
    fclose(ptF);
}


int existDelivery(char emplacement[])
{

    struct PARC trans;
    FILE *ptF;
    int i=0,test=0;
    
    printf("     CHECK DELIVERY.\n");
    printf("------------------------------\n");

    
    if((ptF=fopen("EMPLACEMENT.dat","r+"))==NULL)
    {
        perror("Erreur de lecture de EMPLACEMENT.dat");
    }
    else
    {
		while(fread(&trans,1,sizeof(struct PARC),ptF))
		{
		
			if(strcmp(trans.coord,emplacement)==0)
			{
				test=1;
				printf("Il existe bien un appareil à livrer... \n");
				if(trans.quantite > 0) // il existe bien un appareil à livrer
				{
					test=2;
					printf("Il existe bien un appareil à livrer... \n");
				}
				break;
			}
			i++;
		}

    }   
    fclose(ptF);
    return test;
}
int DecrementeQuantite(char emplacement[])
{

    struct PARC trans;
    FILE *ptF;
    int i=0,test=0;
    
    printf("     DECREMENTE DELIVERY.\n");
    printf("------------------------------\n");

    
    if((ptF=fopen("EMPLACEMENT.dat","r+"))==NULL)
    {
        perror("Erreur de lecture de EMPLACEMENT.dat");
    }
    else
    {
		while(fread(&trans,1,sizeof(struct PARC),ptF))
		{
			if(strcmp(trans.coord,emplacement)==0)
			{
				test=1;
				if(trans.quantite > 0) // il existe bien un appareil à livrer
				{
					fseek(ptF,i*sizeof(struct PARC),SEEK_SET);
					trans.quantite = trans.quantite -1 ;
					printf("DECREMENTE QUANTITE OK...\n");
					if(!fwrite(&trans,sizeof(struct PARC),1,ptF))
					{
						perror("Erreur de fwrite");
					}
				}
				break;
			}
			i++;
		}

    }   
    fclose(ptF);
    return test;
}
char* getIdFromEmplacement(char emplacement[])
{

    struct PARC trans;
    FILE *ptF;
    int i=0;
    
    printf("     CHECK DELIVERY.\n");
    printf("------------------------------\n");

    
    if((ptF=fopen("EMPLACEMENT.dat","r+"))==NULL)
    {
        perror("Erreur de lecture de EMPLACEMENT.dat");
    }
    else
    {
		while(fread(&trans,1,sizeof(struct PARC),ptF))
		{
		
		
			if(strcmp(trans.coord,emplacement)==0)
			{
				if(trans.quantite > 0) // il existe bien un appareil à livrer
				{
					return &trans.idAppareil ;
				}
				else
					return "";
				break;
			}
			i++;
		}

    }   
    fclose(ptF);
}
void Create_EMPLACEMENT_File(void)
{
    struct PARC trans;
    FILE *ptF;
    int ret;
    printf("Creer le fichier EMPLACEMENT.\n");
    printf("------------------------------\n");
    
    if((ptF=fopen("EMPLACEMENT.dat","r+b"))==NULL)
    {
        if((ptF=fopen("EMPLACEMENT.dat","wb"))==NULL)
            perror("Erreur de creation de EMPLACEMENT.dat");
        else
        {
            printf("FICHIER EMPLACEMENT CRÉE! \n");
        }
    }
    else
    {
        
    }
    fclose(ptF);
}
int Verify_Login(char login [],char password[])
{
	FILE * pFile;
	char mystring[100];
	char * token = NULL;
	char * token2 = NULL;
	const char s[2] = ";";
	int test = 0;
	char mdp[100];
	pFile = fopen("login.properties", "r");

	if (pFile == NULL)
	{
		perror("Error opening file");
	}
	else
	{
		while (fgets(mystring, 100, pFile) != NULL)
		{
			printf("%s", mystring);
			/* get the first token */
			token = strtok(mystring, s);

			if (strcmp(token, login) == 0)
			{
				/* walk through other tokens */
				printf(" Le user est le bon! \n");
				if (token != NULL)
				{
					printf(" %s\n", token);
					token2 = strtok(NULL, s);
					printf("pass =%s\n", password);
					printf("token2 =%s\n", token2);
					char tmp[50];
					strcpy(tmp, token2);
					size_t ln = strlen(tmp) - 1;
					if (tmp[ln] == '\n')
						tmp[ln] = '\0';
					if (strcmp(tmp, password) == 0)
					{
						printf(" Le mdp est le bon! \n");
					
						printf(" %s\n", token2);
						test =1;
						break;
					}
				}
				else
					test = 0;
			}
			else
			{
				test = 0;
			}	
		}
		if(test)
			printf("authentification OK\n");
		else
			printf("authentification pas OK\n");
	}
	fclose(pFile);
	return test;
}
int Verify_Fournisseur(char nomFournisseur [])
{
	FILE * pFile;
	char mystring[100];
	char * token = NULL;
	char * token2 = NULL;
	const char s[2] = ";";
	int test = 0;
	char mdp[100];
	pFile = fopen("fournisseurs.txt", "r");

	if (pFile == NULL)
	{
		perror("Error opening file");
	}
	else
	{
		while (fgets(mystring, 100, pFile) != NULL)
		{
			printf("%s", mystring);
			/* get the first token */
			token = strtok(mystring, s);

			
				/* walk through other tokens */
		
				if (token != NULL)
				{
					printf(" %s\n", token);
					token2 = strtok(NULL, s);
					printf("pass =%s\n", nomFournisseur);
					printf("token2 =%s\n", token2);
					char tmp[50];
					strcpy(tmp, token2);
					size_t ln = strlen(tmp) - 1;
					if (tmp[ln] == '\n')
						tmp[ln] = '\0';
					if (strcmp(tmp, nomFournisseur) == 0)
					{
						printf(" Le fournisseur est le bon! \n");
					
						printf(" %s\n", token2);
						test =1;
						break;
					}
				}
				else
					test = 0;
			
			
		}
		if(test)
			printf("authentification OK\n");
		else
			printf("authentification pas OK\n");
	}
	fclose(pFile);
	return test;
}
char* getDateSystem()
{
	time_t t = time(NULL);
	
	return ctime(&t);
}
