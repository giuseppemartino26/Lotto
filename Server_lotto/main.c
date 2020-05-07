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


struct users
{
    char username[N];
    char password[N];
    char session_id[10];
};

void gen_random(char *s, const int len) {
    int i;
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    for ( i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

int main(int argc, char* argv[]) {
  //  printf("Ci sei?\n");
    int sd, new_sd, ret, len_msg_signup;
    int attempt = 0;
    socklen_t len;
    struct sockaddr_in my_addr;
    struct sockaddr_in cl_addr;
    pid_t pid;
    uint16_t lmsg, lmsg_signup;
    char buffer[BUFFER_SIZE];
    char *us;
    char *ustmp;
    char *us_log;
    char *pwd_log;
    char *pwd;
    const char s[2] = " ";
    struct users users_list;
    int i;
    int cont = 0;
    char msg_signup[N];
    FILE* f1; //file contenente tutti gli username con le relative password
    FILE* f2;
    FILE* f3;
    char* temp;
    char* temp_word;
    int len_word;
    int flag = 0;
    const size_t line_size = 300;
    char * lline = NULL;
    size_t length = 0;
    ssize_t read;
    char try[len];

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

                // Rinconverto in formato host
                len = ntohs(lmsg);

                // ricevo il messaggio effettivo di lunghezza len
                ret = recv(new_sd, (void *) buffer, len,0);
                printf("Buffer ricevuto");

                if (ret < 0) {
                    perror("Errore in fase di ricezione: \n");
                    continue;
                }

                /* !signup */
                if (strncmp(buffer, "!signup", 7) == 0) {
                    printf("Sono dentro\n");
                    printf("Il buffer ricevuto è %s",buffer);

                    us= strtok(buffer,s);
                    us= strtok(NULL, s);
                    pwd = strtok(NULL, s);

                    printf("us è %s\n", us);


                    f1 = fopen("/home/giuseppe/Scrivania/utenti.txt","a+");



                    flag = 0;
                    while ((read = getline(&lline, &length, f1)) != -1)
                    {

                    //  if (strncmp(strtok(lline,s),us,strlen(us)) == 0 )
                        if(strcmp(strtok(lline,s),us) == 0 )
                      {
                          flag = 1;
                          printf("Entrato nel blocco, setto flag a %d\n",flag);
                          fflush(stdout);


                          strcpy(msg_signup, "Username non disponibile, riprovare\n");
                          len_msg_signup = strlen(msg_signup) + 1;
                          lmsg_signup = htons(len_msg_signup);
                          ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                          ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);
                          //break;
                      }
                    }

                    //printf("%d\n",flag);

                    if (flag == 0)
                    {
                        printf("Apposto\n");
                        fflush(stdout);


                        //fprintf(f1,"\n");
                        fprintf(f1,"%s ", us);
                        fprintf(f1,"%s", pwd);

                        strcpy(msg_signup, "Registazione avvenuta con successo\n");
                        len_msg_signup = strlen(msg_signup) + 1;
                        lmsg_signup = htons(len_msg_signup);
                        ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                        ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);

                        f2 = fopen("/home/giuseppe/Scrivania/utenti2.txt","a");
                        fprintf(f2,"$%s\n",us);
                        fclose(f2);
                    }


                    fclose(f1);
                    if (lline) {
                        free(lline);
                    }
                }

                /* login */
                if (strncmp(buffer, "!login",6 ) == 0)
                {
                    us= strtok(buffer,s);
                    us= strtok(NULL, s);
                    pwd = strtok(NULL, s);

                    f1 = fopen("/home/giuseppe/Scrivania/utenti.txt","r");

                    flag = 0;
                    while ((read = getline(&lline, &length, f1)) != -1)
                    {
                        us_log = strtok(lline,s);
                        pwd_log = strtok(NULL,s);

                        if (strcmp(us_log,us) == 0 && strcmp(pwd_log,pwd)==0)
                        {
                            flag = 1;
                            printf("Entrato nel blocco, setto flag a %d\n",flag);
                            fflush(stdout);

                          /*  for (i = 0; i < 10; i++)
                            {
                                users_list.session_id[i] = rand();
                            }
                                */
                          gen_random(users_list.session_id, 10);

                            strcpy(msg_signup, "Accesso effettuato, id:");
                            strcat(msg_signup,users_list.session_id);
                            strcat(msg_signup,"\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);





                           // break;
                        }
                    }

                    if (flag == 0)
                    {

                        strcpy(msg_signup, "ERRORE: Accesso negato. (MASSIMO 3 TENTATIVI)\n");
                        len_msg_signup = strlen(msg_signup) + 1;
                        lmsg_signup = htons(len_msg_signup);
                        ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                        ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);

                        attempt ++;

                        if (attempt == 3)
                        {
                            attempt = 0;

                            strcpy(msg_signup, "EFFETTUATO IL MASSIMO DEI TENTATIVI, può riprovare l'accesso tra 30 minuti\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);

                            f3 = fopen("/home/giuseppe/Scrivania/tentativi.txt","a+");
                            inet_ntop(AF_INET, &cl_addr.sin_addr,try,len);
                            fprintf(f3,"%s",try);
                            fclose(f3);






                            close(new_sd);
                            exit(-1);

                        }
                    }


                    fclose(f1);
                    if (lline) {
                        free(lline);
                    }
                }








                }
            } else {

            printf("Ciao sono il padre");
            fflush(stdout);
            close(new_sd);
        }


        }



    }






