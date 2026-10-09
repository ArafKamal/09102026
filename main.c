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

        char msg[] = "Sutidiate per favore anche TPSIT"; // messaggio da mandare
        write(file_descriptor[1], msg, sizeof(msg)); // Come si manda il messaggio, 3 parametri
        // diff fra size of e strlen, sizeof mette anche \0, il terminatore, con strlen no (toglie terminatore)
        // strlen manda la lunghezza, sizeof tutto il buffer

        exit(0);
    }

    // Processo padre
    char buffer[100]; // array 100 elementi dato che non sappiamo cosa ci arriva

    read(file_descriptor[0], buffer, sizeof(buffer)); // parametri = al write, ma invertito, cioè legge
    

    return 0;
}