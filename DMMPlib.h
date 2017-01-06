#define MAXSTRING 100 /* Longueur des messages */
#define DELIM        ";"
#define BUFF_SIZE    128
#define clear; system("clear")
char* Get_properties(char cle[]);
void Create_EMPLACEMENT_File(void);
void Enregistrement_APPAREIL_EMPLACEMENT(char NumImm[],int quantite,char emplacement[],char nomFournisseur[]);
void List_File_EMPLACEMENT();
void ModifContainer(char IdContainer[][MAXSTRING],char Poids[]);
int Verify_Login(char[],char[]);
int Verify_Fournisseur(char nomFournisseur []);
char* getDateSystem();
int existDelivery(char emplacement[]);
char* getIdFromEmplacement(char emplacement[]);
int DecrementeQuantite(char emplacement[]);
typedef struct
{
   char *   login;
   char *   password;
}
Login;


struct trameDMMP
{
	char nomFournisseur[MAXSTRING];
	int  quantiteAppDiff; // sorte d'appareil différents
	int  quantite;
	char idAppareil[MAXSTRING];
	char type[MAXSTRING];
	char user[MAXSTRING];
	char password[MAXSTRING];
	char msg[MAXSTRING];
	char numImmatriculation[MAXSTRING];
	char idContainer[MAXSTRING][MAXSTRING];
	int	 nbContainer;
	char poids[MAXSTRING];
	char numEntreprise[MAXSTRING];
	char coord[MAXSTRING];
	int response;
};
struct PARC
{
	char coord[MAXSTRING];
	char idAppareil[MAXSTRING];
	int quantite;
	char flag[MAXSTRING];
	char nomFournisseur[MAXSTRING];
	char dateArrivee[MAXSTRING];
	char poids[MAXSTRING];
} ; 
	