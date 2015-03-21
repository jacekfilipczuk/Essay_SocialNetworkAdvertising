/* 
 * File:   Graph.cpp
 * Author: Alfredo
 * 
 * Created on 16 aprile 2010, 12.28
 */

#include "Graph.h"
#include "Heap.h"
#include "Arc.h"

#define safe_tests 1

Graph::Graph(int n){

	int i;
    //Allocazione memoria
    nodes = new Node[n];
    matrix= new Arc*[n];

    for (i = 0; i < n ; i++){
        matrix[i]= new  Arc[n];
    }
}

Graph::Graph(string filename){

    int i,j;
    Arc *coda,*tmp_arc,*testa_in;

    this->filename=filename;
    Parser *p=new Parser(filename);
    p->adattaNonOrientato();
    this->is_directed=1;
    this->numarcs=0;
    this->numnodes=p->getNumNodi();

    short aux_matrice[numnodes][numnodes];

    //Allocazione memoria
    nodes = new Node[p->getNumNodi()];
    matrix= new Arc*[p->getNumNodi()];

    for (i = 0; i < p->getNumNodi() ; i++){
        nodes[i].Inizialize(i,p->getNodo(i).id,p);
        nodes[i].SetCorona(NULL);
        nodes[i].SetCorona_dyn(NULL);
        nodes[i].SetCorona_in(NULL);
        nodes[i].SetCorona_in_dyn(NULL);
        nodes[i].SetGrado(0);
        nodes[i].SetGrado_dyn(0);
        nodes[i].SetGrado_in(0);
        nodes[i].SetGrado_in_dyn(0);
        matrix[i]= new  Arc[p->getNumNodi()];
    }

    //Inizializzazione dei puntatori del grafo
    nodes[0].SetNodo_up(NULL);
    nodes[0].SetNodo_up_dyn(NULL);
    nodes[0].SetNodo_down(&nodes[1]);
    nodes[0].SetNodo_down_dyn(&nodes[1]);

    grafo_iniziale = grafo_dinamico = nodes;
    
    for(i=1;i<numnodes-1;i++){  //<size-1   for(i=1;i<=size-1;i++)
        nodes[i].SetNodo_up(&nodes[i-1]);
        nodes[i].SetNodo_down(&nodes[i+1]);
        nodes[i].SetNodo_up_dyn(&nodes[i-1]);
        nodes[i].SetNodo_down_dyn(&nodes[i+1]);
    }

    nodes[i].SetNodo_up(&nodes[i-1]);
    nodes[i].SetNodo_down(NULL);
    nodes[i].SetNodo_up_dyn(&nodes[i-1]);
    nodes[i].SetNodo_down_dyn(NULL);
    
    // inizializzo la matrice degli archi e quella ausiliaria
    for ( i = 0; i < numnodes ; i++) {
        for ( j = 0; j < numnodes ; j++) {
            aux_matrice[i][j]=-1;
        }
    }

    // salvo nella matrice ausiliaria tutti quanti gli archi del grafo.
    for ( j = 0; j < p->getNumArchi(); j++) {
        aux_matrice[p->getArco(j).da-1][p->getArco(j).a-1]=1;
    }

    for(i=0;i<numnodes;i++){

        nodes[i].SetStato(in_G);
        nodes[i].SetStato_dyn(in_G);

        // costruisco le corone uscente ed entrante...
        for(j=0;j<numnodes;j++){
             // se ho trovato un arco del grafo
             if(aux_matrice[i][j] == 1){
                  
                    //if(i<j) 
                    this->numarcs++;
                    // se devo inserire il primo arco della corona uscente...
                    if(nodes[i].GetCorona()==NULL){
                        coda = tmp_arc = &matrix[i][j];
                        nodes[i].SetCorona(tmp_arc);
                        nodes[i].SetCorona_dyn(tmp_arc);

                        coda->SetNodeFrom(&nodes[i]);
                        coda->SetNodeTo(&nodes[j]);

                        coda->setNext_dyn(NULL);
                        coda->setNext(NULL);
                        coda->setPrev(NULL);
                        coda->setPrev_dyn(NULL);

                        coda->setStato(in_G);
                        coda->setStato_dyn(in_G);
                        coda->SetNeed(0);

                        j++;
                        nodes[i].SetGrado(nodes[i].GetGrado()+1);
                    }
                    else{
                        tmp_arc = &matrix[i][j];
                        coda->setNext(tmp_arc);
                        coda->setNext_dyn(tmp_arc);

                        tmp_arc->SetNodeFrom(&nodes[i]);
                        tmp_arc->SetNodeTo(&nodes[j]);
                        
                        tmp_arc->setPrev(coda);
                        tmp_arc->setPrev_dyn(coda);
                        tmp_arc->setNext(NULL);
                        tmp_arc->setNext_dyn(NULL);

                        tmp_arc->setStato(in_G);
                        tmp_arc->setStato_dyn(in_G);
                        tmp_arc->SetNeed(0);

                        coda = coda->getNext();
                        nodes[i].SetGrado(nodes[i].GetGrado()+1);
                    }


                    // Costruisco anche la corona entrante 
                    if( (testa_in = nodes[j].GetCorona_in()) == NULL){
                        tmp_arc = &matrix[i][j];
                        nodes[j].SetCorona_in(tmp_arc);
                        nodes[j].SetCorona_in_dyn(tmp_arc);

                        tmp_arc->setNext_in_dyn(NULL);
                        tmp_arc->setNext_in(NULL);
                        tmp_arc->setPrev_in(NULL);
                        tmp_arc->setPrev_in_dyn(NULL);

                        nodes[j].SetGrado_in(nodes[j].GetGrado_in()+1);
                        nodes[j].SetGrado_in_dyn(nodes[j].GetGrado_in_dyn()+1);
                    }
                    else{
                        tmp_arc = &matrix[i][j];

                        tmp_arc->setNext_in_dyn(testa_in);
                        tmp_arc->setNext_in(testa_in);
                        tmp_arc->setPrev_in(NULL);
                        tmp_arc->setPrev_in_dyn(NULL);

                        testa_in->setPrev_in(tmp_arc);
                        testa_in->setPrev_in_dyn(tmp_arc);

                        nodes[j].SetCorona_in(tmp_arc);
                        nodes[j].SetCorona_in_dyn(tmp_arc);
                        nodes[j].SetGrado_in(nodes[j].GetGrado_in()+1);
                        nodes[j].SetGrado_in_dyn(nodes[j].GetGrado_in_dyn()+1);
                    }

             }

        }
        nodes[i].SetGrado_dyn(nodes[i].GetGrado());
    }//for(i=0;i<size;i++)



	delete p;
}


Graph::Graph(string filename, short parser_type, short oriented){

    int i,j;
    Arc *coda,*tmp_arc,*testa_in;

    this->filename=filename;
    Parser *p=new Parser(filename,parser_type);

    if(oriented==0){ 
        p->adattaNonOrientato();
    }
   
    this->is_directed=oriented;

    this->numarcs=0;
    this->numnodes=p->getNumNodi();
	 this->numcolors = p->getNumColors();
	
    short aux_matrice[numnodes][numnodes];

    //Allocazione memoria
    nodes = new Node[p->getNumNodi()];
    matrix= new Arc*[p->getNumNodi()];
	  	

    for (i = 0; i < p->getNumNodi() ; i++){
        nodes[i].Inizialize(i,p->getNodo(i).id,p);

        nodes[i].SetCorona(NULL);
        nodes[i].SetCorona_dyn(NULL);
        nodes[i].SetCorona_in(NULL);
        nodes[i].SetCorona_in_dyn(NULL);
        nodes[i].SetGrado(0);
        nodes[i].SetGrado_dyn(0);
        nodes[i].SetGrado_in(0);
        nodes[i].SetGrado_in_dyn(0);

        matrix[i]= new  Arc[p->getNumNodi()];
    }
	
    //Inizializzazione dei puntatori del grafo
    nodes[0].SetNodo_up(NULL);
    nodes[0].SetNodo_up_dyn(NULL);
    nodes[0].SetNodo_down(&nodes[1]);
    nodes[0].SetNodo_down_dyn(&nodes[1]);

    grafo_iniziale = grafo_dinamico = nodes;
    
    for(i=1;i<numnodes-1;i++){  //<size-1   for(i=1;i<=size-1;i++)
        nodes[i].SetNodo_up(&nodes[i-1]);

        nodes[i].SetNodo_down(&nodes[i+1]);
        nodes[i].SetNodo_up_dyn(&nodes[i-1]);
        nodes[i].SetNodo_down_dyn(&nodes[i+1]);
    }

    nodes[i].SetNodo_up(&nodes[i-1]);
    nodes[i].SetNodo_down(NULL);
    nodes[i].SetNodo_up_dyn(&nodes[i-1]);
    nodes[i].SetNodo_down_dyn(NULL);
    
    // inizializzo la matrice degli archi e quella ausiliaria
    for ( i = 0; i < numnodes ; i++) {
        for ( j = 0; j < numnodes ; j++) {
            aux_matrice[i][j]=-1;
        }
    }

    // salvo nella matrice ausiliaria tutti quanti gli archi del grafo.
    for ( j = 0; j < p->getNumArchi(); j++) {
        aux_matrice[p->getArco(j).da-1][p->getArco(j).a-1]=p->getArco(j).color;
    }

    for(i=0;i<numnodes;i++){

        nodes[i].SetStato(in_G);
        nodes[i].SetStato_dyn(in_G);

        // costruisco le corone uscente ed entrante...
        for(j=0;j<numnodes;j++){
             // se ho trovato un arco del grafo
             if(aux_matrice[i][j] > 0 ){

							matrix[i][j].SetColor(aux_matrice[i][j]);
                    //if(i<j) 
                    this->numarcs++;
                    // se devo inserire il primo arco della corona uscente...
                    if(nodes[i].GetCorona()==NULL){
                        coda = tmp_arc = &matrix[i][j];
                        nodes[i].SetCorona(tmp_arc);
                        nodes[i].SetCorona_dyn(tmp_arc);

                        coda->SetNodeFrom(&nodes[i]);
                        coda->SetNodeTo(&nodes[j]);

                        coda->setNext_dyn(NULL);
                        coda->setNext(NULL);
                        coda->setPrev(NULL);
                        coda->setPrev_dyn(NULL);

                        coda->setStato(in_G);
                        coda->setStato_dyn(in_G);
                        coda->SetNeed(0);

                        //j++;
                        nodes[i].SetGrado(nodes[i].GetGrado()+1);
                    }
                    else{
                        tmp_arc = &matrix[i][j];
                        coda->setNext(tmp_arc);
                        coda->setNext_dyn(tmp_arc);

                        tmp_arc->SetNodeFrom(&nodes[i]);
                        tmp_arc->SetNodeTo(&nodes[j]);
                        
                        tmp_arc->setPrev(coda);
                        tmp_arc->setPrev_dyn(coda);
                        tmp_arc->setNext(NULL);
                        tmp_arc->setNext_dyn(NULL);

                        tmp_arc->setStato(in_G);
                        tmp_arc->setStato_dyn(in_G);
                        tmp_arc->SetNeed(0);
                        coda = coda->getNext();
                        nodes[i].SetGrado(nodes[i].GetGrado()+1);
                    }


                    // Costruisco anche la corona entrante 
                   if( (testa_in = nodes[j].GetCorona_in()) == NULL){
                        tmp_arc = &matrix[i][j];
                        nodes[j].SetCorona_in(tmp_arc);
                        nodes[j].SetCorona_in_dyn(tmp_arc);

                        tmp_arc->setNext_in_dyn(NULL);
                        tmp_arc->setNext_in(NULL);
                        tmp_arc->setPrev_in(NULL);
                        tmp_arc->setPrev_in_dyn(NULL);

                        nodes[j].SetGrado_in(nodes[j].GetGrado_in()+1);
                        nodes[j].SetGrado_in_dyn(nodes[j].GetGrado_in_dyn()+1);
                    }
                    else{
                        tmp_arc = &matrix[i][j];

                        tmp_arc->setNext_in_dyn(testa_in);
                        tmp_arc->setNext_in(testa_in);
                        tmp_arc->setPrev_in(NULL);
                        tmp_arc->setPrev_in_dyn(NULL);

                        testa_in->setPrev_in(tmp_arc);
                        testa_in->setPrev_in_dyn(tmp_arc);

                        nodes[j].SetCorona_in(tmp_arc);
                        nodes[j].SetCorona_in_dyn(tmp_arc);
                        nodes[j].SetGrado_in(nodes[j].GetGrado_in()+1);
                        nodes[j].SetGrado_in_dyn(nodes[j].GetGrado_in_dyn()+1);
                    }

             }
				 else{
					matrix[i][j].setStato(not_in_G);
					matrix[i][j].setStato_dyn(not_in_G);
				 }
        }
        nodes[i].SetGrado_dyn(nodes[i].GetGrado());
    }//for(i=0;i<size;i++)

	// inizializzo i valori per il grado minimo e massimo (sia entrante che uscente) nel grafo
	max_grado = max_grado_in = 0;
	min_grado = min_grado_in = p->getNumNodi()+1;
	
	for(i=0;i<numnodes;i++){
		if(max_grado < nodes[i].GetGrado()) max_grado = nodes[i].GetGrado();
		if(min_grado > nodes[i].GetGrado()) min_grado = nodes[i].GetGrado();
		if(max_grado_in < nodes[i].GetGrado_in() ) max_grado_in = nodes[i].GetGrado_in();
		if(min_grado_in > nodes[i].GetGrado_in() ) min_grado_in = nodes[i].GetGrado_in();
	}
	 
	delete p;
}


// Da completare perch non viene copiata la corona entrante
Graph* Graph::getCopy(){

	Graph *copy;
	copy = new Graph(this->numnodes);
    int x,y,i,j;
    
    copy->p = this->p;
    copy->numnodes = this->numnodes;
    copy->numarcs = this->numarcs;
	copy->filename = this->filename;
	

	for(i=0;i<this->numnodes;i++){

		copy->nodes[i].Copia(&this->nodes[i]);
		
		// dopo aver copiato i nodi e' necessario aggiornare i puntatori perche' quelli copiati
		// "puntano" al grafo originale e non alla copia che si sta costruendo
	
		if(this->nodes[i].nodo_up !=NULL)
			copy->nodes[i].nodo_up = copy->GetSingleNode(this->nodes[i].nodo_up->GetIndex());
		if(this->nodes[i].nodo_down !=NULL)			
			copy->nodes[i].nodo_down = copy->GetSingleNode(this->nodes[i].nodo_down->GetIndex());

		if(this->nodes[i].nodo_up_dyn !=NULL)
			copy->nodes[i].nodo_up_dyn = copy->GetSingleNode(this->nodes[i].nodo_up_dyn->GetIndex());
		if(this->nodes[i].nodo_down_dyn !=NULL)			
			copy->nodes[i].nodo_down_dyn = copy->GetSingleNode(this->nodes[i].nodo_down_dyn->GetIndex());

		if(this->nodes[i].corona != NULL){
			x = this->nodes[i].corona->GetNodeFrom()->GetIndex();
			y = this->nodes[i].corona->GetNodeTo()->GetIndex();
			copy->nodes[i].corona = &copy->matrix[x][y];
		}
		
		if(this->nodes[i].corona_dyn != NULL){
			x = this->nodes[i].corona_dyn->GetNodeFrom()->GetIndex();
			y = this->nodes[i].corona_dyn->GetNodeTo()->GetIndex();
			copy->nodes[i].corona_dyn = &copy->matrix[x][y];
		}
	}

	
	for(i=0;i<numnodes;i++){
		for(j=0;j<numnodes;j++){
			copy->matrix[i][j].Copia(&this->matrix[i][j]);
			
			if(this->matrix[i][j].nodeFrom != NULL){
				x = this->matrix[i][j].nodeFrom->GetIndex();
				copy->matrix[i][j].nodeFrom = &copy->nodes[x];			
			}
			
			if(this->matrix[i][j].nodeTo != NULL){
				x = this->matrix[i][j].nodeTo->GetIndex();
				copy->matrix[i][j].nodeTo = &copy->nodes[x];			
			}
			
			
			if(this->matrix[i][j].next != NULL){
				x = this->matrix[i][j].next->GetNodeFrom()->GetIndex();
				y = this->matrix[i][j].next->GetNodeTo()->GetIndex();
				copy->matrix[i][j].next = &copy->matrix[x][y];			
			}
			
			if(this->matrix[i][j].prev != NULL){
				x = this->matrix[i][j].prev->GetNodeFrom()->GetIndex();
				y = this->matrix[i][j].prev->GetNodeTo()->GetIndex();
				copy->matrix[i][j].prev = &copy->matrix[x][y];			
			}			
			
			if(this->matrix[i][j].next_dyn != NULL){
				x = this->matrix[i][j].next_dyn->GetNodeFrom()->GetIndex();
				y = this->matrix[i][j].next_dyn->GetNodeTo()->GetIndex();
				copy->matrix[i][j].next_dyn = &copy->matrix[x][y];			
			}
			
			if(this->matrix[i][j].prev_dyn != NULL){
				x = this->matrix[i][j].prev_dyn->GetNodeFrom()->GetIndex();
				y = this->matrix[i][j].prev_dyn->GetNodeTo()->GetIndex();
				copy->matrix[i][j].prev_dyn = &copy->matrix[x][y];			
			}						
		}
	}	
	
	copy->grafo_iniziale = copy->GetSingleNode(this->grafo_iniziale->GetIndex());
	copy->grafo_dinamico = copy->GetSingleNode(this->grafo_dinamico->GetIndex());

	return copy;
}

Graph::~Graph() {

	int i;
	for (i = 0; i < numnodes ; i++){
		delete []matrix[i];
	}
	
	delete []matrix;
	delete []nodes;   
}


Arc* Graph::getArc(Node* from,Node* to){

    return &matrix[from->GetIndex()][to->GetIndex()];
}


Arc* Graph::getArc(int from, int to){
    return &matrix[from][to];
}

// ATTENZIONE: Da completare perch non tiene conto della corona entrante
void Graph::shrink_permanente(int reduced){

    Node *nod,*ptr_nodo;
    Arc  **mat,*coda,*tmp_arc;
    int i,j,new_size, new_size_arc=0;

    cout  << "ATTENZIONE: E' stato attivato lo shrink del grafo" << endl;
    short supporto[numnodes];

    new_size = numnodes-reduced;

    nod = new Node[new_size];
    mat= new Arc*[new_size];

    for (i = 0; i < new_size ; i++){
          mat[i]= new  Arc[new_size];
    }

    ptr_nodo = grafo_iniziale;
    i=0;
    while(ptr_nodo!=NULL){
        
        supporto[ptr_nodo->GetIndex()]=i;
        nod[i].Copia(ptr_nodo);
        nod[i].SetGrado(0);
        nod[i].SetStato(in_G);
        nod[i].SetStato_dyn(in_G);
        nod[i].SetIndex(i);

        cout << "nodo = " << ptr_nodo->GetId() << " index_old= " << ptr_nodo->GetIndex() << " index_new= " << nod[i].GetIndex() << endl;

        i++;
        ptr_nodo = ptr_nodo->GetNodo_down();
    }

    //Inizializzazione dei puntatori del grafo
    nod[0].SetNodo_up(NULL);
    nod[0].SetNodo_up_dyn(NULL);
    nod[0].SetNodo_down(&nod[1]);
    nod[0].SetNodo_down_dyn(&nod[1]);

    //grafo_iniziale = grafo_dinamico = &nod[0];

    for(i=1;i<new_size-1;i++){  //<size-1   for(i=1;i<=size-1;i++)
        nod[i].SetNodo_up(&nod[i-1]);
        nod[i].SetNodo_down(&nod[i+1]);
        nod[i].SetNodo_up_dyn(&nod[i-1]);
        nod[i].SetNodo_down_dyn(&nod[i+1]);
    }

    nod[i].SetNodo_up(&nod[i-1]);
    nod[i].SetNodo_down(NULL);
    nod[i].SetNodo_up_dyn(&nod[i-1]);
    nod[i].SetNodo_down_dyn(NULL);

    int x,y;
//    ptr_nodo = grafo_iniziale;
//    while(ptr_nodo!=NULL){
//
//        ptr_arc = ptr_nodo->GetCorona();
//        while(ptr_arc!=NULL){
//            x = supporto[ptr_arc->GetNodeFrom()->GetIndex()];
//            y = supporto[ptr_arc->GetNodeTo()->GetIndex()];
//            mat[x][y].Copia(ptr_arc);
//            ptr_arc = ptr_arc->getNext();
//        }
//        ptr_nodo = ptr_nodo->GetNodo_down();
//
//    }


     for(i=0;i<numnodes;i++){
        // costruisco la corona uscente...
        for(j=0;j<numnodes;j++){
             // se ho trovato un arco del grafo
             if(matrix[i][j].getStato() == in_G){

                    x = supporto[matrix[i][j].GetNodeFrom()->GetIndex()];
                    y = supporto[matrix[i][j].GetNodeTo()->GetIndex()];
                    
                    //printf("Traslo l'arco (%d,%d)[%d,%d] nel nuovo arco (%d,%d)[%d,%d]\n",matrix[i][j].GetNodeFrom()->GetId(),matrix[i][j].GetNodeTo()->GetId(),i,j,nod[x].GetId(),nod[y].GetId(),x,y);

                    if(x<y) new_size_arc++;

                    coda = tmp_arc = &mat[x][y];
                    nod[x].SetCorona(tmp_arc);
                    nod[x].SetCorona_dyn(tmp_arc);

                    coda->SetNodeFrom(&nod[x]);
                    coda->SetNodeTo(&nod[y]);

                    //printf("Ho inserito come primo arco (%d,%d)\n",coda->GetNodeFrom()->GetId(),coda->GetNodeTo()->GetId());

                    coda->setNext_dyn(NULL);
                    coda->setNext(NULL);
                    coda->setPrev(NULL);
                    coda->setPrev_dyn(NULL);

                    coda->setStato(in_G);
                    coda->setStato_dyn(in_G);
                    coda->SetNeed(0);

                    j++;
                    nod[x].SetGrado(nod[x].GetGrado()+1);
                    break;
             }
        }

        // aggiungo i restanti archi della riga alla corona
        for(;j<numnodes;j++){
              if(matrix[i][j].getStato() == in_G){

                    x = supporto[matrix[i][j].GetNodeFrom()->GetIndex()];
                    y = supporto[matrix[i][j].GetNodeTo()->GetIndex()];

                    //printf("Traslo l'arco (%d,%d)[%d,%d] nel nuovo arco (%d,%d)[%d,%d]\n",matrix[i][j].GetNodeFrom()->GetId(),matrix[i][j].GetNodeTo()->GetId(),i,j,nod[x].GetId(),nod[y].GetId(),x,y);
                    
                    if(x<y) new_size_arc++;

                    tmp_arc = &mat[x][y];
                    coda->setNext(tmp_arc);
                    coda->setNext_dyn(tmp_arc);

                    tmp_arc->SetNodeFrom(&nod[x]);
                    tmp_arc->SetNodeTo(&nod[y]);

                    //printf("Ho inserito come arco successivo (%d,%d)\n",tmp_arc->GetNodeFrom()->GetId(),tmp_arc->GetNodeTo()->GetId());

                    tmp_arc->setPrev(coda);
                    tmp_arc->setPrev_dyn(coda);
                    tmp_arc->setNext(NULL);
                    tmp_arc->setNext_dyn(NULL);

                    tmp_arc->setStato(in_G);
                    tmp_arc->setStato_dyn(in_G);
                    tmp_arc->SetNeed(0);

                    coda = coda->getNext();
                    nod[x].SetGrado(nod[x].GetGrado()+1);
             }
        }


        nod[x].SetGrado_dyn(nod[x].GetGrado());
        //printf("\n");
        
    }//for(i=0;i<size;i++)
    
    // A questo punto bisogna prendere i campi del grafo originale e
    // farli puntare al nuovo grafo ridotto eliminado poi la struttura originale.

    delete [] nodes;
    for (i = 0; i < numnodes; i++){
          delete [] matrix[i];
    }
    delete [] matrix;
    
    grafo_iniziale = grafo_dinamico = nodes = nod;
    matrix = mat;

    numnodes = new_size;
    numarcs = new_size_arc;
}


void Graph::Rimuovi_Nodo(Node *nodo){


    Arc *ptr;

#ifdef safe_tests
    // se il nodo e' presente nel grafo
    if(nodo->GetStato() == in_G){
#endif
  
        // se sono il primo nodo della lista...
        if(nodo->GetNodo_up() == NULL){
                // ...ma anche l'ultimo...
                if(nodo->GetNodo_down() == NULL){
                        grafo_iniziale = NULL;
                }
                else{
                        grafo_iniziale = nodo->GetNodo_down();
                        nodo->GetNodo_down()->SetNodo_up(NULL);
                }
        }    // ...altrimenti se sono l'ultimo nodo della lista...
        else if(nodo->GetNodo_down() == NULL){
                nodo->GetNodo_up()->SetNodo_down(NULL);
        }
        else{   //...infine se solo un nodo nel mezzo...
                nodo->GetNodo_up()->SetNodo_down(nodo->GetNodo_down());
                nodo->GetNodo_down()->SetNodo_up(nodo->GetNodo_up());
        }

        nodo->SetStato(not_in_G);
 
        ptr = nodo->GetCorona();
        while(ptr!=NULL){

                Rimuovi_Arco(ptr);
                ptr = ptr->getNext();

        }
#ifdef safe_tests
    }
    else{
        cout << "[Rimuovi_Nodo] ERRORE: si sta cercando di cancellare il nodo "<< nodo->GetId() << " che non presente nel grafo dinamico!!!" << endl << endl;
        exit(1);
    }
#endif

}

void Graph::Rimuovi_Arco(Arc* arc){


    Arc *arc2;
    //cout << "Sto per rimuovere l'arco " << arc->GetNodeFrom()->GetId() << "," << arc->GetNodeTo()->GetId() << endl;

#ifdef safe_tests
    if(arc->getStato() == in_G){
#endif

        // se sono il primo arco della corona...
        if(arc->getPrev()==NULL){
                //...ma anche l'unico
                if(arc->getNext()==NULL){
                    nodes[arc->GetNodeFrom()->GetIndex()].SetCorona(NULL);
                }
                else{
                        nodes[arc->GetNodeFrom()->GetIndex()].SetCorona(arc->getNext());
                        arc->getNext()->setPrev(NULL);
                }

        }// ...altrimenti se sono l'ultimo arco della corona...
        else if(arc->getNext() == NULL){
            arc->getPrev()->setNext(NULL);
        }
        else{
            arc->getPrev()->setNext(arc->getNext());
            arc->getNext()->setPrev(arc->getPrev());
        }

        nodes[arc->GetNodeFrom()->GetIndex()].SetGrado(nodes[arc->GetNodeFrom()->GetIndex()].GetGrado()-1);

        arc->setStato(not_in_G);

        // rimuovo l'arco anche dalla corona entrante di v
        if(arc->getPrev_in()==NULL){
                //...ma anche l'unico
                if(arc->getNext_in()==NULL){
                    nodes[arc->GetNodeTo()->GetIndex()].SetCorona_in(NULL);
                }
                else{
                        nodes[arc->GetNodeTo()->GetIndex()].SetCorona_in(arc->getNext_in());
                        arc->getNext_in()->setPrev_in(NULL);
                }

        }// ...altrimenti se sono l'ultimo arco della corona...
        else if(arc->getNext_in() == NULL){
            arc->getPrev_in()->setNext_in(NULL);
        }
        else{
            arc->getPrev_in()->setNext_in(arc->getNext_in());
            arc->getNext_in()->setPrev_in(arc->getPrev_in());
        }

        nodes[arc->GetNodeTo()->GetIndex()].SetGrado_in(nodes[arc->GetNodeTo()->GetIndex()].GetGrado_in()-1);

        // se il grafo non e' orientato....
        if(is_directed==0){

            // ... e rimuovo anche l'arco inverso
            arc2 = &matrix[arc->GetNodeTo()->GetIndex()][arc->GetNodeFrom()->GetIndex()];
            
            // se sono il primo arco della corona...
            if(arc2->getPrev()==NULL){
                    //...ma anche l'unico
                    if(arc2->getNext()==NULL){
                        nodes[arc2->GetNodeFrom()->GetIndex()].SetCorona(NULL);
                    }
                    else{
                            nodes[arc2->GetNodeFrom()->GetIndex()].SetCorona(arc2->getNext());
                            arc2->getNext()->setPrev(NULL);
                    }

            }// ...altrimenti se sono l'ultimo arco della corona...
            else if(arc2->getNext() == NULL){
                arc2->getPrev()->setNext(NULL);
            }
            else{
                arc2->getPrev()->setNext(arc2->getNext());
                arc2->getNext()->setPrev(arc2->getPrev());
            }

            nodes[arc2->GetNodeFrom()->GetIndex()].SetGrado(nodes[arc2->GetNodeFrom()->GetIndex()].GetGrado()-1);

            arc2->setStato(not_in_G);

            // rimuovo l'arco anche dalla corona entrante di v
            if(arc2->getPrev_in()==NULL){
                    //...ma anche l'unico
                    if(arc2->getNext_in()==NULL){
                        nodes[arc2->GetNodeTo()->GetIndex()].SetCorona_in(NULL);
                    }
                    else{
                            nodes[arc2->GetNodeTo()->GetIndex()].SetCorona_in(arc2->getNext_in());
                            arc2->getNext_in()->setPrev_in(NULL);
                    }

            }// ...altrimenti se sono l'ultimo arc2o della corona...
            else if(arc2->getNext_in() == NULL){
                arc2->getPrev_in()->setNext_in(NULL);
            }
            else{
                arc2->getPrev_in()->setNext_in(arc2->getNext_in());
                arc2->getNext_in()->setPrev_in(arc2->getPrev_in());
            }

            nodes[arc2->GetNodeTo()->GetIndex()].SetGrado_in(nodes[arc2->GetNodeTo()->GetIndex()].GetGrado_in()-1);


        }
#ifdef safe_tests
    }
    else{
        cout << "[Rimuovi_Arco] ERRORE: si sta cercando di cancellare l'arco ("<< arc->GetNodeFrom()->GetId() << "," << arc->GetNodeTo()->GetId() << ") che non presente nel grafo iniziale!!!" << endl << endl;
        exit(1);
    }
#endif

}

void Graph::Rimuovi_Nodo_Dyn(Node *nodo){

//#define Rimuovi_Nodo_Dyn_DB_P

    Arc *ptr;

#ifdef safe_tests
    // se il nodo e' presente nel grafo
    if(nodo->GetStato_dyn() == in_G){
#endif


#ifdef Rimuovi_Nodo_Dyn_DB_P
        cout << "[Rimuovi_Nodo_Dyn]: Sto per cancellare il nodo " << nodo->GetId() << endl;
#endif
        
        // se sono il primo nodo della lista...
        if(nodo->GetNodo_up_dyn() == NULL){
                // ...ma anche l'ultimo...
                if(nodo->GetNodo_down_dyn() == NULL){
                        grafo_dinamico = NULL;
                }
                else{
                        grafo_dinamico = nodo->GetNodo_down_dyn();
                        nodo->GetNodo_down_dyn()->SetNodo_up_dyn(NULL);
                }
        }    // ...altrimenti se sono l'ultimo nodo della lista...
        else if(nodo->GetNodo_down_dyn() == NULL){
                nodo->GetNodo_up_dyn()->SetNodo_down_dyn(NULL);
        }
        else{   //...infine se solo un nodo nel mezzo...
                nodo->GetNodo_up_dyn()->SetNodo_down_dyn(nodo->GetNodo_down_dyn());
                nodo->GetNodo_down_dyn()->SetNodo_up_dyn(nodo->GetNodo_up_dyn());
        }

        

        ptr = nodo->GetCorona_dyn();
        while(ptr!=NULL){

                Rimuovi_Arco_Dyn(ptr);
                ptr = ptr->getNext_dyn();

        }

        nodo->SetStato_dyn(not_in_G);

#ifdef safe_tests
    }
    else{
        cout << "[Rimuovi_Nodo_Dyn] ERRORE: si sta cercando di cancellare il nodo "<< nodo->GetId() << " che non presente nel grafo dinamico!!!" << endl << endl;
       
        exit(1);
    }
#endif
}



void Graph::Rimuovi_Arco_Dyn(Arc* arc){

    Arc *arc2;

#ifdef safe_tests
    if(arc->getStato_dyn() == in_G){
#endif

        // se sono il primo arco della corona...
        if(arc->getPrev_dyn()==NULL){
                //...ma anche l'unico
                if(arc->getNext_dyn()==NULL){
                    nodes[arc->GetNodeFrom()->GetIndex()].SetCorona_dyn(NULL);
                }
                else{
                        nodes[arc->GetNodeFrom()->GetIndex()].SetCorona_dyn(arc->getNext_dyn());
                        arc->getNext_dyn()->setPrev_dyn(NULL);
                }

        }// ...altrimenti se sono l'ultimo arco della corona...
        else if(arc->getNext_dyn() == NULL){
            arc->getPrev_dyn()->setNext_dyn(NULL);
        }
        else{
            arc->getPrev_dyn()->setNext_dyn(arc->getNext_dyn());
            arc->getNext_dyn()->setPrev_dyn(arc->getPrev_dyn());
        }

        nodes[arc->GetNodeFrom()->GetIndex()].SetGrado_dyn(nodes[arc->GetNodeFrom()->GetIndex()].GetGrado_dyn()-1);

        arc->setStato_dyn(not_in_G);

        // rimuovo l'arco anche dalla corona entrante di v
        if(arc->getPrev_in_dyn()==NULL){
                //...ma anche l'unico
                if(arc->getNext_in_dyn()==NULL){
                    nodes[arc->GetNodeTo()->GetIndex()].SetCorona_in_dyn(NULL);
                }
                else{
                        nodes[arc->GetNodeTo()->GetIndex()].SetCorona_in_dyn(arc->getNext_in_dyn());
                        arc->getNext_in_dyn()->setPrev_in_dyn(NULL);
                }

        }// ...altrimenti se sono l'ultimo arco della corona...
        else if(arc->getNext_in_dyn() == NULL){
            arc->getPrev_in_dyn()->setNext_in_dyn(NULL);
        }
        else{
            arc->getPrev_in_dyn()->setNext_in_dyn(arc->getNext_in_dyn());
            arc->getNext_in_dyn()->setPrev_in_dyn(arc->getPrev_in_dyn());
        }

        nodes[arc->GetNodeTo()->GetIndex()].SetGrado_in_dyn(nodes[arc->GetNodeTo()->GetIndex()].GetGrado_in_dyn()-1);

        // se il grafo non e' orientato....
        if(is_directed==0){
            // ... e rimuovo anche l'arco inverso

            arc2 = &matrix[arc->GetNodeTo()->GetIndex()][arc->GetNodeFrom()->GetIndex()];

            // se sono il primo arco della corona...
            if(arc2->getPrev_dyn()==NULL){
                    //...ma anche l'unico
                    if(arc2->getNext_dyn()==NULL){
                        nodes[arc2->GetNodeFrom()->GetIndex()].SetCorona_dyn(NULL);
                    }
                    else{
                            nodes[arc2->GetNodeFrom()->GetIndex()].SetCorona_dyn(arc2->getNext_dyn());
                            arc2->getNext_dyn()->setPrev_dyn(NULL);
                    }

            }// ...altrimenti se sono l'ultimo arco della corona...
            else if(arc2->getNext_dyn() == NULL){
                arc2->getPrev_dyn()->setNext_dyn(NULL);
            }
            else{
                arc2->getPrev_dyn()->setNext_dyn(arc2->getNext_dyn());
                arc2->getNext_dyn()->setPrev_dyn(arc2->getPrev_dyn());
            }

            nodes[arc2->GetNodeFrom()->GetIndex()].SetGrado_dyn(nodes[arc2->GetNodeFrom()->GetIndex()].GetGrado_dyn()-1);

            arc2->setStato_dyn(not_in_G);

            // rimuovo l'arco anche dalla corona entrante di v
            if(arc2->getPrev_in_dyn()==NULL){
                    //...ma anche l'unico
                    if(arc2->getNext_in_dyn()==NULL){
                        nodes[arc2->GetNodeTo()->GetIndex()].SetCorona_in_dyn(NULL);
                    }
                    else{
                            nodes[arc2->GetNodeTo()->GetIndex()].SetCorona_in_dyn(arc2->getNext_in_dyn());
                            arc2->getNext_in_dyn()->setPrev_in_dyn(NULL);
                    }

            }// ...altrimenti se sono l'ultimo arc2o della corona...
            else if(arc2->getNext_in_dyn() == NULL){
                arc2->getPrev_in_dyn()->setNext_in_dyn(NULL);
            }
            else{
                arc2->getPrev_in_dyn()->setNext_in_dyn(arc2->getNext_in_dyn());
                arc2->getNext_in_dyn()->setPrev_in_dyn(arc2->getPrev_in_dyn());
            }

            nodes[arc2->GetNodeTo()->GetIndex()].SetGrado_in_dyn(nodes[arc2->GetNodeTo()->GetIndex()].GetGrado_in_dyn()-1);
        }
#ifdef safe_tests
    }
    else{
        cout << "[Rimuovi_Arco_Dyn] ERRORE: si sta cercando di cancellare l'arco ("<< arc->GetNodeFrom()->GetId() << "," << arc->GetNodeTo()->GetId() << ") che non presente nel grafo dinamico!!!" << endl << endl;
        exit(1);
    }
#endif

}


void Graph::Visualizza_Grafo(short flag){

	Node *tmp;
	Arc *ptr;
        int num_nodi=0,num_archi=0;
	printf("\nGrafo Iniziale:\t [numnodes= %d, numarcs= %d, numcolors= %d]\n\n",numnodes,numarcs,numcolors);

	tmp = grafo_iniziale;
	while(tmp!=NULL){
            printf("Nodo[%d] = %d\t[peso=%g, grado=%d, stato=%d]\n",tmp->GetIndex(),tmp->GetId(),tmp->GetPeso(),tmp->GetGrado(),tmp->GetStato());
            num_nodi++;
            if(flag==3){
                printf("\tFS: ");
                ptr=tmp->GetCorona();
                while(ptr!=NULL){
                      printf("(%d,%d)=%d, ",ptr->GetNodeFrom()->GetId(),ptr->GetNodeTo()->GetId(),ptr->GetColor());
                      num_archi++;
                      ptr=ptr->getNext();
                }
                printf("\n");				
            }
				
            else if(flag==2){
                printf("\tFS: ");
                ptr=tmp->GetCorona();
                while(ptr!=NULL){
                      printf("(%d,%d), ",ptr->GetNodeFrom()->GetId(),ptr->GetNodeTo()->GetId());
                      num_archi++;
                      ptr=ptr->getNext();
                }
                printf("\n");
                printf("\tBS: ");
                ptr=tmp->GetCorona_in();

                while(ptr!=NULL){
                      printf("(%d,%d), ",ptr->GetNodeFrom()->GetId(),ptr->GetNodeTo()->GetId());
                      ptr=ptr->getNext_in();
                }
                printf("\n");                

            }
            else if(flag==1){
                printf("\tFS: ");
                ptr=tmp->GetCorona();
                while(ptr!=NULL){
                      printf("(%d,%d), ",ptr->GetNodeFrom()->GetId(),ptr->GetNodeTo()->GetId());
                      num_archi++;
                      ptr=ptr->getNext();
                }
                printf("\n");
            }
            else{
                ptr=tmp->GetCorona();
                while(ptr!=NULL){
                      num_archi++;
                      ptr=ptr->getNext();
                }
            }
            tmp = tmp->GetNodo_down();
	}
    printf("\n Num_Nodi = %d, Num_archi = %d\n\n",num_nodi, num_archi/2);

}

void Graph::Visualizza_Grafo_Dyn(short flag){

    Node *tmp;
    Arc *ptr;

    int num_nodi=0,num_archi=0;

    printf("\nGrafo Dinamico:\n\n");

    tmp = grafo_dinamico;
    while(tmp!=NULL){
            printf("Nodo[%d] = %d\t[peso_dyn=%g, grado_dyn=%d, stato_dyn=%d]\n",tmp->GetIndex(),tmp->GetId(),tmp->GetPeso_dyn(),tmp->GetGrado_dyn(),tmp->GetStato_dyn());
            num_nodi++;
            if(flag==2){
                printf("\tFS: ");
                ptr=tmp->GetCorona_dyn();
                while(ptr!=NULL){
                      printf("(%d,%d), ",ptr->GetNodeFrom()->GetId(),ptr->GetNodeTo()->GetId());
                      num_archi++;
                      ptr=ptr->getNext_dyn();
                }
                printf("\n");
                printf("\tBS: ");
                ptr=tmp->GetCorona_in_dyn();
                while(ptr!=NULL){
                      printf("(%d,%d), ",ptr->GetNodeFrom()->GetId(),ptr->GetNodeTo()->GetId());
                      ptr=ptr->getNext_in_dyn();
                }
                printf("\n");

            }
            else if(flag==1){
                printf("\tFS: ");
                ptr=tmp->GetCorona_dyn();
                while(ptr!=NULL){
                      printf("(%d,%d), ",ptr->GetNodeFrom()->GetId(),ptr->GetNodeTo()->GetId());
                      num_archi++;
                      ptr=ptr->getNext_dyn();
                }
                printf("\n");
            }
            else{
                ptr=tmp->GetCorona_dyn();
                while(ptr!=NULL){
                      num_archi++;
                      ptr=ptr->getNext_dyn();
                }
            }
            tmp = tmp->GetNodo_down_dyn();
    }

    printf("\n Num_Nodi = %d, Num_archi = %d\n\n",num_nodi, num_archi/2);
}


void Graph::Riduzione_zero_uno(short flag, Node** eliminati, int starting_point, int *num_eliminati){

    // La funzione individua e rimuove tutti quanti i nodi di grado zero e uno nel grafo. I nodi rimossi
    // vengono salvati nell'array eliminati a partire dalla posizione starting_point ed il numero totale
    // di nodi cancellati viene memorizzato in num_eliminati.

//#define Riduzione_zero_uno_DB

    Node* nodo;
    //int minus_grado[numnodes];
    int i;
    //Heap h(numnodes);
    Arc *ptr_arc;

    //memset(minus_grado,0,numnodes*sizeof(int));

    *num_eliminati = starting_point;

    if(flag==G_Init){
        
       nodo = grafo_iniziale;
       while(nodo!=NULL){
           // vado ad individuare al primo passaggio sull'array dei nodi, tutti quanti quelli di
           // grado zero ed uno e li inserisco nella struttura di appoggio passata in input.
           if(nodo->GetGrado() < 2){
           //if(nodo->GetGrado()-minus_grado[nodo->GetIndex()] < 2){
              
                eliminati[*num_eliminati] = nodo;
                *num_eliminati = *num_eliminati+1;

           }

           nodo = nodo->GetNodo_down();
       }

       // a questo punto comincio a cancellare i nodi salvati nella struttura di appoggio e ogni volta
       // controllo se la loro cancellazione porta ad 1 il valore del grado di un suo vicino.
       i=starting_point;
       for(;i<*num_eliminati;i++){
           ptr_arc = eliminati[i]->GetCorona();
           while(ptr_arc!=NULL){
                if(ptr_arc->GetNodeTo()->GetGrado()-1 == 1){
                    eliminati[*num_eliminati] = ptr_arc->GetNodeTo();
                    *num_eliminati = *num_eliminati+1;
                }
                ptr_arc = ptr_arc->getNext();
           }
           Rimuovi_Nodo(eliminati[i]);
       }
    }// if(flag==G_Init)
    else{
       nodo = grafo_dinamico;

       while(nodo!=NULL){
           // vado ad individuare al primo passaggio sull'array dei nodi, tutti quanti quelli di
           // grado zero ed uno e li inserisco nella struttura di appoggio passata in input.
           if(nodo->GetGrado_dyn() < 2){

#ifdef Riduzione_zero_uno_DB
                cout << "[riduzione_zero_Uno] Inserisco tra i nodi da cancellare il nodo " << nodo->GetId() << endl;
#endif
                
                eliminati[*num_eliminati] = nodo;
                *num_eliminati = *num_eliminati+1;
           }

           nodo = nodo->GetNodo_down_dyn();
       }

       // a questo punto comincio a cancellare i nodi salvati nella struttura di appoggio e ogni volta
       // controllo se la loro cancellazione porta ad 1 il valore del grado di un suo vicino.
       i=starting_point;
       for(;i<*num_eliminati;i++){
           //cout << "Cancello il nodo " << eliminati[i]->GetId() << "che marca come da cancellare i nodi" << endl;
           ptr_arc = eliminati[i]->GetCorona_dyn();
           while(ptr_arc!=NULL){
                if(ptr_arc->GetNodeTo()->GetGrado_dyn()-1 == 1){
                    //cout << ptr_arc->GetNodeTo()->GetId() << ",";
                    eliminati[*num_eliminati] = ptr_arc->GetNodeTo();
                    *num_eliminati = *num_eliminati+1;
                }
                ptr_arc = ptr_arc->getNext_dyn();
           }
           //cout << endl;
           Rimuovi_Nodo_Dyn(eliminati[i]);
       }
    }

    *num_eliminati -= starting_point;

}


void Graph::Inserisci_Nodo_Dyn(Node *nodo){

    Arc *ptr_arc;

#ifdef safe_tests
    if(nodo->GetStato_dyn() == not_in_G){
#endif

       if(grafo_dinamico==NULL){
          grafo_dinamico=nodo;
          nodo->SetNodo_up_dyn(NULL);
          nodo->SetNodo_down_dyn(NULL);
       }
       else{
          nodo->SetNodo_up_dyn(NULL);
          nodo->SetNodo_down_dyn(grafo_dinamico);
          grafo_dinamico->SetNodo_up_dyn(nodo);
          grafo_dinamico = nodo;
       }

       nodo->SetStato_dyn(in_G);

       nodo->SetCorona_dyn(NULL);
       ptr_arc = nodo->GetCorona();
       while(ptr_arc!=NULL){

            // se l'altro estremo si trova nella foresta...
            if( ptr_arc->GetNodeTo()->GetStato_dyn() == in_G){

                    Inserisci_Arco_Dyn(ptr_arc);
            }

           ptr_arc = ptr_arc->getNext();
       }

#ifdef safe_tests
    }// if(nodo->GetStato_dyn()==0)
    else{
        cout << "[Inserisci_Nodo_Dyn] ERRORE: si sta cercando di inserire il nodo "<< nodo->GetId() << " che e' gia' presente nel grafo dinamico!!!" << endl << endl;
        exit(1);
    }
#endif
    
}

void Graph::Inserisci_Arco_Dyn(Arc* arc){


    //Node *nodo_i;
    Arc *arc2;

#ifdef safe_tests
    if(arc->getStato_dyn() == not_in_G){
#endif

        // se il nodo di partenza non ha archi nella corona dinamica...
        if(arc->GetNodeFrom()->GetCorona_dyn()==NULL){
           arc->GetNodeFrom()->SetCorona_dyn(arc);
           arc->setNext_dyn(NULL);
           arc->setPrev_dyn(NULL);
        }
        else{
            arc->GetNodeFrom()->GetCorona_dyn()->setPrev_dyn(arc);
            arc->setNext_dyn(arc->GetNodeFrom()->GetCorona_dyn());
            arc->setPrev_dyn(NULL);
            arc->GetNodeFrom()->SetCorona_dyn(arc);
        }

        arc->setStato_dyn(in_G);
        arc->GetNodeFrom()->SetGrado_dyn(arc->GetNodeFrom()->GetGrado_dyn()+1);

        // se il nodo di destinzione non ha archi nella corona entrante...
        if(arc->GetNodeTo()->GetCorona_in_dyn()==NULL){
           arc->GetNodeTo()->SetCorona_in_dyn(arc);
           arc->setNext_in_dyn(NULL);
           arc->setPrev_in_dyn(NULL);
        }
        else{
            arc->GetNodeTo()->GetCorona_in_dyn()->setPrev_in_dyn(arc);
            arc->setNext_in_dyn(arc->GetNodeTo()->GetCorona_in_dyn());
            arc->setPrev_in_dyn(NULL);
            arc->GetNodeTo()->SetCorona_in_dyn(arc);
        }

        arc->GetNodeTo()->SetGrado_in_dyn(arc->GetNodeTo()->GetGrado_in_dyn()+1);


        // se il grafo non e' orientato....
        if(is_directed==0){
            // l'arco va inserito anche nella corona dinamica dell'altro estremo dell'arco
            arc2 = &matrix[arc->GetNodeTo()->GetIndex()][arc->GetNodeFrom()->GetIndex()];
            
            if(arc2->GetNodeFrom()->GetCorona_dyn()==NULL){
                arc2->GetNodeFrom()->SetCorona_dyn(arc2);
                arc2->setNext_dyn(NULL);
                arc2->setPrev_dyn(NULL);
            }
            else{
                arc2->GetNodeFrom()->GetCorona_dyn()->setPrev_dyn(arc2);
                arc2->setNext_dyn(arc2->GetNodeFrom()->GetCorona_dyn());
                arc2->setPrev_dyn(NULL);
                arc2->GetNodeFrom()->SetCorona_dyn(arc2);
            }
            
            arc2->setStato_dyn(in_G);
            arc2->GetNodeFrom()->SetGrado_dyn(arc2->GetNodeFrom()->GetGrado_dyn()+1);

            // se il nodo di destinzione non ha archi nella corona entrante...
            if(arc2->GetNodeTo()->GetCorona_in_dyn()==NULL){
               arc2->GetNodeTo()->SetCorona_in_dyn(arc2);
               arc2->setNext_in_dyn(NULL);
               arc2->setPrev_in_dyn(NULL);
            }
            else{
                arc2->GetNodeTo()->GetCorona_in_dyn()->setPrev_in_dyn(arc2);
                arc2->setNext_in_dyn(arc2->GetNodeTo()->GetCorona_in_dyn());
                arc2->setPrev_in_dyn(NULL);
                arc2->GetNodeTo()->SetCorona_in_dyn(arc2);
            }

            arc2->GetNodeTo()->SetGrado_in_dyn(arc2->GetNodeTo()->GetGrado_in_dyn()+1);
        }
#ifdef safe_tests
    }
    else{
        cout << "[Inserisci_Arco_Dyn] ERRORE: si sta cercando di inserire l'arco ("<< arc->GetNodeFrom()->GetId() << "," << arc->GetNodeTo()->GetId() << ") che e' gia presente nel grafo dinamico!!!" << endl << endl;
        exit(1);
    }
#endif

}

void Graph::Inserisci_Arco(Arc* arc){


    //Node *nodo_i;
    Arc *arc2;

#ifdef safe_tests
    if(arc->getStato() == not_in_G){
#endif

        // se il nodo di partenza non ha archi nella corona ...
        if(arc->GetNodeFrom()->GetCorona()==NULL){
           arc->GetNodeFrom()->SetCorona(arc);
           arc->setNext(NULL);
           arc->setPrev(NULL);
        }
        else{
            arc->GetNodeFrom()->GetCorona()->setPrev(arc);
            arc->setNext(arc->GetNodeFrom()->GetCorona());
            arc->setPrev(NULL);
            arc->GetNodeFrom()->SetCorona(arc);
        }

        arc->setStato(in_G);
        arc->GetNodeFrom()->SetGrado(arc->GetNodeFrom()->GetGrado()+1);

        // se il nodo di destinzione non ha archi nella corona entrante...
        if(arc->GetNodeTo()->GetCorona_in()==NULL){
           arc->GetNodeTo()->SetCorona_in(arc);
           arc->setNext_in(NULL);
           arc->setPrev_in(NULL);
        }
        else{
            arc->GetNodeTo()->GetCorona_in()->setPrev_in(arc);
            arc->setNext_in(arc->GetNodeTo()->GetCorona_in());
            arc->setPrev_in(NULL);
            arc->GetNodeTo()->SetCorona_in(arc);
        }

        arc->GetNodeTo()->SetGrado_in(arc->GetNodeTo()->GetGrado_in()+1);


        // se il grafo non e' orientato....
        if(is_directed==0){
            // l'arco va inserito anche nella corona dinamica dell'altro estremo dell'arco
            arc2 = &matrix[arc->GetNodeTo()->GetIndex()][arc->GetNodeFrom()->GetIndex()];
            
            if(arc2->GetNodeFrom()->GetCorona()==NULL){
                arc2->GetNodeFrom()->SetCorona(arc2);
                arc2->setNext(NULL);
                arc2->setPrev(NULL);
            }
            else{
                arc2->GetNodeFrom()->GetCorona()->setPrev(arc2);
                arc2->setNext(arc2->GetNodeFrom()->GetCorona());
                arc2->setPrev(NULL);
                arc2->GetNodeFrom()->SetCorona(arc2);
            }
            
            arc2->setStato(in_G);
            arc2->GetNodeFrom()->SetGrado(arc2->GetNodeFrom()->GetGrado()+1);

            // se il nodo di destinzione non ha archi nella corona entrante...
            if(arc2->GetNodeTo()->GetCorona_in()==NULL){
               arc2->GetNodeTo()->SetCorona_in(arc2);
               arc2->setNext_in(NULL);
               arc2->setPrev_in(NULL);
            }
            else{
                arc2->GetNodeTo()->GetCorona_in()->setPrev_in(arc2);
                arc2->setNext_in(arc2->GetNodeTo()->GetCorona_in());
                arc2->setPrev_in(NULL);
                arc2->GetNodeTo()->SetCorona_in(arc2);
            }

            arc2->GetNodeTo()->SetGrado_in(arc2->GetNodeTo()->GetGrado_in()+1);

        }
#ifdef safe_tests
    }
    else{
        cout << "[Inserisci_Arco_Dyn] ERRORE: si sta cercando di inserire l'arco ("<< arc->GetNodeFrom()->GetId() << "," << arc->GetNodeTo()->GetId() << ") che e' gia presente nel grafo dinamico!!!" << endl << endl;
        exit(1);
    }
#endif

}

