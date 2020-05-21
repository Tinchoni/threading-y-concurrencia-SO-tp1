#ifndef LISTA_ATOMICA_HPP
#define LISTA_ATOMICA_HPP

#include <atomic>

template<typename T>
class ListaAtomica {
 private:
    struct Nodo {
        Nodo(const T &val) : _valor(val), _siguiente(nullptr) {}

        T _valor;
        Nodo *_siguiente;
    };

    std::atomic<Nodo *> _cabeza;

 public:
    ListaAtomica() : _cabeza(nullptr) {}

    ~ListaAtomica() {
        Nodo *n, *t;
        n = _cabeza.load();
        while (n) {
            t = n;
            n = n->_siguiente;
            delete t;
        }
    }

    void insertar(const T &valor) {
        // Construimos el nuevo nodo:
        Nodo *nuevoNodo = new Nodo(valor);
        // Obtenemos la vieja cabeza:

        // SOLUCION 1
        std::atomic<Nodo *> viejaCabeza = _cabeza.load();
        // Actualizamos la cabeza:
        _cabeza = nuevoNodo;
        // Enlazamos el nuevo nodo con la vieja cabeza pero atómicamente:
        nuevoNodo->_siguiente = viejaCabeza;

        // SOLUCION 2
        /*Nodo *viejaCabeza = _cabeza.load();
        // if _cabeza == viejaCabeza then
                //_cabeza = nuevoNodo
        // else
                //viejaCabeza = _cabeza y vuelve a intentar, esta vez con viejaCabeza actualizado.

        while(!_cabeza.compare_exchange_weak(viejaCabeza, nuevoNodo)) {
            nuevoNodo -> _siguiente = viejaCabeza;
        }*/
    }

    T &cabeza() const {
        return _cabeza.load()->_valor;
    }

    T &iesimo(unsigned int i) const {
        Nodo *n = _cabeza.load();
        for (unsigned int j = 0; j < i; j++) {
            n = n->_siguiente;
        }
        return n->_valor;
    }

    unsigned int longitud() const {
        Nodo *n = _cabeza.load();
        unsigned int cant = 0;
        while (n != nullptr) {
            cant++;
            n = n->_siguiente;
        }
        return cant;
    }


    class Iterador {
     private:
        ListaAtomica *_lista;

        typename ListaAtomica::Nodo *_nodo_sig;

        Iterador(ListaAtomica<T> *lista, typename ListaAtomica<T>::Nodo *sig)
            : _lista(lista), _nodo_sig(sig) {}

        friend typename ListaAtomica<T>::Iterador ListaAtomica<T>::crearIt();

     public:
        Iterador() : _lista(nullptr), _nodo_sig(nullptr) {}

        Iterador &operator=(const typename ListaAtomica::Iterador &otro) {
            _lista = otro._lista;
            _nodo_sig = otro._nodo_sig;
            return *this;
        }

        bool haySiguiente() const {
            return _nodo_sig != nullptr;
        }

        T &siguiente() {
            return _nodo_sig->_valor;
        }

        void avanzar() {
            _nodo_sig = _nodo_sig->_siguiente;
        }

        bool operator==(const typename ListaAtomica::Iterador &otro) const {
            return _lista->_cabeza.load() == otro._lista->_cabeza.load()
                && _nodo_sig == otro._nodo_sig;
        }
    };

    Iterador crearIt() {
        return Iterador(this, _cabeza);
    }
};

#endif /* LISTA_ATOMICA_HPP */
