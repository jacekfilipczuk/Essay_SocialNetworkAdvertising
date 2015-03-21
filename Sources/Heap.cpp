/* 
 * File:   Heap.cpp
 * Author: francescocarrabs
 * 
 * Created on 14 ottobre 2010, 10.09
 */

#include "Heap.h"
#include "nodo_heap.h"
#include "Node.h"
#include "Arc.h"
#include <iostream>
#include <cstdlib>
#include <stdio.h>

using namespace std;

Heap::Heap(int size) {
    heap_size=0;
    h_degree = 2;
    heap = (nodo_heap**) new nodo_heap*[size];

}

Heap::~Heap() {
    delete [] heap;
}


void Heap::put_to_pos_in_heap(nodo_heap* ptr, int pos){

    heap[pos]=ptr;
    ptr->heap_pos = pos;
}

bool Heap::nodo_in_heap(nodo_heap* ptr){

    if(ptr->heap_pos==-1) return false;
    return true;
}

bool Heap::non_empty_heap(){

    if(heap_size>0) return true;
    else return false;
}

void Heap::Insert_to_heap(nodo_heap* ptr){
    put_to_pos_in_heap(ptr,heap_size);
    heap_size++;
    Heap_Decrease_key(ptr, ptr->chiave_ordinamento);
}

void Heap::Heap_Decrease_key(nodo_heap* ptr, double key_value){

    int h_current_pos,h_new_pos;
    nodo_heap *temp;
    
    for(h_current_pos = ptr->heap_pos;h_current_pos>0; h_current_pos=h_new_pos){

        h_new_pos = ( h_current_pos - 1 ) / 2;
        temp = heap[h_new_pos];
        
        if(key_value >= temp->chiave_ordinamento) break;

        put_to_pos_in_heap(temp,h_current_pos);
        
    }

    put_to_pos_in_heap(ptr,h_current_pos);

}

//Restituisce il nodo nell'heap col peso maggiore ( variante in cui ogni nodo ha come variabile privata il peso del vicinato)
nodo_heap* Heap::Extract_Heaviest_Neighbourhood(){

    int i;
    nodo_heap *ptr,*temp;
    float peso,peso_temp;
    ptr=heap[0];

    peso=0;
    for(i=0; i<heap_size; i++){
        temp =heap[i];
        peso_temp=((Node*)temp)->getPeso_Neighbourhood();
        if(peso_temp>peso){
            ptr=temp;
            peso=peso_temp;
        }
    }
    return ptr;

}


//Calcola il peso del vicinato per ogni nodo nell'heap e restituisce quello col peso maggiore
//nodo_heap* Heap::Extract_Heaviest_Neighbourhood(){

//    int i;
//    nodo_heap *ptr,*temp;
//    float peso,peso_temp;
//    ptr=heap[0];
//    Arc *archi=NULL;
//    peso=0;
//    for(i=0; i<heap_size; i++){
//        temp =heap[i];
//        peso_temp=((Node*)temp)->GetPeso();
//        archi=((Node*)temp)->GetCorona_dyn();
//        while(archi!= NULL){
//            peso_temp+=archi->GetNodeTo()->GetPeso();

//            archi=archi->getNext_dyn();
//        }
//        if(peso_temp>peso){
//            ptr=temp;
//            peso=peso_temp;
//        }
//    }
//    return ptr;

//}

nodo_heap* Heap::Extract_Heaviest(){

    int i;
    nodo_heap *ptr,*temp;
    float peso,peso_temp;
    ptr=heap[0];
    peso = ((Node*)ptr)->GetPeso()*((Node*)ptr)->GetGrado_dyn();
    for(i=1; i<heap_size; i++){
        temp =heap[i];
        peso_temp=((Node*)temp)->GetPeso()*((Node*)temp)->GetGrado_dyn();
        if(peso_temp>peso){
            ptr=temp;
            peso=peso_temp;
        }


    }
    return ptr;
}


nodo_heap* Heap::Extract_Min(){

    nodo_heap *ptr, *nodo_k;
    long h_current_pos, h_new_pos ,h_last_pos,h_pos;
    double dist_k,dist_min;
    
    ptr = heap[0];
    ptr->heap_pos=-1;
    
    heap_size--;
    
    if(heap_size>0){
    
        nodo_k = heap[heap_size];
        dist_k = nodo_k->chiave_ordinamento;
        
        h_current_pos=0;
        
        while(1){
            
            h_new_pos = (h_current_pos * 2)+1;
            if(h_new_pos >= heap_size) break;

            dist_min = heap[h_new_pos]->chiave_ordinamento;

            h_last_pos = h_new_pos + h_degree;
            if(h_last_pos > heap_size) h_last_pos = heap_size;

            for(h_pos = h_new_pos+1;h_pos< h_last_pos;h_pos++){
                if(heap[h_pos]->chiave_ordinamento < dist_min){
                    h_new_pos = h_pos;
                    dist_min = heap[h_pos]->chiave_ordinamento;
                }

            }

            if(dist_k <= dist_min) break;

            put_to_pos_in_heap(heap[h_new_pos],h_current_pos);
            h_current_pos = h_new_pos;
        }

        put_to_pos_in_heap( nodo_k,h_current_pos);
    
    }
    return ptr;
    
}


void Heap::Remove_From_Heap(int pos){


    nodo_heap *nodo_k;
    long h_current_pos, h_new_pos ,h_last_pos,h_pos;
    double dist_k,dist_min;

    if(pos > heap_size-1){
       printf("\n[Remove_From_Heap] ERRORE: e' stata richiesta la cancellazione del nodo nella pos %d che e' al di la' della dimensione dell'heap (%ld)\n\n",pos,heap_size);
       exit(1);
    }

    heap[pos]->heap_pos=-1;
    heap_size--;

    if(heap_size>0){
        nodo_k = heap[heap_size];
        dist_k = nodo_k->chiave_ordinamento;

        h_current_pos=pos;

        while(1){

            h_new_pos = (h_current_pos * 2)+1;
            if(h_new_pos >= heap_size) break;

            dist_min = heap[h_new_pos]->chiave_ordinamento;

            h_last_pos = h_new_pos + h_degree;
            if(h_last_pos > heap_size) h_last_pos = heap_size;

            for(h_pos = h_new_pos+1;h_pos< h_last_pos;h_pos++){
                if(heap[h_pos]->chiave_ordinamento < dist_min){
                    h_new_pos = h_pos;
                    dist_min = heap[h_pos]->chiave_ordinamento;
                }

            }

            if(dist_k <= dist_min) break;

            put_to_pos_in_heap(heap[h_new_pos],h_current_pos);
            h_current_pos = h_new_pos;
        }

        put_to_pos_in_heap( nodo_k,h_current_pos);
    }

}

void Heap::Stampa_Heap(){

    int i;
    for(i=0;i<heap_size;i++){
        cout << i << heap[i]->chiave_ordinamento << ", " ;

    }
    cout << endl;
}

void Heap::Clean_Heap(){

    int i;
    for(i=0;i<heap_size;i++){
        heap[i]->SetHeap_pos(-1);
        heap[i]=NULL;
    }
    heap_size=0;
}
