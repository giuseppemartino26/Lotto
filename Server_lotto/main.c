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
#define _GNU_SOURCE


/*int  accesso (const char* stringa)
{
        FILE* f1;
        char* temp;
        char* temp_word;
        int len_word;
        const char sp[2] = " ";
        f1 = fopen("/home/giuseppe/Scrivania/utenti.txt","a+");

        while (!feof(f1))
        {
            fgets(temp,N,f1);
            temp_word = strtok(temp,sp);
            len_word = strlen(temp_word);

            if (strncmp(temp_word,stringa,len_word)== 0 )
            {
                fclose(f1);
                return 1;
            }
        }
        fclose(f1);
        return 0;
}
*/
struct users
{
    char username[N];
    char password[N];
    int session_id;
    FILE* fp;
};
//struct users users_list[N];

/*void signup_fun(char string[], char* tab[])
{

} */

int main(int argc, char* argv[]) {
    printf("Ci sei?\n");
    int sd, new_sd, ret, len, len_msg_signup;
    struct sockaddr_in my_addr;
    struct sockaddr_in cl_addr;
    pid_t pid;
    uint16_t lmsg, lmsg_signup;
    char buffer[BUFFER_SIZE];
    char *us;
    char *ustmp;
    char *pwd;
    const char s[2] = " ";
    struct users users_list[N];
    int i;
    int ciao = 0;
    int cont = 0;
    int len_list = 0; //lunghezza lista di utenti registrati
    char msg_signup[50];
    FILE* f1;
    char* temp;
    char* temp_word;
    int len_word;
    int flag = 0;
    const size_t line_size = 300;
    char * lline = NULL;
    size_t length = 0;
    ssize_t read;

    /* Creazione socket */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Socket creato\n");
    fflush(stdout);

    /* Creazione indirizzo di bind */
    memset(&my_addr, 0, sizeof(my_addr)); // Pulizia
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(4242);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    ret = bind(sd, (struct sockaddr *) &my_addr, sizeof(my_addr));

    printf("Ascolto\n");
    ret = listen(sd, 10);


    if (ret < 0) {
        perror("Errore in fase di bind: \n");
        exit(-1);
    }

    while (1) {

        len = sizeof(cl_addr);

        // Accetto nuove connessioni
        new_sd = accept(sd, (struct sockaddr *) &cl_addr, &len);
        printf("Connessione accettata\n");
        fflush(stdout);

        pid = fork();

        // Sono nel processo figlio
        if (pid == 0) {
            close(sd);
            printf("Ciao sono il figlio\n");
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
                printf("Buffer ricevuto");

                if (ret < 0) {
                    perror("Errore in fase di ricezione: \n");
                    continue;
                }


                if (strncmp(buffer, "!signup", 7) == 0) {
                    printf("Sono dentro\n");
                    printf("Il buffer ricevuto è %s",buffer);

                    us= strtok(buffer,s);
                    us= strtok(NULL, s);

                    printf("us è %s\n", us);


                    f1 = fopen("/home/giuseppe/Scrivania/utenti.txt","a+");

                    //while (!feof(f1))
                  //  for (int j = 1; j != feof(f1) ; j++)
                   // for (int j = 0; j <= 10; j++)
                  //  while (fgets(line, line_size, f1) != 0)

                 //   temp_word = strtok(lline,s);
                  //  printf("%s\n", temp_word);
                    flag = 0;
                    while ((read = getline(&lline, &length, f1)) != -1)
                  {
                       // ustmp = strtok(lline,s);
                       // ustmp = strtok(NULL,s);
                      //  printf("ustmp è %s", ustmp);

                      if (strncmp(strtok(lline,s),us,strlen(us)) == 0 )
                      {
                          flag = 1;
                          printf("Entrato nel blocco, setto flag a %d\n",flag);
                          fflush(stdout);


                          strcpy(msg_signup, "Username non disponibile, riprovare\n");
                          len_msg_signup = strlen(msg_signup) + 1;
                          lmsg_signup = htons(len_msg_signup);
                          ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                          ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);




                          break;
                      }
                       /* fgets(temp,N,f1);
                        temp_word = strtok(line,s);
                      //  len_word = strlen(temp_word);

                        if (strcmp(temp_word,us) == 0 )
                        {
                            strcpy(msg_signup, "Username non disponibile, riprovare\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);
                           // fclose(f1);
                          //  continue;
                        } else{
                            strcpy(msg_signup, "Registazione avvenuta con successo\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);
                            printf("finito");
                            fflush(stdout);
                           // fprintf(f1,"%s",buffer);
                            fprintf(f1,"%s",us);
                         //   flag = 1;
                          //  fclose(f1);
                        }
                        */
                    /*  printf("%s\n", lline);
                      fflush(stdout);*/


                    }
                    printf("%d\n",flag);

                    if (flag == 0)
                    {
                        printf("Apposto\n");
                        fflush(stdout);


                        fprintf(f1,"%s ", us);

                        strcpy(msg_signup, "Registazione avvenuta con successo\n");
                        len_msg_signup = strlen(msg_signup) + 1;
                        lmsg_signup = htons(len_msg_signup);
                        ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                        ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);




                    }


                    fclose(f1);
                    if (lline) {
                        free(lline);
                    }




                   // if (accesso(us) == 1)
                  // ciao = accesso(us);
                    //    printf("%d", ciao);
                      /*
                    {
                        strcpy(msg_signup, "Username non disponibile, riprovare\n");
                        len_msg_signup = strlen(msg_signup) + 1;
                        lmsg_signup = htons(len_msg_signup);
                        ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                        ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);
                    } else{
                        strcpy(msg_signup, "Registazione avvenuta con successo\n");
                        len_msg_signup = strlen(msg_signup) + 1;
                        lmsg_signup = htons(len_msg_signup);
                        ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                        ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);

                    }
                 /*   pwd = strtok(NULL, s);

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
*/

                }

            }


        }
        printf("Ciao sono il padre");
        fflush(stdout);
        close(new_sd);


    }

}




