#define _GNU_SOURCE
#define _XOPEN_SOURCE  #include <stdio.h>
#define __USE_XOPEN
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define BUFFER_SIZE 1024
#define N 100
//enum puntate {estratto,ambo,terno,quaterna,cinquina};

struct Schedina
{
    struct tm timestamp_giocata;
    char ruote_giocate[150];
    long numeri_giocati[N];
    float importo_giocato[10];
    char* puntate[5];
};

struct users
{
    char username[N];
    //char password[N];
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

int Differenza(struct tm t1)
{
    int diff;
    time_t tt;
    struct tm* tmm;
    char buf[N];
    struct tm tmmn;

    tt = time(NULL);
    tmm = localtime(&tt);
    strftime(buf,sizeof(buf), "%d/%m/%Y-%H:%M", tmm);
    strptime(buf,"%d/%m/%Y-%H:%M",&tmmn);
    //  printf("%d\n",tmmn.tm_hour);
    //  printf("%d\n",t1.tm_hour);


    diff = (tmmn.tm_min - t1.tm_min) + (tmmn.tm_hour - t1.tm_hour)*60 + (tmmn.tm_mday - t1.tm_mday)*1440;
    return diff;
}


int main(int argc, char* argv[]) {
    //  printf("Ci sei?\n");
    int sd, new_sd, ret, len_msg_signup;
    int attempt = 0;
    socklen_t len;
    struct sockaddr_in my_addr;
    struct sockaddr_in cl_addr;
    pid_t pid, pid_estr;
    uint16_t lmsg, lmsg_signup;
    char buffer[BUFFER_SIZE];
    //  char buffer_ID[BUFFER_SIZE];
    char *buffer_ID = malloc(sizeof(char) * 11);
    char *us;
    char *temp;
    char *us_log;
    char *pwd_log;
    char *pwd;
    const char s[2] = " ";
    struct users users_list;
    int i;
    int k = 0;
    int cont = 0;
    char msg_signup[N];
    FILE *f1; //file contenente tutti gli username con le relative password
    FILE *f2;
    FILE *f3;
    FILE *f5;
    FILE *f_estr;
    int flag = 0;
    const size_t line_size = 300;
    char *lline = NULL;
    size_t length = 0;
    ssize_t read;
    char try[len];

    char buf[100];
    char newbuf[N];
    time_t t;
    struct tm *timeptr;


    char lline2[N];
    FILE *f4;
    struct tm tmm2;
    int a, l, j;
    int b;

    char *tokl = NULL;
    char *tokl2 = NULL;
    char tokl22[N];

    char *lline3 = NULL;
    ssize_t read2;
    size_t lenght2;

    FILE *f_utente;
    char nomefile[N];

    char *id_session = malloc(sizeof(char) * 11);
    int importo_int;

    struct Schedina sched;
    long numero;
    char *eptr;

    float vector[10];

    long ciao;
    char importo[25];
    char numeri[N];

    int dimensione = 0;
    int numero_estr = -1;

    sched.puntate[0] = "Estratto";
    sched.puntate[1] = "Ambo";
    sched.puntate[2] = "Terno";
    sched.puntate[3] = "Quaterna";
    sched.puntate[4] = "Cinquina";

    char *ruote[] = {"Bari", "Cagliari", "Firenze", "Genova", "Milano", "Napoli", "Palermo", "Roma", "Torino",
                     "Venezia", "Nazionale"};
    char *spazi[] = {"      ","  ","   ","    ","    ","    ","   ","      ","    ","   "," "};


    pid_estr = fork();

    if (pid_estr == 0) {
        while (1) {
            f_estr = fopen("/home/giuseppe/Scrivania/estrazione.txt", "a+");
            fprintf(f_estr, "%d ", numero_estr);
            t = time(NULL);
            timeptr = localtime(&t);
            strftime(buf, sizeof(buf), "%d/%m/%Y-%H:%M", timeptr);
            fprintf(f_estr, "%s ", buf);
            fprintf(f_estr, "**************************\n");



            for (i = 0; i < 11; i++) {
                fprintf(f_estr, "%d ", numero_estr);


                fprintf(f_estr, "%s %s%s", buf, ruote[i],spazi[i]);
                  /* for (l = 0; l < 10 - strlen(ruote[i]); l++) {
                       fprintf(f_estr, " ");                      //aggiungo spazi per allineare i numeri <-- DA PROBLEMI ALLA invia_giocata
                   }*/
                   for (j = 0; j < 5; j++) {
                       fprintf(f_estr, "%d ", rand() % 90 + 1);
                   }

                   fprintf(f_estr, "\n");

            }


            fclose(f_estr);

            sleep(300);
            numero_estr--;

        }



    } else {


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
            printf("Connessione accettata");


            pid = fork();

            // Sono nel processo figlio
            if (pid == 0) {
                close(sd);
                printf("Ciao sono il figlio\n");
                fflush(stdout);


                f4 = fopen("/home/giuseppe/Scrivania/tentativi.txt", "r");

                memset(&tmm2, 0, sizeof(struct tm));
                while (fgets(lline2, 100, f4) != NULL)
                    //  while ((read2 = getline(&lline3, &lenght2, f1)) != -1)
                {


                    tokl = strtok(lline2, " ");
                    tokl2 = strtok(NULL, " ");




                    //a = strlen(tokl2);
                    strncpy(tokl22, tokl2, 16);


                    strptime(tokl22, "%d/%m/%Y-%H:%M", &tmm2);

                    printf("giorno: %d, ,mese: %d, ore: %d, minuti: %d\n", tmm2.tm_mday, tmm2.tm_mon + 1, tmm2.tm_hour,
                           tmm2.tm_min);
                    printf("La differenza in minuti è: %d\n", Differenza(tmm2));

                    inet_ntop(AF_INET, &cl_addr.sin_addr, try, len);
                    if (strcmp(try, tokl) == 0 && Differenza(tmm2) < 30) {
                        strcpy(msg_signup,
                               "Non sono ancora trascorsi 30 minuti dal suo ultimo tentativo di accesso. Aspettare\n");
                        len_msg_signup = strlen(msg_signup) + 1;
                        lmsg_signup = htons(len_msg_signup);
                        ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                        ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);

                        fclose(f4);
                        close(new_sd);
                        exit(-1);
                    }


                }
                strcpy(msg_signup, "Puoi andare avanti\n");
                len_msg_signup = strlen(msg_signup) + 1;
                lmsg_signup = htons(len_msg_signup);
                ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);


                fclose(f4);


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
                    ret = recv(new_sd, (void *) buffer, len, 0);
                    printf("Buffer ricevuto\n");

                    if (ret < 0) {
                        perror("Errore in fase di ricezione: \n");
                        continue;
                    }

                    printf("%c", buffer[0]);
                    fflush(stdout);

                    /* !signup */
                    if (strncmp(buffer, "!signup", 7) == 0) {
                        printf("Sono dentro\n");
                        printf("Il buffer ricevuto è %s", buffer);

                        us = strtok(buffer, s);
                        us = strtok(NULL, s);
                        pwd = strtok(NULL, s);

                        printf("us è %s\n", us);


                        f1 = fopen("/home/giuseppe/Scrivania/utenti.txt", "a+");


                        flag = 0;
                        while ((read = getline(&lline, &length, f1)) != -1) {

                            //  if (strncmp(strtok(lline,s),us,strlen(us)) == 0 )
                            if (strcmp(strtok(lline, s), us) == 0) {
                                flag = 1;
                                printf("Entrato nel blocco, setto flag a %d\n", flag);
                                fflush(stdout);


                                strcpy(msg_signup, "Username non disponibile, riprovare\n");
                                len_msg_signup = strlen(msg_signup) + 1;
                                lmsg_signup = htons(len_msg_signup);
                                ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                                ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);
                                //break;
                            }
                        }


                        //USERNAME DISPONIBILE
                        if (flag == 0) {
                            fprintf(f1, "%s ", us);
                            fprintf(f1, "%s", pwd);
                            fclose(f1);

                            strcpy(msg_signup, "Registazione avvenuta con successo\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);

                            strcpy(nomefile, "/home/giuseppe/Scrivania/");
                            printf("%s", nomefile);
                            fflush(stdout);
                            strcat(nomefile, us);
                            strcat(nomefile, ".txt");
                            printf("%s", nomefile);
                            fflush(stdout);
                            f_utente = fopen(nomefile, "a+");
                            fclose(f_utente);

                        }


                        fclose(f1);
                        if (lline) {
                            free(lline);
                        }
                    }

                    /* login */
                    if (strncmp(buffer, "!login", 6) == 0) {
                        us = strtok(buffer, s);
                        us = strtok(NULL, s);
                        pwd = strtok(NULL, s);

                        f1 = fopen("/home/giuseppe/Scrivania/utenti.txt", "r");

                        flag = 0;
                        while ((read = getline(&lline, &length, f1)) != -1) {
                            us_log = strtok(lline, s);
                            pwd_log = strtok(NULL, s);


                            //DATI LOGIN CORRETTI
                            if (strcmp(us_log, us) == 0 && strcmp(pwd_log, pwd) == 0) {
                                strcpy(nomefile, "/home/giuseppe/Scrivania/giocate_");
                                printf("%s", nomefile);
                                fflush(stdout);
                                strcat(nomefile, us);
                                strcat(nomefile, ".txt");
                                printf("%s", nomefile);
                                fflush(stdout);
                                /*f_utente = fopen(nomefile,"a+");
                                fclose(f_utente); */

                                flag = 1;
                                printf("Entrato nel blocco, setto flag a %d\n", flag);
                                fflush(stdout);

                                //Salvo il session id e lo mando al client
                                gen_random(id_session, 10);
                                printf("%s", id_session);

                                strcpy(msg_signup, "Accesso effettuato, id:");
                                printf("%s", msg_signup);
                                fflush(stdout);
                                strcat(msg_signup, id_session);

                                printf("%s", msg_signup);
                                fflush(stdout);

                                //  strcat(msg_signup,"\n");
                                len_msg_signup = strlen(msg_signup) + 1;
                                lmsg_signup = htons(len_msg_signup);
                                ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                                ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);

                                // break;
                            }
                        }

                        //DATI LOGIN ERRATI
                        if (flag == 0) {
                            //Mando messaggio di errore al client
                            strcpy(msg_signup, "ERRORE: Accesso negato. (MASSIMO 3 TENTATIVI)\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);

                            attempt++; //incremento il numero di tentativi effettuati

                            //Se il client ha sbagliato per la terza volta
                            if (attempt == 3) {
                                attempt = 0;

                                strcpy(msg_signup,
                                       "EFFETTUATO IL MASSIMO DEI TENTATIVI, può riprovare l'accesso tra 30 minuti\n");
                                len_msg_signup = strlen(msg_signup) + 1;
                                lmsg_signup = htons(len_msg_signup);
                                ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                                ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);

                                //Salvo il timestamp in "buf" e nel file "tentativi.txt"
                                t = time(NULL);
                                timeptr = localtime(&t);
                                strftime(buf, sizeof(buf), "%d/%m/%Y-%H:%M", timeptr);

                                f3 = fopen("/home/giuseppe/Scrivania/tentativi.txt", "a+");
                                inet_ntop(AF_INET, &cl_addr.sin_addr, try, len);
                                // fprintf(f3,"%s %d:%d-%d/%d/%d\n",try,timeptr->tm_hour, timeptr->tm_min, timeptr->tm_mday, timeptr->tm_mon + 1, timeptr->tm_year + 1900);
                                fprintf(f3, "%s %s\n", try, buf);
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

                    if (strncmp(buffer, "!invia_giocata", 14) == 0) {
                        printf("Giocata ricevuta\n");
                        fflush(stdout);

                        // Attendo dimensione dell'ID
                        ret = recv(new_sd, (void *) &lmsg, sizeof(uint16_t), 0);
                        // Rinconverto in formato host
                        len = ntohs(lmsg);
                        // ricevo l'ID
                        ret = recv(new_sd, (void *) buffer_ID, len, 0);

                        // ID CORRETTO:
                        if (strcmp(buffer_ID, id_session) == 0) {
                            printf("ID valido\n");
                            fflush(stdout);

                            printf("%s\n", nomefile);

                            strcpy(msg_signup, "ID valido\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);

                            t = time(NULL);
                            timeptr = localtime(&t);
                            strftime(buf, sizeof(buf), "%d/%m/%Y-%H:%M", timeptr);

                            f5 = fopen(nomefile, "a+");
                            fprintf(f5, "%s ", buf);
                            fclose(f5);

                            temp = &buffer[18];
                            tokl = strtok(temp, "-");
                            strncpy(sched.ruote_giocate, tokl, strlen(tokl) - 1);
                            printf("%s\n", sched.ruote_giocate);
                            fflush(stdout);
                            f5 = fopen(nomefile, "a+");
                            fprintf(f5, "%s ", sched.ruote_giocate);
                            fclose(f5);

                            tokl = strtok(NULL, " ");
                            tokl = strtok(NULL, "-");
                            strncpy(numeri, tokl, strlen(tokl) - 1);
                            printf("%s\n", numeri);
                            strcat(numeri, "$");
                            sched.numeri_giocati[0] = strtol(numeri, &eptr, 10);
                            a = 1;
                            while (*eptr != '$') {
                                sched.numeri_giocati[a] = strtol(eptr, &eptr, 10);
                                a++;
                            }
                            dimensione = a;
                            printf("%d\n", dimensione);
                            for (int j = 0; j < dimensione; ++j) {
                                printf("%ld ", sched.numeri_giocati[j]);
                                f5 = fopen(nomefile, "a+");
                                fprintf(f5, "%ld ", sched.numeri_giocati[j]);
                                fclose(f5);

                            }
                            memset(numeri, 0, 100);
                            printf("%s\n", numeri);


                            /* IMPORTO: Salvo la parte del buffer ricevuto relativo agli importi puntati nella stringa "importo", per poi convertire in float ogni singolo importo e salvarlo in sched.importo_giocato */
                            tokl = strtok(NULL, " ");
                            tokl = strtok(NULL, "\0");
                            strncpy(importo, tokl, strlen(tokl) - 1);
                            strcat(importo, "$");

                            sched.importo_giocato[0] = strtof(importo, &eptr);
                            printf("%.2f ", sched.importo_giocato[0]);
                            f5 = fopen(nomefile, "a+");
                            if (sched.importo_giocato[0] != 0) {
                                fprintf(f5, "* %.2f Estratto ", sched.importo_giocato[0]);
                            }

                            for (i = 1; i < dimensione; i++) {
                                sched.importo_giocato[i] = strtof(eptr, &eptr);
                                printf("%.2f ", sched.importo_giocato[i]);
                                if (sched.importo_giocato[i] != 0) {
                                    fprintf(f5, "* %.2f ", sched.importo_giocato[i]);
                                    fprintf(f5, "%s ", sched.puntate[i]);
                                }

                            }
                            fprintf(f5, "%c", '\n');
                            fclose(f5);

                            strcpy(msg_signup, "Giocata effettuata\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);


                        } else {
                            printf("ID non valido");
                            strcpy(msg_signup, "ERROR_ID: Effettuare il LOGIN prima di poter cominciare a giocare\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);
                        }
                        /* printf("%s",id_session);
                        fflush(stdout);
                         */
                    }


                }
            } else {

                printf("Ciao sono il padre");
                fflush(stdout);
                close(new_sd);
            }


        }


    }

}