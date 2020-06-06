
#include <iostream>
#include <fstream>
#include <time.h>
#include "./HashMapConcurrente.hpp"
#include "./CargarArchivos.hpp"


int main(int argc, char **argv) {
    struct timespec start, end;

    HashMapConcurrente* hashMap = new HashMapConcurrente;
    const int CANT_ARCHIVOS = 200;

    std::vector<std::string> vectorDeFilepaths;
    
    for ( int i = 0; i < CANT_ARCHIVOS; i++){
        std::ifstream  src("data/test-1", std::ios::binary);
        std::string dstName = "data/exp-" + std::to_string(i);
        std::ofstream  dst(dstName,   std::ios::binary);
        dst << src.rdbuf();

        vectorDeFilepaths.push_back(dstName);
    }

    // columnas para el csv
    std::cout << "cantThreadsCarga,tiempoCarga" << std::endl;

for(int cant_Threads = 1; cant_Threads <= 26; cant_Threads++){
        const int REPETICIONES = 500;
        double tiempo_promediado = 0;

        //repeticiones para promediar resultado
        for(int repeticion = 0; repeticion < REPETICIONES; repeticion++){
        
        // inicia reloj
        clock_gettime(CLOCK_MONOTONIC, &start);
        std::ios_base::sync_with_stdio(false);

        // corre funcion
        cargarMultiplesArchivos(*hashMap,cant_Threads,vectorDeFilepaths);
        
        // detiene reloj
        clock_gettime(CLOCK_MONOTONIC, &end); 


        // calcula tiempo transcurrido
        double tiempo_transcurrido;
        tiempo_transcurrido = (end.tv_sec - start.tv_sec) * 1e9; 
        tiempo_transcurrido = (tiempo_transcurrido + (end.tv_nsec - start.tv_nsec)) * 1e-9; 

        tiempo_promediado += tiempo_transcurrido;
        
        }

        tiempo_promediado /= REPETICIONES;

        std::cout << cant_Threads << ",";
        std::cout << std::fixed 
            << tiempo_promediado; 
        std::cout << std::endl;
    }

    int cantidad_de_palabras = cargarArchivo(*hashMap, "data/corpus");

    // columnas para el csv
    std::cout << "cantThreads,tiempo" << std::endl;

    for(int cant_Threads = 1; cant_Threads <= 26; cant_Threads++){
        const int REPETICIONES = 500;
        double tiempo_promediado = 0;

        //repeticiones para promediar resultado
        for(int repeticion = 0; repeticion < REPETICIONES; repeticion++){
        
        // inicia reloj
        clock_gettime(CLOCK_MONOTONIC, &start);
        std::ios_base::sync_with_stdio(false);

        // corre funcion
        hashMap->maximoParalelo(cant_Threads);
        
        // detiene reloj
        clock_gettime(CLOCK_MONOTONIC, &end); 


        // calcula tiempo transcurrido
        double tiempo_transcurrido;
        tiempo_transcurrido = (end.tv_sec - start.tv_sec) * 1e9; 
        tiempo_transcurrido = (tiempo_transcurrido + (end.tv_nsec - start.tv_nsec)) * 1e-9; 

        tiempo_promediado += tiempo_transcurrido;
        
        }

        tiempo_promediado /= REPETICIONES;

        std::cout << cant_Threads << ",";
        std::cout << std::fixed 
            << tiempo_promediado; 
        std::cout << std::endl;
    }

	return 0;
}