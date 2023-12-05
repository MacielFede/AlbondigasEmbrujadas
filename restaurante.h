#ifndef RESTAURANTE_H
#define RESTAURANTE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <sys/mman.h>

#define MAX_PLATOS_X_DIA 180
#define LLENANDO_HELADERA 4
#define TIEMPO_DE_COCCION 2

typedef struct
{
    bool inicializado;
    int flanesEnHeladera;
    int platosPreparadosEnElDia;
    int platosEnMesada;
    int procesoIntentoInicializar;
} MemoriaCompartida;

void abrirRestaurante(MemoriaCompartida* sd);

void limpiarSemaforos();

void generarPedido(int* tipoComida, int* pedidoCantidad);

#endif