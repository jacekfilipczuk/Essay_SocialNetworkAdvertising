/*
 * Smart_clique
 *
        programmed by: 	Francesco Carrabs
                        University of Salerno (UniSa) - Italy

 * 	CODING START:
                5.05.2011

        LAST MODIFIED:
                ?.?.2011
 */


#include <sys/types.h>
#include <sys/times.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <math.h>

#include "../../Graph/Graph.h"
#include "../../Graph/Heap.h"

#include <iomanip>
#include <signal.h>

#define DIRECTED 1 
#define UNDIRECTED 0

#define MBV 0
#define FVS 1
#define CLQ 2

//#define Performance 1
//#define DEBUG 1


// TODO: Scrivere la funzione per deallocare la memoria delle clique.

//typedef struct Cella_Incidenze_st {
//    short era_nella_clique;
//    int incidenza;
//} Cella_Incidenze;

typedef struct Nodo_Clique_st {
    short era_nella_clique_iniziale; // serve nella fase di plateau per sapere quali nodi erano nella clique passata al plateau
    int incidenza; // per ogni nodo del grafo viene riportato il numero di nodi della clique a cui e' incidente.
    short e_nella_clique;
    Nodo_Clique_st *clique_next, *clique_prev;
    Nodo_Clique_st *graph_next, *graph_prev;
    Node *nodo_ptr;
} Nodo_Clique;

typedef struct Clique_st {
    int size, colori_nella_clique;
    int dim_array, dim_array_occorrenze; // N.B. dim_array_occorrenze e' pari al numero di colori totali +1 perche' non essendoci il colore zero
    //      la corrispettiva casella non viene usata.
    Nodo_Clique *testa_clique;
    Nodo_Clique *testa_graph;
    Nodo_Clique *array_nodi;
    int *occorrenze_colori;
} Clique;

// Variabili globali
Graph *g;
Clique Best, *C_Best;
//Lista_Candidati NLC,NIC;
Heap *NLC, *NIC;
int numSteps = 0;
short print_option = 0;
double t;
int nodi_cancellati=0;
bool trovata_new_max_clique;
long miglioramenti_cp=0;
//Cella_Incidenze *incidenze;

int budget, tcs, penalty_delay, maxSteps;

//int top_archi_sel,top_archi_scar;
//short print_option;
//long nodi_visitati=0;
int numNodes, numColors, numArcs, numNodes_dyn;
//int *frequenza_colori;
int *tmp_occorrenze;

float timer() {
    struct tms hold;

    times(&hold);
    return (float) (hold.tms_utime) / sysconf(_SC_CLK_TCK);
}

int i_random(long lower, long upper) {
    int valore;
    double frazione;
    do {
        frazione = (double) rand() / RAND_MAX;
        valore = (int) lower + ((upper - lower + 1) * frazione);
    } while (frazione == 1);

    return valore;
}

/***************************************
 * functioni per la gestione dell'heap *
 ***************************************/


typedef struct Coppia_st {
    Node *da_rimuovere, *da_inserire;
    double key;
    long heap_pos; /* number of position in the heap */
} Coppia;

typedef /* heap */
struct my_heap_st {
    int h_degree;
    Coppia_st *heap; /* heap of the pointers to nodes       */
    long heap_size; /* the number of the last heap element */
} my_heap;

my_heap NIL; // definizione dell'heap


void My_Init_heap(int size, my_heap *z);

void My_Delete_Heap(my_heap *z);

void My_put_to_pos_in_heap(Coppia* ptr, int pos, my_heap *z);

bool My_non_empty_heap(my_heap *z);

void My_heap_Decrease_key(Coppia* ptr, double key_value, my_heap *z);

void My_Insert_to_heap(Coppia* ptr, my_heap *z);

Coppia* My_Extract_Min(my_heap *z);

void My_Remove_From_Heap(int pos, my_heap *z);

void My_Stampa_Heap(my_heap *z);

void My_Clean_Heap(my_heap *z);

/******************************
 * FINE LOCAL HEAP             *
 ******************************/

void Stampa_NIC_NLC(Heap *h);
void Stampa_NIL(my_heap *h);


void Recupera_Dati_Memoria(char *inputfile, float *vmpeak, float *vmsize, float *vmhwn, float *vmrss, float *vmdata, float *vmstk, float *vmlib);
void Stampa_Soluzione(Clique *C, short flag);

void GestoreKill(int sig) {

    Stampa_Soluzione(C_Best, print_option);
    exit(0);
}

void Quicksort(int a[], long begin, long end);
int Partition(int a[], long p, long r);
int Confronta(int a[], int i, int j);
void Swap(int a[], int i, int j);

void Dynamic_Local_Search(Graph *g, Clique *C, Clique *C1, bool dalla_terza_fase);

void Inizializza_Clique(Clique *ptr_C, int n, int ncolors);
void Copia_Clique(Clique *originale, Clique *copia);
void Inserisci_in_C(Node *ptr, Clique *C);
bool Swap_Nodi_C(Node *da_rimuovere, Node *da_inserire, Clique *C);
bool Swap_Nodi_NIL(Node *da_rimuovere, Node *da_inserire, Clique *C);
void Rimuovi_da_C(Node *ptr, Clique *C);
void Stampa_Clique(Clique *C, short flag);
void Svuota_Clique(Clique *C);
void Update_Penalties(int pd, Clique *C);
void Inserisci_e_Rimuovi_Inconsistenti(Node *ptr, Clique *C);

Node *Find_nodo_in_G(int pos, Clique *C);
Node *Expand(Clique *, bool build_NIC);
Node *Plateau(Clique *C, Clique *C1);
Clique *Colored_Plateau(Clique *C_Best3, Coppia *coppia_swap);
bool Intersezione(Clique *C, Clique *C1);
bool Verifica_Clique(Clique *C);

int Incrementa_Occorrenze_Colori(Node *da_inserire, Clique *C);
int Decrementa_Occorrenze_Colori(Node *da_inserire, Clique *C);
int Verifica_Costo_Inserimento(Node *da_inserire, Clique *C);
int Verifica_Costo_Rimozione(Node *da_rimuovere, Clique *C);
int Verifica_Costo_Swap(Node *da_inserire, Node *da_rimuovere, Clique *C);

void Riduzione_Grafo(Graph *g_ptr, Clique *C, int b);
void Calcola_Delta_U(Graph *g_ptr, int b);
void Calcola_Delta_k(Graph *g_ptr, int k);

void Color_Reduction(Graph *g, Clique *Best2);

/***********************************
 * 					MAIN					*
 **********************************/


int main(int argc, char *argv[]) {

    int seme;
    //Node* nodoIniziale, *ultimo_aggiunto, *tmp_nodo;
    Clique a1, a2;
    Clique *C, *C1;
    bool restart=false;
    C = &a1;
    C1 = &a2;
    C_Best = &Best;

    if (argc == 7) {

        t = timer();

        struct timeval tv;
        gettimeofday(&tv, NULL);
        double tt = tv.tv_sec+tv.tv_usec/1000000.0;
        unsigned int seme = tt/getpid();        
        
        srand(seme);

        // TODO: IMPORTANTISSIMO da eliminare la prossima istruzione una volta terminato il debug
        //srand(1);


        struct rlimit limitbuf;

        getrlimit(RLIMIT_CPU, &limitbuf);
        limitbuf.rlim_cur = 3606;
        limitbuf.rlim_max = 3706;
        setrlimit(RLIMIT_CPU, &limitbuf);

        if (signal(SIGXCPU, GestoreKill) == SIG_ERR) {
            printf("errore di ritorno della signal (SIGXCPU). \n");
            exit(0);
        }

        // TODO : c'e' qualcosa che non va nel parser (troppo lento)
        g = new Graph(argv[1], CLQ_BIN, UNDIRECTED);

        numNodes = numNodes_dyn = g->GetNumnodes();
        numColors = g->GetNumcolors();
        numArcs = g->GetNumarcs();

        char temp_name[500], *estensione_file, *pch;
        strcpy(temp_name, argv[1]);
        pch = strtok(temp_name, "/");
        while (pch != NULL) {
            estensione_file = pch;
            pch = strtok(NULL, "/");
        }

        printf("%s ", estensione_file);
        fflush(stdout);

        // Controllo se e' stato passato in input il budget in percentule rispetto ai colori presenti nel grafo oppure il numero esatto
        strcpy(temp_name, argv[2]);
        pch = strpbrk(temp_name, "%");
        if (pch == NULL) {
            budget = atoi(argv[2]);
            if (budget < 1) {
                printf("ERROR: wrong value (%d) for budget", budget);
                exit(1);
            }
        } else {
            pch = strtok(temp_name, "%");
            double val = (double) (numColors * atoi(pch)) / 100.0;
            budget = ceil(val);            
        }

        tcs = atoi(argv[3]);
        penalty_delay = atoi(argv[4]);
        maxSteps = atoi(argv[5]);
        print_option = atoi(argv[6]);



        Inizializza_Clique(C, numNodes, numColors);
        Inizializza_Clique(C1, numNodes, numColors);
        Inizializza_Clique(C_Best, numNodes, numColors);

        NLC = (Heap*) new Heap(numNodes + 1);
        NIC = (Heap*) new Heap(numNodes + 1);

        tmp_occorrenze = (int*) malloc(sizeof (int) *(numColors + 1));

        trovata_new_max_clique=false;
        
//        while(trovata_new_max_clique==true){
//            
//            trovata_new_max_clique=false;
//            
//            // Fase 1
//            Dynamic_Local_Search(g, C, C1,restart);
//
//            // Fase 2
//            Riduzione_Grafo(g, C_Best, C_Best->colori_nella_clique);
//
//            // Fase 3
//            restart = Color_Reduction(g,C_Best);
//        }
        
        
        do{
            
            // Fase 1
            Dynamic_Local_Search(g, C, C1,trovata_new_max_clique);
            trovata_new_max_clique=false;
            
            // Fase 2
            Riduzione_Grafo(g, C_Best, C_Best->colori_nella_clique);

            // Fase 3
            Color_Reduction(g,C_Best);
        
        }while(trovata_new_max_clique==true);
        
        
        
        Stampa_Soluzione(C_Best, print_option);
        
        
    } else {
        printf("\n\nERROR: Wrong number of parameters!!!\n");
        printf("USAGE: alg input_file budget tcs penalty_delay maxsteps print_option\n");
        exit(0);
    }

}

/***********************************
 * 	implementazione funzioni   *
 **********************************/


void Dynamic_Local_Search(Graph *g, Clique *C, Clique *C1, bool dalla_terza_fase) {

    int i, rand_value;
    Node* nodoIniziale, *ultimo_aggiunto, *tmp_nodo;
    
    bool dtf = dalla_terza_fase;
    
    // Pongo a zero il peso (le penalties nel paper) dei nodi del grafo

    tmp_nodo = g->getGrafo_dinamico();
    while (tmp_nodo != NULL) {
        tmp_nodo->SetChiave_ordinamento(0);
        tmp_nodo = tmp_nodo->GetNodo_down_dyn();
    }


    //for (i = 0; i < numNodes; i++) g->GetSingleNode(i)->SetChiave_ordinamento(0);

    //        g->Visualizza_Grafo(3);
    //        printf("budget= %d\n",budget);
    //        exit(0);

    Svuota_Clique(C);
    
    // Scelgo il nodo di partenza e lo inserisco nella clique vuota.
    // TODO: questo nodo deve essere scelto in modo random.
    nodoIniziale = g->getGrafo_dinamico();
    //nodoIniiale =g->GetSingleNode(0);

    Inserisci_in_C(nodoIniziale, C);


    // TODO: Eliminare questo assegnamento se si vuole utilizzare il tcs dell'alg. originale
    tcs = numNodes;

    while (numSteps < maxSteps) {
        
//        printf("\n\n********************\n");
//        Stampa_Clique(C,0);
        
        if ((tmp_nodo = Expand(C, true)) != NULL) ultimo_aggiunto = tmp_nodo;
            
        if (C->size > C_Best->size) {
            Copia_Clique(C, C_Best);

            if (C_Best->size >= tcs) {
                Stampa_Soluzione(C_Best, print_option);
                exit(1);
            }
        }
        else if(C->size == C_Best->size && C->colori_nella_clique < C_Best->colori_nella_clique) Copia_Clique(C, C_Best);    

            

        Copia_Clique(C, C1);

        if ((tmp_nodo = Plateau(C, C1)) != NULL) ultimo_aggiunto = tmp_nodo;
  
        while (NIC->getHeapSize() > 0 && numSteps < maxSteps) {

            if ((tmp_nodo = Expand(C, false)) != NULL) ultimo_aggiunto = tmp_nodo;

        
            if (C->size > C_Best->size) {
                Copia_Clique(C, C_Best);

                if (C_Best->size >= tcs) {
                    Stampa_Soluzione(C_Best, print_option);
                    exit(1);
                }
            }
            else if(C->size == C_Best->size && C->colori_nella_clique < C_Best->colori_nella_clique) Copia_Clique(C, C_Best);    
            
            if ((tmp_nodo = Plateau(C, C1)) != NULL) ultimo_aggiunto = tmp_nodo;

        }

        Update_Penalties(penalty_delay, C);

//#ifndef Performance
//        if (ultimo_aggiunto == NULL) {  
//            printf("Warning [main]: nella corrente iterazione non si e' riusciti ad aggiungere alcun nodo alla clique ossia ultimo_aggiunto e' NULL (non puo' essere)\n\n");
//            exit(0);
//        }
//#endif			

        if(dtf==true){
            Copia_Clique(C_Best,C);
            dtf=false;
        }
        else if (penalty_delay > 1) {
            Svuota_Clique(C);
            Inserisci_in_C(ultimo_aggiunto, C);
        } else {
            rand_value = i_random(1, numNodes_dyn - C->size);
            nodoIniziale = Find_nodo_in_G(rand_value, C);

            // TODO: oltre a rimuovere i nodi inconsistenti bisogna anche verificare che la nuova clique non sfori il budget.
            Inserisci_e_Rimuovi_Inconsistenti(nodoIniziale, C);
            
            
        }

        ultimo_aggiunto = NULL;

    }
}

void Riduzione_Grafo(Graph *g_ptr, Clique *C, int b) {

    //#define DB_Riduzione_Grafo 1

    Node *ptr_nodo;
    
    // Calcolo delta_U e cancello tutti quanti i nodi il cui valore e' inferiore a k-1 (dove k e' la dimensione della clique) dato che questi ultimi
    // non possono appartenere ad una clique di tale dimensione senza sforare il budget.
    Calcola_Delta_U(g_ptr, b);
    ptr_nodo = g_ptr->getGrafo_dinamico();
    while (ptr_nodo != NULL) {
        //printf("%d=%d, ",ptr_nodo->GetId(),ptr_nodo->GetDelta_U());
        if (ptr_nodo->GetDelta_U() < C->size - 1) {
            g->Rimuovi_Nodo_Dyn(ptr_nodo);
            nodi_cancellati++;
#ifdef DB_Riduzione_Grafo
            printf("\nRimuovo il nodo %d perche' ha il delta_U troppo basso!\n", ptr_nodo->GetId());
#endif            
        }
        ptr_nodo = ptr_nodo->GetNodo_down_dyn();
    }

    //printf("\n****\n");

    // Calcolo delta_k e cancello tutti quanti i nodi che per poter appartenere ad una clique di dimensione k sforano il budget.
    Calcola_Delta_k(g_ptr, C->size);
    ptr_nodo = g_ptr->getGrafo_dinamico();
    while (ptr_nodo != NULL) {
        //printf("%d=%d, ",ptr_nodo->GetId(),ptr_nodo->GetDelta_k());
        if (ptr_nodo->GetDelta_k() > C->colori_nella_clique) {
			nodi_cancellati++;
            g->Rimuovi_Nodo_Dyn(ptr_nodo);
#ifdef DB_Riduzione_Grafo
            printf("\nRimuovo il nodo %d perche' ha il delta_k (%d) troppo alto!\n", ptr_nodo->GetId(), ptr_nodo->GetDelta_k());
#endif
        }
        ptr_nodo = ptr_nodo->GetNodo_down_dyn();
    }

    //printf("\n");
    

}

void Calcola_Delta_U(Graph *g_ptr, int b) {

    //#define DB_Calcola_Delta_U 1

    Node *ptr_nodo;
    Arc *ptr_arco;

    int i, contatore, ncolors;

    ncolors = g->GetNumcolors() + 1;

    ptr_nodo = g_ptr->getGrafo_dinamico();
    while (ptr_nodo != NULL) {
        memset(tmp_occorrenze, 0, sizeof (int) * ncolors);

        ptr_arco = ptr_nodo->GetCorona_dyn();
        while (ptr_arco != NULL) {
            tmp_occorrenze[ptr_arco->GetColor()] += 1;

            ptr_arco = ptr_arco->getNext_dyn();
        }

        Quicksort(tmp_occorrenze, 0, ncolors - 1);

        contatore = 0;
        for (i = 0; i < b; i++) {
            //printf("occ[%d]=%d, ",i,tmp_occorrenze[i]);
            contatore += tmp_occorrenze[i];
        }
        //printf("\n");
        ptr_nodo->SetDelta_U(contatore);

        ptr_nodo = ptr_nodo->GetNodo_down_dyn();
    }

#ifdef DB_Calcola_Delta_U   
    printf("\n\n ||delta_U:||\n");
    ptr_nodo = g_ptr->getGrafo_dinamico();
    while (ptr_nodo != NULL) {
        printf("delta_U(%d)=%d\n", ptr_nodo->GetId(), ptr_nodo->GetDelta_U());
        ptr_nodo = ptr_nodo->GetNodo_down_dyn();
    }

#endif    

}

void Calcola_Delta_k(Graph *g_ptr, int k) {

    //#define DB_Calcola_Delta_k 1

    Node *ptr_nodo;
    Arc *ptr_arco;

    int i, contatore, ncolors;

    ncolors = g->GetNumcolors() + 1;

    ptr_nodo = g_ptr->getGrafo_dinamico();
    while (ptr_nodo != NULL) {
        memset(tmp_occorrenze, 0, sizeof (int) * ncolors);

        ptr_arco = ptr_nodo->GetCorona_dyn();
        while (ptr_arco != NULL) {
            tmp_occorrenze[ptr_arco->GetColor()] += 1;

            ptr_arco = ptr_arco->getNext_dyn();
        }

        Quicksort(tmp_occorrenze, 0, ncolors - 1);

        contatore = 0;
        for (i = 0; i < ncolors; i++) {
            contatore += tmp_occorrenze[i];
            if (contatore >= k - 1) break;
        }

        ptr_nodo->SetDelta_k(i + 1);

        ptr_nodo = ptr_nodo->GetNodo_down_dyn();
    }

#ifdef DB_Calcola_Delta_k  
    printf("\n\n ||delta_k:||\n");
    ptr_nodo = g_ptr->getGrafo_dinamico();
    while (ptr_nodo != NULL) {
        printf("delta_k(%d)=%d\n", ptr_nodo->GetId(), ptr_nodo->GetDelta_k());
        ptr_nodo = ptr_nodo->GetNodo_down_dyn();
    }
#endif   

}


void Color_Reduction(Graph *g, Clique *Best2){

    int i;
    Nodo_Clique *ptr;
    Node *da_rimuovere,*v,*da_inserire, *ultimo;
    Clique a,*C_Start;
    Coppia tmp_coppia;
    Coppia tmp_coppia_swap;
    Coppia *coppia_swap;
    int colori_rimossi, colori_inseriti;
    Clique *Clique_migliorata;

    C_Start = &a;
    coppia_swap =&tmp_coppia_swap;

    Inizializza_Clique(C_Start, numNodes, numColors);

    Copia_Clique(Best2,C_Start);
    
    
    My_Init_heap(numNodes*numNodes,&NIL);
    
   
    // TODO: verificare se le incidenze riportate dalla clique passata in input sono corrette oppure vanno calcolate.
    // TODO: poiche' l'heap non serve piu' sarebbe opportuno eliminare inserimenti ed estrazioni delle coppie del NIL ed 
    //       usare un semplice array per velocizzare il tutto.
    
    // Per prima cosa effettuiamo la costruzione dell'insieme NIL. Deve contenere tutti quanti i nodi esterni alla clique aventi
    //  un'incidenza pari o superiore a k-1 e che garantiscano un decremento dei colori della clique dopo lo swap. 
    
    da_inserire = g->getGrafo_dinamico();    
    while(da_inserire!=NULL){
        
        if(C_Start->array_nodi[da_inserire->GetIndex()].e_nella_clique==false){ 
            
            if(C_Start->array_nodi[da_inserire->GetIndex()].incidenza==C_Start->size){
                
                //printf("\nci sono nodi con indicenza k\n");
                da_rimuovere = NULL;
                ptr = C_Start->testa_clique;
                while(ptr != NULL){
                    v = ptr->nodo_ptr;
                    da_rimuovere=v;
                    colori_rimossi = Decrementa_Occorrenze_Colori(da_rimuovere, C_Start);         // ...poi aggiorno lo stesso array quando inserisco il nodo da_inserire
                    colori_inseriti = Incrementa_Occorrenze_Colori(da_inserire, C_Start);
                    Decrementa_Occorrenze_Colori(da_inserire, C_Start);
                    Incrementa_Occorrenze_Colori(da_rimuovere, C_Start);

                    if (colori_rimossi > colori_inseriti) {
                        tmp_coppia.da_inserire = da_inserire;
                        tmp_coppia.da_rimuovere = da_rimuovere;
                        tmp_coppia.key = colori_inseriti-colori_rimossi;
                        My_Insert_to_heap(&tmp_coppia,&NIL);
                        //printf("Ho inserito nel NIL la coppia (%d,%d)|%f\n", tmp_coppia.da_inserire->GetId(),tmp_coppia.da_rimuovere->GetId(),tmp_coppia.key);

                    } //end if (colori_rimossi > colori_inseriti)

                    ptr =ptr->clique_next;
                } //end primo while(ptr != NULL)
        
            }//end if per il controllo dell'insierimento dei jolly in NIL
            else if(C_Start->array_nodi[da_inserire->GetIndex()].incidenza==C_Start->size-1){

                
                // individuo qual'è il nodo "da_rimuovere" dalla clique.
                da_rimuovere = NULL;
                ptr = C_Start->testa_clique;
                while (ptr != NULL) {
                    v = ptr->nodo_ptr;
                    if (g->getArc(da_inserire, v)->getStato_dyn() == 0) {
                        da_rimuovere = v;
                        //printf("ho trovato da rimuovere\n");
                        break;
                    } // end if (g->getArc(da_inserire, v)->getStato_dyn() == 0)
                    ptr = ptr->clique_next;
               } //end while (ptr != NULL)
                
                // Per prima cosa devo verificare se questo swap non porti a sforare il budget
                        // aggiorno l'array delle occorrenze quando rimuovo il nodo da_rimuovere...
                colori_rimossi = Decrementa_Occorrenze_Colori(da_rimuovere, C_Start);

                // ...poi aggiorno lo stesso array quando inserisco il nodo da_inserire
                colori_inseriti = Incrementa_Occorrenze_Colori(da_inserire, C_Start);

                // Rimettere l'array delle occorrenze a posto riportandolo allo stato iniziale.
                Decrementa_Occorrenze_Colori(da_inserire, C_Start);
                Incrementa_Occorrenze_Colori(da_rimuovere, C_Start);
                
                //printf("il costo per l'inserimento di %d è %d ", da_rimuovere->GetIndex(),  (colori_rimossi -colori_inseriti));
                // se lo swap sfora il budget viene scartato
                if (colori_rimossi > colori_inseriti) {
                    tmp_coppia.da_inserire = da_inserire;
                    tmp_coppia.da_rimuovere = da_rimuovere;
                    tmp_coppia.key = colori_inseriti-colori_rimossi;
                    My_Insert_to_heap(&tmp_coppia,&NIL);
                    
                    //printf("Ho inserito nel NIL la coppia (%d,%d)|%f\n", tmp_coppia.da_inserire->GetId(),tmp_coppia.da_rimuovere->GetId(),tmp_coppia.key);

                } //end if (colori_rimossi > colori_inseriti)


            }//end if per il controllo e inserimento nel NIL dei nodi con incidenza k-1
        
        }//end if per il controllo sull'appartenza o meno del nodo in esame nella clique
        
        da_inserire = da_inserire->GetNodo_down_dyn();
    } // END while(da_inserire!=NULL)

    
  //Qui estraggo la coppia con valore di chiave minimo e avvio il colored plateau
  //MULTISTART
//  if(NIL.heap_size==0){
//      printf("\n[NIL VUOTO]:Impossibile avviare il Colored Plateau\n");
//  }//end if
//  else{
      while(NIL.heap_size>0){
        coppia_swap=My_Extract_Min(&NIL); 
        
  
        Clique_migliorata = Colored_Plateau(C_Start, coppia_swap);

        
        if(trovata_new_max_clique==true){ 
            miglioramenti_cp++;
            return;
        }
        if(Clique_migliorata->colori_nella_clique < C_Best->colori_nella_clique){
            miglioramenti_cp++;
            Copia_Clique(Clique_migliorata, C_Best);
        }
      }//end while      
  //}//end else

    
}//END FUNCTION COLOR_REDUCTION



Clique *Colored_Plateau(Clique *C_Best3, Coppia *coppia_swap){
    Node *ultimo, *nuovo_da_inserire, *da_rimuovere, *v;
    Clique c3,*C_Start3;
    int colori_rimossi=0;
    int colori_inseriti=0;
    Nodo_Clique *ptr;
    //Coppia tmp_coppia, tmp_best_coppia;
    Coppia best_coppia_swap;
    long miglioramento;
    
    //best_coppia =&tmp_best_coppia;
    
    C_Start3=&c3;
    Inizializza_Clique(C_Start3, numNodes, numColors);
    Copia_Clique(C_Best3, C_Start3);
                        
    //best_coppia = coppia_swap;
    
    best_coppia_swap.da_inserire = coppia_swap->da_inserire;
    best_coppia_swap.da_rimuovere = coppia_swap->da_rimuovere;
    best_coppia_swap.key = coppia_swap->key;
  
    
    //Adesso viene la fase effettiva di colored plateau in cui
    //cerco continuatamente una coppia(best_swap) per effettuare uno swap 
    //che riduca il numero di colori nella clique
    do{
        
        
//        printf("\n**** yuhu *******\n");
//        Verifica_Clique(C_Start3);
//        printf("\n**** yuhu1 *******\n");

        if(Swap_Nodi_NIL(best_coppia_swap.da_rimuovere, best_coppia_swap.da_inserire, C_Start3)){
                //printf("\nSwap avvenuto con successo!\n");
                //miglioramento=best_coppia_swap->key;
                //ultimo = best_coppia_swap->da_inserire; 
                ;
        
        }//end if Swap_Nodi_NIL
#ifndef Performance        
        else{
                printf("\n[Colored_Plateau] ERRORE: Lo swap  peggiora la soluzione.\n");
                exit(0);
        }//end else
#endif    
    
        
//        printf("\n**** yuhu2 *******\n");
//        Verifica_Clique(C_Start3);
//        printf("\n**** yuhu3 *******\n");
    
        // Svuoto lo swap migliore e setto la chiave ad un valore molto grande.
        best_coppia_swap.da_inserire = NULL;
        best_coppia_swap.da_rimuovere = NULL;
        best_coppia_swap.key = numColors;
                    
        nuovo_da_inserire = g->getGrafo_dinamico();   
    
        while(nuovo_da_inserire!=NULL){
            if(C_Start3->array_nodi[nuovo_da_inserire->GetIndex()].e_nella_clique==false){
                if(C_Start3->array_nodi[nuovo_da_inserire->GetIndex()].incidenza==C_Start3->size){

                    // TODO: verificare se l'inserimento del nodo nella clique non sfora il budget. In questo caso bsogna tornare alla fase 1 con la nuova clique.
                        
                    if(Verifica_Costo_Inserimento(nuovo_da_inserire,C_Start3) + C_Start3->colori_nella_clique <= budget){
                       
                        Inserisci_in_C(nuovo_da_inserire, C_Start3);
                        Copia_Clique(C_Start3, C_Best);

                        trovata_new_max_clique = true;
                        return C_Start3;
                    }
                            
                    //printf("\nci sono nodi con indicenza k\n");
                    da_rimuovere = NULL;
                    ptr = C_Start3->testa_clique;
                    
                    while(ptr != NULL){
                        
                        da_rimuovere = ptr->nodo_ptr;    
                        colori_rimossi = Decrementa_Occorrenze_Colori(da_rimuovere, C_Start3);         // ...poi aggiorno lo stesso array quando inserisco il nodo da_inserire
                        colori_inseriti = Incrementa_Occorrenze_Colori(nuovo_da_inserire, C_Start3);
                        Decrementa_Occorrenze_Colori(nuovo_da_inserire, C_Start3);
                        Incrementa_Occorrenze_Colori(da_rimuovere, C_Start3);

                        if(colori_rimossi > colori_inseriti && (colori_inseriti - colori_rimossi < best_coppia_swap.key) ){
                            best_coppia_swap.da_inserire = nuovo_da_inserire;
                            best_coppia_swap.da_rimuovere = da_rimuovere;
                            best_coppia_swap.key = colori_inseriti - colori_rimossi;                            
                        }

                        ptr =ptr->clique_next;
                    } //end primo while(ptr != NULL)

                }//end if per il controllo dell'insierimento dei jolly in NIL
                else if(C_Start3->array_nodi[nuovo_da_inserire->GetIndex()].incidenza==C_Start3->size-1){


                    // individuo qual'è il nodo "da_rimuovere" dalla clique.
                    da_rimuovere = NULL;
                    ptr = C_Start3->testa_clique;
                    while (ptr != NULL) {
                        v = ptr->nodo_ptr;
                        if (g->getArc(nuovo_da_inserire, v)->getStato_dyn() == 0) {
                            da_rimuovere = v;
                            //printf("ho trovato da rimuovere\n");
                            break;
                        } // end if (g->getArc(da_inserire, v)->getStato_dyn() == 0)
                        ptr = ptr->clique_next;
                   } //end while (ptr != NULL)


                    colori_rimossi = Decrementa_Occorrenze_Colori(da_rimuovere, C_Start3);
                    colori_inseriti = Incrementa_Occorrenze_Colori(nuovo_da_inserire, C_Start3);
                    Decrementa_Occorrenze_Colori(nuovo_da_inserire, C_Start3);
                    Incrementa_Occorrenze_Colori(da_rimuovere, C_Start3);                

                    if(colori_rimossi > colori_inseriti ){
                        best_coppia_swap.da_inserire = nuovo_da_inserire;
                        best_coppia_swap.da_rimuovere = da_rimuovere;
                        best_coppia_swap.key = colori_inseriti - colori_rimossi;                            
                    }                    

                }//end if per il controllo e inserimento nel NIL dei nodi con incidenza k-1

            }//end if(C_Start->array_nodi[da_inserire->GetIndex()].e_nella_clique==false)
        
            nuovo_da_inserire = nuovo_da_inserire->GetNodo_down_dyn();
        } //while(nuovo_da_inserire!=NULL)
 
    }//end do
    while( best_coppia_swap.key < 0);
    
    
    return C_Best3;
}



bool Swap_Nodi_NIL(Node *da_rimuovere, Node *da_inserire, Clique *C) {

    //int colori_rimossi=0, colori_inseriti=0;
    int index, l_index, r_index;
    Nodo_Clique *rmv, *ins;
    Arc *ptr_arco;
    Node *v;

    
    // Per prima cosa devo verificare se questo swap non porti a sforare il budget
    // aggiorno l'array delle occorrenze quando rimuovo il nodo da_rimuovere...
    int colori_rimossi = Decrementa_Occorrenze_Colori(da_rimuovere, C);

    // ...poi aggiorno lo stesso array quando inserisco il nodo da_inserire
    int colori_inseriti = Incrementa_Occorrenze_Colori(da_inserire, C);
    
    C->colori_nella_clique = C->colori_nella_clique - colori_rimossi + colori_inseriti;
    
    rmv = &(C->array_nodi[da_rimuovere->GetIndex()]);
    ins = &(C->array_nodi[da_inserire->GetIndex()]);


    index = da_rimuovere->GetIndex();
    rmv = &(C->array_nodi[index]);

    // Elimino il nodo dalla lista della clique...
    if (rmv->clique_prev == NULL) { // se sono il primo nodo nella lista della clique
        if (rmv->clique_next == NULL) C->testa_clique = NULL;
        else {
            C->testa_clique = rmv->clique_next;
            rmv->clique_next->clique_prev = NULL;
        }
    } else if (rmv->clique_next == NULL) { // se sono l'ultimo nodo nella lista della clique
        rmv->clique_prev->clique_next = NULL;
    } else {
        rmv->clique_next->clique_prev = rmv->clique_prev;
        rmv->clique_prev->clique_next = rmv->clique_next;

    }

    // ...a questo punto il nodo va inserito nella lista del grafo.
    for (l_index = index - 1; l_index >= 0; l_index--) {
        if (C->array_nodi[l_index].e_nella_clique == false) break;
    }

    for (r_index = index + 1; r_index < C->dim_array; r_index++) {
        if (C->array_nodi[r_index].e_nella_clique == false) break;
    }

    if (l_index < 0) { // se devo essere inserito come primo nodo della lista...
        if (r_index >= C->dim_array) { //...ma anche come unico
            C->testa_graph = rmv;
        } else {
            rmv->graph_next = C->testa_graph;
            C->testa_graph->graph_prev = rmv;
            C->testa_graph = rmv;
        }
    } else { //... non sono il primo della lista...
        if (r_index >= C->dim_array) { //... ma sono l'ultimo
            rmv->graph_prev = &(C->array_nodi[l_index]);
            rmv->graph_prev->graph_next = rmv;
        } else { //... ne l'ultimo
            rmv->graph_prev = &(C->array_nodi[l_index]);
            rmv->graph_next = &(C->array_nodi[r_index]);

            rmv->graph_prev->graph_next = rmv;
            rmv->graph_next->graph_prev = rmv;
        }
    }

    // Decremento l'incidenza di tutti quanti i nodi che sono adiacenti a quello rimosso
    // dalla clique ma che non sono adiancenti a quello da inserire
    ptr_arco = da_rimuovere->GetCorona_dyn();
    while (ptr_arco != NULL) {

        v = ptr_arco->GetNodeTo();
        index = v->GetIndex();

        //Se un nodo v della corona del nodo uscente non è vicino al nodo da
        //inserire allora decementa l'incidenza
        if (g->getArc(da_inserire, v)->getStato_dyn() == 0) {
            C->array_nodi[index].incidenza -= 1;
        }

        ptr_arco = ptr_arco->getNext_dyn();
    } //end while (ptr_arco != NULL)


    // aggiorno i campi nella casella
    rmv->clique_next = rmv->clique_prev = NULL;
    rmv->e_nella_clique = false;
    rmv->era_nella_clique_iniziale = true;
    C->size -= 1;


    // Adesso passo all'inserimento del nodo...

    index = da_inserire->GetIndex();
    ins = &(C->array_nodi[index]);

    // Per prima cosa bisogna eliminare il nodo dalla lista dei nodi del grafo residuo
    if (ins->graph_prev == NULL) { // se sono il primo nodo nella lista del grafo
        if (ins->graph_next == NULL) C->testa_graph = NULL;
        else {
            C->testa_graph = ins->graph_next;
            ins->graph_next->graph_prev = NULL;
        }
    } else if (ins->graph_next == NULL) { // se sono l'ultimo nodo nella lista del grafo
        ins->graph_prev->graph_next = NULL;
    } else {
        ins->graph_next->graph_prev = ins->graph_prev;
        ins->graph_prev->graph_next = ins->graph_next;
    }

    ins->graph_prev = ins->graph_next = NULL;

    // ... a questo punto devo inserire il nodo nella lista della clique
    for (l_index = index - 1; l_index >= 0; l_index--) {
        if (C->array_nodi[l_index].e_nella_clique == true) break;
    }

    for (r_index = index + 1; r_index < C->dim_array; r_index++) {
        if (C->array_nodi[r_index].e_nella_clique == true) break;
    }

    if (l_index < 0) { // se devo essere inserito come primo nodo della lista...
        if (r_index >= C->dim_array) { //...ma anche come unico
            C->testa_clique = ins;
        } else {
            ins->clique_next = C->testa_clique;
            C->testa_clique->clique_prev = ins;
            C->testa_clique = ins;
        }
    } else { //... non sono il primo della lista...
        if (r_index >= C->dim_array) { //... ma sono l'ultimo
            ins->clique_prev = &(C->array_nodi[l_index]);
            ins->clique_prev->clique_next = ins;
        } else { //... ne l'ultimo
            ins->clique_prev = &(C->array_nodi[l_index]);
            ins->clique_next = &(C->array_nodi[r_index]);

            ins->clique_prev->clique_next = ins;
            ins->clique_next->clique_prev = ins;
        }
    }

    ins->e_nella_clique = true;
    C->size++;

    // Incremento l'incidenza di tutti quanti i nodi che sono adiacenti a quello inserito
    // nella clique tranne quelli adiacenti al nodo rimosso
    ptr_arco = da_inserire->GetCorona_dyn();
    while (ptr_arco != NULL) {

        v = ptr_arco->GetNodeTo();
        index = v->GetIndex();

        if (g->getArc(da_rimuovere, v)->getStato_dyn() == 0) {
            C->array_nodi[index].incidenza += 1;
        } // end if (g->getArc(da_rimuovere, v)->getStato_dyn() == 0)

        ptr_arco = ptr_arco->getNext_dyn();
    }//end  while (ptr_arco != NULL)


    return true;
}//end function  swap_nodi_nil



void Inizializza_Clique(Clique *C, int n, int ncolors) {

    int i;
    C->size = 0;
    C->dim_array = n;
    C->testa_clique = NULL;
    C->dim_array_occorrenze = ncolors + 1;
    C->colori_nella_clique = 0;

    C->array_nodi = (Nodo_Clique*) malloc(sizeof (Nodo_Clique) * n);
    C->occorrenze_colori = (int*) malloc(sizeof (int) * (ncolors + 1));
    memset(C->occorrenze_colori, 0, sizeof (int) * (ncolors + 1));

    //C->testa_clique = &(C->array_nodi[0]);

    // Per la prima cella dell'array bisogna usare del codice diverso da quello dentro il for 
    C->array_nodi[0].clique_next = C->array_nodi[0].clique_prev = NULL;
    C->array_nodi[0].graph_next = &(C->array_nodi[1]);
    C->array_nodi[0].graph_prev = NULL;
    C->array_nodi[0].nodo_ptr = g->GetSingleNode(0);
    C->array_nodi[0].e_nella_clique = false;
    C->array_nodi[0].incidenza = 0;

    for (i = 1; i < n - 1; i++) {
        C->array_nodi[i].clique_next = C->array_nodi[i].clique_prev = NULL;
        C->array_nodi[i].graph_next = &(C->array_nodi[i + 1]);
        C->array_nodi[i].graph_prev = &(C->array_nodi[i - 1]);
        C->array_nodi[i].e_nella_clique = false;
        C->array_nodi[i].incidenza = 0;
        C->array_nodi[i].nodo_ptr = g->GetSingleNode(i);

    }

    C->array_nodi[i].incidenza = 0;
    C->array_nodi[i].clique_next = C->array_nodi[i].clique_prev = NULL;
    C->array_nodi[i].graph_next = NULL;
    C->array_nodi[i].graph_prev = &(C->array_nodi[i - 1]);
    C->array_nodi[i].nodo_ptr = g->GetSingleNode(i);
    C->array_nodi[i].era_nella_clique_iniziale = false;
    C->array_nodi[i].e_nella_clique = false;

}

void Copia_Clique(Clique *originale, Clique *copia) {

    int n, dim, index, i;

    n = originale->size;
    dim = originale->dim_array;

    copia->size = originale->size;
    copia->dim_array = originale->dim_array;
    copia->colori_nella_clique = originale->colori_nella_clique;
    copia->dim_array_occorrenze = originale->dim_array_occorrenze;

    for (i = 0; i < originale->dim_array_occorrenze; i++)
        copia->occorrenze_colori[i] = originale->occorrenze_colori[i];


    if (originale->testa_clique != NULL) {
        index = originale->testa_clique->nodo_ptr->GetIndex();
        copia->testa_clique = &(copia->array_nodi[index]);
    } else copia->testa_clique = NULL;

    if (originale->testa_graph != NULL) {
        index = originale->testa_graph->nodo_ptr->GetIndex();
        copia->testa_graph = &(copia->array_nodi[index]);
    } else copia->testa_graph = NULL;

    for (i = 0; i < dim; i++) {

        if (originale->array_nodi[i].clique_next == NULL) copia->array_nodi[i].clique_next = NULL;
        else {
            index = originale->array_nodi[i].clique_next->nodo_ptr->GetIndex();
            copia->array_nodi[i].clique_next = &(copia->array_nodi[index]);
        }

        if (originale->array_nodi[i].clique_prev == NULL) copia->array_nodi[i].clique_prev = NULL;
        else {
            index = originale->array_nodi[i].clique_prev->nodo_ptr->GetIndex();
            copia->array_nodi[i].clique_prev = &(copia->array_nodi[index]);
        }

        if (originale->array_nodi[i].graph_next == NULL) copia->array_nodi[i].graph_next = NULL;
        else {
            index = originale->array_nodi[i].graph_next->nodo_ptr->GetIndex();
            copia->array_nodi[i].graph_next = &(copia->array_nodi[index]);
        }

        if (originale->array_nodi[i].graph_prev == NULL) copia->array_nodi[i].graph_prev = NULL;
        else {
            index = originale->array_nodi[i].graph_prev->nodo_ptr->GetIndex();
            copia->array_nodi[i].graph_prev = &(copia->array_nodi[index]);
        }

        copia->array_nodi[i].incidenza = originale->array_nodi[i].incidenza;
        copia->array_nodi[i].e_nella_clique = originale->array_nodi[i].e_nella_clique;
        copia->array_nodi[i].era_nella_clique_iniziale = originale->array_nodi[i].era_nella_clique_iniziale;
        copia->array_nodi[i].nodo_ptr = originale->array_nodi[i].nodo_ptr;
    }

}

int Incrementa_Occorrenze_Colori(Node *da_inserire, Clique *C) {

    int colori_inseriti = 0;
    Arc *ptr_arco;

    //#ifndef Performance  	
    //    if (C->array_nodi[da_inserire->GetIndex()].e_nella_clique==true) {
    //        printf("\n\nERRORE [Incrementa_Occorrenze_Colori]: Il nodo %d e' gia' presente nella clique quindi perche' incrementare le occorrenze dei colori??\n\n",da_inserire->GetId());
    //        exit(0);
    //    }
    //#endif

    ptr_arco = da_inserire->GetCorona_dyn();
    while (ptr_arco != NULL) {
        if (C->array_nodi[ptr_arco->GetNodeTo()->GetIndex()].e_nella_clique == true) {
            if (C->occorrenze_colori[ptr_arco->GetColor()] == 0) colori_inseriti++;
            C->occorrenze_colori[ptr_arco->GetColor()] += 1;
        }

        ptr_arco = ptr_arco->getNext_dyn();
    }

    return colori_inseriti;

}

int Decrementa_Occorrenze_Colori(Node *da_rimuovere, Clique *C) {

    int colori_rimossi = 0;
    Arc *ptr_arco;

    //#ifndef Performance  	
    //    if (C->array_nodi[da_rimuovere->GetIndex()].e_nella_clique==false) {
    //        printf("\n\nERRORE [Decrementa_Occorrenze_Colori]: Il nodo %d non e' presente nella clique quindi perche' decrementare le occorrenze dei colori??\n\n",da_rimuovere->GetId());
    //        exit(0);
    //    }
    //#endif

    ptr_arco = da_rimuovere->GetCorona_dyn();
    while (ptr_arco != NULL) {
        if (C->array_nodi[ptr_arco->GetNodeTo()->GetIndex()].e_nella_clique == true) {
            C->occorrenze_colori[ptr_arco->GetColor()] -= 1;
            if (C->occorrenze_colori[ptr_arco->GetColor()] == 0) colori_rimossi++;
        }

        ptr_arco = ptr_arco->getNext_dyn();
    }

    return colori_rimossi;

}

int Verifica_Costo_Inserimento(Node *da_inserire, Clique *C) {

    int costo_inserimento;

    costo_inserimento = Incrementa_Occorrenze_Colori(da_inserire, C);

    // Rimetto a posto l'array delle occorrenze_colori
    Decrementa_Occorrenze_Colori(da_inserire, C);

    return costo_inserimento;
}

void Inserisci_in_C(Node *da_inserire, Clique *C) {

#define DB_Inserisci_in_C 1 

    int index, l_index, r_index, colori_nuovi = 0;
    Nodo_Clique *ins;
    Arc *ptr_arco;
    Node *v;

    index = da_inserire->GetIndex();
    ins = &(C->array_nodi[index]);

    if (ins->e_nella_clique == false) {

        // Per prima cosa aggiorno l'array delle occorrenze anche per stabilire il costo di inserimento.
        colori_nuovi = Incrementa_Occorrenze_Colori(da_inserire, C);

// #ifndef Performance            
//         // se l'inserimento del nodo fa sforare il budget il nodo viene scartato
//         if (C->colori_nella_clique + colori_nuovi > budget) {
//             printf("\nERRORE [Inserisci_in_C]: L'inserimento del nodo %d costa %d nodi che aggiunti ai %d della clique sfora il budget %d. Il NIC non era corretto!!!\n", da_inserire->GetId(), colori_nuovi, C->colori_nella_clique, budget);
//             exit(0);
// 
//         }
// #endif       

        C->colori_nella_clique += colori_nuovi;
        // Poi bisogna eliminare il nodo dalla lista dei nodi del grafo residuo
        if (ins->graph_prev == NULL) { // se sono il primo nodo nella lista del grafo
            if (ins->graph_next == NULL) C->testa_graph = NULL;
            else {
                C->testa_graph = ins->graph_next;
                ins->graph_next->graph_prev = NULL;
            }
        } else if (ins->graph_next == NULL) { // se sono l'ultimo nodo nella lista del grafo 
            ins->graph_prev->graph_next = NULL;
        } else {
            ins->graph_next->graph_prev = ins->graph_prev;
            ins->graph_prev->graph_next = ins->graph_next;
        }

        ins->graph_prev = ins->graph_next = NULL;

        // ... a questo punto devo inserire il nodo nella lista della clique
        for (l_index = index - 1; l_index >= 0; l_index--) {
            if (C->array_nodi[l_index].e_nella_clique == true) break;
        }

        for (r_index = index + 1; r_index < C->dim_array; r_index++) {
            if (C->array_nodi[r_index].e_nella_clique == true) break;
        }

        if (l_index < 0) { // se devo essere inserito come primo nodo della lista...
            if (r_index >= C->dim_array) { //...ma anche come unico
                C->testa_clique = ins;
            } else {
                ins->clique_next = C->testa_clique;
                C->testa_clique->clique_prev = ins;
                C->testa_clique = ins;
            }
        } else { //... non sono il primo della lista...
            if (r_index >= C->dim_array) { //... ma sono l'ultimo		
                ins->clique_prev = &(C->array_nodi[l_index]);
                ins->clique_prev->clique_next = ins;
            } else { //... ne l'ultimo
                ins->clique_prev = &(C->array_nodi[l_index]);
                ins->clique_next = &(C->array_nodi[r_index]);

                ins->clique_prev->clique_next = ins;
                ins->clique_next->clique_prev = ins;
            }
        }

        ins->e_nella_clique = true;
        C->size++;

        // Incremento l'incidenza di tutti quanti i nodi adiacenti a quello inserito nella clique
        ptr_arco = da_inserire->GetCorona_dyn();
        while (ptr_arco != NULL) {
            v = ptr_arco->GetNodeTo();
            C->array_nodi[v->GetIndex()].incidenza += 1;

            // se l'altro estremo ha un incidenza pari a |C|, non e' nel NIC e il suo costo di inserimento non sfora il budget, allora deve essere inserito nel NIC.
            // N.B. Questa ipotesi si verifica quando si passa dalla fase di plateau (che solitamente inserisce un singolo nodo nel NIC) a quella di expand.  
            //if(NIC->nodo_in_heap(v) == false && C->array_nodi[v->GetIndex()].incidenza==C->size && (Verifica_Costo_Inserimento(v,C)+C->colori_nella_clique <= budget) )
            //                if(NIC->nodo_in_heap(v) == false && C->array_nodi[v->GetIndex()].incidenza==C->size && (Verifica_Costo_Inserimento(v,C)+C->colori_nella_clique <= budget) )
            //                    NIC->Insert_to_heap(v);

            ptr_arco = ptr_arco->getNext_dyn();
        }
    }
    else {
        printf("ERRORE [Inserisci_in_C]: il nodo %d e' gia' presente nella clique!!!\n\n", da_inserire->GetId());
        exit(0);
    }

}

int Verifica_Costo_Rimozione(Node *da_rimuovere, Clique *C) {

    int colori_rimossi = 0;

    colori_rimossi = Decrementa_Occorrenze_Colori(da_rimuovere, C);

    // Rimetto a posto l'array delle occorrenze_colori
    Incrementa_Occorrenze_Colori(da_rimuovere, C);

    return colori_rimossi;
}

void Rimuovi_da_C(Node *da_rimuovere, Clique *C) {

    int index, l_index, r_index;
    Nodo_Clique *rmv;
    Arc *ptr_arco;

    index = da_rimuovere->GetIndex();
    rmv = &(C->array_nodi[index]);

    if (rmv->e_nella_clique == true) {

        // Per prima cosa elimino il nodo dalla lista della clique...
        if (rmv->clique_prev == NULL) { // se sono il primo nodo nella lista della clique
            if (rmv->clique_next == NULL) C->testa_clique = NULL;
            else {
                C->testa_clique = rmv->clique_next;
                rmv->clique_next->clique_prev = NULL;
            }
        } else if (rmv->clique_next == NULL) { // se sono l'ultimo nodo nella lista della clique 
            rmv->clique_prev->clique_next = NULL;
        } else {
            rmv->clique_next->clique_prev = rmv->clique_prev;
            rmv->clique_prev->clique_next = rmv->clique_next;

        }

        // annullo tutti i campi della casella dell'array
        rmv->clique_next = rmv->clique_prev = NULL;
        rmv->e_nella_clique = false;
        C->size -= 1;

        // ...a questo punto il nodo va inserito nella lista del grafo.
        for (l_index = index - 1; l_index >= 0; l_index--) {
            if (C->array_nodi[l_index].e_nella_clique == false && C->array_nodi[l_index].nodo_ptr->GetStato_dyn() == 1) break;
        }

        for (r_index = index + 1; r_index < C->dim_array; r_index++) {
            if (C->array_nodi[r_index].e_nella_clique == false && C->array_nodi[r_index].nodo_ptr->GetStato_dyn() == 1) break;
        }

        if (l_index < 0) { // se devo essere inserito come primo nodo della lista...
            if (r_index >= C->dim_array) { //...ma anche come unico
                C->testa_graph = rmv;
            } else {
                rmv->graph_next = C->testa_graph;
                C->testa_graph->graph_prev = rmv;
                C->testa_graph = rmv;
            }
        } else { //... non sono il primo della lista...
            if (r_index >= C->dim_array) { //... ma sono l'ultimo		
                rmv->graph_prev = &(C->array_nodi[l_index]);
                rmv->graph_prev->graph_next = rmv;
            } else { //... ne l'ultimo
                rmv->graph_prev = &(C->array_nodi[l_index]);
                rmv->graph_next = &(C->array_nodi[r_index]);

                rmv->graph_prev->graph_next = rmv;
                rmv->graph_next->graph_prev = rmv;
            }
        }

        // Decremento l'incidenza di tutti quanti i nodi adiacenti a quello rimosso dalla clique
        ptr_arco = da_rimuovere->GetCorona_dyn();
        while (ptr_arco != NULL) {
            C->array_nodi[ptr_arco->GetNodeTo()->GetIndex()].incidenza -= 1;

            ptr_arco = ptr_arco->getNext_dyn();
        }

        // Aggiorno l'array delle occorrenze_colori 
        ptr_arco = da_rimuovere->GetCorona_dyn();
        while (ptr_arco != NULL) {
            if (C->array_nodi[ptr_arco->GetNodeTo()->GetIndex()].e_nella_clique == true) {
                C->occorrenze_colori[ptr_arco->GetColor()] -= 1;
                if (C->occorrenze_colori[ptr_arco->GetColor()] == 0) C->colori_nella_clique -= 1;
            }

            ptr_arco = ptr_arco->getNext_dyn();
        }

    } else {
        printf("ERRORE [Rimuovi_da_C]: il nodo %d non e' presente nella clique!!!\n\n", da_rimuovere->GetId());
        exit(0);
    }

}

int Verifica_Costo_Swap(Node *da_inserire, Node *da_rimuovere, Clique *C) {

    int colori_rimossi = 0, colori_inseriti = 0;


    // Per prima cosa aggiorno l'array delle occorrenze quando rimuovo il nodo da_rimuovere...
    colori_rimossi = Decrementa_Occorrenze_Colori(da_rimuovere, C);

    // ...poi aggiorno lo stesso array quando inserisco il nodo da_inserire
    colori_inseriti = Incrementa_Occorrenze_Colori(da_inserire, C);


    // A questo punto bisogna rimettere l'array delle occorrenze a posto riportandolo allo stato iniziale. 
    Decrementa_Occorrenze_Colori(da_inserire, C);
    Incrementa_Occorrenze_Colori(da_rimuovere, C);

    return colori_inseriti - colori_rimossi;

}

bool Swap_Nodi_C(Node *da_rimuovere, Node* da_inserire, Clique *C) {


    //#define DB_Swap_Nodi 1
    //Stampa_NIC_NLC(NLC);
    
    int colori_rimossi = 0, colori_inseriti = 0;
    int index, l_index, r_index;
    Nodo_Clique *rmv, *ins;
    Arc *ptr_arco;
    Node *v;

#if defined(DB_Swap_Nodi) || defined(DEBUG)	
    printf("\n*** [Swap_Nodi_C] ***\n ");
#endif

    rmv = &(C->array_nodi[da_rimuovere->GetIndex()]);
    ins = &(C->array_nodi[da_inserire->GetIndex()]);

#ifndef Performance  
    if (rmv->e_nella_clique == false) {
        printf("ERRORE [Swap_Nodi_C]: il nodo %d che si vuole rimuovere non e' presente nella clique!!!\n\n", da_rimuovere->GetId());
        exit(0);
    }
    if (ins->e_nella_clique == true) {
        printf("ERRORE [Swap_Nodi_C]: il nodo %d che si vuole inserire e' gia' presente nella clique!!!\n\n", da_inserire->GetId());
        exit(0);
    }
#endif
    
//    printf("\nSWAP: %d[%d] <--> %d[%d], dim_clique = %d\n",da_rimuovere->GetId(),C->array_nodi[da_rimuovere->GetIndex()].incidenza,da_inserire->GetId(),C->array_nodi[da_rimuovere->GetIndex()].incidenza,C->size);
//    Stampa_NIC_NLC(NLC);
        
        
    // Salvo questo valore per evitare problemi con i test quando inserisco e rimuovo nodi dalla clique 
    int clique_size = C->size;

    // Per prima cosa devo verificare se questo swap non porti a sforare il budget
    // aggiorno l'array delle occorrenze quando rimuovo il nodo da_rimuovere...
    colori_rimossi = Decrementa_Occorrenze_Colori(da_rimuovere, C);

    // ...poi aggiorno lo stesso array quando inserisco il nodo da_inserire
    colori_inseriti = Incrementa_Occorrenze_Colori(da_inserire, C);

    // se lo swap sfora il budget viene scartato
    if (C->colori_nella_clique - colori_rimossi + colori_inseriti > budget) {
        // Rimettere l'array delle occorrenze a posto riportandolo allo stato iniziale. 
        Decrementa_Occorrenze_Colori(da_inserire, C);
        Incrementa_Occorrenze_Colori(da_rimuovere, C);
        
        //printf("\n*!*!*!* Lo swap %d->%d e' troppo costoso.\n",da_rimuovere->GetId(),da_inserire->GetId());
        return false;
    }
    
    C->colori_nella_clique = C->colori_nella_clique - colori_rimossi + colori_inseriti;

    // Per prima cosa effettuo la cancellazione dalla clique del nodo "da_rimuovere"
    index = da_rimuovere->GetIndex();
    rmv = &(C->array_nodi[index]);

    // Elimino il nodo dalla lista della clique...
    if (rmv->clique_prev == NULL) { // se sono il primo nodo nella lista della clique
        if (rmv->clique_next == NULL) C->testa_clique = NULL;
        else {
            C->testa_clique = rmv->clique_next;
            rmv->clique_next->clique_prev = NULL;
        }
    } else if (rmv->clique_next == NULL) { // se sono l'ultimo nodo nella lista della clique 
        rmv->clique_prev->clique_next = NULL;
    } else {
        rmv->clique_next->clique_prev = rmv->clique_prev;
        rmv->clique_prev->clique_next = rmv->clique_next;

    }

    // ...a questo punto il nodo va inserito nella lista del grafo.
    for (l_index = index - 1; l_index >= 0; l_index--) {
        if (C->array_nodi[l_index].e_nella_clique == false) break;
    }

    for (r_index = index + 1; r_index < C->dim_array; r_index++) {
        if (C->array_nodi[r_index].e_nella_clique == false) break;
    }

    if (l_index < 0) { // se devo essere inserito come primo nodo della lista...
        if (r_index >= C->dim_array) { //...ma anche come unico
            C->testa_graph = rmv;
        } else {
            rmv->graph_next = C->testa_graph;
            C->testa_graph->graph_prev = rmv;
            C->testa_graph = rmv;
        }
    } else { //... non sono il primo della lista...
        if (r_index >= C->dim_array) { //... ma sono l'ultimo		
            rmv->graph_prev = &(C->array_nodi[l_index]);
            rmv->graph_prev->graph_next = rmv;
        } else { //... ne l'ultimo
            rmv->graph_prev = &(C->array_nodi[l_index]);
            rmv->graph_next = &(C->array_nodi[r_index]);

            rmv->graph_prev->graph_next = rmv;
            rmv->graph_next->graph_prev = rmv;
        }
    }

    // Decremento l'incidenza di tutti quanti i nodi adiacenti a quello rimosso dalla clique ma non adiancenti a quello da inserire
    ptr_arco = da_rimuovere->GetCorona_dyn();
    while (ptr_arco != NULL) {

        v = ptr_arco->GetNodeTo();
        index = v->GetIndex();

        if (g->getArc(da_inserire, v)->getStato_dyn() == 0) {
            C->array_nodi[index].incidenza -= 1;

#if defined(DB_Swap_Nodi) || defined(DEBUG)	
            printf("Decremento a %d l'incidenza del nodo %d\n", C->array_nodi[index].incidenza, v->GetId());
#endif

            // se il nodo era all'interno dell'NLC, decrementandone l'incidenza va rimosso da questo insieme.
            // N.B. I nodi presenti nella clique di partenza non verranno mai inseriti nell'NLC e quindi non devono 
            //      essere rimossi da esso (a questo serve il secondo test dell'if)
            if (C->array_nodi[index].era_nella_clique_iniziale == false) {

                if (C->array_nodi[index].incidenza == clique_size - 2 && NLC->nodo_in_heap(v) == true) {

#if defined(DB_Swap_Nodi) || defined(DEBUG)	
                    //        if(NLC->nodo_in_heap(v)==false){
                    //            printf("ERRORE [Swap_Nodi_C]: il nodo %d non e' nell'NLC ma io sto cercando di cancellarlo lo stesso da li!!!\n",v->GetId());
                    //            exit(0);
                    //        } 
                    //        printf("il nodo %d esce dall'NLC perche' la sua incidenza ora e' pari a %d.\n",v->GetId(),C->array_nodi[v->GetIndex()].incidenza);
#endif               
                    NLC->Remove_From_Heap(v->GetHeap_pos());
                } else if (C->array_nodi[index].incidenza == clique_size - 1) {
                    // quando un nodo ha un costo di inserimento troppo alto, pur essendo adiacente a tutti quelli della clique non viene inserito
                    // nel NIC. Questo implica che durante la fase di decremento delle incidenze tale nodo potrebbe cambiera' da k a k-1 l'incidenza e quindi
                    // deve essere inserito nell'insieme NLC (dove invece ci possono essere nodi che sforano il budget).
                    NLC->Insert_to_heap(v);
                }
            }
        }
        ptr_arco = ptr_arco->getNext_dyn();
    }

//    printf("Dopo la rimozione del nodo %d: ",da_rimuovere->GetId());
//    Stampa_NIC_NLC(NLC);
    
    // aggiorno i campi nella casella
    rmv->clique_next = rmv->clique_prev = NULL;
    rmv->e_nella_clique = false;
    rmv->era_nella_clique_iniziale = true;
    C->size -= 1;


    // Adesso passo all'inserimento del nodo...
    index = da_inserire->GetIndex();
    ins = &(C->array_nodi[index]);

    // Per prima cosa bisogna eliminare il nodo dalla lista dei nodi del grafo residuo
    if (ins->graph_prev == NULL) { // se sono il primo nodo nella lista del grafo
        if (ins->graph_next == NULL) C->testa_graph = NULL;
        else {
            C->testa_graph = ins->graph_next;
            ins->graph_next->graph_prev = NULL;
        }
    } else if (ins->graph_next == NULL) { // se sono l'ultimo nodo nella lista del grafo 
        ins->graph_prev->graph_next = NULL;
    } else {
        ins->graph_next->graph_prev = ins->graph_prev;
        ins->graph_prev->graph_next = ins->graph_next;
    }

    ins->graph_prev = ins->graph_next = NULL;

    // ... a questo punto devo inserire il nodo nella lista della clique
    for (l_index = index - 1; l_index >= 0; l_index--) {
        if (C->array_nodi[l_index].e_nella_clique == true) break;
    }

    for (r_index = index + 1; r_index < C->dim_array; r_index++) {
        if (C->array_nodi[r_index].e_nella_clique == true) break;
    }

    if (l_index < 0) { // se devo essere inserito come primo nodo della lista...
        if (r_index >= C->dim_array) { //...ma anche come unico
            C->testa_clique = ins;
        } else {
            ins->clique_next = C->testa_clique;
            C->testa_clique->clique_prev = ins;
            C->testa_clique = ins;
        }
    } else { //... non sono il primo della lista...
        if (r_index >= C->dim_array) { //... ma sono l'ultimo		
            ins->clique_prev = &(C->array_nodi[l_index]);
            ins->clique_prev->clique_next = ins;
        } else { //... ne l'ultimo
            ins->clique_prev = &(C->array_nodi[l_index]);
            ins->clique_next = &(C->array_nodi[r_index]);

            ins->clique_prev->clique_next = ins;
            ins->clique_next->clique_prev = ins;
        }
    }

    ins->e_nella_clique = true;
    C->size++;

    // Incremento l'incidenza di tutti quanti i nodi adiacenti a quello inserito nella clique tranne quelli adiacenti al nodo rimosso
    ptr_arco = da_inserire->GetCorona_dyn();
    while (ptr_arco != NULL) {

        v = ptr_arco->GetNodeTo();
        index = v->GetIndex();

        if (g->getArc(da_rimuovere, v)->getStato_dyn() == 0) {
            C->array_nodi[index].incidenza += 1;

#if defined(DB_Swap_Nodi) || defined(DEBUG)	
            printf("Incremento a %d l'incidenza del nodo %d\n", C->array_nodi[index].incidenza, v->GetId());
#endif                
            // Se la nuova incidenza di v e' pari a |C| ... 
            if (C->array_nodi[index].incidenza == clique_size) {

                
                // ...il nodo deve essere rimosso dall'NLC...   
                if (NLC->nodo_in_heap(v) == true) NLC->Remove_From_Heap(v->GetHeap_pos());

                //...e se il suo inserimento non sfora il budget va inserito nel NIC
                if (C->colori_nella_clique + Verifica_Costo_Inserimento(C->array_nodi[index].nodo_ptr, C) <= budget) {

                    NIC->Insert_to_heap(v);
                    

#if defined(DB_Swap_Nodi) || defined(DEBUG)	
                    printf("Il nodo %d ha ora un incidenza pari a %d, quindi va inserito nella clique (NIC!=0).\n", v->GetId(), C->array_nodi[v->GetIndex()].incidenza);
#endif 

                }

            } else if (C->array_nodi[index].era_nella_clique_iniziale == false && C->array_nodi[index].incidenza == clique_size - 1) {
                NLC->Insert_to_heap(v);

#if defined(DB_Swap_Nodi) || defined(DEBUG)	
                printf("Il nodo %d ha ora un incidenza pari a %d e siccome non era nella clique iniziale va inserito nel NLC.\n", v->GetId(), C->array_nodi[v->GetIndex()].incidenza);
#endif  

            }
        } else {
            // Ci potrebbero essere dei nodi la cui incidenza non e' cambiata ed e' pari a |C|, ma il costo di inserimento si e' ridotto.
            // In questo caso il nodo deve essere inserito nel NIC.
            // Se la nuova incidenza di v e' pari a |C| ... 
            if (C->array_nodi[index].incidenza == clique_size) {

                //...e se il suo inserimento non sfora il budget va inserito nel NIC
                if (C->colori_nella_clique + Verifica_Costo_Inserimento(C->array_nodi[index].nodo_ptr, C) <= budget) {

                    NIC->Insert_to_heap(v);
                    
                    
#if defined(DB_Swap_Nodi) || defined(DEBUG)	
                    printf("Il nodo %d ha ora un incidenza pari a %d, quindi va inserito nella clique (NIC!=0).\n", v->GetId(), C->array_nodi[v->GetIndex()].incidenza);
#endif 

                }
            }
        }

        ptr_arco = ptr_arco->getNext_dyn();
    }    

//    printf("Dopo l'inserimento del nodo %d: ",da_rimuovere->GetId());
//    Stampa_NIC_NLC(NLC);
    
#if defined(DB_Swap_Nodi) || defined(DEBUG)	
    printf("\n*** [FINE Swap_Nodi_C] ***\n ");
#endif        
    
    return true;
}


void Svuota_Clique(Clique *C) {

    int i, index, prev_index;
    Node* ptr_nodo;

    C->size = 0;
    C->testa_clique = NULL;
    C->colori_nella_clique = 0;

    memset(C->occorrenze_colori, 0, sizeof (int) * C->dim_array_occorrenze);

    //C->testa_clique = &(C->array_nodi[0]);

    // Per la prima cella dell'array bisogna usare del codice diverso da quello dentro il for 
    C->array_nodi[0].clique_next = C->array_nodi[0].clique_prev = NULL;
    C->array_nodi[0].graph_next = NULL;
    C->array_nodi[0].graph_prev = NULL;
    //C->array_nodi[0].nodo_ptr = g->GetSingleNode(0);
    C->array_nodi[0].e_nella_clique = false;
    C->array_nodi[0].era_nella_clique_iniziale = false;
    C->array_nodi[0].incidenza = 0;

    for (i = 1; i < C->dim_array - 1; i++) {
        C->array_nodi[i].clique_next = C->array_nodi[i].clique_prev = NULL;
        C->array_nodi[i].graph_next = NULL;
        C->array_nodi[i].graph_prev = NULL;
        C->array_nodi[i].e_nella_clique = false;
        C->array_nodi[i].era_nella_clique_iniziale = false;
        C->array_nodi[i].incidenza = 0;
        //C->array_nodi[i].nodo_ptr = g->GetSingleNode(i);

    }

    C->array_nodi[i].incidenza = 0;
    C->array_nodi[i].clique_next = C->array_nodi[i].clique_prev = NULL;
    C->array_nodi[i].graph_next = NULL;
    C->array_nodi[i].graph_prev = NULL;
    //C->array_nodi[i].nodo_ptr = g->GetSingleNode(i);
    C->array_nodi[i].era_nella_clique_iniziale = false;
    C->array_nodi[i].e_nella_clique = false;



    ptr_nodo = g->getGrafo_dinamico();
    index = ptr_nodo->GetIndex();
    C->testa_graph = &(C->array_nodi[index]);
    C->array_nodi[index].graph_prev = NULL;
    prev_index = index;
    ptr_nodo = ptr_nodo->GetNodo_down_dyn();

    while (ptr_nodo != NULL) {
        index = ptr_nodo->GetIndex();
        C->array_nodi[index].graph_next = NULL;
        C->array_nodi[index].graph_prev = &(C->array_nodi[prev_index]);
        C->array_nodi[prev_index].graph_next = &(C->array_nodi[index]);

        prev_index = index;
        ptr_nodo = ptr_nodo->GetNodo_down_dyn();
    }


}


void Libera_mem_Clique(Clique *C) {
    
    if(C!=NULL){
        free(C->array_nodi);
        free(C->occorrenze_colori);
    }
    else{
        printf("\n[Libera_mem_Clique] ERRORE: il puntatore passato in input e' null.\n");
        exit(0);
    }
    
}


void Inserisci_e_Rimuovi_Inconsistenti(Node *ptr, Clique *C) {

    Nodo_Clique *rmv, *tmp;

    rmv = C->testa_clique;
    while (rmv != NULL) {

        if (rmv->nodo_ptr != ptr && (g->getArc(ptr, rmv->nodo_ptr))->getStato_dyn() == 0) {
            tmp = rmv->clique_next;
            Rimuovi_da_C(rmv->nodo_ptr, C);
            rmv = tmp;
        } else rmv = rmv->clique_next;

    }
    Inserisci_in_C(ptr, C);
    
    //TODO: qui vengono casualmente rimossi nodi fino a quando la clique non diventa ammissibile. Sarebbe il caso di utilizzare 
    //          qualche metodo piu' furbo di cancellazione.
    int pos,i;
    while(C->colori_nella_clique > budget){ 
        
        // scelgo in modo casuale un nodo della clique da rimuovere
        pos = i_random(1,C->size);
        rmv = C->testa_clique;
        
        for(i=1;i<pos;i++){
            rmv = rmv->clique_next;
        }
        
        if(rmv->nodo_ptr != ptr) Rimuovi_da_C(rmv->nodo_ptr, C);
        else{
            if(pos==C->size) Rimuovi_da_C(rmv->clique_prev->nodo_ptr, C);
            else Rimuovi_da_C(rmv->clique_next->nodo_ptr, C);
        }
    }
}

Node *Find_nodo_in_G(int pos, Clique *C) {

    // TODO : questa funzione non e' efficientissima. Trovare se possibile un modo piu' veloce di scegliere in modo random un nodo del grafo residuo.
    Nodo_Clique *ptr;
    int i;

#ifndef Performance  	
    if (numNodes_dyn - C->size < pos) {
        printf("\nERRORE (find_nodo_in_G): La posizione %d indicata e' piu' grande del numero di nodi (%d) disponibili nel grafo!!!\n\n", pos, numNodes_dyn);
        exit(0);
    }
#endif

    ptr = C->testa_graph;

    for (i = 1; i < pos; i++) ptr = ptr->graph_next;

    return ptr->nodo_ptr;
}

void Update_Penalties(int pd, Clique *C) {

    Nodo_Clique *ptr;
    Node *ptr_nodo;

    // ogni pd iterazioni ciclicamente vengono decrementati di uno le penalties dei nodi.
    // Poichè le penalties dei nodi nella clique dovrebbero essere incrementate di uno, si evita di modificarle per non 
    // sommare e poi sottrarre, quindi si decrementano le penalties solo dei nodi fuori clique.
    if (numSteps % pd == 0) {
        ptr_nodo = g->getGrafo_dinamico();
        while (ptr_nodo != NULL) {
            if (ptr_nodo->GetChiave_ordinamento() > 0) ptr_nodo->SetChiave_ordinamento(ptr_nodo->GetChiave_ordinamento() - 1);

            ptr_nodo = ptr_nodo->GetNodo_down_dyn();
        }
    } else {
        ptr = C->testa_clique;
        while (ptr != NULL) {

            ptr->nodo_ptr->SetChiave_ordinamento(ptr->nodo_ptr->GetChiave_ordinamento() + 1);
            ptr = ptr->clique_next;
        }
    }
}

Node *Expand(Clique *C, bool build_NIC) {

    // Il valore booleano serve per stabilire se l'insieme NIC deve essere costruito da zero
    // oppure qualcuno lo ha gia' fatto e quindi expand deve solo utilizzarlo. Nello specifico questo flag viene
    // usato quando il plateau individua uno o piu' nodi da inserire nel NIC. In questo caso l'insieme viene
    // costruito direttamente dal plateau e poi passato all'expand.
    // N.B. Chiunque sia a costruire l'insieme NIC deve garantire che il costo di inserimento dei nodi in esso non ecceda mai il budget.

    int i, costo_inserimento;
    Nodo_Clique *ptr;
    Node *u, *ultimo = NULL, *temp_estratto;
    Arc *ptr_arco;

    //#define DB_EXPAND 1
    
#ifndef Performance  	
    if (C->size < 0) {
        printf("\nERRORE (Expand): Non ci sono nodi (size=%d) nella clique passata in input!!!\n\n", C->size);
        exit(0);
    }

#endif
#if defined(DB_EXPAND) || defined(DEBUG)
    printf("\n*** [Expand] ***\n");
    Stampa_Clique(C,0);
#endif

    if (build_NIC == true) {
        // Per prima cosa bisogna creare l'insieme NIC
        // Il while viene usato per inserire nell'insieme tutti quanti i nodi del grafo residuo adiacenti al primo nodo della clique...
        ptr = C->testa_clique;
        u = ptr->nodo_ptr;
        ptr_arco = u->GetCorona_dyn();
        while (ptr_arco != NULL) {
            // se l'altro estremo non fa parte della clique e la sua penalita' e' inferiore o uguale a 10
            if (C->array_nodi[ptr_arco->GetNodeTo()->GetIndex()].e_nella_clique == false && ptr_arco->GetNodeTo()->GetPenalties() <= 10 ) {
                NIC->Insert_to_heap(ptr_arco->GetNodeTo());
            }
            ptr_arco = ptr_arco->getNext_dyn();
        }

        //...a questo punto bisogna controllare che ognuno dei nodi inseriti temporaneamente nel NIC siano effettivamente adiacenti ai restanti nodi della clique
        ptr = ptr->clique_next;
        while (ptr != NULL) {

            u = ptr->nodo_ptr;
            for (i = 0; i < NIC->getHeapSize(); i++) {
                // se il nodo considerato non e' adiancente a quello della clique, allora va rimosso 
                if (g->getArc((Node*) NIC->getNodo(i), u)->getStato_dyn() == 0) {
                    NIC->Remove_From_Heap(i);
                    i--;
                }
            }
            ptr = ptr->clique_next;
        }

        // Infine vado a controllare se qualcuno dei nodi del NIC ha un costo di inserimento troppo elevato
        for (i = 0; i < NIC->getHeapSize(); i++) {
            temp_estratto = (Node*) NIC->getNodo(i);
            costo_inserimento = Verifica_Costo_Inserimento(temp_estratto, C);
            if (costo_inserimento + C->colori_nella_clique > budget) {
                NIC->Remove_From_Heap(i);
                i--;
            }
        }
 
    }

#if defined(DB_EXPAND) || defined(DEBUG)

    printf("NIC: ");
    for (i = 0; i < NIC->getHeapSize(); i++) {
        u = (Node*) NIC->getNodo(i);
        printf("%d(%g), ", u->GetId(), u->GetChiave_ordinamento());
    }
    printf("\n");
#endif	

    while (NIC->getHeapSize() > 0) {
        temp_estratto = (Node*) NIC->Extract_Min();

#if defined(DB_EXPAND) || defined(DEBUG)
        printf("\nInserisco il nodo %d.\n", temp_estratto->GetId());
#endif        

        Inserisci_in_C(temp_estratto, C);

        ultimo = temp_estratto;
        numSteps += 1;

        //printf("Rimuovo dal NIC : ");
        for (i = 0; i < NIC->getHeapSize(); i++) {
            u = (Node*) NIC->getNodo(i);
            if (g->getArc(ultimo, u)->getStato_dyn() == 0 || (C->colori_nella_clique + Verifica_Costo_Inserimento(u, C) > budget)) {
                NIC->Remove_From_Heap(i);
                //printf("%d , ",u->GetId());
                i--;
            }
        }
        //printf("\n");

#if defined(DB_EXPAND) || defined(DEBUG)        
        printf("incidenze: ");
        for (int k = 0; k < numNodes; k++) {
            printf("%d(%d), ", k, C->array_nodi[k].incidenza);
        }
        printf("\n");
#endif               

    }

#if defined(DB_EXPAND) || defined(DEBUG)
    Stampa_Clique(C,0);
    Verifica_Clique(C);
    printf("\n*** [FINE Expand] ***\n");
#endif	

    return ultimo;

}

Node *Plateau(Clique *C, Clique *C1) {

    // ATTENZIONE: A differenza dell'insieme NIC, nell'insieme NLC ci possono essere nodi il cui swap eccede ul budget. Questo perche'
    //             il filtraggio di questi nodi durante la costruzione dell'NLC e' troppo costoso e quindi meglio farli scartare direttamente 
    //             dalla funzione di swap.

    Nodo_Clique *ptr;
    Node *v, *u, *ultimo = NULL, *da_rimuovere, *temp_estratto;
    int i, k;

    //#define DB_PLATEAU

#ifndef	Performance
    // TODO : La costruzione dell'insieme NLC e' stata fatta supponendo che il NIC e' vuoto quando viene invocato il plateau 
    // 		ed inoltre supponendo che nella clique ci siano almeno due nodi. Nel caso in cui ce n'è uno solo questo nodo dovrebbe
    // 		essere sconnesso dagli altri (altrimenti )l'insieme NLC
    //			dovrebbe contenere i restanti n-1 nodi del grafo. 
    if (C->size == 1) {
        printf("\nERRORE [Plateau]: la clique passata in input ha un solo elemento.\n\n");
        exit(1);
    }
#endif

#if defined(DB_PLATEAU) || defined(DEBUG)	
    printf("\n*** [PLATEAU] ***\n ");
    Stampa_Clique(C,0);
#endif

    for (i = 0; i < numNodes; i++) {
        if (C->array_nodi[i].e_nella_clique == true) C->array_nodi[i].era_nella_clique_iniziale = true;
        else C->array_nodi[i].era_nella_clique_iniziale = false;
        // TODO: la seguente inizializzazione e' una pezza a colori per garantire che tutti i nodi del grafo abbiamo la pos_heap a -1.
        //       in realta' bisognerebbe fare in modo che quando si termina la expand e quando si interrompe la plateau (perche' e' stato
        //       trovato un nodo di expand) gli heap vengano correttamente ripuliti. 
        g->GetSingleNode(i)->SetHeap_pos(-1);
        
    }

    // Scorro le incidenze per inserire in NLC tutti quanti i nodi con un'incidenza pari a |C|-1 (tranne quelli della clique) e il cui inserimento non sfora il budget.
    NLC->Clean_Heap();

    ptr = C->testa_graph;
    while (ptr != NULL) {
        // TODO la seconda condizione dovrebbe essere sempre vera dato che stiamo considerando solo i nodi del grafo residuo.
        if (ptr->incidenza == C->size - 1 && ptr->e_nella_clique == false && ptr->nodo_ptr->GetPenalties() <= 10){
            NLC->Insert_to_heap(ptr->nodo_ptr);
            
        }
        
#ifndef	Performance
        // TODO : Al termine della costruzione dell'insieme NLC, non ci devono essere nodi collegati a tutti i vertici della clique altrimenti
        // 		l'ipotesi di partenza che il plateau viene chiamato solo quando NIC e' vuoto non e' valida.       
        if (ptr->incidenza == C->size && ptr->e_nella_clique == false) {
            int temp_costo = Verifica_Costo_Inserimento(ptr->nodo_ptr, C);
            if (C->colori_nella_clique + temp_costo <= budget) {
                printf("\nERRORE [Plateau]: il nodo %d e' collegato a tutti i nodi della clique mentre l'insieme NIC dovrebbe essere vuoto!!!.\n\n", ptr->nodo_ptr->GetId());
                printf("temp_costo = %d\n", temp_costo);
                Stampa_Clique(C,0);
                exit(1);
            }
        }
#endif	        

        ptr = ptr->graph_next;
    }

 
#if defined(DB_PLATEAU) || defined(DEBUG)	

    printf("incidenze: ");
    for (k = 0; k < numNodes; k++) {
        printf("%d(%d), ", k, C->array_nodi[k].incidenza);
    }
    printf("\n\n");
    printf("NLC: ");
    for (k = 0; k < NLC->getHeapSize(); k++) {
        u = (Node*) NLC->getNodo(k);
        //printf("%d(pri=%g,inc=%d), ", u->GetId(), u->GetChiave_ordinamento(),C->array_nodi[u->GetIndex()].incidenza);
        printf("%d(%d), ", u->GetId(), C->array_nodi[u->GetIndex()].incidenza);
    }
    printf("\n\n");
#endif	

    
    while (NIC->getHeapSize() == 0 && NLC->getHeapSize() > 0 && Intersezione(C, C1) == true) {
        temp_estratto = (Node*) NLC->Extract_Min();

        // individuo qual'è il nodo "da_rimuovere" dalla clique.
        da_rimuovere = NULL;
        ptr = C->testa_clique;
        while (ptr != NULL) {
            v = ptr->nodo_ptr;
            if (g->getArc(temp_estratto, v)->getStato_dyn() == 0) {
                da_rimuovere = v;
                break;
            }
            ptr = ptr->clique_next;
        }

#ifndef	Performance
        if (da_rimuovere == NULL) {
            printf("\nERRORE [Plateau]: Non e' stato individuato alcun nodo della clique da rimpiazzare con %d\n\n", temp_estratto->GetId());
            exit(0);
        }
#endif

#if defined(DB_PLATEAU) || defined(DEBUG)	
        printf("\n--> Provo lo swap %d<-->%d ", da_rimuovere->GetId(), temp_estratto->GetId());
        fflush(stdout);
#endif	

        // se l'operazione di swap avviene con successo incremento le iterazioni effettuate
        if (Swap_Nodi_C(da_rimuovere, temp_estratto, C) == true) {
            numSteps += 1;
            //printf("numsteps = %d\n",numSteps);
            ultimo = temp_estratto;

            if(C->size == C_Best->size && C->colori_nella_clique < C_Best->colori_nella_clique) Copia_Clique(C, C_Best);      
            
#if defined(DB_PLATEAU) || defined(DEBUG)	
            printf("e riesco ad effettuarlo\n\n");
#endif	

#if defined(DB_PLATEAU) || defined(DEBUG)
            // TODO : da rimuovere questo #if
            printf("\n");
            Stampa_Clique(C,0);
            printf("incidenze: ");
            for (k = 0; k < numNodes; k++) {
                printf("%d(%d), ", k, C->array_nodi[k].incidenza);
            }
            printf("\n");
            printf("NIC <%d>: ", NIC->getHeapSize());
            for (k = 0; k < NIC->getHeapSize(); k++) {
                u = (Node*) NIC->getNodo(k);
                //printf("%d(pri=%g,inc=%d), ", u->GetId(), u->GetChiave_ordinamento(),C->array_nodi[u->GetIndex()].incidenza);
                printf("%d(%d), ", u->GetId(), C->array_nodi[u->GetIndex()].incidenza);
                fflush(stdout);
            }
            printf("\n");
            printf("NLC <%d>: ", NLC->getHeapSize());
            for (k = 0; k < NLC->getHeapSize(); k++) {
                u = (Node*) NLC->getNodo(k);
                //printf("%d(pri=%g,inc=%d), ", u->GetId(), u->GetChiave_ordinamento(),C->array_nodi[u->GetIndex()].incidenza);
                printf("%d(%d), ", u->GetId(), C->array_nodi[u->GetIndex()].incidenza);
                fflush(stdout);
            }
            printf("\n\n");
#endif           
        }
#if defined(DB_PLATEAU) || defined(DEBUG)	
        else printf("ma fallisco!!!\n\n");
#endif	        

    }

#if defined(DB_PLATEAU) || defined(DEBUG)	
    //Stampa_Clique(C,0);
    Verifica_Clique(C);
    printf("\n *** [FINE PLATEAU] ***\n ");
#endif

    return ultimo;
}

bool Intersezione(Clique *C, Clique *C1) {

    Nodo_Clique *ptr1, *ptr2;
    //bool risposta=false; 

    ptr1 = C->testa_clique;
    ptr2 = C1->testa_clique;

    while (ptr1 != NULL && ptr2 != NULL) {
        if (ptr1->nodo_ptr->GetId() == ptr2->nodo_ptr->GetId()) return true;
        else if (ptr1->nodo_ptr->GetId() > ptr2->nodo_ptr->GetId()) ptr2 = ptr2->clique_next;
        else ptr1 = ptr1->clique_next;
    }

    return false;

}

void Stampa_Clique(Clique *C, short flag) {

    int i;
    Nodo_Clique *ptr, *ptr1, *ptr2;
    Arc *ptr_arco;

    printf("\nSize= %d, Colori=%d, CLIQUE: ", C->size, C->colori_nella_clique);

    ptr = C->testa_clique;
    while (ptr != NULL) {
        printf("%d, ", ptr->nodo_ptr->GetId());
        if (ptr->e_nella_clique == false) {
            printf("ERRORE [Stampa_Clique]: il nodo %d e' nella clique ma il suo flag is_in_clique vale %d\n\n", ptr->nodo_ptr->GetId(), ptr->e_nella_clique);
            exit(0);
        }
        ptr = ptr->clique_next;
    }
    printf("\t");
    //Verifica_Clique(C);
    
    if(flag>0){
        printf("\n\n->ARCHI:");
        ptr1 = C->testa_clique;
        while (ptr1->clique_next != NULL) {
            ptr2 = ptr1->clique_next;
            while (ptr2 != NULL) {
                ptr_arco = g->getArc(ptr1->nodo_ptr, ptr2->nodo_ptr);
                printf("(%d,%d)=%d, ", ptr1->nodo_ptr->GetId(), ptr2->nodo_ptr->GetId(), ptr_arco->GetColor());

                ptr2 = ptr2->clique_next;
            }
            ptr1 = ptr1->clique_next;
        }
        printf("\n");
    }
    
    if(flag>1){
        printf("\n->Occorr_colori: ");
        for (i = 0; i < C->dim_array_occorrenze; i++)
            printf("%d[%d], ", i, C->occorrenze_colori[i]);

        printf("\n\n");     
    }
    

    
}

void Swap(int a[], int i, int j) {

    int temp;

    temp = a[i];
    a[i] = a[j];
    a[j] = temp;

}

int Confronta(int a[], int i, int j) {
    if (a[i] >= a[j])return 1;
    return 0;
}

int Partition(int a[], long p, long r) {

    int i, j;

    i = p - 1;

    for (j = p; j < r; j++) {
        if (Confronta(a, j, r) == 1) {
            i = i + 1;
            Swap(a, i, j);
        }
    }

    Swap(a, i + 1, r);
    return i + 1;

}

void Quicksort(int a[], long begin, long end) {

    if (begin < end) {

        int pivot;
        pivot = Partition(a, begin, end);

        Quicksort(a, begin, pivot - 1);
        Quicksort(a, pivot + 1, end);

    }
}

void Recupera_Dati_Memoria(char *inputfile, float *vmpeak, float *vmsize, float *vmhwn, float *vmrss, float *vmdata, float *vmstk, float *vmlib) {

    char command_str[200], command_str2[200];
    long temp = -1;
    FILE * fp;

    sprintf(command_str, "less /proc/%d/status |grep Vm > %s", getpid(), inputfile);
    system(command_str);

    if ((fp = fopen(inputfile, "r")) == NULL) {
        printf("ERROR: Can't open input file %s !\n\n\n", inputfile);
        exit(1);
    }

    fscanf(fp, "%s %ld %s", command_str, &temp, command_str2);
    //printf("Leggo dal file %s e %ld\n",command_str, temp);
    *vmpeak = (float) temp / 1000;
    //printf("vmpeak = %.2f\n",*vmpeak);		

    fscanf(fp, "%s %ld %s", command_str, &temp, command_str2);
    //printf("Leggo dal file %s e %ld\n",command_str, temp);
    *vmsize = (float) temp / 1000;
    //printf("vmsize = %.2f\n",*vmsize);			

    fscanf(fp, "%s %ld %s", command_str, &temp, command_str2);
    //printf("Leggo dal file %s e %ld\n",command_str, temp);

    fscanf(fp, "%s %ld %s", command_str, &temp, command_str2);
    //printf("Leggo dal file %s e %ld\n",command_str, temp);
    *vmhwn = (float) temp / 1000;
    //printf("vmhwn = %.2f\n",*vmhwn);		

    fscanf(fp, "%s %ld %s", command_str, &temp, command_str2);
    //printf("Leggo dal file %s e %ld\n",command_str, temp);
    *vmrss = (float) temp / 1000;
    //printf("vmrss = %.2f\n",*vmrss);

    fscanf(fp, "%s %ld %s", command_str, &temp, command_str2);
    //printf("Leggo dal file %s e %ld\n",command_str, temp);
    *vmdata = (float) temp / 1000;
    //printf("vmrss = %.2f\n",*vmdata);

    fscanf(fp, "%s %ld %s", command_str, &temp, command_str2);
    //printf("Leggo dal file %s e %ld\n",command_str, temp);
    *vmstk = (float) temp / 1000;
    //printf("vmrss = %.2f\n",*vmstk);

    fscanf(fp, "%s %ld %s", command_str, &temp, command_str2);
    //printf("Leggo dal file %s e %ld\n",command_str, temp);

    fscanf(fp, "%s %ld %s", command_str, &temp, command_str2);
    //printf("Leggo dal file %s e %ld\n",command_str, temp);
    *vmlib = (float) temp / 1000;
    //printf("vmrss = %.2f\n",*vmlib);

    fclose(fp);
}

void Stampa_Soluzione(Clique *C, short flag) {


    Nodo_Clique *ptr;

    t = timer() - t;

    if( Verifica_Clique(C)==true ){
        
        if (flag == 0) {
            printf("\t %d \t %d \t %d \t %d \t %d \t %d \t %.2f \t %d\t%ld\t%d\tOK!\n", g->GetNumnodes(), g->GetNumarcs() / 2, g->GetNumcolors(), budget, C->size, C->colori_nella_clique, t,nodi_cancellati,miglioramenti_cp,maxSteps);
            //fflush(stdout);
        } else {
            printf("\t#nodes= %d \t #numarcs= %d \t #numcolors= %d \t budget= %d \t |C|= %d\t l(C)=%d\t time= %.2f \t nodi_cancellati= %d\tmiglioramento_3_fase= %ld\t%d\nCLIQUE: ", g->GetNumnodes(), g->GetNumarcs() / 2, g->GetNumcolors(), budget, C->size, C->colori_nella_clique, t,nodi_cancellati,miglioramenti_cp,maxSteps);
            ptr = C->testa_clique;
            while (ptr != NULL) {
                printf("%d, ", ptr->nodo_ptr->GetId());
                ptr = ptr->clique_next;
            }
            //fflush(stdout);
            printf("\tOK!\n");
        }
    }
}

bool Verifica_Clique(Clique *C) {

    Nodo_Clique *ptr1, *ptr2;
    int counter = 1, colori_in_clique = 0;
    int *occ_colori;
    Arc *ptr_arco;

    
    occ_colori = (int*) malloc(sizeof (int) *C->dim_array_occorrenze);
    memset(occ_colori, 0, sizeof (int) *C->dim_array_occorrenze);

    if(C->testa_clique == NULL){
       printf("\n\nERRORE [Verifica_Soluzione]: La clique passata in inpur e' vuota.!!!\n\n");
       exit(0);    
    }
    
    ptr1 = C->testa_clique;
    while (ptr1->clique_next != NULL) {
        ptr2 = ptr1->clique_next;
        while (ptr2 != NULL) {
            ptr_arco = g->getArc(ptr1->nodo_ptr, ptr2->nodo_ptr);
            if (ptr_arco->getStato_dyn() == 0) {
                printf("\n\nERRORE [Verifica_Soluzione]: I nodi %d e %d sono nella clique ma non sono adiacenti.!!!\n\n", ptr1->nodo_ptr->GetId(), ptr2->nodo_ptr->GetId());
                exit(0);
            }
            if (occ_colori[ptr_arco->GetColor()] == 0) colori_in_clique++;

            occ_colori[ptr_arco->GetColor()] += 1;

            ptr2 = ptr2->clique_next;
        }
        ptr1 = ptr1->clique_next;
        counter++;
    }

    if (colori_in_clique != C->colori_nella_clique) {
        printf("\n\nERRORE [Verifica_Soluzione]: I colori riportati nella clique sono %d mentre quelli contati effettivamente sono %d. !!!\n\n", C->colori_nella_clique, colori_in_clique);
        exit(0);
    }

    if (C->colori_nella_clique > budget) {
        printf("\n\nERRORE [Verifica_Soluzione]: I colori nella clique (%d) sono maggiori del budget %d. !!!\n\n", C->colori_nella_clique, budget);
        exit(0);
    }

    if (counter != C->size) {
        printf("\n\nERRORE [Verifica_Soluzione]: La dimensione della clique e' %d ma i nodi contati sono %d. !!!\n\n", C->size, counter);
        exit(0);
    }
    
    return true;
    
    //printf("\tOK!\n");
}

void Stampa_NIC_NLC(Heap *h){

    int i;
    Node *ptr;
    
    for(i=0;i<h->getHeapSize();i++){
        ptr = (Node*) h->getNodo(i);
        printf("[%d]=(%d,%g),  ",i,ptr->GetId(),ptr->GetChiave_ordinamento());
        if(ptr->GetChiave_ordinamento()>10){
            printf("\n\n[Stampa_NIC_NLC] ERRORE: Nell'heap ci sono elementi con penalties maggior di 10!!!\n\n");
            exit(0);
        }
    } 
    printf("\n");
}

void Stampa_NIL(my_heap *h){

    int i;
    Node *ptr;
    
    printf("\nheapsize = %ld: ",h->heap_size);
    for(i=0;i<h->heap_size;i++){
        printf("[%d]=(%d,%d)|%f,%ld|, ",i,h->heap[i].da_inserire->GetId(),h->heap[i].da_rimuovere->GetId(),h->heap[i].key,h->heap[i].heap_pos);
    } 
    printf("\n");
}

void My_Init_heap(int size, my_heap *z) {
    z->heap_size = 0;
    z->h_degree = 2;
    //z->heap = (Coppia*) new Coppia*[size];
    z->heap = (Coppia*) malloc(sizeof(Coppia)*(size+1));

}

void My_Delete_Heap(my_heap *z) {
    delete [] z->heap;
}

void My_put_to_pos_in_heap(Coppia* ptr, int pos, my_heap *z) {
    
    z->heap[pos].da_inserire = ptr->da_inserire;
    z->heap[pos].da_rimuovere = ptr->da_rimuovere;
    z->heap[pos].key = ptr->key;
    z->heap[pos].heap_pos = pos;

}

//bool My_nodo_in_heap(Coppia* ptr) {
//
//    if (ptr->heap_pos == -1) return false;
//    return true;
//}

bool My_non_empty_heap(my_heap *z) {

    if (z->heap_size > 0) return true;
    else return false;
}

void My_heap_Decrease_key(Coppia* ptr, double key_value, my_heap *z) {

    int h_current_pos, h_new_pos;
    Coppia *temp;
    //printf("\nMy_heap_Decrease_key invocata sulla coppia (%d,%d)|%f la cui pos nell'heap e' %d\n",ptr->da_inserire->GetId(),ptr->da_rimuovere->GetId(),key_value,ptr->heap_pos);

    for (h_current_pos = ptr->heap_pos; h_current_pos > 0; h_current_pos = h_new_pos) {
        //printf("\nh_current_pos %d, h_new_pos %d\n", h_current_pos, h_new_pos);
        h_new_pos = (h_current_pos - 1) / 2;
        temp = &z->heap[h_new_pos];
        
        if (key_value >= temp->key) break;
        
        My_put_to_pos_in_heap(temp, h_current_pos, z);

    }

    My_put_to_pos_in_heap(ptr, h_current_pos, z);

}

void My_Insert_to_heap(Coppia* ptr, my_heap *z) {
    //printf("\nDevo inserire la coppia (%d,%d)|%f nella posizione %d dell'heap.\n",ptr->da_inserire->GetId(),ptr->da_rimuovere->GetId(),ptr->key,z->heap_size);
    My_put_to_pos_in_heap(ptr, z->heap_size, z);
    //printf("\nsono uscito dalla funzione My_put_to_pos_in_heap\n");
    z->heap_size++;
    //Stampa_NIL(z);
    //printf("\nsto per entrare in My_heap_decrease_key\n");
    My_heap_Decrease_key(&z->heap[z->heap_size-1], z->heap[z->heap_size-1].key, z);
    //My_heap_Decrease_key(ptr, ptr->key, z);

}

Coppia* My_Extract_Min(my_heap *z) {

    Coppia *ptr, *nodo_k;
    long h_current_pos, h_new_pos, h_last_pos, h_pos;
    double dist_k, dist_min;

    ptr = &z->heap[0];
    ptr->heap_pos = -1;

    z->heap_size--;

    if (z->heap_size > 0) {

        nodo_k = &z->heap[z->heap_size];
        dist_k = nodo_k->key;

        h_current_pos = 0;

        while (1) {

            h_new_pos = (h_current_pos * 2) + 1;
            if (h_new_pos >= z->heap_size) break;

            dist_min = z->heap[h_new_pos].key;

            h_last_pos = h_new_pos + z->h_degree;
            if (h_last_pos > z->heap_size) h_last_pos = z->heap_size;

            for (h_pos = h_new_pos + 1; h_pos < h_last_pos; h_pos++) {
                if (z->heap[h_pos].key < dist_min) {
                    h_new_pos = h_pos;
                    dist_min = z->heap[h_pos].key;
                }

            }

            if (dist_k <= dist_min) break;

            My_put_to_pos_in_heap(&z->heap[h_new_pos], h_current_pos, z);
            h_current_pos = h_new_pos;
        }

        My_put_to_pos_in_heap(nodo_k, h_current_pos, z);

    }
    return ptr;

}

void My_Remove_From_Heap(int pos, my_heap *z) {


    Coppia *nodo_k;
    long h_current_pos, h_new_pos, h_last_pos, h_pos;
    double dist_k, dist_min;

    if (pos > z->heap_size - 1) {
        printf("\n[My_Remove_From_Heap] ERRORE: e' stata richiesta la cancellazione del nodo nella pos %d che e' al di la' della dimensione dell'heap (%ld)\n\n", pos, z->heap_size);
        exit(1);
    }

    z->heap[pos].heap_pos = -1;
    z->heap_size--;

    if (z->heap_size > 0) {
        nodo_k = &z->heap[z->heap_size];
        dist_k = nodo_k->key;

        h_current_pos = pos;

        while (1) {

            h_new_pos = (h_current_pos * 2) + 1;
            if (h_new_pos >= z->heap_size) break;

            dist_min = z->heap[h_new_pos].key;

            h_last_pos = h_new_pos + z->h_degree;
            if (h_last_pos > z->heap_size) h_last_pos = z->heap_size;

            for (h_pos = h_new_pos + 1; h_pos < h_last_pos; h_pos++) {
                if (z->heap[h_pos].key < dist_min) {
                    h_new_pos = h_pos;
                    dist_min = z->heap[h_pos].key;
                }

            }

            if (dist_k <= dist_min) break;

            My_put_to_pos_in_heap(&z->heap[h_new_pos], h_current_pos, z);
            h_current_pos = h_new_pos;
        }

        My_put_to_pos_in_heap(nodo_k, h_current_pos, z);
    }

}

void My_Stampa_Heap(my_heap *z) {

    int i;
    printf("\nNIL= \n");
    for (i = 0; i < z->heap_size; i++) {
        printf("[%d,%d]=%f\n", z->heap[i].da_rimuovere->GetId(), z->heap[i].da_inserire->GetId(), z->heap[i].key);
    }
    cout << endl;
}

void My_Clean_Heap(my_heap *z) {

//    int i;
//    for (i = 0; i < z->heap_size; i++)
//        z->heap[i] = NULL;

    z->heap_size = 0;
}

