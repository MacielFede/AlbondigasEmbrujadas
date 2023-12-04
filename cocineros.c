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
#include "restaurante.h"


int main()
{
    char nombre[] = "Cocinero X";
    bool cocinerosDespiertos = true;
    // Crea o abre el objeto de memoria compartida
    int fileDescriptor = shm_open("/fileDescriptor", O_RDWR | O_CREAT, 0666);
    if (fileDescriptor == -1 && !errno == EEXIST) // Me fijo si hubo algún error, si ya existía el objeto dejo pasar
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
        limpiarSemaforos();
        sem_unlink("/semIniciarMemoria");
        abrirRestaurante(memComp);
    }
    else {
        while (!memComp->inicializado) {
            printf("Esperando que abra el local.\n");
            sleep(2);
        }
    }

    // Crear semáforos
    // Protege a platos preparados en el día
    sem_t* semPlatosPreparados = sem_open("/semPlatosPreparados", O_CREAT, 0666, 1);
    // Protege a el acceso a la mesada
    sem_t* semMesada = sem_open("/semMesada", O_CREAT, 0666, 1);

    int pid = fork();
    if (pid == 0)
    {
        strcpy(nombre, "Cocinero Carlos");
        pid = fork();
        if (pid > 0)
        {
            strcpy(nombre, "Cocinero Ciro");
        }
        if (pid < 0)
        {
            perror("Error en el fork.\n");
            exit(-1);
        }
    }
    else if (pid > 0)
    {
        strcpy(nombre, "Cocinera Catalina");
    }
    if (pid < 0)
    {
        perror("Error en el fork.\n");
        exit(-1);
    }


    do
    {
        sem_wait(semPlatosPreparados); // Tengo que quedarme con este semáforo hasta que termine
        cocinerosDespiertos = memComp->platosPreparadosEnElDia < MAX_PLATOS_X_DIA;
        sem_wait(semMesada);
        if (cocinerosDespiertos && memComp->platosEnMesada < 27) // Si no terminamos el dia y la mesada no esta completa
        {
            // Antes de cocinar otro plato no necesito chequear si alguien mas cocino otro plato porque al quedarme con el semáforo bloqueo a los demás cocineros
            printf("%s: Hay %d platos en la mesada, se cocina uno más.\n", nombre, memComp->platosEnMesada);
            sleep(TIEMPO_DE_COCCION);
            memComp->platosEnMesada++;
            memComp->platosPreparadosEnElDia++;
        }
        sem_post(semMesada);
        sem_post(semPlatosPreparados);
    } while (cocinerosDespiertos);
    printf("%s: Se cerró la cocina. Se prepararon: %d platos.\n", nombre, memComp->platosPreparadosEnElDia);

    // Desprendo la memoria compartida
    munmap(memComp, sizeof(MemoriaCompartida));

    return 0;
}