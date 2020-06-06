#ifndef HMC_HPP
#define HMC_HPP

#include <atomic>
#include <string>
#include <vector>
#include <mutex>

#include "ListaAtomica.hpp"

typedef std::pair<std::string, unsigned int> hashMapPair;

class HashMapConcurrente {
 public:
    static const unsigned int cantLetras = 26;

    HashMapConcurrente();

    void incrementar(std::string clave);
    std::vector<std::string> claves();
    unsigned int valor(std::string clave);

    hashMapPair maximo();
    hashMapPair maximoParalelo(unsigned int cantThreads);

   ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];
 private:

    static unsigned int hashIndex(std::string clave);

    
    struct claveStruct{
      std::vector<std::string> vectorClaves;
      unsigned int cantClaves;

      bool operator==(const claveStruct & nuevo) const {
         return(this->cantClaves == nuevo.cantClaves);
      }
    };

    claveStruct* vectorDeClaves = nullptr; 
   
};

#endif  /* HMC_HPP */
