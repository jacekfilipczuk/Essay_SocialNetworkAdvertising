/* 
 * File:   Node.cpp
 * Author: Alfredo
 * 
 * Created on 16 aprile 2010, 12.57
 */

#include "Node.h"

Node::Node() {

    this->index = 0;
    this->id = 0;
    this->flag = 0;
    this->peso = 0;
    this->peso_dyn = 0;
    this->peso_neighbourhood=0;
	this->penalties=0;
    this->grado = 0;
    this->grado_dyn = 0;
    this->father=NULL;
    this->data = INT_MAX;
    this->arcToFather=NULL;
    this->numComponentCreated=0;
    this->sureBranch=false;
    this->stato=this->stato_dyn = 0;
    counter=0;

}

Node::Node(int index, int id, Parser* p) {

    this->index = index;
    this->id = id;
    this->flag = 0;
	 this->penalties=0;
    this->peso = p->getNodo(index).peso;
    this->peso_dyn = p->getNodo(index).peso;
    this->grado = p->getNodo(index).grado;
    this->grado_dyn = p->getNodo(index).grado;
    this->father=NULL;
    this->data = INT_MAX;
    this->arcToFather=NULL;
    this->numComponentCreated=0;
    this->sureBranch=false;
    this->stato=this->stato_dyn=0;
    counter=0;
}

Node::Node(const Node& orig) {
}

void Node::Copia(Node* source){

    index = source->GetIndex();
    id = source->GetId();
    flag = source->GetFlag();
	 
	 penalties=source->GetPenalties();
    peso = source->GetPeso();
    peso_dyn = source->GetPeso_dyn();
    peso_neighbourhood=source->getPeso_Neighbourhood();
    grado = source->GetGrado();
    grado_dyn = source->GetGrado_dyn();
    rappresentante = source->GetRappresentante();
    father = source->GetFather();
    data = source->getData();
    arcToFather = source->getArcToFather();

    numComponentCreated = source->GetNumComponentCreated();
    nodo_up = source->GetNodo_up();
    nodo_down = source->GetNodo_down();
    nodo_up_dyn = source->GetNodo_up_dyn();
    nodo_down_dyn = source->GetNodo_down_dyn();
    corona = source->GetCorona();
    corona_dyn = source->GetCorona_dyn();
    stato = source->GetStato();
    stato_dyn = source->GetStato_dyn();

}

Node::~Node() {
    //delete [] arcs;
}

void Node::Inizialize(int index, int id, Parser* p){

    this->index = index;
    this->id = id;
    this->flag = 0;
	 this->penalties = 0;

    this->peso=p->getNodo(index).peso;
    this->peso_dyn = p->getNodo(index).peso;
    this->peso_neighbourhood=0;



    this->grado = p->getNodo(index).grado;
    this->grado_dyn = p->getNodo(index).grado;
    this->father=NULL;
    this->data = INT_MAX;
    this->arcToFather=NULL;
    this->numComponentCreated=0;
    this->sureBranch=false;
    this->stato=this->stato_dyn = 1;
    this->rappresentante=-1;
}
