#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFFER_SIZE 1024


void help_cmd(char string[])
{
    char *token;
    const char s[2] = " ";
    token = strtok(string, s);
    token = strtok(NULL, s);



    if (strncmp(token,"!signup",7)==0)
    {
        printf("!signup <username> <password> --> crea un nuovo utente");
    }
    if (strncmp(token,"!help",5)==0)
    {
        printf("!help <comando> --> mostra i dettagli di un comando\n");
    }
    if (strncmp(token,"!login",6)==0)
    {
        printf("!login <username> <password> --> autentica un utente\n");
    }
    if (strncmp(token,"!invia_giocata",14)==0)
    {
        printf("!invia_giocata g --> invia una giocata g al server\n");
    }
    if (strncmp(token,"!vedi_giocate",13)==0)
    {
        printf("!vedi_giocate <tipo> --> visualizza le giocate precedenti dove tipo = {0,1}\\n\"\n"
               "               \" e permette di visualizzare le giocate passate ‘0’\\n\"\n"
               "               \"oppure le giocate attive ‘1’ (ancora non estratte)\\n\"");
    }
    if (strncmp(token,"!vedi_estrazione",15)==0)
    {
        printf("!vedi_estrazione <n> <ruota> --> mostra i numeri delle ultime n estrazioni\nsulla ruota specificata\n");
    }
    if (strncmp(token,"!esci",5)==0)
    {
        printf("!esci --> termina il client\n");
    }


}


int main(int argc, char* argv[]) {
    int ret, sd, len;
    uint16_t lmsg;
    struct sockaddr_in srv_addr; //indirizzo server
    char str_cmd[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];


    /* Creazione socket */
    sd = socket(AF_INET, SOCK_STREAM, 0);

    /* Creazione indirizzo del server */
    memset(&srv_addr, 0, sizeof(srv_addr)); // Pulizia

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(4242);
    inet_pton(AF_INET, "127.0.0.1", &srv_addr.sin_addr);

    ret = connect(sd, (struct sockaddr*)&srv_addr, sizeof(srv_addr));

    if(ret < 0){
        perror("Errore in fase di connessione: \n");
        exit(-1);
    }

    printf("Benvenuto nel gioco del lotto, digitare un comando o digitare '!help' per conoscere i comandi disponibili\n");

    while (1)
    {
        fgets(str_cmd, BUFFER_SIZE, stdin); //Attendo input da tastiera

        if (strncmp(str_cmd, "!help ", 6) == 0)
        {
            help_cmd(str_cmd);
        }

        if (strncmp(str_cmd, "!help", 5) == 0 && strlen(str_cmd) == 6)
        {
            printf("Sono disponibili i seguenti comandi:\n"
                   "1) !help <comando> --> mostra i dettagli di un comando\n"
                   "2) !signup <username> <password> --> crea un nuovo utente \u2028\n"
                   "3) !login <username> <password> --> autentica un utente\n"
                   "4) !invia_giocata g --> invia una giocata g al server\n"
                   "5) !vedi_giocate tipo --> visualizza le giocate precedenti dove tipo = {0,1}\n"
                   " e permette di visualizzare le giocate passate ‘0’\n"
                   "oppure le giocate attive ‘1’ (ancora non estratte)\n"
                   "6) !vedi_estrazione <n> <ruota> --> mostra i numeri delle ultime n estrazioni\n"
                   " sulla ruota specificata\n"
                   "7) !esci --> termina il client\n");
        }

        if (strncmp(str_cmd, "!signup",7)==0)
        {
            printf("okk");
            // Invio al server la quantita di dati
            len = strlen(str_cmd) + 1; // Voglio inviare anche il carattere di fine stringa
            lmsg = htons(len); // Per passarlo al server lo converto in formato big endian
            ret = send(sd, (void*) &lmsg, sizeof(uint16_t), 0); // mando la dimensione del messaggio

            ret = send(sd, (void*) str_cmd, len, 0); // invio il messaggio

            //Attendo dimensione messaggio di risposta
            ret = recv(sd,(void*) &lmsg, sizeof(uint16_t),0 );
            len = ntohs(lmsg); // Rinconverto in formato host



            //Ricevo il messaggio di risposta
            ret = recv(sd, (void*)buffer, len, 0);

            if(ret < 0){
                perror("Errore in fase di ricezione: \n");
                continue;
            }

            if(strncmp(buffer, "U",1) ==0)
            {
                printf("Username non disponibile, riprovare");
                continue;
            }

            if (strncmp(buffer,"R",1)== 0)
            {
                printf("Registazione avvenuta con successo");
                continue;
            }

        }







    }

    return 0;
}
