#ifndef CHM_CPP
#define CHM_CPP

#include <vector>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <atomic>

#include "CargarArchivos.hpp"

int cargarArchivo(
    HashMapConcurrente &hashMap,
    std::string filePath
) {
    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }
    while (file >> palabraActual) {
        // Completar (Ejercicio 4)
        hashMap.incrementar(palabraActual);
        cant++;
    }
    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return -1;
    }
    file.close();
    return cant;
}

struct thread_args {
    HashMapConcurrente &hashMap;
    std::vector<std::string> filePaths;
    std::atomic<int> &contador;
};

void *funcion_thread(void *arg) {
    thread_args *args_struct = (thread_args *) arg;

    int indice = args_struct->contador.fetch_add(1);

    cargarArchivo(args_struct->hashMap, args_struct->filePaths[indice]);
}

void cargarMultiplesArchivos(
    HashMapConcurrente &hashMap,
    unsigned int cantThreads,
    std::vector<std::string> filePaths
) {
    // Completar (Ejercicio 4)
    pthread_t tid[cantThreads];
    
    std::atomic<int> contador(0);
    
    thread_args args = {hashMap, filePaths, contador};
    

    for (int i = 0; i < cantThreads; i++){
        pthread_create(&tid[i], NULL, funcion_thread, &args);
    }

    for (int i = 0; i < cantThreads; i++){
        pthread_join(tid[i], NULL);
    }
}

#endif
