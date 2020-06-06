#ifndef CHM_CPP
#define CHM_CPP

#include <iostream>
#include <fstream>
#include <pthread.h>

#include "HashMapConcurrente.hpp"

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
        claveStruct* punt = new claveStruct;
        vectorDeClaves = punt;
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

std::mutex mutexes[HashMapConcurrente::cantLetras]; // se inicializa en estado unlocked.
// deberia ir en el .hpp pero me tiraba error de multiples declaraciones y no pude arreglarlo. Si alguien puede, joya

void HashMapConcurrente::incrementar(std::string clave) {
    /* Se debe garantizar que solo haya contencion en caso de colision de hash.
    Supongamos que tenemos dos threads. Uno ejecuta incrementar en tabla[i] y otro en tabla[j]. Entre ellos no tienen por qué restringirse.
    Pero si ambos ejecutan incrementar en tabla[i], necesitamos permitirle la ejecución a uno y hacer esperar al otro. Para esto, tenemos un arreglo de 26 mutex.*/
    static unsigned int indice = HashMapConcurrente::hashIndex(clave);
    ListaAtomica<hashMapPair> *lista = tabla[indice];
    mutexes[indice].lock(); // Comienzo de la sección crítica:
    bool encontrado = false;
    ListaAtomica<hashMapPair>::Iterador it = (*lista).crearIt(); // capaz podiamos meter auto it.
    while(it.haySiguiente() && !encontrado) {
        if(it.siguiente().first == clave) {
            it.siguiente().second++;
            encontrado = true;
        }
        it.avanzar();
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
    std::vector<std::string> res = vectorDeClaves->vectorClaves;
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
            //Tal vez habria que hacer algo por si se corre mas de un maximo() a la vez pero no dice nada el enunciado
        }
    }
    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
    mutexes[index].unlock();
    }
    //Supongo que tengo tres listas, el máximo de la primera es 3, el de la segunda es 5, y el de la tercera es 7, el global es 7
    //Cuando termino de recorrer la primera fila tengo que el max es 3, mientras recorro la segunda se incrementa el maximo de la primera
    // a 10, lo que lo hace el maximo global, sigo recorriendo la segunda, se incrementa el maximo de esta a 8, no es el maximo global
    // pero sera el return de la funcion.

    //Para evitar esto lockeo las listas que visito hasta que termine de ejecutar en este caso el return sera 7, solo devolvera algun maximo global
    // de algun momento y no local

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
        /*if (args_struct->filas[index].atomic_compare_exchange_strong(1,1)){

            for (auto it = args_struct->dicc->tabla[index]->crearIt(); it.haySiguiente(); it.avanzar()) {
                if (it.siguiente().second > args_struct->max->second) {
                    args_struct->max->first = it.siguiente().first;
                    args_struct->max->second = it.siguiente().second;
                }
            }
        }*/
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
    //return std::make_pair("",0); // BORRAR ESTA LINEA, era para que compilen bien las cosas, perdon!
}

#endif
