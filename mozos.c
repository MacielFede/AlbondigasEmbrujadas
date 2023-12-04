#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include "restaurante.h"


int main()
{
    int pedidoCantidad = 0, pedidoComida = 0, cantidadAux = 0, platosRestantesPorEntregar = 0, iteradorAlbondigas = 0;
    char nombre[] = "Mozo X", log[] = "Esto es para escribir cosas";
    bool terminaTurno = false;

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

    // Creación de semáforos
    // Protege la mesada
    sem_t* semMesada = sem_open("/semMesada", O_CREAT, 0666, 1);
    // Protege la heladera
    sem_t* semHeladera = sem_open("/semHeladera", O_CREAT, 0666, 1);
    // Protege la memoria compartida
    sem_t* semPlatosPreparados = sem_open("/semPlatosPreparados", O_CREAT, 0666, 1);
    // Protege la mesada (único para mozos)
    sem_t* semMesadaMozo = sem_open("/semMesadaMozo", O_CREAT, 0666, 1);
    // Despierta al repostero
    sem_t* semDespertarRepostero = sem_open("/semDespertarRepostero", O_CREAT, 0666, 0);

    int pid = fork();
    if (pid == 0)
    {
        strcpy(nombre, "Mozo Martin");
        pid = fork();
        if (pid > 0)
        {
            strcpy(nombre, "Mozo Manuel");
        }
        if (pid < 0)
        {
            perror("Error en el fork.\n");
            exit(-1);
        }
    }
    else if (pid > 0)
    {
        strcpy(nombre, "Moza Margarita");
        pid = fork();
        if (pid > 0)
        {
            strcpy(nombre, "Moza Mayra");
            pid = fork();
            if (pid > 0)
            {
                strcpy(nombre, "Mozo Michael");
            }
            if (pid < 0)
            {
                perror("Error en el fork.\n");
                exit(-1);
            }
        }
        if (pid < 0)
        {
            perror("Error en el fork.\n");
            exit(-1);
        }
    }
    if (pid < 0)
    {
        perror("Error en el fork.\n");
        exit(-1);
    }

    do
    {
        sleep(ESPERANDO_PEDIDO);
        generarPedido(&pedidoComida, &pedidoCantidad);
        cantidadAux = pedidoCantidad;

        if (pedidoComida == 1) { // Me piden flanes
            sem_wait(semHeladera);
            printf("%s: Se recibió pedido de %d flan/es\n", nombre, pedidoCantidad);

            if (pedidoCantidad <= memComp->flanesEnHeladera)
            {
                memComp->flanesEnHeladera -= pedidoCantidad;
                printf("%s: Se entregó el pedido de %d flan/es\n", nombre, pedidoCantidad);
            }
            else
            {
                pedidoCantidad -= memComp->flanesEnHeladera;
                memComp->flanesEnHeladera = 0;
                printf("%s: Me quede sin flanes, despierto a Roberto\n", nombre, pedidoCantidad);
                // Despierto al repostero para que haga los flanes y espero que me avise que termino
                sem_post(semDespertarRepostero);
                sleep(LLENANDO_HELADERA);
                sem_wait(semDespertarRepostero);

                memComp->flanesEnHeladera -= pedidoCantidad;
                printf("%s: Se entregó el pedido de %d flan/es\n", nombre, pedidoCantidad);
            }
            printf("%s: Quedan %d flan/es en la heladera\n", nombre, memComp->flanesEnHeladera);
            sem_post(semHeladera);
        }
        else // Me piden albóndigas
        {
            sem_wait(semMesadaMozo);
            sem_wait(semMesada);
            // Me fijo si voy a poder entregar todos los platos de mi pedido o menos
            sem_wait(semPlatosPreparados);
            platosRestantesPorEntregar = MAX_PLATOS_X_DIA - memComp->platosPreparadosEnElDia + memComp->platosEnMesada;
            sem_post(semPlatosPreparados);

            if (pedidoCantidad <= platosRestantesPorEntregar) { // Puedo entregar todo el pedido
                printf("%s: Se recibió pedido de %d albóndiga/s\n", nombre, cantidadAux);
                while (pedidoCantidad > 0)
                {
                    if (memComp->platosEnMesada < pedidoCantidad) // No me alcanzan los platos de la mesada
                    {
                        pedidoCantidad -= memComp->platosEnMesada;
                        memComp->platosEnMesada = 0;
                        // Les doy tiempo a los cocineros a cocinar mas platos
                        sem_post(semMesada)
                            sleep(TIEMPO_DE_COCCION);
                        sem_wait(semMesada);
                    }
                    else // Me alcanzan los platos de la mesada para entregar el pedido
                    {
                        memComp->platosEnMesada -= pedidoCantidad;
                        pedidoCantidad = 0;
                    }
                }
                printf("%s: Se entregaron %d albóndigas\n", nombre, cantidadAux);
                if (platosRestantesPorEntregar - pedidoCantidad <= 0) // Me fijo si quedan platos por entregar o si ya terminamos
                    terminaTurno = true;
                else
                    printf("%s: Quedan %d platos de albóndigas en la mesada.\n", nombre, memComp->platosEnMesada);
            }
            else // No puedo entregar todo el pedido
            {
                printf("%s: Se recibió pedido de %d albóndigas, pero no se pudo entregar porque faltan platos en la mesada.\n", nombre, cantidadAux);
            }
            sem_post(semMesada);
            sem_post(semMesadaMozo);
        }
    } while (!terminaTurno);
    printf("%s: Cierra el local, ta' manana.\n", nombre);

    // Desprendo la memoria compartida
    munmap(memComp, sizeof(MemoriaCompartida));

    return 0;
}