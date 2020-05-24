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
    /* El enunciado dice "Se debe garantizar que solo haya contencion en caso de colision de hash: es decir, si dos o mas
    threads intentan incrementar concurrentemente claves que no colisionan, deben poder hacerlo sin inconvenientes"
    PONELE que tenemos dos threas. Uno ejecuta incrementar en tabla[i] y otro en tabla[j]. Entre ellos no tienen por qué restringirse.
    Pero si ambos ejecutan incrementar en tabla[i], necesitamos algo que deje pasar a uno y haga esperar al otro. Mepa que necesitamos
    eso para cada una de las 26 posiciones de la tabla... Un arreglo de 26 mutex?*/
    static unsigned int indice = HashMapConcurrente::hashIndex(clave);
    ListaAtomica<hashMapPair> *lista = tabla[indice];
    bool encontrado = false;
    // TODO: aca iria algo que maneje bien las colisiones. mutex[indice]? hacerle .wait()?
    //       tambien algo para manejar la concurrencia con maximo?
    ListaAtomica<hashMapPair>::Iterador it = (*lista).crearIt(); // capaz podiamos meter auto it.
    while(it.haySiguiente() && !encontrado) {
        if(it.siguiente().first == clave) {
            it.siguiente().second++;
            encontrado = true;
        }
        it.avanzar();
    }
    if(!encontrado) {
        (*lista).insertar(make_pair(clave,1));
    }
    // TODO: hacer .signal()?
}

std::vector<std::string> HashMapConcurrente::claves() {
    // Completar (Ejercicio 2)
    vector<string> res = {};
    // Si recorremos todos los tabla[i] y cada uno de los nodos de esas listas, nos puede re cagar que entre medio ejecuten incrementar().
    // Si esperamos a que no haya NINGUN incrementar() ejecutandose, podria bloquearse para siempre esperando a que todos los incrementar() terminen asi que un semaforo no parece lo mejor.
    // Y si usamos un mutex que valga 0 mientras claves() se ejecuta, estamos bloqueando a incrementar(). Claves() debe ser no bloqueante.
    // Capaz la mejor solucion es tener armadita una struct con el vector de claves y el tamaño de este vector, asi lo actualizamos atomicamente cada vez que alguien incrementa.

/* Ponele que ya tenemos una struct global asi:
struct vectorDeClaves{
    vector<string> claves = {};
    unsigned int contador = 0;
};
Y en cada llamada a incrementar(), si se crea una nueva clave, agregarla a esta estructura y hacer contador++
Entonces acá en claves() solo haria falta ver si contador sigue igual que antes (o sea que nadie se metio en el medio) y en caso positivo devolver vectorDeClaves.claves*/

    return res;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    // es recorrer la tabla[indice] con un iterador mientras it.haySiguiente() && it.siguiente().first != clave... cuando llegas
    // a esa condicion, te fijas si devolves el valor guardado o cero porque no encontraste la clave en el diccionario.
    // asi todo bien, es no bloqueante porque no tiene semaforos y es wait free porque no espera por nadie, se ejecuta y punto.
    // tiene condiciones de carrera? maybe...

    // TODO: capaz hay que ver mutex[indice] y hacerle .wait()
    auto it = tabla[HashMapConcurrente::hashIndex(clave)]->crearIt();
    while(it.haySiguiente() && it.siguiente().first != clave) {
        it.avanzar();
    }
    return it.haySiguiente() ? it.siguiente().second : 0; // si it.haySiguiente() NO es null, entonces el ciclo terminó porque se cumplió it.siguiente().first == clave y entonces devolvemos su valor.
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
