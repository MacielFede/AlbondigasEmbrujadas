#include "restaurante.h"

void abrirRestaurante(MemoriaCompartida* sd)
{
    sd->platosPreparadosEnElDia = 0;
    sd->flanesEnHeladera = 25;
    sd->platosEnMesada = 0;
    sd->inicializado = true;
}

void limpiarSemaforos()
{
    sem_unlink("/semPlatosPreparados");
    sem_unlink("/semMesada");
    sem_unlink("/semMesadaMozo");
    sem_unlink("/semDespertarRepostero");
    sem_unlink("/semHeladera");
}

void generarPedido(int* tipoComida, int* pedidoCantidad)
{
    srand(time(NULL));
    *tipoComida = rand() % 2;
    switch (*tipoComida)
    {
    case 0:
        *pedidoCantidad = ((rand() % 4) + 1);
        break;
    case 1:
        *pedidoCantidad = ((rand() % 6) + 1);
        break;
    default:
        break;
    };
}