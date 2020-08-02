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

std::mutex mutexes[HashMapConcurrente::cantLetras]; // se inicializa en estado unlocked.

void HashMapConcurrente::incrementar(std::string clave) {
    /* Se debe garantizar que solo haya contencion en caso de colision de hash.
    Supongamos que tenemos dos threads. Uno ejecuta incrementar en tabla[i] y otro en tabla[j]. Entre ellos no tienen por qué restringirse.
    Pero si ambos ejecutan incrementar en tabla[i], necesitamos permitirle la ejecución a uno y hacer esperar al otro. Para esto, tenemos un arreglo de 26 mutex.*/
    unsigned int indice = HashMapConcurrente::hashIndex(clave);
    ListaAtomica<hashMapPair> *lista = tabla[indice];
    mutexes[indice].lock(); // Comienzo de la sección crítica:
    bool encontrado = false;

    for (auto it = tabla[indice]->crearIt(); it.haySiguiente(); it.avanzar()) {
        if(it.siguiente().first ==   clave) {
            it.siguiente().second++;
            encontrado = true;
            break;
        }
    }

    if(!encontrado) {
        (*lista).insertar(std::make_pair(clave,1));
        vectorDeClaves->vectorClaves.push_back(clave);
        vectorDeClaves->cantClaves++;

    }
    mutexes[indice].unlock(); // Fin de la sección crítica.
}

std::vector<std::string> HashMapConcurrente::claves() {
    // Completar (Ejercicio 2)
    std::vector<std::string> res;

    for (int indice = 0; indice < HashMapConcurrente::cantLetras; indice++){
        for (auto it = tabla[indice]->crearIt(); it.haySiguiente(); it.avanzar()) {
            res.push_back(it.siguiente().first);
        }
    }

    return res;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    auto it = tabla[HashMapConcurrente::hashIndex(clave)]->crearIt();
    while(it.haySiguiente() && it.siguiente().first != clave) {
        it.avanzar();
    }
    return it.haySiguiente() ? it.siguiente().second : 0;
}

hashMapPair HashMapConcurrente::maximo() {
    hashMapPair *max = new hashMapPair();
    max->second = 0;

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {

        //No quiero que haya un thread incrementando en una lista al mismo tiempo que la leo   
        mutexes[index].lock();

        for (
            auto it = tabla[index]->crearIt();
            it.haySiguiente();
            it.avanzar()
        ) {
            if (it.siguiente().second > max->second) {
                max->first = it.siguiente().first;
                max->second = it.siguiente().second;
            }
        }
    }
    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
    mutexes[index].unlock();
    }

    return *max;
}

struct thread_args {
    HashMapConcurrente* dicc;
    hashMapPair* max;
    int filas[HashMapConcurrente::cantLetras];
};

// Recorre las filas sin ocupar y las marca atómicamente cuando empieza
void *funcion_thread2(void *arg) {
    thread_args *args_struct = (thread_args *) arg;

    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        mutexes[index].lock();
        
        // Si filas[index] es 1, ya fue tomada y avanzo. Sino la ocupo
        if(args_struct->filas[index]==0){
            for (auto it = args_struct->dicc->tabla[index]->crearIt(); it.haySiguiente(); it.avanzar()) {
                
                if (it.siguiente().second > args_struct->max->second) {
                    args_struct->max->first = it.siguiente().first;
                    args_struct->max->second = it.siguiente().second;
                }
            }
            args_struct->filas[index]==1;
        }
        mutexes[index].unlock();
    }
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cantThreads) {
    // Completar (Ejercicio 3)
    pthread_t tid[cantThreads];

    // declarar thread_args
    int filas[HashMapConcurrente::cantLetras] = {0};

    hashMapPair* max = new hashMapPair();
    max->second = 0;

    thread_args args = {this, max, *filas};

    for (int i = 0; i < cantThreads; i++){
        pthread_create(&tid[i], NULL, funcion_thread2, &args);
    }

    for (int i = 0; i < cantThreads; i++){
        pthread_join(tid[i], NULL);
    }

    return *max;
}

#endif
