#define _GNU_SOURCE
#define _XOPEN_SOURCE 
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
    char *ruote_v[11];             // Per salvare in memoria le ruote della giocata considerata
    int num_ruote;                 // numero di ruote giocate
    long numeri_giocati_v[5];      // numeri giocati
    int dim_num;                   //quanti numeri sono stati giocati
    float importi[5];
    float importi_vinti[5];
    int numeri_indovinati[5];      //tutti i numeri indovinati
};

//struttura di una schedina giocata
struct Schedina
{
    struct tm timestamp_giocata;
    char ruote_giocate[150];
    long numeri_giocati[N];
    float importo_giocato[10];
    char *puntate[5];
};

//Funzione che calcola il fattoriale di un numero
int Fattoriale(int val)
{
    int i;
    int fatt = 1;
    for (i = 1; i <= val; i++)
        fatt = fatt * i;
    return fatt;
}

//calcola la vincita per una giocata, prende come parametri: quanti numeri sono stati giocati, il numero di ruote, l'importo giocato e quanti numeri sono stati indovinati
float calcola_vincita(int ng, int nr, int x, float soldi, int n_i)
{
    float vincita = 0;
    int k = Fattoriale(ng) / (Fattoriale(ng - (x + 1)) * Fattoriale(x + 1));
    int w = Fattoriale(n_i) / (Fattoriale(n_i - (x + 1)) * Fattoriale(x + 1));

//vincita Estratto
    if (x == 0)
    {
        vincita = w * 11.23 * soldi / (nr * k);
    }
//vincita Ambo
    if (x == 1)
    {
        vincita = w * 250 * soldi / (nr * k);
    }
//vincita Terno
    if (x == 2)
    {
        vincita = w * 4500 * soldi / (nr * k);
    }
//vincita Quaterna
    if (x == 3)
    {
        vincita = w * 120000 * soldi / (nr * k);
    }
//vincita Cinquina
    if (x == 4)
    {
        vincita = w * 6000000 * soldi / (nr * k);
    }

    return vincita;
}

//Funzione per generare la stringa alfanumerica del session id
void gen_random(char *s, const int len)
{
    int i;
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (i = 0; i < len; ++i)
    {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

//Funzione che ritorna la differenza in minuti tra due date
int Diff(struct tm t1, struct tm t2)
{
    int diff;
    diff = (t2.tm_min - t1.tm_min) + (t2.tm_hour - t1.tm_hour) * 60 + (t2.tm_mday - t1.tm_mday) * 1440 + (t2.tm_mon - t1.tm_mon) * 44640 + (t2.tm_year - t1.tm_year) * 525600;
    return diff;
}

/* Funzione che ritorna la differenza in minuti tra un orario passato come parametro e l'ora attuale */ 
int Differenza(struct tm t1)
{
    int diff;
    time_t tt;
    struct tm *tmm;
    char buf[N];
    struct tm tmmn;

    tt = time(NULL);
    tmm = localtime(&tt);
    strftime(buf, sizeof(buf), "%d/%m/%Y-%H:%M", tmm);
    strptime(buf, "%d/%m/%Y-%H:%M", &tmmn);

    diff = (tmmn.tm_min - t1.tm_min) + (tmmn.tm_hour - t1.tm_hour) * 60 + (tmmn.tm_mday - t1.tm_mday) * 1440 + (tmmn.tm_mon - t1.tm_mon) * 44640 + (tmmn.tm_year - t1.tm_year) * 525600;
    return diff;
}

int main(int argc, char *argv[])
{

    int sd, new_sd, ret; //socket
    u_long len_msg_signup;
    int attempt = 0; //intero per registrare il numero di tentativi di login falliti
    socklen_t len;
    struct sockaddr_in my_addr;
    struct sockaddr_in cl_addr;
    pid_t pid, pid_estr;
    uint16_t lmsg, lmsg_signup;
    char msg_signup[N]; // stringa usata per le comunicazioni con il client
    char buffer[BUFFER_SIZE];
    char *buffer_ID = malloc(sizeof(char) * 11);

    char *us;          //username inserito dall'utente nella signup
    char *pwd;         //password inserita dall'utente nella signup
    int flag = 0;      //uso un flag in signup e login per controllare che un username o password siano presenti o meno nel file
    size_t length = 0; //lunghezza della riga letta dalla getline del file contenente username e password

    char *us_log;       //username letto riga per riga dal file degli utenti registrati
    char *pwd_log;      //password letto riga per riga dal file degli utenti registrati
    struct tm *timeptr; //variabile di appoggio per calcolare il tempo corrente
    char buf[100];      //orario in formato stringa

    char numeri[N];     //stringa di appoggio per salvare i numeri giocati dall'utente
    int dimensione = 0; // quantità di numeri giocati dall'utente
    const char s[2] = " ";

// interi utilizzati per cicli e contatori
    int i, y, z, q, j,contatore, temprn;
    int k = 0;
    int a = 0;
    int contatore2 = 0;

    FILE *f1; // file per salvare utenti e password  
    FILE *f4; //file per salvare i tentativi degli utenti con i relativi host che bisogna bloccare nel caso si facciano 3 tentativi di login errati
    FILE *f5; //file giocate dell'utente
    FILE *f_estr; //file estrazioni
    FILE *f7; //file orario prossima estrazione
    
    char try [len];

    time_t t, next_t; //orario attuale e quello della prossima estrazione
    struct tm tmm2; //timestamp terzo tentativo login fallito

// Variabili per spezzare e gestire le stringhe 
    char *tokl = NULL;
    char *tokl2 = NULL;
    char *lline = NULL;
    char *tokl3;
    char tokl22[N];
    char tokl24[N];
    char *tokl26;
    char *temp;
    char lline2[BUFFER_SIZE];
    char lline5[BUFFER_SIZE];
    char lline4[BUFFER_SIZE];
    char *eptr, *eptr2;
    ssize_t read;

    char nomefile[N]; //stringa contenente il nome del file delle giocate di un utente
    char *id_session = malloc(sizeof(char) * 11); //puntatore a stringa alfanumerica per la session id

    struct Schedina sched;

    char importo[25]; //stringa per salvare gli importi giocati dall'utente
    int numero_estr = 1; //intero utile per trovare le ultime n estrazioni
    int numero_estrazioni; //quantità di tutte le estrazioni che sono uscite
    int n_righe_f_estr = 1; // numero righe del file contenente tutte le estrazioni


    sched.puntate[0] = "Estratto"; 
    sched.puntate[1] = "Ambo";
    sched.puntate[2] = "Terno";
    sched.puntate[3] = "Quaterna";
    sched.puntate[4] = "Cinquina";

    char *ruote[] = {"Bari", "Cagliari", "Firenze", "Genova", "Milano", "Napoli", "Palermo", "Roma", "Torino",
                     "Venezia", "Nazionale"};
    char *spazi[] = {"      ", "  ", "   ", "    ", "    ", "    ", "   ", "      ", "    ", "   ", " "};

    struct tm tmvg; //per salvare l'orario di una giocata nella "!vedi_giocata"
    struct tm *next_estr_p; //puntatore a orario della prossima estrazione
    struct tm next_estr;    //orario della prossima estrazione
    
// buffer per i messaggi da mandare ai client
    char buffer_per_c[BUFFER_SIZE];
    char buffer_per_c2[BUFFER_SIZE];
    char buffer_per_c3[ BUFFER_SIZE];
    char buffer_per_c4[BUFFER_SIZE];
    char superbuffer6[BUFFER_SIZE];
    char sbuffer7[BUFFER_SIZE];

    long n; // numero estrazioni da visualizzare nella !vedi_estrazione
    long n2; // n2 è il numero della i-esima estrazione per i = {1,..,numero_estrazioni} nella !vedi_estrazione

    int array[5]; 
    int randomIndex; //stringa alfanumerica per il session_id
    

//Variabili usare in !vedi_vincite
    char ruota_v[BUFFER_SIZE]; //ruote giocate
    struct Giocata_V ver_giocata; //salvo le giocate dell'utente in una struttura per fare il confronto con le estrazioni
    struct tm estr_v; //salvo in estr_v ora e data estrazione per confrontarli con quelle delle giocate
    int ni = 0; // quantità di numeri indovinati
    char data_estr_vincente[N];
    int quanti_num; //quantità di numeri giocati
    float imp_temp; //importo puntato 
    int vett_num_v[5]; //vettore contenente i numeri indovinati
    float riepilogo[5];
    int vittoria = 0;


//Creo un processo che ogni 5 minuti crei una estrazione, scrivendola in un file e calcoli l'orario della prossima estrazione
    pid_estr = fork();

    if (pid_estr == 0)
    {
        while (1)
        {
            f_estr = fopen("/home/giuseppe/Scrivania/estrazione.txt", "a+");
            fprintf(f_estr, "%d ", numero_estr);
            t = time(NULL);

            next_t = time(NULL) + 300;        //in next_t ho l'orario della prossima estrazione
            next_estr_p = localtime(&next_t);

             // salvo l'orario della prossima estrazione in una istanza di struct tm che userò per controllare se una estrazione è attiva o meno
            f7 = fopen("/home/giuseppe/Scrivania/prossima_estrazione.txt", "w");
            strftime(buf, sizeof(buf), "%d/%m/%Y-%H:%M", next_estr_p);
            fprintf(f7, "%s ", buf);
            fclose(f7);

            //scrivo l'orario in cui esce l'estrazione
            timeptr = localtime(&t); 
            fprintf(f_estr, " O Estrazione del ");
            strftime(buf, sizeof(buf), "%d/%m/%Y-%H:%M", timeptr);
            fprintf(f_estr, "%s ", buf);
            fprintf(f_estr, "*************\n");

            for (i = 0; i < 11; i++)
            {
                fprintf(f_estr, "%d ", numero_estr);
                fprintf(f_estr, "%s %s%s", buf, ruote[i], spazi[i]);

                //riempio i campi di un array con numeri casuali che saranno i numeri dell'estrazione
                for (j = 0; j < 90; j++)
                {
                    array[j] = j;
                }

                for (j = 0; j < 90; j++)
                {
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

            sleep(300); //blocca il processo per 5 minuti e poi riprende per la prossima estrazione
            numero_estr++;
        }
    }
    else
    {

        /* Creazione socket */
        sd = socket(AF_INET, SOCK_STREAM, 0);
        printf("Socket creato\n");
        fflush(stdout);

        /* Creazione indirizzo di bind */
        memset(&my_addr, 0, sizeof(my_addr)); // Pulizia
        my_addr.sin_family = AF_INET;
        my_addr.sin_port = htons(4242);
        my_addr.sin_addr.s_addr = INADDR_ANY;

        ret = bind(sd, (struct sockaddr *)&my_addr, sizeof(my_addr));

        printf("Ascolto\n");
        ret = listen(sd, 10);

        if (ret < 0)
        {
            perror("Errore in fase di bind: \n");
            exit(-1);
        }

        while (1)
        {

            len = sizeof(cl_addr);

            // Accetto nuove connessioni
            new_sd = accept(sd, (struct sockaddr *)&cl_addr, &len);
            printf("Connessione accettata");

            pid = fork();

            // Sono nel processo figlio
            if (pid == 0)
            {
                close(sd); //chiudo il socket di ascolto
                printf("Ciao sono il figlio\n");
                fflush(stdout);

                //Verifico che l'host non sia bloccato a causa dei tentativi di login errati
                //controllo nel file "tentativi.txt" se c'è una coppia di valori IP e timestamp ultimo tentativo e controllo che la differenza tra l'orario attuale e quello sul file sia > 30 minuti
                f4 = fopen("/home/giuseppe/Scrivania/tentativi.txt", "r");

                memset(&tmm2, 0, sizeof(struct tm));

                //leggo riga per riga il file "tentativi.txt"
                while (fgets(lline2, 100, f4) != NULL)
                {

                    tokl = strtok(lline2, " ");
                    tokl2 = strtok(NULL, " ");

                    //per ogni riga ho una coppia di valori IP e timestamp dell'ultimo terzo tentativo
                    //converto il timestamp da stringa di caratteri in struct tm
                    strncpy(tokl22, tokl2, 16);
                    strptime(tokl22, "%d/%m/%Y-%H:%M", &tmm2);

                    //salvo l'IP dell'host che sta provando a entrare in try
                    inet_ntop(AF_INET, &cl_addr.sin_addr, try, len);

                    if (strcmp(try, tokl) == 0 && Differenza(tmm2) < 30)
                    {
                        strcpy(msg_signup, "Non sono ancora trascorsi 30 minuti dal suo ultimo tentativo di accesso. Aspettare\n");
                        len_msg_signup = strlen(msg_signup) + 1;
                        lmsg_signup = htons(len_msg_signup);
                        ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                        ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);

                        // Non sono ancora trascorsi 30 minuti, l'utente non può entrare con questo host: chiudo il file, il socket e termino il processo
                        fclose(f4);
                        close(new_sd);
                        exit(-1);
                    }
                }
                strcpy(msg_signup, "Puoi andare avanti\n");
                len_msg_signup = strlen(msg_signup) + 1;
                lmsg_signup = htons(len_msg_signup);
                ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);

                fclose(f4);

                while (1)
                {
                    printf("pronto per ricevere\n");
                    fflush(stdout);

                    // Attendo dimensione del mesaggio
                    ret = recv(new_sd, (void *)&lmsg, sizeof(uint16_t), 0);

                    // Rinconverto in formato host
                    len = ntohs(lmsg);

                    // ricevo il messaggio effettivo di lunghezza len
                    ret = recv(new_sd, (void *)buffer, len, 0);
                    printf("Buffer ricevuto\n");

                    if (ret < 0)
                    {
                        perror("Errore in fase di ricezione: \n");
                        continue;
                    }

                    /* !signup */
                    if (strncmp(buffer, "!signup", 7) == 0)
                    {

                        // separo username e password inseriti dall'utente
                        us = strtok(buffer, s);
                        us = strtok(NULL, s);
                        pwd = strtok(NULL, s);

                        // controllo se nel file degli utenti registrati c'è già un utente con lo stesso username
                        f1 = fopen("/home/giuseppe/Scrivania/utenti.txt", "a+");

                        flag = 0;
                        while ((read = getline(&lline, &length, f1)) != -1)
                        {

                            if (strcmp(strtok(lline, s), us) == 0)
                            {
                                flag = 1; //setto flag a 1 se ho trovato uno username uguale

                                // se ho trovato nel file uno username uguale a quello scelto mando al client un messaggio invitandolo a riprovare la regiistrazione
                                strcpy(msg_signup, "Username non disponibile, riprovare\n");
                                len_msg_signup = strlen(msg_signup) + 1;
                                lmsg_signup = htons(len_msg_signup);
                                ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                                ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);
                            }
                        }

                        //USERNAME DISPONIBILE
                        if (flag == 0)
                        {

                            //salvo nel file username e password
                            fprintf(f1, "%s ", us);
                            fprintf(f1, "%s", pwd);
                            fclose(f1);

                            //mando al client un messaggio di avvenuta registrazione
                            strcpy(msg_signup, "Registazione avvenuta con successo\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);
                        }
                        fclose(f1);
                        /* if (lline) {
                            free(lline);
                        } */
                    }

                    /* login */
                    if (strncmp(buffer, "!login", 6) == 0)
                    {

                        // separo username e password inseriti dall'utente
                        us = strtok(buffer, s);
                        us = strtok(NULL, s);
                        pwd = strtok(NULL, s);

                        // controllo nel file degli utenti se esiste la coppia username e password e se esiste setto il flag a 1
                        f1 = fopen("/home/giuseppe/Scrivania/utenti.txt", "r");

                        flag = 0;
                        //leggo riga per riga, confronto username e password inseriti dall'utente (us_log e pwd_log) con quelli presenti nel file (us , pwd)
                        while ((read = getline(&lline, &length, f1)) != -1)
                        {

                            us_log = strtok(lline, s);
                            pwd_log = strtok(NULL, s);

                            //DATI LOGIN CORRETTI
                            if (strcmp(us_log, us) == 0 && strcmp(pwd_log, pwd) == 0)
                            {
                                //creo un file relativo all'utente in cui salverò tutte le sue giocate
                                strcpy(nomefile, "/home/giuseppe/Scrivania/giocate_");
                                strcat(nomefile, us);
                                strcat(nomefile, ".txt");

                                flag = 1;

                                //Genero il session id e lo mando al client, insieme a un messaggio di accesso effettuato
                                gen_random(id_session, 10);

                                strcpy(msg_signup, "Accesso effettuato, id:");
                                strcat(msg_signup, id_session);

                                len_msg_signup = strlen(msg_signup) + 1;
                                lmsg_signup = htons(len_msg_signup);
                                ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                                ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);
                            }
                        }

                        //DATI LOGIN ERRATI
                        if (flag == 0)
                        {
                            //Mando messaggio di errore al client
                            strcpy(msg_signup, "ERRORE: Accesso negato. (MASSIMO 3 TENTATIVI)\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);

                            attempt++; //incremento il numero di tentativi effettuati

                            //Se il client ha sbagliato per la terza volta
                            if (attempt == 3)
                            {
                                attempt = 0; //resetto il numero di tentativi

                                //invio al client un messaggio per informarlo che ha fatto 3 tentativi
                                strcpy(msg_signup, "EFFETTUATO IL MASSIMO DEI TENTATIVI, può riprovare l'accesso tra 30 minuti\n");
                                len_msg_signup = strlen(msg_signup) + 1;
                                lmsg_signup = htons(len_msg_signup);
                                ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                                ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);

                                //Salvo l'orario in cui è stato effettuato il terzo tentativo e l'indirizzo IP dell'host dell'utente che ha provato in un file
                                t = time(NULL);
                                timeptr = localtime(&t);
                                strftime(buf, sizeof(buf), "%d/%m/%Y-%H:%M", timeptr); //converto l'orario in stringa di caratteri per salvarla nel file "tentativi.txt"

                                f4 = fopen("/home/giuseppe/Scrivania/tentativi.txt", "a+");
                                inet_ntop(AF_INET, &cl_addr.sin_addr, try, len);
                                fprintf(f4, "%s %s\n", try, buf); //scrivo nel file l'orario e l'indirizzo IP dell'host che ha sbagliato per 3 volte
                                fclose(f4);

                                //chiudo la connessione e termino il processo
                                close(new_sd);
                                exit(-1);
                            }
                        }

                        fclose(f1);
                        if (lline)
                        {
                            free(lline);
                        }
                    }

                    /* invia giocata */
                    if (strncmp(buffer, "!invia_giocata", 14) == 0)
                    {
                        printf("Giocata ricevuta\n");
                        fflush(stdout);

                        // Attendo dimensione dell'ID
                        ret = recv(new_sd, (void *)&lmsg, sizeof(uint16_t), 0);
                        // Rinconverto in formato host
                        len = ntohs(lmsg);
                        // ricevo l'ID
                        ret = recv(new_sd, (void *)buffer_ID, len, 0);

                        // ID CORRETTO:
                        if (strcmp(buffer_ID, id_session) == 0)
                        {
                            printf("ID valido\n");
                            fflush(stdout);

                            strcpy(msg_signup, "ID valido\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);

                            //calcolo data e ora della giocata e le salvo in una stringa, buf
                            t = time(NULL);
                            timeptr = localtime(&t);
                            strftime(buf, sizeof(buf), "%d/%m/%Y-%H:%M", timeptr);

                            // spezzo il buffer ricevuto dal client salvando nella schedina Sched le ruote giocate
                            temp = &buffer[18];
                            tokl = strtok(temp, "-");
                            strcpy(sched.ruote_giocate, tokl);

                            //numeri giocati
                            tokl = strtok(NULL, " ");
                            tokl = strtok(NULL, "-");
                            strcpy(numeri, tokl);
                            printf("%s\n", numeri); // salvo i numeri giocati in una stringa
                            strcat(numeri, "$");

                            //scandisco la stringa, converto i numeri da carattere a long e li salvo in memoria nel campo vettore di long numeri_giocati della schedina Sched
                            sched.numeri_giocati[0] = strtol(numeri, &eptr, 10);
                            a = 0;
                            while (*eptr != '$')
                            {
                                a++;
                                sched.numeri_giocati[a] = strtol(eptr + 1, &eptr, 10);
                            }

                            dimensione = a; // in dimensione ho quanti numeri l'utente ha digitato

                            if (dimensione > 10) // controllo che l'utente non abbia digitato più di 10 numeri
                            {
                                strcpy(msg_signup, "ERRORE: puoi giocare un massimo di 10 numeri\n");
                                len_msg_signup = strlen(msg_signup) + 1;
                                lmsg_signup = htons(len_msg_signup);
                                ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                                ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);
                            }
                            else
                            {
                                // se i numeri inseriti dall'utente sono al massimo 10
                                f5 = fopen(nomefile, "a+");
                                fprintf(f5, "%s", buf);                    // stampa sul file data e ora giocata
                                fprintf(f5, " %s* ", sched.ruote_giocate); //stampa su file le ruote giocate
                                fclose(f5);

                                for (j = 0; j < dimensione; ++j) //stampo sul file i numeri inseriti dall'utente
                                {
                                    f5 = fopen(nomefile, "a+");
                                    fprintf(f5, "%ld ", sched.numeri_giocati[j]);
                                    fclose(f5);
                                }
                                memset(numeri, 0, 100);
                                memset(importo, 0, 25);

                                /* IMPORTO: Salvo la parte del buffer ricevuto relativo agli importi puntati nella stringa "importo", per poi convertire in float ogni singolo importo e salvarlo in sched.importo_giocato e poi salvo nel file */
                                tokl = strtok(NULL, " ");
                                tokl = strtok(NULL, "\0");
                                strncpy(importo, tokl, strlen(tokl) - 1);
                                strcat(importo, "$");

                                sched.importo_giocato[0] = strtof(importo, &eptr);
                                f5 = fopen(nomefile, "a+");
                                if (sched.importo_giocato[0] != 0)
                                {
                                    fprintf(f5, "* %.2f Estratto ", sched.importo_giocato[0]);
                                }

                                for (i = 1; i < dimensione; i++)
                                {
                                    sched.importo_giocato[i] = strtof(eptr, &eptr);
                                    if (sched.importo_giocato[i] != 0)
                                    {
                                        fprintf(f5, "* %.2f ", sched.importo_giocato[i]);
                                        fprintf(f5, "%s ", sched.puntate[i]);
                                    }
                                }
                                fprintf(f5, "%c", '\n');
                                fclose(f5);

                                //mando al client un messaggio informndolo che la giocata è stata correttamente effettuata
                                strcpy(msg_signup, "Giocata effettuata\n");
                                len_msg_signup = strlen(msg_signup) + 1;
                                lmsg_signup = htons(len_msg_signup);
                                ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                                ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);
                            }
                        }
                        else
                        {
                            printf("ID non valido");
                            strcpy(msg_signup, "ERROR_ID: Effettuare il LOGIN prima di poter cominciare a giocare\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);
                        }
                    }

                    /* vedi giocate */
                    if (strncmp(buffer, "!vedi_giocate", 13) == 0)
                    {
                        // Attendo dimensione dell'ID
                        ret = recv(new_sd, (void *)&lmsg, sizeof(uint16_t), 0);
                        // Rinconverto in formato host
                        len = ntohs(lmsg);
                        // ricevo l'ID
                        ret = recv(new_sd, (void *)buffer_ID, len, 0);

                        // ID CORRETTO:
                        if (strcmp(buffer_ID, id_session) == 0)
                        {
                            printf("ID valido\n");
                            fflush(stdout);

                            strcpy(msg_signup, "ID valido\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);

                            // GIOCATE NON ATTIVE
                            if (buffer[14] == '0')
                            {
                                // salvo l'ora della prossima estrazione (scritta nel file "prossima_estrazione.txt") in next_estr
                                f7 = fopen("/home/giuseppe/Scrivania/prossima_estrazione.txt", "r");
                                fgets(lline2, 17, f7);
                                strptime(lline2, "%d/%m/%Y-%H:%M", &next_estr);
                                fclose(f7);

                                //leggo riga per riga il file delle giocate dell'utente
                                f5 = fopen(nomefile, "a+");
                                memset(&tmvg, 0, sizeof(struct tm));
                                memset(&lline2, 0, sizeof(lline2));

                                contatore = 0;
                                while (fgets(lline2, 100, f5) != NULL)
                                {
                                    tokl = strtok(lline2, " ");
                                    tokl2 = strtok(NULL, "\n"); //giocata

                                    strcpy(tokl24, tokl);
                                    tokl26 = malloc(strlen(tokl2));

                                    strcpy(tokl26, tokl2);

                                    //prelevo la stringa contenente data e ora della giocata, la converto in tm e l'assegno a tmvg
                                    strptime(tokl24, "%d/%m/%Y-%H:%M", &tmvg);
                                    // se ci sono 5 minuti di differenza o più tra la data della giocata e la data dell'ultima estrazione, allora la schedina non è più attiva e la copio nel buffer che invierò al client
                                    if (Diff(tmvg, next_estr) > 5 || Diff(tmvg, next_estr) == 5)
                                    {
                                        contatore++;

                                        if (contatore == 1)
                                        {
                                            strcpy(buffer_per_c, tokl26);
                                            free(tokl26);
                                            strcat(buffer_per_c, "\n");
                                        }
                                        else
                                        {
                                            strcat(buffer_per_c, tokl26);
                                            free(tokl26);
                                            strcat(buffer_per_c, "\n");
                                        }
                                    }
                                }

                                fclose(f5);

                                if (contatore == 0)
                                {
                                    strcpy(buffer_per_c, "Nessuna giocata non più attiva");
                                }

                                //Mando le giocate non più attive al client
                                len_msg_signup = strlen(buffer_per_c) + 1;
                                lmsg_signup = htons(len_msg_signup);
                                ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                                ret = send(new_sd, (void *)buffer_per_c, len_msg_signup, 0);
                            }

                            //GIOCATE ATTIVE
                            //come la parte per le giocate non attive, cambia solo l'if alla riga 796
                            if (buffer[14] == '1')
                            {
                                f7 = fopen("/home/giuseppe/Scrivania/prossima_estrazione.txt", "r");
                                fgets(lline2, 17, f7);
                                strptime(lline2, "%d/%m/%Y-%H:%M", &next_estr);
                                fclose(f7);

                                f5 = fopen(nomefile, "a+");
                                memset(&tmvg, 0, sizeof(struct tm));
                                memset(&lline2, 0, sizeof(lline2));
                                
                                contatore2 = 0;
                                while (fgets(lline2, 100, f5) != NULL)
                                {

                                    tokl = strtok(lline2, " ");
                                    tokl2 = strtok(NULL, "\n");
                                    printf("%s\n", tokl2);
                                    strcpy(tokl24, tokl);
                                    tokl26 = malloc(strlen(tokl2));
                                    strcpy(tokl26, tokl2);
                                    printf("%s\n", tokl26);

                                    strptime(tokl24, "%d/%m/%Y-%H:%M", &tmvg);

                                    if (Diff(tmvg, next_estr) < 5) //se la prossima estrazione è tra meno di 5 minuti allora la giocata è ancora attiva
                                    {
                                        contatore2++;

                                        if (contatore2 == 1)
                                        {
                                            strcpy(buffer_per_c2, tokl26);
                                            free(tokl26);
                                            strcat(buffer_per_c2, "\n");
                                        }
                                        else
                                        {
                                            strcat(buffer_per_c2, tokl26);
                                            free(tokl26);
                                            strcat(buffer_per_c2, "\n");
                                        }
                                    }
                                }
                                fclose(f5);

                                //Mando le giocate attive (se ci sono)
                                if (contatore2 == 0)
                                {
                                    strcpy(buffer_per_c2, "Nessuna giocata attiva");
                                }
                                len_msg_signup = strlen(buffer_per_c2) + 1;
                                lmsg_signup = htons(len_msg_signup);
                                ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                                ret = send(new_sd, (void *)buffer_per_c2, len_msg_signup, 0);
                                
                            }
                        }
                        else
                        {
                            printf("ID non valido");
                            strcpy(msg_signup, "ERROR_ID: Effettuare il LOGIN prima di poter cominciare a giocare\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);
                        }
                    }

                    /* vedi estrazione */
                    if (strncmp(buffer, "!vedi_estrazione", 16) == 0)
                    {
                        // Attendo dimensione dell'ID
                        ret = recv(new_sd, (void *)&lmsg, sizeof(uint16_t), 0);
                        // Rinconverto in formato host
                        len = ntohs(lmsg);
                        // ricevo l'ID
                        ret = recv(new_sd, (void *)buffer_ID, len, 0);

                        // ID CORRETTO:
                        if (strcmp(buffer_ID, id_session) == 0)
                        {
                            printf("ID valido\n");
                            fflush(stdout);

                            strcpy(msg_signup, "ID valido\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);

                            tokl = strtok(buffer, " ");
                            tokl2 = strtok(NULL, " ");
                            tokl = strtok(NULL, " "); // se la ruota è stata specificata, sarà puntata da tokl

                            n = strtol(tokl2, &eptr, 10); // salvo in n quante estrazioni il client vuole visualizzare
       
                            if (tokl == NULL) //nessuna ruota specificata --> tutte le ruote
                            {
                                
                                f_estr = fopen("/home/giuseppe/Scrivania/estrazione.txt", "r");
                                n_righe_f_estr = 0;
                                //leggo riga per riga dal file delle estrazioni
                                while (fgets(lline2, 100, f_estr) != NULL)
                                {
                                    n_righe_f_estr++; //conto il numero di righe del file
                                }
                                numero_estrazioni = n_righe_f_estr / 12; //per ogni estrazione ci sono 12 righe nel file
                                fclose(f_estr);

                                f_estr = fopen("/home/giuseppe/Scrivania/estrazione.txt", "r");
                                memset(buffer_per_c3, 0, BUFFER_SIZE);
                                while (fgets(lline4, 100, f_estr) != NULL)
                                {
                                    strcpy(lline2, lline4);
                                    tokl3 = strtok(lline4, " ");
                                    n2 = strtol(tokl3, &eptr2, 10); // n2 è il numero della i-esima estrazione per i = {1,..,numero_estrazioni}
                                    if (n2 > (numero_estrazioni - n)) //condizione per stampare nel buffer solo le ultime estrazioni richieste dal client
                                    {
                                        strcat(buffer_per_c3, lline2 + 19); // copia nel buffer le estrazioni richieste (ruote e numeri)
                                    }
                                }
                                fclose(f_estr);

                                //Mando le estrazioni al client
                                len_msg_signup = strlen(buffer_per_c3) + 1;
                                lmsg_signup = htons(len_msg_signup);
                                ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                                ret = send(new_sd, (void *)buffer_per_c3, len_msg_signup, 0);
                            }
                            else //ruota specifica
                            {
                                // come per tutte le ruote
                                f_estr = fopen("/home/giuseppe/Scrivania/estrazione.txt", "r");
                                n_righe_f_estr = 0;
                                while (fgets(lline2, 100, f_estr) != NULL)
                                {
                                    n_righe_f_estr++;
                                }
                                numero_estrazioni = n_righe_f_estr / 12;
                                fclose(f_estr);

                                f_estr = fopen("/home/giuseppe/Scrivania/estrazione.txt", "r");
                                memset(buffer_per_c3, 0, BUFFER_SIZE);
                                while (fgets(lline4, 100, f_estr) != NULL) 
                                {
                                    strcpy(lline2, lline4);
                                    tokl3 = strtok(lline4, " ");
                                    tokl26 = strtok(NULL, " ");
                                    tokl26 = strtok(NULL, " "); // ruota di un'estrazione
                                    n2 = strtol(tokl3, &eptr2, 10); 
                                    

                                    if (n2 > (numero_estrazioni - n) && strncmp(tokl, tokl26, strlen(tokl) - 1) == 0) //copio nel buffer da mandare al client solo le righe con la ruota specificata dal client
                                    {                                 
                                        strcat(buffer_per_c3, lline2 + 2);
                                        
                                    }
                                }
                                fclose(f_estr);

                                //Mando le estrazioni al client
                                len_msg_signup = strlen(buffer_per_c3) + 1;
                                lmsg_signup = htons(len_msg_signup);
                                ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                                ret = send(new_sd, (void *)buffer_per_c3, len_msg_signup, 0);
                            }
                        }
                        else
                        {
                            printf("ID non valido");
                            strcpy(msg_signup, "ERROR_ID: Effettuare il LOGIN prima di poter cominciare a giocare\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);
                        }
                    }

                    /* vedi vincite */
                    if (strncmp(buffer, "!vedi_vincite", 13) == 0)
                    {
                        // Attendo dimensione dell'ID
                        ret = recv(new_sd, (void *)&lmsg, sizeof(uint16_t), 0);
                        // Rinconverto in formato host
                        len = ntohs(lmsg);
                        // ricevo l'ID
                        ret = recv(new_sd, (void *)buffer_ID, len, 0);

                        // ID CORRETTO:
                        if (strcmp(buffer_ID, id_session) == 0)
                        {
                            printf("ID valido\n");
                            fflush(stdout);

                            strcpy(msg_signup, "ID valido\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);

                            f5 = fopen(nomefile, "a+");
                            //leggo riga per riga il file delle giocate dell'utente: per ogni riga c'è una schedina
                            //salvo in memoria tutte le informazioni utili per calcolare la vincita di ogni schedina
                            while (fgets(lline2, 100, f5) != NULL)
                            {
                                tokl = strtok(lline2, " ");
                                strptime(tokl, "%d/%m/%Y-%H:%M", &ver_giocata.timestamp_giocata_v); //leggo e salvo l'orario della schedina

                                i = 0;
                                while (strncmp(tokl, "*", 1) != 0)
                                {
                                    tokl = strtok(NULL, " ");
                                    ver_giocata.ruote_v[i] = tokl;
                                    i++;
                                }
                                ver_giocata.num_ruote = i; //numero ruote giocate della schedina
                                

                                tokl = strtok(NULL, " ");
                                for (y = 0; y < 5; y++)
                                {
                                    ver_giocata.numeri_giocati_v[y] = 0; //setto a 0 il vettore dei numeri giocati
                                }
                                
                                //leggo e salvo nel vettore i numeri giocati della schedina
                                ver_giocata.numeri_giocati_v[0] = strtol(tokl, &eptr, 10);
                                i = 1;
                                while (strncmp(tokl, "*", 1) != 0)
                                {
                                    tokl = strtok(NULL, " ");
                                    ver_giocata.numeri_giocati_v[i] = strtol(tokl, &eptr, 10);
                                    i++;
                                }
                                ver_giocata.dim_num = i; //salvo quanti sono i numeri giocati della schedina
                                

                                for (q = 0; q < 5; q++)
                                {
                                    ver_giocata.importi[q] = 0; //setto a 0 tutti gli elementi del vettore degli importi della schedina
                                }

                                //salvo gli importi nel vettore degli importi della schedina
                                while (*tokl != '\n')
                                {
                                    tokl = strtok(NULL, " ,*");

                                    if (strcmp(tokl, "Estratto") == 0)
                                    {
                                        ver_giocata.importi[0] = imp_temp;
                                    }
                                    if (strcmp(tokl, "Ambo") == 0)
                                    {
                                        ver_giocata.importi[1] = imp_temp;
                                    }
                                    if (strcmp(tokl, "Terno") == 0)
                                    {
                                        ver_giocata.importi[2] = imp_temp;
                                    }
                                    if (strcmp(tokl, "Quaterna") == 0)
                                    {
                                        ver_giocata.importi[3] = imp_temp;
                                    }
                                    if (strcmp(tokl, "Cinquina") == 0)
                                    {
                                        ver_giocata.importi[4] = imp_temp;
                                    }

                                    imp_temp = strtof(tokl, &eptr); //converto la parte di stringa contenente l'importo puntato per la giocata in float 
                                }

                                //per ogni ruota giocata
                                for (i = 0; i < ver_giocata.num_ruote - 1; i++)
                                {
                                    //leggo le estrazioni 
                                    f_estr = fopen("/home/giuseppe/Scrivania/estrazione.txt", "a+");
                                    while (fgets(lline5, 100, f_estr) != NULL)
                                    {
                                        tokl = strtok(lline5, " ");
                                        tokl = strtok(NULL, " ");
                                        
                                        if (strncmp(tokl, "0 E", 3) != 0) //per saltare il "titolo di apertura"
                                        {
                                            strptime(tokl, "%d/%m/%Y-%H:%M", &estr_v); //salvo il timestamp dell'estrazione considerata
                                            strcpy(data_estr_vincente, tokl);
                                        }

                                        strcpy(ruota_v, strtok(NULL, " "));

                                       // se la ruota è "Tutte"
                                        if (Diff(estr_v, ver_giocata.timestamp_giocata_v) < 0 && Diff(estr_v, ver_giocata.timestamp_giocata_v) > -5 && strcmp(ver_giocata.ruote_v[i], "Tutte") == 0 && strcmp(ruota_v, "Estrazione") != 0) // la schedina è relativa a un'estrazione se la differenza tra gli orari della schedina e dell'estrazione è < 5 minuti
                                        {
                                            printf("%s\n", "Hai giocato tutte le ruote");

                                            j = 0;
                                            while (*tokl != '\n' && j < 5)
                                            {
                                                tokl = strtok(NULL, " ");
                                                vett_num_v[j] = strtol(tokl, &eptr, 10); //salvo i numeri dell'estrazione in vett_num_v
                                                printf("%d\n", vett_num_v[j]);
                                                j++;
                                            }

                                            z = 0;
                                            for (k = 0; k < 5; k++)
                                            {
                                                for (y = 0; y < 5; y++)
                                                {
                                                    if (vett_num_v[k] == ver_giocata.numeri_giocati_v[y])
                                                    {
                                                        z++;
       
                                                        ver_giocata.numeri_indovinati[z] = vett_num_v[k]; //salvo i numeri indovinati                                                        
                                                    }
                                            
                                                }
                                            }

                                            ni = z; //quanti numeri sono stati indovinati
                                            vittoria = 0;
                                            for (q = 0; q < 5; q++)
                                            {
                                                if (calcola_vincita(ver_giocata.dim_num - 1, 11, q, ver_giocata.importi[q], ni) > 0)
                                                {
                                                    vittoria++;
                                                }
                                            }

                                            if (ni > 0 && vittoria > 0) //stampo solo se è stato vinto effettivamente qualcosa
                                            {
                                                //preparo il buffer da mandare al client in cui ci saranno scritte le eventuali vincite

                                                strcat(superbuffer6, "*******************************\n");
                                                strcat(superbuffer6, "Estrazione del ");
                                                strcat(superbuffer6, data_estr_vincente);
                                                strcat(superbuffer6, "\n");
                                                strcat(superbuffer6, ruota_v);
                                                strcat(superbuffer6, " : ");

                                                for (q = 0; q < z; q++)
                                                {
                                                    sprintf(superbuffer6 + strlen(superbuffer6), "%d ", ver_giocata.numeri_indovinati[q + 1]);
                                                }
                                                strcat(superbuffer6, ">> ");

                                                for (q = 0; q < 5; q++)
                                                {
                                                    if (ver_giocata.importi[q] != 0 && (z == (q + 1) || z > (q + 1)))
                                                    {
                                                        quanti_num = ver_giocata.dim_num - 1;
                                                        strcat(superbuffer6, sched.puntate[q]);
                                                        strcat(superbuffer6, " ");
                                                        sprintf(superbuffer6 + strlen(superbuffer6), "%.2f EURO ", calcola_vincita(quanti_num, 11, q, ver_giocata.importi[q], ni));
                                                        riepilogo[q] += calcola_vincita(quanti_num, 11, q, ver_giocata.importi[q], ni); //mantengo aggiornato il vettore di riepilogo delle vincite
                                                    }
                                                }
                                                strcat(superbuffer6, "\n");
                                            }
                                        }


                                        //Giocate NON comprendenti la ruota "Tutte"
                                        if (Diff(estr_v, ver_giocata.timestamp_giocata_v) < 0 && Diff(estr_v, ver_giocata.timestamp_giocata_v) > -5 && strcmp(ver_giocata.ruote_v[i], ruota_v) == 0) // considero solo le corrispondenze tra le ruote giocate della schedina e le ruote dell'estrazione corrispondenti
                                        //il resto è come per "Tutte"
                                        {
                                            j = 0;
                                            while (*tokl != '\n' && j < 5)
                                            {
                                                tokl = strtok(NULL, " ");
                                                vett_num_v[j] = strtol(tokl, &eptr, 10);
                                                j++;
                                            }

                                            z = 0;
                                            
                                            for (k = 0; k < 5; k++)
                                            {
                                                for (y = 0; y < 5; y++)
                                                {
                                                    if (vett_num_v[k] == ver_giocata.numeri_giocati_v[y])
                                                    {
                                                        z++;
                                                        ver_giocata.numeri_indovinati[z] = vett_num_v[k];
                                                    }
                                                }
                                            }
                                            ni = z;

                                            vittoria = 0;
                                            for (q = 0; q < 5; q++)
                                            {
                                                if (calcola_vincita(ver_giocata.dim_num - 1, ver_giocata.num_ruote - 1, q, ver_giocata.importi[q], ni) > 0)
                                                {
                                                    vittoria++;
                                                }
                                            }
                                            if (ni > 0 && vittoria > 0)
                                            {
                                                
                                                strcat(superbuffer6, "*******************************\n");
                                                strcat(superbuffer6, "Estrazione del ");
                                                strcat(superbuffer6, data_estr_vincente);
                                                strcat(superbuffer6, "\n");
                                                strcat(superbuffer6, ruota_v);
                                                strcat(superbuffer6, " : ");

                                                for (q = 0; q < z; q++)
                                                {
                                                    sprintf(superbuffer6 + strlen(superbuffer6), "%d ", ver_giocata.numeri_indovinati[q + 1]);
                                                }

                                                strcat(superbuffer6, ">> ");

                                                for (q = 0; q < 5; q++)
                                                {
                                                    if (ver_giocata.importi[q] != 0 && (z == (q + 1) || z > (q + 1)))
                                                    {
                                                        quanti_num = ver_giocata.dim_num - 1;

                                                        strcat(superbuffer6, sched.puntate[q]);
                                                        strcat(superbuffer6, " ");

                                                        sprintf(superbuffer6 + strlen(superbuffer6), "%.2f EURO ", calcola_vincita(quanti_num, ver_giocata.num_ruote - 1, q, ver_giocata.importi[q], ni));
                                                        riepilogo[q] += calcola_vincita(quanti_num, ver_giocata.num_ruote - 1, q, ver_giocata.importi[q], ni);
                                                    }
                                                }
                                                
                                                strcat(superbuffer6, "\n");
                                            }
                                        }
                                    }
                                    fclose(f_estr);
                                }
                            }
                            fclose(f5);

                            //Stampo il riepilogo delle vincite
                            strcat(superbuffer6, "\nRiepilogo vincite:\n");
                            for (q = 0; q < 5; q++)
                            {
                                sprintf(superbuffer6 + strlen(superbuffer6), "Vincite su %s %.2f EURO\n", sched.puntate[q], riepilogo[q]);
                                riepilogo[q] = 0;
                            }

                            len_msg_signup = strlen(superbuffer6) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *)superbuffer6, len_msg_signup, 0);
                            strcpy(superbuffer6, "\n");
                        }
                        else
                        {
                            printf("ID non valido");
                            strcpy(msg_signup, "ERROR_ID: Effettuare il LOGIN prima di poter cominciare a giocare\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);
                        }
                    }

                    /* esci */
                    if (strncmp(buffer, "!esci", 5) == 0)
                    {
                        // Attendo dimensione dell'ID
                        ret = recv(new_sd, (void *)&lmsg, sizeof(uint16_t), 0);
                        // Rinconverto in formato host
                        len = ntohs(lmsg);
                        // ricevo l'ID
                        ret = recv(new_sd, (void *)buffer_ID, len, 0);

                        // ID CORRETTO:
                        if (strcmp(buffer_ID, id_session) == 0)
                        {
                            printf("ID valido\n");
                            fflush(stdout);

                            strcpy(msg_signup, "ID valido\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);

                            strcpy(sbuffer7, "Disconnessione avvenuta\n");

                            //Mando il messaggio di disconnessione al client
                            len_msg_signup = strlen(sbuffer7) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *)sbuffer7, len_msg_signup, 0);

                            close(new_sd); //chiudo il socket della comunicazione
                            exit(0);
                        }
                        else
                        {
                            //ID non valido
                            strcpy(msg_signup, "ERROR_ID: Effettuare il LOGIN prima di poter cominciare a giocare\n");
                            len_msg_signup = strlen(msg_signup) + 1;
                            lmsg_signup = htons(len_msg_signup);
                            ret = send(new_sd, (void *)&lmsg_signup, sizeof(uint16_t), 0);
                            ret = send(new_sd, (void *)msg_signup, len_msg_signup, 0);
                        }
                    }
                }
            }
            else
            {
                printf("Ciao sono il padre");
                fflush(stdout);
                close(new_sd);
            }
        }
    }
}

