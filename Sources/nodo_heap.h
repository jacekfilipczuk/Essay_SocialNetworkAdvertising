/* 
 * File:   nodo_heap.h
 * Author: francescocarrabs
 *
 * Created on 14 ottobre 2010, 12.27
 */


#ifndef NODO_HEAP_H
#define	NODO_HEAP_H

 class Heap;

class nodo_heap {
friend class Heap;
public:
    nodo_heap();
    virtual ~nodo_heap();

    long GetHeap_pos() const {
        return heap_pos;
    }

    void SetHeap_pos(long heap_pos) {
        this->heap_pos = heap_pos;
    }

    double GetChiave_ordinamento() const {
        return chiave_ordinamento;
    }

    void SetChiave_ordinamento(double chiave_ordinamento) {
        this->chiave_ordinamento = chiave_ordinamento;
    }

   
protected:
    //virtual bool isless(nodo_heap* x){return x->chiave_ordinamento < chiave_ordinamento;}

    long heap_pos;
    double chiave_ordinamento;

};

#endif	/* NODO_HEAP_H */

