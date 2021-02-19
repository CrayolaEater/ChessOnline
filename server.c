#include<stdbool.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<errno.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>
#define PORT 2908
extern int errno;

typedef struct thData
{
    int idThread; //id-ul thread-ului tinut in evidenta de acest program
    int cl; //descriptorul intors de accept
} thData;
thData Clienti[10];
unsigned char nrClienti;
int TablaSah[8][8];
char mutare[5];
void ReseteazaJocul()
{
    for(int i=0; i<= 7; i ++)
    {
        TablaSah[0][i]=TablaSah[1][i]=120;
        TablaSah[6][i]=TablaSah[7][i]=20;
    }
}
bool MutaAlb=true;
bool SePoateMuta(char unde_col, char unde_linie, char spre_col, char spre_line)
{
    int linie_curenta = unde_linie - 49;
    int col_curenta = unde_col - 'a' - 1;
    int linie_dest = spre_line - 49;
    int col_dest = spre_col - 'a' - 1;
    if(TablaSah[linie_curenta][col_curenta]%100==2) // daca avem pion
    {
        if(MutaAlb)
        {
            if(col_dest != col_curenta || linie_dest > linie_curenta)
            {
                return false;
            }
        }
        else
        {
            if(col_dest != col_curenta || linie_dest < linie_curenta)
            {
                return false;
            }
        }
        return true;
    }
    return true;
}
void raspuns_clienti(void *arg)
{
    struct thData tdL= *((struct thData*)arg);
    int playerNR=0;
    read(tdL.cl, &playerNR, sizeof(playerNR));
    if(playerNR==1)
    {
        if(MutaAlb)
        {
            playerNR = 0;
        }
        else

        {
            playerNR = -1;
        }
        write(tdL.cl, &playerNR, sizeof(playerNR));
        if(MutaAlb)
        {
            write(tdL.cl, mutare, sizeof(mutare));
            read(tdL.cl, mutare, sizeof(mutare));
            printf("Player-ul 1 a facut mutarea[%s]\n", mutare);
            MutaAlb = !MutaAlb;
        }
    }
    else if(playerNR==2)
    {
        if(MutaAlb)
        {
            playerNR = -1;
        }
        else
        {
            playerNR = 0;
        }
        write(tdL.cl, &playerNR, sizeof(playerNR));
        if(!MutaAlb)
        {
            write(tdL.cl, mutare, sizeof(mutare));
            read(tdL.cl, mutare, sizeof(mutare));
            printf("Player-ul 2 a facut mutarea[%s]\n", mutare);
            MutaAlb = !MutaAlb;
        }
    }
    else
    {
        //spectator
        write(tdL.cl, mutare, sizeof(mutare));
    }
    //write(tdl.cl, mutare, sizeof(mutare));
}
static void comms(void * arg)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);
    while(1)
    {
        //printf ("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
        pthread_detach(pthread_self());
        raspuns_clienti((struct thData*)arg);
    }
    //trb sa fac ceva aici ca sa verifice cand a iesit un player sa anuleze conexiunea
    close ((intptr_t)arg);
    return;
}
int main ()
{
    struct sockaddr_in server;	// structura folosita de server
    struct sockaddr_in from;
    int socket_descriptor, i=0;
    pthread_t th[100];
    if ((socket_descriptor = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("Eroare la crearea socket-ului!\n");
        return errno;
    }
    int on=1;
    setsockopt(socket_descriptor,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    server.sin_port = htons (PORT);

    if (bind (socket_descriptor, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
        perror ("[server]Eroare la bind().\n");
        return errno;
    }

    if (listen (socket_descriptor, 2) == -1)
    {
        perror ("Eroare la ascultare!\n");
        return errno;
    }
    while (1)
    {
        int client;
        thData * td; //parametru functia executata de thread
        int length = sizeof (from);

        printf ("...se asteapta o conexiune la portul %d...\n",PORT);
        fflush (stdout);

        if ( (client = accept (socket_descriptor, (struct sockaddr *) &from, &length)) < 0)
        {
            perror ("Eroare la conectarea clientului\n");
        }
        else
        {
            //cand ajungem aici inseamna ca s-a intamplat conexiunea
            td=(struct thData*)malloc(sizeof(struct thData));
            td->idThread=i++;
            td->cl=client;
            printf("Player nr %d a intrat!", i);
            if(i == 1)
            {
                printf("(Alb)\n");
            }
            else if(i == 2)
            {
                printf("(Negru)\n");
            }
            else
            {
                printf("(Spectator)\n");
            }
            if (write (td->cl, &i, sizeof(i)) <= 0) // trimitem nr jucatorului inapoi la clienti
            {
                perror("Eroare la stabilirea numarului jucatorului!\n");
            }
            pthread_create(&th[i], NULL, &comms, td);
        }
    }
}

