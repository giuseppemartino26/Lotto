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

    if (strncmp(token, "!signup", 7) == 0)
    {
        printf("!signup <username> <password> --> crea un nuovo utente\n");
    }
    if (strncmp(token, "!help", 5) == 0)
    {
        printf("!help <comando> --> mostra i dettagli di un comando\n");
    }
    if (strncmp(token, "!login", 6) == 0)
    {
        printf("!login <username> <password> --> autentica un utente\n");
    }
    if (strncmp(token, "!invia_giocata", 14) == 0)
    {
        printf("!invia_giocata g --> gioca una schedina\nFormato della giocata: -r <Ruote> -n <Numeri> -i <Importi>\n");
    }
    if (strncmp(token, "!vedi_giocate", 13) == 0)
    {
        printf("!vedi_giocate <tipo> --> visualizza le giocate precedenti dove tipo = {0,1}\ne permette di visualizzare le giocate passate ‘0’ oppure le giocate attive ‘1’ (ancora non estratte)\n");
    }
    if (strncmp(token, "!vedi_estrazione", 15) == 0)
    {
        printf("!vedi_estrazione <n> <ruota> --> mostra i numeri delle ultime n estrazioni sulla ruota specificata\n");
    }
    if (strncmp(token, "!esci", 5) == 0)
    {
        printf("!esci --> termina il client\n");
    }
}

int main(int argc, char *argv[])
{

    int ret, sd;
    u_long len;
    uint16_t lmsg;
    uint32_t mssg;
    struct sockaddr_in srv_addr; //indirizzo server
    char str_cmd[BUFFER_SIZE];   // stringa per salvare il comando inserito da tastiera dall'utente

    /* buffer che uso per ricevere i messaggi dal server */
    char buffer[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];
    char buffer3[BUFFER_SIZE];
    char buffer4[BUFFER_SIZE];
    char buffer5[BUFFER_SIZE];
    char buffer6[BUFFER_SIZE];
    char buffer7[BUFFER_SIZE];

    int attempt = 0; //tentativi di login falliti

    const char st[2] = ":";
    char *tok;

    char *id_session = malloc(sizeof(char) * 10);
    id_session = "1111111111";

    /* Creazione socket */
    sd = socket(AF_INET, SOCK_STREAM, 0);

    /* Creazione indirizzo del server */
    memset(&srv_addr, 0, sizeof(srv_addr)); // Pulizia
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &srv_addr.sin_addr);

    ret = connect(sd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));

    if (ret < 0)
    {
        perror("Errore in fase di connessione: \n");
        exit(-1);
    }

    ret = recv(sd, (void *)&lmsg, sizeof(uint16_t), 0);
    if (ret == -1)
    {
        perror("Errore:\n");
        exit(-1);
    }
    // Rinconverto in formato host
    len = ntohs(lmsg);
    //Ricevo il messaggio di risposta
    ret = recv(sd, (void *)buffer, len, 0);
    if (ret == -1)
    {
        perror("Errore:\n");
        exit(-1);
    }

    if (strncmp(buffer, "Non", 3) == 0) //Ho ricevuto messaggio dal server che dice che sono bloccato per il massimo numero di tentativi effettuati
    {
        printf("%s\n", buffer);
        close(sd);
        exit(-1);
    }
    else
    {
        printf("Connessione effettuata\n");
    }

    printf("Benvenuto nel gioco del lotto, digitare un comando o digitare '!help' per conoscere i comandi disponibili\n");

    while (1)
    {

        fgets(str_cmd, BUFFER_SIZE, stdin); //Attendo input da tastiera

        /* help  */
        if (strncmp(str_cmd, "!help ", 6) == 0)
        {
            help_cmd(str_cmd);
        }

        /* !help senza comando specificato */
        else if (strncmp(str_cmd, "!help", 5) == 0 && strlen(str_cmd) == 6)
        {
            printf("!help <comando> --> mostra i dettagli di un comando\n\n");
            printf("Sono disponibili i seguenti comandi:\n!signup <username> <password> --> crea un nuovo utente\n\n");
            printf("!login <username> <password> --> autentica un utente\n\n");
            printf("!invia_giocata g --> Gioca una schedina\nFormato della giocata: -r <Ruote> -n <Numeri> -i <Importi>\n\n");
            printf("!vedi_giocate <tipo> --> visualizza le giocate precedenti dove tipo = {0,1}\ne permette di visualizzare le giocate passate ‘0’ oppure le giocate attive ‘1’ (ancora non estratte)\n\n");
            printf("!vedi_estrazione <n> <ruota> --> mostra i numeri delle ultime n estrazioni sulla ruota specificata\n\n");
            printf("!esci --> termina il client\n");
        }

        /* !signup */
        else if (strncmp(str_cmd, "!signup", 7) == 0)
        {

            // Invio al server la quantita di dati
            len = strlen(str_cmd) + 1;                          // Voglio inviare anche il carattere di fine stringa
            lmsg = htons(len);                                  // Per passarlo al server lo converto in formato big endian
            ret = send(sd, (void *)&lmsg, sizeof(uint16_t), 0); // mando la dimensione del messaggio
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }

            ret = send(sd, (void *)str_cmd, len, 0); // invio il messaggio
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }

            //Attendo dimensione messaggio di risposta
            ret = recv(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            len = ntohs(lmsg); // Rinconverto in formato host

            //Ricevo il messaggio di risposta
            ret = recv(sd, (void *)buffer, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }

            if (ret < 0)
            {
                perror("Errore in fase di ricezione: \n");
                continue;
            }

            // Ricevo U dal server nel caso in cui ci sia già un username, altrimenti ricevo R
            if (strncmp(buffer, "U", 1) == 0)
            {
                printf("Username non disponibile, riprovare\n");
                continue;
            }

            if (strncmp(buffer, "R", 1) == 0)
            {
                printf("Registazione avvenuta con successo\n");
                continue;
            }
        }

        /*login*/
        else if (strncmp(str_cmd, "!login", 6) == 0)
        {
            // Invio al server la quantita di dati
            len = strlen(str_cmd) + 1;                          // Voglio inviare anche il carattere di fine stringa
            lmsg = htons(len);                                  // Per passarlo al server lo converto in formato big endian
            ret = send(sd, (void *)&lmsg, sizeof(uint16_t), 0); // mando la dimensione del messaggio
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            ret = send(sd, (void *)str_cmd, len, 0); // invio il messaggio
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            printf("Messaggio inviato\n");

            //Attendo dimensione messaggio di risposta
            ret = recv(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            len = ntohs(lmsg); // Rinconverto in formato host

            //Ricevo il messaggio di risposta
            ret = recv(sd, (void *)buffer, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }

            if (ret < 0)
            {
                perror("Errore in fase di ricezione: \n");
                continue;
            }

            //Se ricevo "A" dal server significa accesso effettuato e salvo l'id_session
            if (strncmp(buffer, "A", 1) == 0)
            {
                printf("%s\n", buffer);

                id_session = strtok(buffer, ":");
                id_session = strtok(NULL, ":");

                continue;
            }

            //Se ricevo "E" significa che ho sbagliato username o password
            if (strncmp(buffer, "E", 1) == 0)
            {
                attempt++; //conto i tentativi di accesso falliti
                printf("%s", buffer);

                if (attempt == 3)
                {
                    printf("Numero massimo di tentativi provati. Può effettuare altri 3 tentativi tra 30 minuti\n");
                    //chiudo il socket
                    close(sd);
                    exit(-1);
                }
                continue;
            }
        }

        /* invia giocata */
        else if (strncmp(str_cmd, "!invia_giocata", 14) == 0)
        {
            len = strlen(str_cmd) + 1;
            lmsg = htons(len);
            ret = send(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            ret = send(sd, (void *)str_cmd, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }

            // Mando l'ID
            len = strlen(id_session) + 1;
            lmsg = htons(len);
            ret = send(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            ret = send(sd, (void *)id_session, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            //Ricevo l'ID
            ret = recv(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            len = ntohs(lmsg);
            ret = recv(sd, (void *)buffer6, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            //Se l'ID non è valido
            if (strncmp(buffer6, "ERROR_ID", 8) == 0)
            {
                printf("%s", buffer6);
                continue;
            }
            //Ricevo il messaggio di giocata effettuata
            ret = recv(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            len = ntohs(lmsg);
            ret = recv(sd, (void *)buffer6, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            printf("%s\n", buffer6);
        }

        /* vedi giocate */
        else if (strncmp(str_cmd, "!vedi_giocate", 13) == 0)
        {
            len = strlen(str_cmd) + 1;
            lmsg = htons(len);
            ret = send(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            ret = send(sd, (void *)str_cmd, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }

            // Mando l'ID
            len = strlen(id_session) + 1;
            lmsg = htons(len);
            ret = send(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            ret = send(sd, (void *)id_session, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            //Ricevo l'ID
            ret = recv(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            len = ntohs(lmsg);
            ret = recv(sd, (void *)buffer, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            //Se l'ID non è valido
            if (strncmp(buffer, "ERROR_ID", 8) == 0)
            {
                printf("%s", buffer);
                continue;
            }
            //Ricevo le giocate e stampo a video
            ret = recv(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            len = ntohs(lmsg);
            ret = recv(sd, (void *)buffer2, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            printf("%s", buffer2);
        }

        else if (strncmp(str_cmd, "!vedi_estrazione", 16) == 0)
        {
            len = strlen(str_cmd) + 1;
            lmsg = htons(len);
            ret = send(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            ret = send(sd, (void *)str_cmd, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }

            // Mando l'ID
            len = strlen(id_session) + 1;
            lmsg = htons(len);
            ret = send(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            ret = send(sd, (void *)id_session, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            //Ricevo l'ID
            ret = recv(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            len = ntohs(lmsg);
            ret = recv(sd, (void *)buffer, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            //Se l'ID non è valido
            if (strncmp(buffer, "ERROR_ID", 8) == 0)
            {
                printf("%s", buffer);
                continue;
            }

            //Ricevo le estrazioni e stampo a video
            ret = recv(sd, (void *)&mssg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            len = ntohs(mssg);
            ret = recv(sd, (void *)buffer4, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            printf("%s", buffer4);
        }

        else if (strncmp(str_cmd, "!vedi_vincite", 13) == 0)
        {
            len = strlen(str_cmd) + 1;
            lmsg = htons(len);
            ret = send(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            ret = send(sd, (void *)str_cmd, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }

            // Mando l'ID
            len = strlen(id_session) + 1;
            lmsg = htons(len);
            ret = send(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            ret = send(sd, (void *)id_session, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            //Ricevo l'ID
            ret = recv(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            len = ntohs(lmsg);
            ret = recv(sd, (void *)buffer, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            //Se l'ID non è valido
            if (strncmp(buffer, "ERROR_ID", 8) == 0)
            {
                printf("%s\n", buffer);
                continue;
            }

            //Ricevo le estrazioni e stampo a video
            ret = recv(sd, (void *)&mssg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            len = ntohs(mssg);
            ret = recv(sd, (void *)buffer5, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            printf("%s", buffer5);
        }

        else if (strncmp(str_cmd, "!esci", 5) == 0)
        {
            len = strlen(str_cmd) + 1;
            lmsg = htons(len);
            ret = send(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            ret = send(sd, (void *)str_cmd, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }

            // Mando l'ID
            len = strlen(id_session) + 1;
            lmsg = htons(len);
            ret = send(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            ret = send(sd, (void *)id_session, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            //Ricevo l'ID
            ret = recv(sd, (void *)&lmsg, sizeof(uint16_t), 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            len = ntohs(lmsg);
            ret = recv(sd, (void *)buffer, len, 0);
            if (ret == -1)
            {
                perror("Errore:\n");
                exit(-1);
            }
            //Se l'ID non è valido
            if (strncmp(buffer, "ERROR_ID", 8) == 0)
            {
                printf("%s\n", buffer);
                continue;
            }

            //Ricevo e stampo il messaggio ricevuto dal server
            ret = recv(sd, (void *)&mssg, sizeof(uint16_t), 0);
            len = ntohs(mssg);
            ret = recv(sd, (void *)buffer7, len, 0);
            printf("%s", buffer7);
            if (strcmp(buffer7, "Disconnessione avvenuta"))
            {
                id_session = "1111111111";
                close(sd);
                exit(0);
            }
        }

        else
        {
            printf("Formato del comando non valido. Riprovare\n");
        }
    }

    return 0;
}
