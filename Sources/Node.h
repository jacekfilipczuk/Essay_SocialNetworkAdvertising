/*
 * File:   Node.h
 * Author: Alfredo
 *
 * Created on 16 aprile 2010, 12.57
 */

#ifndef _NODE_H
#define	_NODE_H

#include <limits.h>
#include "Arc.h"
#include "Parser.h"
#include "nodo_heap.h"

class Node : public nodo_heap {
    friend class Graph;

public:

    Node();
    Node(int index, int id, Parser* p);
    Node(const Node& orig);
    virtual ~Node();

    Node* GetFather() const {
        return father;
    }

    void Inizialize(int index, int id, Parser* p);

    void Copia(Node* source);

    void SetFather(Node* father) {
        this->father = father;
    }

    int GetFlag() const {
        return flag;
    }

    void SetFlag(int flag) {
        this->flag = flag;
    }

    int getData() const {
        return data;
    }

    void setData(int data) {
        this->data = data;
    }

    Arc* getArcToFather() const {
        return arcToFather;
    }

    void setArcToFather(Arc* arcToFather) {
        this->arcToFather = arcToFather;
    }

    int GetNumComponentCreated() const {
        return numComponentCreated;
    }

    void SetNumComponentCreated(int numComponentCreated) {
        this->numComponentCreated = numComponentCreated;
    }

    bool IsSureBranch() const {
        return sureBranch;
    }

    void SetSureBranch(bool sureBranch) {
        this->sureBranch = sureBranch;
    }

    int GetGrado() const {
        return grado;
    }

    void SetGrado(int grado) {
        this->grado = grado;
    }

    int GetGrado_dyn() const {
        return grado_dyn;
    }

    void SetGrado_dyn(int grado_dyn) {
        this->grado_dyn = grado_dyn;
    }

    int GetGrado_in() const {
        return grado_in;
    }

    void SetGrado_in(int grado) {
        this->grado_in = grado;
    }

    int GetGrado_in_dyn() const {
        return grado_in_dyn;
    }

    void SetGrado_in_dyn(int grado) {
        this->grado_in_dyn = grado;
    }

    int GetRappresentante() const {
        return rappresentante;
    }

    void SetRappresentante(int rappresentante) {
        this->rappresentante = rappresentante;
    }

    Arc* GetCorona() const {
        return corona;
    }

    void SetCorona(Arc* corona) {
        this->corona = corona;
    }

    Arc* GetCorona_in() const {
        return corona_in;
    }

    void SetCorona_in(Arc* corona) {
        this->corona_in = corona;
    }

    Arc* GetCorona_dyn() const {
        return corona_dyn;
    }

    void SetCorona_dyn(Arc* corona_dyn) {
        this->corona_dyn = corona_dyn;
    }

    Arc* GetCorona_in_dyn() const {
        return corona_in_dyn;
    }

    void SetCorona_in_dyn(Arc* corona_in_dyn) {
        this->corona_in_dyn = corona_in_dyn;
    }

    int GetId() const {
        return id;
    }

    void SetId(int id) {
        this->id = id;
    }

    int GetIndex() const {
        return index;
    }

    void SetIndex(int index) {
        this->index = index;
    }

    Node* GetNodo_down() const {
        return nodo_down;
    }

    void SetNodo_down(Node* nodo_down) {
        this->nodo_down = nodo_down;
    }

    Node* GetNodo_up() const {
        return nodo_up;
    }

    void SetNodo_up(Node* nodo_up) {
        this->nodo_up = nodo_up;
    }

    short GetStato() const {
        return stato;
    }

    void SetStato(short stato) {
        this->stato = stato;
    }

    Node* GetNodo_down_dyn() const {
        return nodo_down_dyn;
    }

    void SetNodo_down_dyn(Node* nodo_down_dyn) {
        this->nodo_down_dyn = nodo_down_dyn;
    }

    Node* GetNodo_up_dyn() const {
        return nodo_up_dyn;
    }

    void SetNodo_up_dyn(Node* nodo_up_dyn) {
        this->nodo_up_dyn = nodo_up_dyn;
    }

    double GetPeso() const {
        return peso;
    }

    void SetPeso(double peso) {
        this->peso = peso;
    }

    double GetPeso_dyn() const {
        return peso_dyn;
    }

    void setPeso_Neighbourhood(double peso){
        peso_neighbourhood=peso;
    }

    double getPeso_Neighbourhood(){
        return peso_neighbourhood;
    }

    void SetPeso_dyn(double peso_dyn) {
        this->peso_dyn = peso_dyn;
    }

    short GetStato_dyn() const {
        return stato_dyn;
    }


    void SetStato_dyn(short stato_dyn) {
        this->stato_dyn = stato_dyn;
    }

    void SetPenalties(double penalties) {
        this->penalties = penalties;
    }

    double GetPenalties() const {
        return penalties;
    }

    int GetL_vk() const {
        return L_vk;
    }

    void SetL_vk(int L_vk) {
        this->L_vk = L_vk;
    }

    int GetDelta_vc() const {
        return delta_vc;
    }

    void SetDelta_vc(int delta_vc) {
        this->delta_vc = delta_vc;
    }

    int counter;


private:
    int index; // indica l'indice della posizione che il nodo assume nell'array dei nodi
    int id; // indica l'identificativo del nodo così come viene letto dal file.
    int flag;
    double peso;
    double peso_dyn;
    double peso_neighbourhood;
    double penalties;

    int grado; // indica il grado del nodo nel grafo originale
    int grado_dyn; // indica il grado corrente del nodo

    int grado_in; // indica il grado del nodo nel grafo originale
    int grado_in_dyn; // indica il grado corrente del nodo

    int rappresentante; // valore utilizzato per indicare il rappresentante della componente a cui il nodo appartiene.
    Node *father;
    int data;
    Arc* arcToFather;
    bool sureBranch;
    int numComponentCreated;

    Node* nodo_up, *nodo_down; // Puntatore al precedente e successivo nodo del grafo nell'array dei nodi.
    Node* nodo_up_dyn, *nodo_down_dyn; // Puntatore al precedente e successivo nodo del grafo corrente nell'array dei nodi.
    Arc *corona_dyn; // puntatore alla corona degli archi del nodo nel grafo corrente
    Arc *corona; // puntatore alla corona uscente degli archi del nodo nel grafo iniziale
    Arc *corona_in_dyn; // puntatore alla corona entrante degli archi del nodo nel grafo corrente
    Arc *corona_in; // puntatore alla corona entrante degli archi del nodo nel grafo iniziale

    short stato; // indica se il nodo e' presente o meno nel grafo iniziale.
    short stato_dyn; // indica se il nodo e' presente o meno nel grafo dinamico.

    int delta_vc, L_vk;

};

#endif	/* _NODE_H */
