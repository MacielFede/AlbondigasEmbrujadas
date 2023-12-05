#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <errno.h>
#include "restaurante.h"

int main()
{
    // Crea o abre el objeto de memoria compartida
    int fileDescriptor = shm_open("/fileDescriptor", O_RDWR | O_CREAT, 0666);
    if (fileDescriptor == -1 && errno != EEXIST) // Me fijo si hubo algún error, si ya existía el objeto dejo pasar
    {
        perror("shm_open");
    }

    if (ftruncate(fileDescriptor, sizeof(MemoriaCompartida)) == -1)
    {
        perror("ftruncate");
    }

    // Mapeo la memoria compartida
    MemoriaCompartida* memComp = (MemoriaCompartida*)mmap(NULL, sizeof(MemoriaCompartida), PROT_READ | PROT_WRITE, MAP_SHARED, fileDescriptor, 0);
    if (memComp == MAP_FAILED)
    {
        perror("mmap");
    }

    // Protege a la variable de para que solo 1 proceso inicialice la memoria
    sem_t* semIniciarMemoria = sem_open("/semIniciarMemoria", O_CREAT | O_EXCL, 0666, 1);

    if (semIniciarMemoria != SEM_FAILED) {
        printf("Abriendo el local.\n");
        limpiarSemaforos();
        abrirRestaurante(memComp);
    }
    else {
        while (!memComp->inicializado) {
            printf("Esperando que abra el local.\n");
            sleep(2);
        }
        memComp->procesoIntentoInicializar++;
        if (memComp->procesoIntentoInicializar >= 3) {
            sem_unlink("/semIniciarMemoria");
        }
        printf("Iniciando la jornada.\n");
    }

    // Creación de semáforos
    // Despierta al repostero
    sem_t* semDespertarRepostero = sem_open("/semDespertarRepostero", O_CREAT, 0666, 0);

    do
    {
        printf("Roberto repostero: Esperando que se vacié la heladera.\n");
        // Espero que un mozo me llame para llenar la heladera
        sem_wait(semDespertarRepostero);
        // Lleno la heladera
        printf("Roberto repostero: Estoy llenando la heladera de flanes.\n");
        memComp->flanesEnHeladera = 25;
        // Libero al mozo que me despertó
        printf("Roberto repostero: Llene la heladera de flanes.\n\n");
        sem_post(semDespertarRepostero);
        sleep(5);
    } while (true);

    return 0;
}