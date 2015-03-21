/*
 * File:   Graph.h
 * Author: Alfredo
 *
 * Created on 16 aprile 2010, 12.28
 */

#ifndef _GRAPH_H
#define	_GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>
#include "Parser.h"
#include "Arc.h"
#include "Node.h"

#define G_Init 0    // Questo ed il successivo flag servono per specificare se le operazioni devono
// essere effettuate sul grafo originale o su quello dinamico.
#define G_Dyn 1

#define in_G 1      // indica se il nodo e' presente o meno nel grafo
#define not_in_G 0
#define in_FVS -1

#define TO_BUILD 1
#define BUILT 0


using namespace std;

class Graph {
public:
	Graph(string filename);
	Graph(string filename, short parser_type, short oriented);
	Graph(Graph *orig);
	Graph(int n);
	virtual ~Graph();


	Arc** GetMatrix() const {
		return matrix;
	}

	Node* GetNodes() const {
		return nodes;
	}

	int GetNumarcs() const {
		return numarcs;
	}

	int GetNumnodes() const {
		return numnodes;
	}

	Node* GetSingleNode(int pos) const {
		return &nodes[pos];
	}

	short GetIsDirect() {
		return is_directed;
	}

	void SetIsDirect(short x) {
		this->is_directed = x;
	}

	Graph* getCopy();

	void Inserisci_Nodo_Dyn(Node *nodo);
	void Inserisci_Arco(Arc* arc);
	void Inserisci_Arco_Dyn(Arc* arc);

	void Rimuovi_Nodo(Node *nodo);
	void Rimuovi_Nodo_Dyn(Node *nodo);
	void Rimuovi_Arco(Arc* arc);
	void Rimuovi_Arco_Dyn(Arc* arc);
	void Rimuovi_Nodo_MGA(Node *nodo,double ratio, Heap *h, Node** eliminati, int starting_point, int *num_eliminati);

	void Riduzione_zero_uno(short flag, Node** eliminati, int starting_point, int *num_eliminati);

	void Visualizza_Grafo(short flag);
	void Visualizza_Grafo_Dyn(short flag);

	Node* getGrafo_dinamico() const {
		return grafo_dinamico;
	}

	void setGrafo_dinamico(Node* grafo_dinamico) {
		this->grafo_dinamico = grafo_dinamico;
	}



	Node* getGrafo_iniziale() const {
		return grafo_iniziale;
	}

	void setGrafo_iniziale(Node* grafo_iniziale) {
		this->grafo_iniziale = grafo_iniziale;
	}

	void SetNumcolors(int numcolors) {
		this->numcolors = numcolors;
	}
	int GetNumcolors() const {
		return numcolors;
	}

	void shrink_permanente(int reduced);

	void SetMaxGrado(int max_grado) {
		this->max_grado = max_grado;
	}
	void SetMaxGradoIn(int max_grado_in) {
		this->max_grado_in = max_grado_in;
	}
	void SetMinGrado(int min_grado) {
		this->min_grado = min_grado;
	}
	void SetMinGradoIn(int min_grado_in) {
		this->min_grado_in = min_grado_in;
	}
	int GetMaxGrado() const {
		return max_grado;
	}
	int GetMaxGradoIn() const {
		return max_grado_in;
	}
	int GetMinGrado() const {
		return min_grado;
	}
	int GetMinGradoIn() const {
		return min_grado_in;
	}
	
	Arc* getArc(Node* from,Node* to);
	Arc* getArc(int from, int to);

private:
	Parser *p;
	int numnodes;
	int numarcs;
	int numcolors;
	Node *nodes;
	Arc  **matrix;
	int max_grado, max_grado_in;
	int min_grado, min_grado_in;
	
	//int visit(Node *n, Arc** sol,int actualSize,bool random,Arc *notUse);

	string filename;
	Node* grafo_iniziale;            // puntatore al primo nodo del grafo iniziale.
	Node* grafo_dinamico;   // puntatore al primo nodo dell'array (non necessariamente 0) presente nel grafo corrente.
	short is_directed;      // flag che stabilisce se il grafo e' o meno orientato (questa info serve alle funzioni che inseriscono e cancellano archi
	// per sapere se dopo l'inserimento (cancellazione) dell'arco (u,v) debba inserire (cancellare) anche l'arco (v,u) )
};


#endif	/* _GRAPH_H */
