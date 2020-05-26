#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFFER_SIZE 1024
#define N 100


void help_cmd(char string[])
{
    char *token;
    const char s[2] = " ";
    token = strtok(string, s);
    token = strtok(NULL, s); // in token ho il comando che deve essere descritto



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

/*struct users
{
    char username[N];
    char password[N];
    char session_id[10];
};
*/

int main(int argc, char* argv[]) {
    int ret, sd;
    u_long len;
    uint16_t lmsg;
    uint32_t mssg;
    struct sockaddr_in srv_addr; //indirizzo server
    char str_cmd[BUFFER_SIZE];   // stringa per salvare il comando inserito da tastiera dall'utente
    char buffer[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];
    char buffer3[BUFFER_SIZE];
    char buffer4[BUFFER_SIZE];
    char buffer5[BUFFER_SIZE];
    const char st[2] = ":";
    char* tok;
   // struct users users_list;
    int attempt = 0;
    char *id_session = malloc(sizeof(char)*10);
    id_session = "1111111111";


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

    ret = recv(sd,(void*) &lmsg, sizeof(uint16_t),0 );
    len = ntohs(lmsg); // Rinconverto in formato host
    //Ricevo il messaggio di risposta
    ret = recv(sd, (void*)buffer, len, 0);

    if (strncmp(buffer,"Non",3)==0)
    {
        perror(buffer);
        exit(-1);
        close(sd);
    } else{
        printf("Connessione effettuata");
    }



    printf("Benvenuto nel gioco del lotto, digitare un comando o digitare '!help' per conoscere i comandi disponibili\n");

    while (1)
    {
        printf("Comincio con ");
        printf("%s\n",id_session);

        fgets(str_cmd, BUFFER_SIZE, stdin); //Attendo input da tastiera

        /* help senza specificare il comando */
        if (strncmp(str_cmd, "!help ", 6) == 0)
        {
            help_cmd(str_cmd);
        }

        /* !help  */
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

        /* !signup */
        if (strncmp(str_cmd, "!signup",7)==0)
        {
            printf("okk\n");
            // Invio al server la quantita di dati
            len = strlen(str_cmd) + 1; // Voglio inviare anche il carattere di fine stringa
            lmsg = htons(len); // Per passarlo al server lo converto in formato big endian
            ret = send(sd, (void*) &lmsg, sizeof(uint16_t), 0); // mando la dimensione del messaggio

            ret = send(sd, (void*) str_cmd, len, 0); // invio il messaggio
            printf("Messaggio inviato\n");

            //Attendo dimensione messaggio di risposta
            ret = recv(sd,(void*) &lmsg, sizeof(uint16_t),0 );
            len = ntohs(lmsg); // Rinconverto in formato host



            //Ricevo il messaggio di risposta
            ret = recv(sd, (void*)buffer, len, 0);

            if(ret < 0){
                perror("Errore in fase di ricezione: \n");
                continue;
            }

            // Ricevo U dal server nel caso in cui ci sia già un username, altrimenti ricevo R
            if(strncmp(buffer, "U",1) ==0)
            {
                printf("Username non disponibile, riprovare\n");
                continue;
            }

            if (strncmp(buffer,"R",1)== 0)
            {
                printf("Registazione avvenuta con successo\n");
                continue;
            }

        }

        /*login*/
        if (strncmp(str_cmd, "!login",6)==0){
            // Invio al server la quantita di dati
            len = strlen(str_cmd) + 1; // Voglio inviare anche il carattere di fine stringa
            lmsg = htons(len); // Per passarlo al server lo converto in formato big endian
            ret = send(sd, (void*) &lmsg, sizeof(uint16_t), 0); // mando la dimensione del messaggio

            ret = send(sd, (void*) str_cmd, len, 0); // invio il messaggio
            printf("Messaggio inviato\n");

            //Attendo dimensione messaggio di risposta
            ret = recv(sd,(void*) &lmsg, sizeof(uint16_t),0 );
            len = ntohs(lmsg); // Rinconverto in formato host



            //Ricevo il messaggio di risposta
            ret = recv(sd, (void*)buffer, len, 0);



            if(ret < 0){
                perror("Errore in fase di ricezione: \n");
                continue;

            }

            /* printf("%s",buffer);
             fflush(stdout);
             continue;
             */




            if(strncmp(buffer, "A",1) ==0)
            {
                printf("%s\n", buffer);

                id_session = strtok(buffer, ":");
                id_session = strtok(NULL, ":");

                printf("%s\n",id_session);
             /*   tok = strtok(buffer,st);
                tok = strtok(NULL,st);
                strcpy(users_list.username,tok);
                printf("%s", users_list.username);
                */


                continue;
            }

            if(strncmp(buffer, "E",1) ==0)
            {
                attempt ++;
                printf("%s",buffer);

                if (attempt == 3)
                {
                    perror("Numero massimo di tentativi provati. Può effettuare altri 3 tentativi tra 30 minuti");
                    close(sd);
                    // break;
                    exit(-1);
                }

                continue;
            }



        }

        if (strncmp(str_cmd, "!invia_giocata",14)==0)
        {
            len = strlen(str_cmd) + 1;
            lmsg = htons(len);
            ret = send(sd, (void*) &lmsg, sizeof(uint16_t), 0);
            ret = send(sd, (void*) str_cmd, len, 0);

           // Mando l'ID
            len = strlen(id_session) + 1;
            lmsg = htons(len);
            ret = send(sd, (void*) &lmsg, sizeof(uint16_t), 0);
            ret = send(sd, (void*) id_session, len, 0);
            //Ricevo l'ID
            ret = recv(sd,(void*) &lmsg, sizeof(uint16_t),0 );
            len = ntohs(lmsg);
            ret = recv(sd, (void*)buffer, len, 0);
            //Se l'ID non è valido
            if (strncmp(buffer,"ERROR_ID",8)==0)
            {
                perror(buffer);
                continue;
            }
            //Ricevo il messaggio di giocata effettuata
            ret = recv(sd,(void*) &lmsg, sizeof(uint16_t),0 );
            len = ntohs(lmsg);
            ret = recv(sd, (void*)buffer, len, 0);
            printf("%s\n",buffer);

        }

        if (strncmp(str_cmd, "!vedi_giocate",13)==0)
        {
            len = strlen(str_cmd) + 1;
            lmsg = htons(len);
            ret = send(sd, (void*) &lmsg, sizeof(uint16_t), 0);
            ret = send(sd, (void*) str_cmd, len, 0);

            // Mando l'ID
            len = strlen(id_session) + 1;
            lmsg = htons(len);
            ret = send(sd, (void*) &lmsg, sizeof(uint16_t), 0);
            ret = send(sd, (void*) id_session, len, 0);
            //Ricevo l'ID
            ret = recv(sd,(void*) &lmsg, sizeof(uint16_t),0 );
            len = ntohs(lmsg);
            ret = recv(sd, (void*)buffer, len, 0);
            //Se l'ID non è valido
            if (strncmp(buffer,"ERROR_ID",8)==0)
            {
                perror(buffer);
                continue;
            }
            //Ricevo le giocate e stampo a video
            ret = recv(sd,(void*) &lmsg, sizeof(uint16_t),0 );
            len = ntohs(lmsg);
            ret = recv(sd, (void*)buffer2, len, 0);
            printf("%s",buffer2);
            printf("ciao");






        }

        if (strncmp(str_cmd, "!vedi_estrazione",16)==0)
        {
            len = strlen(str_cmd) + 1;
            lmsg = htons(len);
            ret = send(sd, (void*) &lmsg, sizeof(uint16_t), 0);
            ret = send(sd, (void*) str_cmd, len, 0);

            // Mando l'ID
            len = strlen(id_session) + 1;
            lmsg = htons(len);
            ret = send(sd, (void*) &lmsg, sizeof(uint16_t), 0);
            ret = send(sd, (void*) id_session, len, 0);
            //Ricevo l'ID
            ret = recv(sd,(void*) &lmsg, sizeof(uint16_t),0 );
            len = ntohs(lmsg);
            ret = recv(sd, (void*)buffer, len, 0);
            //Se l'ID non è valido
            if (strncmp(buffer,"ERROR_ID",8)==0)
            {
                perror(buffer);
                continue;
            }

            //Ricevo le giocate e stampo a video
            ret = recv(sd,(void*) &lmsg, sizeof(uint16_t),0 );
            len = ntohs(lmsg);
            ret = recv(sd, (void*)buffer3, len, 0);
            printf("%s",buffer3);

        }



        if (strncmp(str_cmd, "!vedi_estrazione",16)==0) {
            len = strlen(str_cmd) + 1;
            lmsg = htons(len);
            ret = send(sd, (void *) &lmsg, sizeof(uint16_t), 0);
            ret = send(sd, (void *) str_cmd, len, 0);

            // Mando l'ID
            len = strlen(id_session) + 1;
            lmsg = htons(len);
            ret = send(sd, (void *) &lmsg, sizeof(uint16_t), 0);
            ret = send(sd, (void *) id_session, len, 0);
            //Ricevo l'ID
            ret = recv(sd, (void *) &lmsg, sizeof(uint16_t), 0);
            len = ntohs(lmsg);
            ret = recv(sd, (void *) buffer, len, 0);
            //Se l'ID non è valido
            if (strncmp(buffer, "ERROR_ID", 8) == 0) {
                perror(buffer);
                continue;
            }

            //Ricevo le estrazioni e stampo a video
            ret = recv(sd, (void *) &mssg, sizeof(uint16_t), 0);
            len = ntohs(mssg);
            ret = recv(sd, (void *) buffer4, len, 0);
            printf("%s", buffer4);


        }

        if (strncmp(str_cmd, "!vedi_vincite",13)==0) {
            len = strlen(str_cmd) + 1;
            lmsg = htons(len);
            ret = send(sd, (void *) &lmsg, sizeof(uint16_t), 0);
            ret = send(sd, (void *) str_cmd, len, 0);

            // Mando l'ID
            len = strlen(id_session) + 1;
            lmsg = htons(len);
            ret = send(sd, (void *) &lmsg, sizeof(uint16_t), 0);
            ret = send(sd, (void *) id_session, len, 0);
            //Ricevo l'ID
            ret = recv(sd, (void *) &lmsg, sizeof(uint16_t), 0);
            len = ntohs(lmsg);
            ret = recv(sd, (void *) buffer, len, 0);
            //Se l'ID non è valido
            if (strncmp(buffer, "ERROR_ID", 8) == 0) {
                perror(buffer);
                continue;
            }

            //Ricevo le estrazioni e stampo a video
            ret = recv(sd, (void *) &mssg, sizeof(uint16_t), 0);
            len = ntohs(mssg);
            ret = recv(sd, (void *) buffer5, len, 0);
            printf("%s", buffer5);


        }











    }

    return 0;
}
