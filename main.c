#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
    // pipe, 2 processi, bisogna fare in modo che si comunichino
    // KERNEL si prende cura, dentro memoria RAM fa un file di testo, una pipe anonima
    // Un processo può scrivere e anche leggere,
    // Problema: Serve file descritor (riferimenti per capire dove si trova il file nella RAM)
    // Race condition, comunicazione simplex, monodirezionale+
    // Se c'è processo che scrive, l'altro deve solo leggere, questa concorrenza viene gestita dal Kernel
    // Per scrittura aspetta per scrivere, per lettura, se c'è qualcosa da leggere, quindi mette in wait il proc
    // wait finchè non trova qualcosa dentro il file
    // Dentro pipe cè un buffer dati, max buffer size è 64kBit
    // Carattere invertitore etc
    // Per creare file descriptor, sono array di due elementi; int fd[0]
    // questo ha fd[0], riferimento per leggere
    // fd[1], per scrivere
    // 2 funz, una write, una read e hanno dei ritorni

    int file_descriptor[2];

    if (pipe(file_descriptor) == -1) { // questo va a creare la pipe, returna un -1 se non funziona
        perror("Errore pipe"); // stessa cosa di fprintf("stderr, "Errore pipe")
        exit(-1); // se errore esce direttamente con -1
    }

    pid_t ritorno_fork = fork(); // per creare il fork
    if (ritorno_fork == -1) { // Errore nella creazione del fork
        perror("Errore fork");
        exit(-2); // se n negativo, errore, se -1 errore pipe, se -2 errore fork
    }

    if (!ritorno_fork) { // per trovare il figlio se è == 0
        // PID 1 è systemD
        // Qua dentro siamo dentro al processo figlio

        close(file_descriptor[0]); // se ho deciso che uso fd[0] per un processo, fd[1] non serve (qualcosa che bisogna chiudere)

        sleep(3); // test del wait automatico del Kernel

        char msg[] = "Sutidiate per favore anche TPSIT"; // messaggio da mandare
        if (write(file_descriptor[1], msg, sizeof(msg) /*-1*/ ) == -1) { // Come si manda il messaggio, 3 parametri
            perror("Errore write pipe");
        }
        // diff fra size of e strlen, sizeof mette anche \0, il terminatore, con strlen no (toglie terminatore)
        // strlen manda la lunghezza, sizeof tutto il buffer

        close(file_descriptor[1]); // chiudo anche questo per ottimizzare la memoria

        exit(0);
    }

    // Processo padre
    close(file_descriptor[1]);

    printf("Aspetto il messaggio...\n"); // si aspettano 3 secondi e arriva giusto dato che il kernal li sincronizza
    char buffer[100]; // array 100 elementi dato che non sappiamo cosa ci arriva

    // lunghezza di byte
    ssize_t byte_number = read(file_descriptor[0], buffer, sizeof(buffer)); // parametri = al write, ma invertito, cioè legge

    for (int i = 0; i < byte_number; i++) {
        printf("%c", buffer[i]); // stampa uno alla volta senza terminatore, dato che c'è msg - 1
    }

    printf("%p\n", byte_number);
    printf("[PADRE] %s", buffer); // dovrebbe stampare fino al terminatore
    // printf stampa finchè non trova il terminatore (\0)
    // read ritorna il numero di byte letti

    close(file_descriptor[0]);

    return 0;
}