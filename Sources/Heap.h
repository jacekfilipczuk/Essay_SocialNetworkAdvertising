/* 
 * File:   Heap.h
 * Author: francescocarrabs
 *
 * Created on 14 ottobre 2010, 10.09
 *
 * Per far funzionare questa classe dovete estendere la vostra classe
 */

#ifndef HEAP_H
#define	HEAP_H

#include "nodo_heap.h"

// Il template richiede la classe di nodi che faranno parte dell'heap, il numero massimo di
// elementi che conterra' al piu' l'heap, e i due campi "heap_pos" e "sort_key" che la classe dei nodi DEVE contenere
// per indicare la posizione del nodo nell'array e la chiave dei nodi su cui fare l'ordinamento.

class nodo_heap;

class Heap {

    
public:
    Heap(int s);
    Heap(const Heap& orig);
    virtual ~Heap();

    nodo_heap* Extract_Heaviest();
    nodo_heap* Extract_Heaviest_Neighbourhood();
    void setHeapSize(int size){heap_size=size;}
    int getHeapSize(){return heap_size;}
    nodo_heap* getNodo(int pos){return heap[pos];}
    bool nodo_in_heap(nodo_heap *ptr);
    void put_to_pos_in_heap(nodo_heap *ptr, int pos);
    bool non_empty_heap();
    void Heap_Decrease_key(nodo_heap *ptr, double key_value);
    void Insert_to_heap (nodo_heap* ptr);
    void Stampa_Heap();
    nodo_heap* Extract_Min();
    void Clean_Heap();
    void Remove_From_Heap(int pos);

private:
    long heap_size;
    int h_degree;
    nodo_heap **heap;
    
};

#endif	/* HEAP_H */

