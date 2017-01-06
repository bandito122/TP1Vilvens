/***************************************************************/
/*          Librairie Socket :                                    */
/*          Mastrolilli Bob                                    */
/***************************************************************/
#define MAXSTRING 100 /* Longueur des messages */
int connectClient(char* nommachine, int port);
struct sockaddr_in connectServeur(int* hSocket, char* nommachine, int port);
int acceptConnection(int hSocketEcoute, struct sockaddr_in adresseSocket);
int closeSocket(int hSocket);
int sendMessageTCP(int hSocket, void* msg, int length);
int receiveMessageTCP(int hSocket, void* msg,int length);
int PORT_SERVICE;
int sendMessage(int hSocket, char* msg);
int receiveMessage(int hSocket, char* msg, int length);
char marqueurRecu (char *m, int nc);