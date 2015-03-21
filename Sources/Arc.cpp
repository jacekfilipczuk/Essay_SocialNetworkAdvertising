/* 
 * File:   Arc.cpp
 * Author: Alfredokk
 * 
 * Created on 16 aprile 2010, 12.57
 */

#include "Arc.h"
#include "Node.h"

Arc::Arc() {
    this->nodeFrom=NULL;
    this->nodeTo=NULL;
    this->weight=0;
    this->need=false;
    this->inverse=0;
    this->stato=0;
    this->next = this->next_dyn = NULL;
    this->prev = this->prev_dyn = NULL;
}

Arc::Arc(Node *nodeFrom,Node *nodeTo,double weight) {
    this->nodeFrom=nodeFrom;
    this->nodeTo=nodeTo;
    this->weight=weight;
    this->need=false;
    this->inverse=0;
    this->stato=0;
}

Arc::Arc(Node *nodeFrom,Node *nodeTo) {
    this->nodeFrom=nodeFrom;
    this->nodeTo=nodeTo;
    this->weight=0;
    this->need=false;
    this->inverse=0;
    this->next=NULL;
    this->next_dyn=NULL;
    this->stato=0;
    this->prev = this->prev_dyn = NULL;
}

void Arc::Copia(Arc *source){

    inverse = source->GetInverse();
    nodeFrom = source->GetNodeFrom();
    nodeTo = source->GetNodeTo();
    weight = source->GetWeight();
    need = source->IsNeed();
    next = source->getNext();
    next_dyn = source->getNext_dyn();
    stato = source->getStato();
    stato_dyn = source->getStato_dyn();
    prev = source->getPrev();
    prev_dyn  = source->getPrev_dyn();

}

Arc::Arc(const Arc& orig) {
}

Arc::~Arc() {
}

void Arc::print(){

    cout<<"( "<<nodeFrom->GetId()<<" , "<<nodeTo->GetId()<<" - w:"<<GetWeightAsInteger()<<" ) "<<endl;

}