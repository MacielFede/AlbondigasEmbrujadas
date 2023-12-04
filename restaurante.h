#ifndef RESTAURANTE_H
#define RESTAURANTE_H

#include<stdbool.h>
#include <stdio.h>
#include <time.h>

#define MAX_PLATOS_X_DIA 180
#define LLENANDO_HELADERA 2
#define TIEMPO_DE_COCCION 2

typedef struct
{
    bool inicializado;
    int flanesEnHeladera;
    int platosPreparadosEnElDia;
    int platosEnMesada;
} MemoriaCompartida;

void abrirRestaurante(MemoriaCompartida* sd);

void limpiarSemaforos();

void generarPedido(int* tipoComida, int* pedidoCantidad);

#endif