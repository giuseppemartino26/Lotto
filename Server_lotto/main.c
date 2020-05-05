#include <stdio.h>
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

struct users
{
    char username[N];
    char password[N];
    FILE* fp;
};

/*void signup_fun(char string[], char* tab[])
{

} */

int main(int argc, char* argv[]) {
    printf("Ci sei?");
    int sd, new_sd, ret, len,len_msg_signup;
    struct sockaddr_in my_addr;
    struct sockaddr_in cl_addr;
    pid_t pid;
    uint16_t lmsg, lmsg_signup;
    char buffer[BUFFER_SIZE];
    char *us;
    char *pwd;
    const char s[2] = " ";
    struct users users_list[N];
    int i;
    int cont = 0;
    int len_list = 0; //lunghezza lista di utenti registrati
    char msg_signup[50];

    /* Creazione socket */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Socket creato");
    fflush(stdout);

    /* Creazione indirizzo di bind */
    memset(&my_addr, 0, sizeof(my_addr)); // Pulizia
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(4242);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    ret = bind(sd, (struct sockaddr *) &my_addr, sizeof(my_addr));

    printf("Ascolto");
    ret = listen(sd, 10);


    if (ret < 0) {
        perror("Errore in fase di bind: \n");
        exit(-1);
    }

    while (1) {

        len = sizeof(cl_addr);

        // Accetto nuove connessioni
        new_sd = accept(sd, (struct sockaddr *) &cl_addr, &len);
        printf("Connessione accettata");
        fflush(stdout);

        pid = fork();
        printf("%d",pid);
        fflush(stdout);

        // Sono nel processo figlio
        if (pid == 0) {
            close(sd);
            printf("Ciao sono il figlio");
            fflush(stdout);
            while (1) {
                printf("pronto per ricevere\n");
                fflush(stdout);
                // Attendo dimensione del mesaggio
                ret = recv(new_sd, (void *) &lmsg, sizeof(uint16_t), 0);
                printf("ok");
                fflush(stdout);

                len = ntohs(lmsg); // Rinconverto in formato host

                ret = recv(new_sd, (void *) buffer, len,
                           0); // ora che so la lunghezza (len), ricevo il messaggio effettivo di lunghezza len

                if (ret < 0) {
                    perror("Errore in fase di ricezione: \n");
                    continue;
                }


                if (strncmp(buffer, "!signup", 7) == 0) {
                    us = strtok(buffer,s);
                    us = strtok(NULL, s);
                    pwd = strtok(NULL, s);

                    cont=0;
                    printf("Cont prima del for: %d\n",cont);

                    for (i = 0; i < len_list; i++) {
                        if (strncmp(us, users_list[i].username, strlen(us)) == 0) {
                            cont++;
                        }
                    }

                    printf("Cont dopo del for: %d\n",cont);

                    if (cont > 0) {
                        strcpy(msg_signup, "Username non disponibile, riprovare\n");
                        len_msg_signup = strlen(msg_signup) + 1;
                        lmsg_signup = htons(len_msg_signup);
                        ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                        ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);
                    } else {
                        strcpy(users_list[i].username, us);
                        strcpy(users_list[i].password, pwd);
                        len_list++;
                        strcpy(msg_signup, "Registazione avvenuta con successo\n");
                        len_msg_signup = strlen(msg_signup) + 1;
                        lmsg_signup = htons(len_msg_signup);
                        ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                        ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);
                    }


                }

            }


        }
        printf("Ciao sono il padre");
        fflush(stdout);
        close(new_sd);


    }


}

