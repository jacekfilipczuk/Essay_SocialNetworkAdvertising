/*
 * File:   Arc.h
 * Author: Alfredo
 *
 * Created on 16 aprile 2010, 12.57
 */

#ifndef _ARC_H
#define	_ARC_H

#include <iostream>
using namespace std;

//gcc su linux ha bisogno di questo tipo di dichiarazione.
class Node;

class Arc {

	friend class Node;
	friend class Graph;

public:
	Arc();
	Arc(Node *nodeFrom,Node *nodeTo);
	Arc(Node *nodeFrom,Node *nodeTo,double weight);
	Arc(const Arc& orig);
	virtual ~Arc();

	void print();

	Node* GetNodeFrom() const {
		return nodeFrom;
	}

	void SetNodeFrom(Node* nodeFrom) {
		this->nodeFrom = nodeFrom;
	}

	Node* GetNodeTo() const {
		return nodeTo;
	}

	void SetNodeTo(Node* nodeTo) {
		this->nodeTo = nodeTo;
	}


	Arc* GetInverse() const {
		return inverse;
	}

	void SetInverse(Arc* inverse) {
		this->inverse = inverse;
	}


	double GetWeight() const {
		return weight;
	}

	void SetWeight(double weight) {
		this->weight = weight;
	}

	int GetWeightAsInteger() const {
		return (int)weight;
	}

	bool IsNeed() const {
		return need;
	}

	void SetNeed(bool need) {
		this->need = need;
	}

	Arc* getNext() const {
		return next;
	}

	void setNext(Arc* next) {
		this->next = next;
	}

	Arc* getNext_dyn() const {
		return next_dyn;
	}

	void setNext_dyn(Arc* next_dyn) {
		this->next_dyn = next_dyn;
	}

	Arc* getPrev() const {
		return prev;
	}

	void setPrev(Arc* prev) {
		this->prev = prev;
	}

	Arc* getPrev_dyn() const {
		return prev_dyn;
	}

	void setPrev_dyn(Arc* prev_dyn) {
		this->prev_dyn = prev_dyn;
	}

	short getStato() const {
		return stato;
	}

	void setStato(short stato) {
		this->stato = stato;
	}

	short getStato_dyn() const {
		return stato_dyn;
	}

	void setStato_dyn(short stato_dyn) {
		this->stato_dyn = stato_dyn;
	}

	void Copia(Arc *source);




	Arc* getNext_in() const {
		return next_in;
	}

	void setNext_in(Arc* next) {
		this->next_in = next;
	}

	Arc* getNext_in_dyn() const {
		return next_in_dyn;
	}

	void setNext_in_dyn(Arc* next_dyn) {
		this->next_in_dyn = next_dyn;
	}

	Arc* getPrev_in() const {
		return prev_in;
	}

	void setPrev_in(Arc* prev) {
		this->prev_in = prev;
	}

	Arc* getPrev_in_dyn() const {
		return prev_in_dyn;
	}

	void setPrev_in_dyn(Arc* prev_dyn) {
		this->prev_in_dyn = prev_dyn;
	}

	void SetColor(int color) {
		this->color = color;
	}
	int GetColor() const {
		return color;
	}
	
private:
	Arc *inverse;
	Node *nodeFrom;
	Node *nodeTo;
	int color;
	double weight;
	bool need;
	short stato;                        // indica se il nodo e' presente o meno nel grafo iniziale.
	short stato_dyn;                    // indica se il nodo e' presente o meno nel grafo dinamico.
	Arc *next, *prev;                  //Puntatore al nodo successivo  e precedente della corona uscente (struttura fissa che non viene mai modificata)
	Arc *next_dyn, *prev_dyn;          //Puntatore al nodo precedente della corona uscente (struttura fissa che non viene mai modificata)
	Arc *next_in, *prev_in;            //Puntatore al nodo successivo  e precedente della corona entrante (struttura fissa che non viene mai modificata)
	Arc *next_in_dyn, *prev_in_dyn;    //Puntatore al nodo precedente della corona entrante (struttura fissa che non viene mai modificata)


};

#endif	/* _ARC_H */
