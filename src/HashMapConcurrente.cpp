#ifndef CHM_CPP
#define CHM_CPP

#include <iostream>
#include <fstream>
#include <pthread.h>

#include "HashMapConcurrente.hpp"

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    // Completar (Ejercicio 2)
}

std::vector<std::string> HashMapConcurrente::claves() {
    // Completar (Ejercicio 2)
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    // Completar (Ejercicio 2)
}

hashMapPair HashMapConcurrente::maximo() {
    hashMapPair *max = new hashMapPair();
    max->second = 0;

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (
            auto it = tabla[index]->crearIt();
            it.haySiguiente();
            it.avanzar()
        ) {
            // version original
            if (it.siguiente().second > max->second) {
                max->first = it.siguiente().first;
                max->second = it.siguiente().second;
            }
            /*
            // ejercicio 3a
            // hashMapPair *possibleNewMax = new hashMapPair;
            // possibleNewMax->first = it.siguiente().first;
            // possibleNewMax->second = it.siguiente().second;
            
            // // habría que ver si se puede hacer todo el if atómico
            // if (possibleNewMax->second > max->second){
            //     max = possibleNewMax;
            // }
            */
        }
    }

    return *max;
}
/*
struct thread_args {
    hashMapPair &max;
    int (&filas)[HashMapConcurrente::cantLetras];
};

// Recorre las filas sin ocupar y las marca atómicamente cuando empieza
void *funcion_thread(void *arg) {
    thread_args *args_struct = (thread_args *) arg;

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        // Si filas[index] es 1, ya fue tomada y avanzo. Sino la ocupo
        if (args_struct->filas[index].atomic_compare_exchange_strong(1,1)) break;
        for (
            auto it = tabla[index]->crearIt();
            it.haySiguiente();
            it.avanzar()
        ) {
            if (it.siguiente().second > args_struct->max->second) {
                args_struct->max->first = it.siguiente().first;
                args_struct->max->second = it.siguiente().second;
            }
        }
    }
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cantThreads) {
    // Completar (Ejercicio 3)
    pthread_t tid[cantThreads];

    // declarar thread_args
    int filas[HashMapConcurrente::cantLetras] = {0};

    hashMapPair *max = new hashMapPair();
    max->second = 0;

    thread_args args = {*max, filas};

    for (int i = 0; i < cantThreads; i++){
        pthread_create(&tid[i], NULL, funcion_thread, &args);
    }

    for (int i = 0; i < cantThreads; i++){
        pthread_join(tid[i], NULL);
    }

    return *max;
}
*/

#endif
