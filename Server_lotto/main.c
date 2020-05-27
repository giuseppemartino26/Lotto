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



//Struttura di appoggio utile per salvare temporaneamente la giocata e calcolare eventuali vincite
struct Giocata_V
{
    struct tm timestamp_giocata_v; // Per salvare in memoria il timestamp della giocata considerata quando si controllano le vincite
    char* ruote_v[11]; // Per salvare in memoria le ruote della giocata considerata
    int num_ruote; // numero di ruote giocate
    long numeri_giocati_v[5]; // numeri giocati
    int dim_num; //quanti numeri sono stati giocati
    float importi[5]; 
    float importi_vinti[5];
    int numeri_indovinati[5];
};


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

 int Fattoriale(int val)
{
  int i;
  int fatt = 1;
  for (i = 1; i <= val; i++)
    fatt = fatt * i;
  return fatt;
}

float calcola_vincita (int ng, int nr, int x, float soldi, int n_i)
{
    float vincita = 0;
     int k = Fattoriale(ng)/(Fattoriale(ng - (x + 1)) * Fattoriale(x + 1));
     int w = Fattoriale(n_i)/(Fattoriale(n_i - (x + 1))* Fattoriale(x + 1));
    
   if (x == 0)
   {
       vincita = w * 11.23 * soldi / (nr * k);
   }

   if (x == 1)
   {
       vincita = w * 250 * soldi / (nr * k);
   }

   if (x == 2)
   {
       vincita = w * 4500 * soldi / (nr * k);
   }

   if (x == 3)
   {
       vincita = w * 120000 * soldi / (nr * k);
   }

   if (x == 4)
   {
       vincita = w * 6000000 * soldi / (nr * k);
   }
   
   return vincita; 
   
}

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

int Diff (struct tm t1, struct tm t2)
{
    int diff;
    diff = (t2.tm_min - t1.tm_min) + (t2.tm_hour - t1.tm_hour)*60 + (t2.tm_mday - t1.tm_mday)*1440;
    return diff;
}

/* Funzione che ritorna la differenza in minuti tra un orario passato come parametro e l'ora attuale */  // NON MI PIACE, TROPPO COMPLESSA, PROVARE A ELIMINARE STRFTIME E STRPTIME
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
    int sd, new_sd, ret;
    u_long len_msg_signup;
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
    int i,y,z,q;
    int k = 0;
    int cont = 0;
    char msg_signup[N];
    FILE *f1; //file contenente tutti gli username con le relative password
    FILE *f2;
    FILE *f3;
    FILE *f5;
    FILE *f_estr;
    FILE * f6;
    FILE *f7;
    FILE *f8;
    FILE *f9;
    int flag = 0;
    const size_t line_size = 300;
    char *lline = NULL;
    size_t length = 0;
    ssize_t read;
    char try[len];

    char buf[100];
    char newbuf[N];
    time_t t, next_t;
    struct tm *timeptr;


    char lline2[N];
    char lline5[BUFFER_SIZE];
    char lline4[N];
    FILE *f4;
    struct tm tmm2;
    int a = 0;
    int l, j;
    int b;

    char *tokl = NULL;
    char *tokl2 = NULL;
    char tokl22[N];
    char tokl23[N];
    char tokl24[N];
    char *tokl26;

    char *lline3 = NULL;
    ssize_t read2;
    size_t lenght2;

    FILE *f_utente;
    char nomefile[N];

    char *id_session = malloc(sizeof(char) * 11);
    int importo_int;

    struct Schedina sched;
    long numero;
    char *eptr, *eptr2;

    char importo[25];
    char numeri[N];

    int dimensione = 0;
    int numero_estr = 1; //intero utile per trovare le ultime n estrazioni

    int numero_estrazioni;
    int n_righe_f_estr = 1;

    float imp_temp;
    int vett_num_v[5];

    sched.puntate[0] = "Estratto"; //non mi piace così, cambiare e farlo come fatto con "ruote"
    sched.puntate[1] = "Ambo";
    sched.puntate[2] = "Terno";
    sched.puntate[3] = "Quaterna";
    sched.puntate[4] = "Cinquina";

    char *ruote[] = {"Bari", "Cagliari", "Firenze", "Genova", "Milano", "Napoli", "Palermo", "Roma", "Torino",
                     "Venezia", "Nazionale"};
    char *spazi[] = {"      ", "  ", "   ", "    ", "    ", "    ", "   ", "      ", "    ", "   ", " "};

    struct tm tmvg;
    struct tm* next_estr_p; //puntatore a orario della prossima estrazione
    struct tm next_estr; //orario della prossima estrazione
    struct Giocata_V ver_giocata;
    struct tm estr_v;
    
    
   

    char superbuffer[N];

    //char superbuffer[BUFFER_SIZE];
    char superbuffer2[N];
    char superbuffer3[N];
    char superbuffer5[BUFFER_SIZE];
    char superbuffer6[BUFFER_SIZE];

    long n , n2;
    char* tokl3;

    int contatore;
    int contatore2 = 0;

    int flag2 = 1;

    char ruota_v[N];
    char data_estr_vincente[N];
    int quanti_num;

    int array[5];
    int temprn, randomIndex;
    int ni; // quantità di numeri indovinati

    float riepilogo[5];



    pid_estr = fork();

    if (pid_estr == 0) {
        while (1) {
            f_estr = fopen("/home/giuseppe/Scrivania/estrazione.txt", "a+");
            fprintf(f_estr, "%d ", numero_estr);
            t = time(NULL);

            next_t =time(NULL) + 300; //in next_t ho l'orario della prossima estrazione
            next_estr_p = localtime(&next_t); // salvo l'orario della prossima estrazione in una istanza di struct tm che userò per controllare se una estrazione è attiva o meno
            f7 = fopen("/home/giuseppe/Scrivania/prossima_estrazione.txt","w");
            strftime(buf, sizeof(buf), "%d/%m/%Y-%H:%M", next_estr_p);
            fprintf(f7, "%s ", buf);
            //fprintf(f7,"%d/%d/%d-%d:%d",next_estr_p->tm_mday,next_estr_p->tm_mon,next_estr_p->tm_year + 1900,next_estr_p->tm_hour,next_estr_p->tm_min);
            fclose(f7);
            // fprintf(f_estr,"%d:%d\n",next_estr_p->tm_hour,next_estr_p->tm_min);

            timeptr = localtime(&t);

            fprintf(f_estr," O Estrazione del ");
            strftime(buf, sizeof(buf), "%d/%m/%Y-%H:%M", timeptr);
            fprintf(f_estr, "%s ", buf);
            fprintf(f_estr, "*************\n");


            for (i = 0; i < 11; i++) {
                fprintf(f_estr, "%d ", numero_estr);


                fprintf(f_estr, "%s %s%s", buf, ruote[i], spazi[i]);
                /* for (l = 0; l < 10 - strlen(ruote[i]); l++) {
                     fprintf(f_estr, " ");                      //aggiungo spazi per allineare i numeri <-- DA PROBLEMI ALLA invia_giocata
                 }*/
                for (j = 0; j < 90; j++) {
                    array[j] = j;
                }

                for (j = 0; j < 90; j++) {
                    temprn = array[j];
                    randomIndex = rand() % 90 + 1;
                    array[j] = array[randomIndex];
                    array[randomIndex] = temprn;
                }

                for (j = 0; j < 5; j++)
                {

                    fprintf(f_estr, "%d ", (array[j]));
                }

                fprintf(f_estr, "\n");

            }


            fclose(f_estr);

            sleep(300);
            numero_estr++;

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
                            fprintf(f5, "%s", buf);
                            fclose(f5);

                            temp = &buffer[18];
                            //memset(&tokl,0,8);
                            tokl = strtok(temp, "-");
                            strcpy(sched.ruote_giocate, tokl);
                            printf("%s\n", sched.ruote_giocate);
                            fflush(stdout);
                            f5 = fopen(nomefile, "a+");
                            fprintf(f5, " %s* ", sched.ruote_giocate);
                            fclose(f5);

                            tokl = strtok(NULL, " ");
                            tokl = strtok(NULL, "-");
                            strcpy(numeri, tokl);
                            printf("%s\n", numeri);
                            strcat(numeri, "$");
                            //printf("%s",numeri);

                            sched.numeri_giocati[0] = strtol(numeri, &eptr, 10);
                            printf("%ld\n",sched.numeri_giocati[0]);
                            a = 0;
                            printf("%c\n", *eptr+1);
                            //while (*eptr != '$')
                            while (*eptr != '$')
                            {
                                a++;
                                sched.numeri_giocati[a] = strtol(eptr+1, &eptr, 10);

                                printf("%d\n",a);
                            }
                            dimensione = a;
                            printf("%d\n", dimensione);
                            for ( j = 0; j < dimensione; ++j) {
                                printf("%ld ", sched.numeri_giocati[j]);
                                f5 = fopen(nomefile, "a+");
                                fprintf(f5, "%ld ", sched.numeri_giocati[j]);
                                fclose(f5);

                            }
                            memset(numeri, 0, 100);
                            printf("%s\n", numeri);
                            memset(importo,0,25);


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

                    if (strncmp(buffer, "!vedi_giocate", 13) == 0)
                    {
                        // Attendo dimensione dell'ID
                        ret = recv(new_sd, (void *) &lmsg, sizeof(uint16_t), 0);
                        // Rinconverto in formato host
                        len = ntohs(lmsg);
                        // ricevo l'ID
                        ret = recv(new_sd, (void *) buffer_ID, len, 0);

                        // ID CORRETTO:
                        if (strcmp(buffer_ID, id_session) == 0)
                        {
                            printf("ID valido\n");
                            fflush(stdout);

                            strcpy(msg_signup, "ID valido\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);



                            if (buffer[14] == '0')
                            {

                                f7 = fopen("/home/giuseppe/Scrivania/prossima_estrazione.txt","r");
                                fgets(lline2,17,f7);
                                strptime(lline2, "%d/%m/%Y-%H:%M", &next_estr);
                                fclose(f7);

                                f6 = fopen(nomefile, "a+");
                                memset(&tmvg, 0, sizeof(struct tm));
                                memset(&lline2,0,sizeof(lline2 ));

                                printf("%s",superbuffer2);
                                contatore = 0;
                                while ( fgets(lline2, 100, f6) != NULL)
                                {

                                    tokl = strtok(lline2, " ");
                                    //printf("%s\n",tokl);
                                    tokl2 = strtok(NULL,"\n");
                                    printf("%s\n",tokl2);
                                    strcpy(tokl24, tokl);
                                    tokl26 = malloc(strlen(tokl2));
                                  // printf("%s\n",tokl23);
                                    strcpy(tokl26,tokl2);
                                    printf("%s",tokl26);


                                    strptime(tokl24, "%d/%m/%Y-%H:%M", &tmvg);

                                    printf("%d\n",Diff(tmvg,next_estr));

                                    //free(superbuffer);

                                    if (Diff(tmvg,next_estr) > 5 || Diff(tmvg,next_estr) == 5)
                                    {
                                        contatore++;

                                        if (contatore == 1)
                                        {
                                            strcpy(superbuffer2,tokl26);
                                            strcat(superbuffer2,"\n");
                                        } else {
                                            //  strcpy(appoggio,tokl2);
                                            // printf("%s\n",tokl23);
                                            //  memset(superbuffer,0,BUFFER_SIZE);
                                            //strcpy(superbuffer2,tokl26);
                                            strcat(superbuffer2, tokl26);
                                            strcat(superbuffer2, "\n");
                                            // strncat(superbuffer,tokl2,strlen(tokl2));
                                        }

                                    }

                                }
                               // printf("%s",superbuffer);
                                fclose(f6);


                                if (contatore == 0)
                                {
                                    strcpy(superbuffer2,"Nessuna giocata attiva");
                                }

                                //Mando le giocate non più attive
                                //  free(superbuffer);
                                len_msg_signup = strlen(superbuffer2) + 1;
                                lmsg_signup = htons(len_msg_signup);
                                ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                                ret = send(new_sd, (void *) superbuffer2, len_msg_signup, 0);
                               // free(superbuffer);
                            }


                            if (buffer[14] == '1')
                            {

                                f7 = fopen("/home/giuseppe/Scrivania/prossima_estrazione.txt","r");
                                fgets(lline2,17,f7);
                                strptime(lline2, "%d/%m/%Y-%H:%M", &next_estr);
                                fclose(f7);

                                f6 = fopen(nomefile, "a+");
                                memset(&tmvg, 0, sizeof(struct tm));
                                memset(&lline2,0,sizeof(lline2 ));
                                //    memset(superbuffer,0,BUFFER_SIZE);
                                //superbuffer = malloc(sizeof(char )* N); // serve per memorizzare le giocate da mandare al client
                                //printf("%s",superbuffer3);
                                contatore2 = 0;
                                while ( fgets(lline2, 100, f6) != NULL)
                                {

                                    tokl = strtok(lline2, " ");
                                    //printf("%s\n",tokl);
                                    tokl2 = strtok(NULL,"\n");
                                    printf("%s\n",tokl2);
                                    strcpy(tokl24, tokl);
                                    tokl26 = malloc(strlen(tokl2));
                                    // printf("%s\n",tokl23);
                                    strcpy(tokl26,tokl2);
                                    printf("%s\n",tokl26);


                                    strptime(tokl24, "%d/%m/%Y-%H:%M", &tmvg);

                                       printf("%d\n",Diff(tmvg,next_estr));

                                    //free(superbuffer);

                                    if (Diff(tmvg,next_estr) < 5)
                                    {
                                        contatore2++;

                                        printf("Il contatore2 è: %d",contatore2);
                                        if (contatore2 == 1)
                                        {
                                            strcpy(superbuffer3,tokl26);
                                            printf("%s\n",superbuffer3);
                                            free(tokl26);
                                            strcat(superbuffer3,"\n");
                                        } else {
                                            //  strcpy(appoggio,tokl2);
                                            // printf("%s\n",tokl23);
                                            //  memset(superbuffer,0,BUFFER_SIZE);
                                            //strcpy(superbuffer2,tokl26);
                                            strcat(superbuffer3, tokl26);
                                            free(tokl26);
                                            strcat(superbuffer3, "\n");
                                            // strncat(superbuffer,tokl2,strlen(tokl2));
                                        }



                                    }



                                }
                                // printf("%s",superbuffer);
                                fclose(f6);
                                printf("%s",superbuffer3);





                                //Mando le giocate non più attive
                                //  free(superbuffer);
                                if (contatore2 == 0)
                                {
                                    strcpy(superbuffer3,"Nessuna giocata attiva");
                                }
                                len_msg_signup = strlen(superbuffer3) + 1;
                                lmsg_signup = htons(len_msg_signup);
                                ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                                ret = send(new_sd, (void *) superbuffer3, len_msg_signup, 0);
                                // free(superbuffer);

                            }


                        } else
                        {
                            printf("ID non valido");
                            strcpy(msg_signup, "ERROR_ID: Effettuare il LOGIN prima di poter cominciare a giocare\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);
                        }


                    }

                    if (strncmp(buffer, "!vedi_estrazione", 16) == 0)
                    {
                        // Attendo dimensione dell'ID
                        ret = recv(new_sd, (void *) &lmsg, sizeof(uint16_t), 0);
                        // Rinconverto in formato host
                        len = ntohs(lmsg);
                        // ricevo l'ID
                        ret = recv(new_sd, (void *) buffer_ID, len, 0);

                        // ID CORRETTO:
                        if (strcmp(buffer_ID, id_session) == 0)
                        {
                            printf("ID valido\n");
                            fflush(stdout);

                            strcpy(msg_signup, "ID valido\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);

                            tokl = strtok(buffer," ");
                            tokl2 = strtok(NULL, " ");
                            tokl = strtok(NULL," ");

                            n = strtol(tokl2, &eptr, 10);

                            if (tokl == NULL)
                            {
                                printf("Tutte le ruote");
                                fflush(stdout);

                                f_estr = fopen("/home/giuseppe/Scrivania/estrazione.txt","r");

                                n_righe_f_estr = 0;

                                while ( fgets(lline2, 100, f_estr) != NULL)
                                {
                                    n_righe_f_estr++;
                                }
                                numero_estrazioni = n_righe_f_estr/12;

                                printf("%d",numero_estrazioni);

                                fclose(f_estr);

                                f_estr = fopen("/home/giuseppe/Scrivania/estrazione.txt","r");

                                memset(superbuffer,0,N);

                                while ( fgets(lline4, 100, f_estr) != NULL)
                                {
                                    strcpy(lline2,lline4);
                                    tokl3 = strtok(lline4, " ");
                                    n2 = strtol(tokl3, &eptr2,10);


                                    if (n2 > (numero_estrazioni -n) && flag2 % 2 != 0)
                                    {
                                        // printf("%s",lline2);
                                        strcat(superbuffer,lline2 + 19);
                                        //strcat(superbuffer,"\n");

                                    }

                                }
                                fclose(f_estr);

                                printf("%s",superbuffer);

                                //Mando le giocate ancora attive
                                len_msg_signup = strlen(superbuffer) + 1;
                                lmsg_signup = htons(len_msg_signup);
                                ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                                ret = send(new_sd, (void *) superbuffer, len_msg_signup, 0);
                                flag2 ++;

                            } else{
                                printf("%s",tokl);

                                f_estr = fopen("/home/giuseppe/Scrivania/estrazione.txt","r");

                                n_righe_f_estr = 0;

                                while ( fgets(lline2, 100, f_estr) != NULL)
                                {
                                    n_righe_f_estr++;
                                }
                                numero_estrazioni = n_righe_f_estr/12;

                                printf("%d",numero_estrazioni);

                                fclose(f_estr);

                                f_estr = fopen("/home/giuseppe/Scrivania/estrazione.txt","r");

                                memset(superbuffer,0,N);

                                while ( fgets(lline4, 100, f_estr) != NULL)
                                {
                                    strcpy(lline2,lline4);
                                    tokl3 = strtok(lline4, " ");
                                    tokl26 = strtok(NULL," ");
                                    tokl26 = strtok(NULL, " ");
                                    n2 = strtol(tokl3, &eptr2,10);

                                    printf("%s\n", tokl26);


                                    if (n2 > (numero_estrazioni -n) && flag2 % 2 != 0 && strncmp(tokl,tokl26, strlen(tokl)-1)==0 )
                                    {
                                        // printf("%s",lline2);
                                        strcat(superbuffer,lline2 + 2);
                                        //strcat(superbuffer,"\n");

                                    }

                                }
                                fclose(f_estr);

                                printf("%s",superbuffer);

                                //Mando le giocate ancora attive
                                len_msg_signup = strlen(superbuffer) + 1;
                                lmsg_signup = htons(len_msg_signup);
                                ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                                ret = send(new_sd, (void *) superbuffer, len_msg_signup, 0);
                                flag2 ++;

                            }

                        } else {
                            printf("ID non valido");
                            strcpy(msg_signup, "ERROR_ID: Effettuare il LOGIN prima di poter cominciare a giocare\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);
                        }
                    }



                    if (strncmp(buffer, "!vedi_vincite", 13) == 0)
                    {
                        // Attendo dimensione dell'ID
                        ret = recv(new_sd, (void *) &lmsg, sizeof(uint16_t), 0);
                        // Rinconverto in formato host
                        len = ntohs(lmsg);
                        // ricevo l'ID
                        ret = recv(new_sd, (void *) buffer_ID, len, 0);

                        // ID CORRETTO:
                        if (strcmp(buffer_ID, id_session) == 0)
                        {
                            printf("ID valido\n");
                            fflush(stdout);

                            strcpy(msg_signup, "ID valido\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);

                            f8 = fopen(nomefile,"a+");
                            while ( fgets(lline2, 100, f8) != NULL)
                            {
                                tokl = strtok(lline2," ");
                               
                                strptime(tokl, "%d/%m/%Y-%H:%M", &ver_giocata.timestamp_giocata_v);
                                
                                i = 0;
                               while (strncmp(tokl ,"*",1)!=0 )
                                {
                                   tokl = strtok(NULL, " ");
                                    //printf("%s ", tokl);
                                    ver_giocata.ruote_v[i] = tokl;
                                    i ++;

                                }
                                ver_giocata.num_ruote = i;
                              //  printf("%d\n", ver_giocata.num_ruote);
                             

                                tokl = strtok(NULL," "); 
                               // printf("%s\n",tokl);
                               ver_giocata.numeri_giocati_v[0] =strtol(tokl,&eptr,10);
                              // printf("%ld\n",ver_giocata.numeri_giocati_v[0]);
                               
                               i = 1;
                                while (strncmp(tokl ,"*",1)!=0 )
                                {
                                    tokl = strtok(NULL," ");
                                    ver_giocata.numeri_giocati_v[i] =strtol(tokl,&eptr,10);
                                   // printf("%s\n",tokl);
                                  // printf("%ld\n",ver_giocata.numeri_giocati_v[i]);
                                   i++;
                                } 
                                ver_giocata.dim_num = i;
                               // printf("Quanti numeri? %d\n",i - 1);

                                for (q = 0; q < 5; q++)
                                {
                                    ver_giocata.importi[q] = 0;
                                }
                                

                               while (*tokl != '\n')
                               {
                                   tokl = strtok(NULL," ,*");
                                   
                                   
                                   if (strcmp(tokl,"Estratto") == 0)
                                   {
                                       ver_giocata.importi[0] = imp_temp;
                                       
                                   }
                                   if (strcmp(tokl,"Ambo") == 0)
                                   {
                                       ver_giocata.importi[1] = imp_temp; 
                                       
                                   }
                                   if (strcmp(tokl,"Terno") == 0)
                                   {
                                       ver_giocata.importi[2] = imp_temp;
                                       
                                   }
                                   if (strcmp(tokl,"Quaterna") == 0)
                                   {
                                       ver_giocata.importi[3] = imp_temp;
                                       
                                   }
                                   if (strcmp(tokl,"Cinquina") == 0)
                                   {
                                       ver_giocata.importi[4] = imp_temp;
                                       
                                   }


                                   imp_temp = strtof(tokl,&eptr);     
                               }

                               
                                
                               // printf("Numero ruote: %d\n",ver_giocata.num_ruote -1);

                               for ( i = 0; i < ver_giocata.num_ruote - 1; i++)
                               {
                                   f9 = fopen("/home/giuseppe/Scrivania/estrazione.txt", "a+");
                                   while (fgets(lline5, 100, f9) != NULL)
                                   {
                                       tokl = strtok(lline5," ");
                                       tokl = strtok(NULL," ");
                                      // printf("%s\n",tokl);

                                       if (strncmp(tokl, "0",1)!= 0)
                                       {
                                       strptime(tokl, "%d/%m/%Y-%H:%M", &estr_v);
                                       strcpy(data_estr_vincente,tokl);
                                      // printf("%d/%d/%d- %d:%d\n",estr_v.tm_mday,estr_v.tm_mon,estr_v.tm_year,estr_v.tm_hour,estr_v.tm_min);
                                       //printf("Differenza minuti: %d\n", Diff(ver_giocata.timestamp_giocata_v,estr_v));
                                       }

                                       strcpy(ruota_v,strtok(NULL," "));
                                       //printf("%s\n",ruota_v);
                                       

                                       if (Diff(estr_v, ver_giocata.timestamp_giocata_v) < 0 && Diff(estr_v, ver_giocata.timestamp_giocata_v) > -5  && strcmp(ver_giocata.ruote_v[i],ruota_v)== 0)
                                       {
                                           //printf("Differenza %d\n",Diff(estr_v, ver_giocata.timestamp_giocata_v));

                                           j = 0;
                                           while (*tokl != '\n' && j < 5)
                                           {
                                               tokl = strtok(NULL," ");
                                               vett_num_v[j] = strtol(tokl, &eptr, 10);
                                              // printf("%d\n", vett_num_v[j]);
                                               j++;

                                           }

                                            z = 0;
                                            memset(superbuffer5,0,BUFFER_SIZE);
                                           for (k = 0; k < 5; k++)
                                           {
                                               for (y = 0; y < 5; y++)
                                               {
                                                   if (vett_num_v[k] == ver_giocata.numeri_giocati_v[y] )
                                                   {
                                                       z++;
                                                       ver_giocata.numeri_indovinati[z] = vett_num_v[k];
                                                     
                                                   }
                                                   
                                               }
                                               
                                           }
                                           ni = z;
                                           //printf("%s\n", "************************************");
                                           strcat(superbuffer6, "*******************************\n");
                                           strcat(superbuffer6,"Estrazione del ");
                                           strcat(superbuffer6,data_estr_vincente);
                                           strcat(superbuffer6,"\n");
                                           strcat(superbuffer6,ruota_v);
                                           strcat(superbuffer6," : ");
                                           
                                         // printf("Estrazione del %s:\n ",data_estr_vincente);
                                         // printf("%s : ",ruota_v);

                                           for (q = 0; q < z; q++)
                                           {
                                             //  printf("%d ",ver_giocata.numeri_indovinati[q + 1]);
                                               sprintf(superbuffer6 + strlen(superbuffer6),"%d ",ver_giocata.numeri_indovinati[q + 1]);
                                           }
                                          
                                         //  printf("%s",">> ");
                                         strcat(superbuffer6,">> ");
                                         

                                           for (q = 0; q < 5; q++)
                                           {
                                               if (ver_giocata.importi[q] != 0 && (z == (q + 1) || z > (q + 1)))
                                               {
                                                   quanti_num = ver_giocata.dim_num - 1;
                                                  // printf("\n%d\n%d\n%d\n%.2f\n",quanti_num,ver_giocata.num_ruote,q,ver_giocata.importi[q]);
                                                  // printf("%s %.2f ", sched.puntate[q],ver_giocata.importi[q]);
                                                  
                                                 //printf("%s %.2f ", sched.puntate[q], calcola_vincita(quanti_num,ver_giocata.num_ruote - 1,q,ver_giocata.importi[q],ni));
                                                 strcat(superbuffer6,sched.puntate[q]);
                                                 strcat(superbuffer6, " ");
                                                 
                                                 sprintf(superbuffer6 + strlen(superbuffer6),"%.2f ", calcola_vincita(quanti_num,ver_giocata.num_ruote - 1,q,ver_giocata.importi[q],ni));
                                                 //printf("Fattoriale di 3 è %d\n", Fattoriale(3));
                                                 riepilogo[q] += calcola_vincita(quanti_num,ver_giocata.num_ruote - 1,q,ver_giocata.importi[q],ni);

                                               }
                                               
                                           }
                                          // printf("%c",'\n');
                                          strcat(superbuffer6,"\n");

                                         // printf("%s",superbuffer6);
                                           
                                           
                                       }
                                       

                                   }
                                   fclose(f9);
                                   
                               }
                               
                               
                               

                    

                
                                

                            }
                            fclose(f8); 
                            //printf("\n%s\n",superbuffer6);

                            strcat(superbuffer6, "\nRiepilogo vincite:\n");


                            for ( q = 0; q < 5; q++)
                            {
                               // strcat(superbuffer6,"Vincite su ");
                               sprintf(superbuffer6 + strlen(superbuffer6), "Vincite su %s %.2f\n",sched.puntate[q],riepilogo[q]);

                              //  printf("Vincite su %s %.2f\n",sched.puntate[q],riepilogo[q]);
                            }
                            



                            len_msg_signup = strlen(superbuffer6) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *) superbuffer6, len_msg_signup, 0);














                        } else {
                        printf("ID non valido");
                        strcpy(msg_signup, "ERROR_ID: Effettuare il LOGIN prima di poter cominciare a giocare\n");
                        len_msg_signup = strlen(msg_signup) + 1;
                        lmsg_signup = htons(len_msg_signup);
                        ret = send(new_sd, (void *) &lmsg_signup, sizeof(uint16_t), 0);
                        ret = send(new_sd, (void *) msg_signup, len_msg_signup, 0);
                         }
                    }



                    



                }
            }
            else {
                printf("Ciao sono il padre");
                fflush(stdout);
                close(new_sd);
            }


        }


    }

}

