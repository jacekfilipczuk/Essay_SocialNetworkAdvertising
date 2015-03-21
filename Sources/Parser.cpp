#include "Parser.h"
#include <string.h>
#include <stdio.h>

using namespace std;

typedef struct intestazione_st {
    char name[500];
    char comment[1000];
    int nnodes;
    int nedges;
    int ncolors;
} intestazione;

Parser::Parser(string nomeFile, short parser_type) {

    FILE *f1_bin;
    ifstream f1;

    if (parser_type == CLQ_BIN) {

        if ((f1_bin = fopen(nomeFile.c_str(), "rb")) == NULL) {
            cout << "Impossibile aprire il file: " << nomeFile << "||" << endl;
            exit(1);
        }
    } else {
        f1.open(nomeFile.c_str());
        if (!f1) {
            cout << "Impossibile aprire il file: " << nomeFile << "||" << endl;
            exit(1);
        }
    }

    switch (parser_type) {

        case MBV: ParserMBV(f1);
            break;
        case FVS: ParserFVS2(f1);
            break;
        case CLQ: ParserCLQ(f1);
            break;
        case CLQ_BIN: ParserCLQ_BIN(f1_bin);
            break;
        default: printf("ERROR: %d is not a valid parser type\n\n", parser_type);
            exit(0);
    }

}

Parser::Parser(string nomeFile) {
    ifstream f1;

    f1.open(nomeFile.c_str());
    if (!f1) {
        cout << "Impossibile aprire il file: " << nomeFile << "||" << endl;
        exit(1);
    }



    ParserMBV(f1);
    /*
            char temp_name[1000],*pch,*finale;

            // individuo l'estensione del file in input per stabilire quale parser lanciare
            strcpy(temp_name,nomeFile.c_str());
            pch = strtok (temp_name,".");

            while(pch != NULL){
              finale=pch;
              pch = strtok (NULL, ".");
            }


            if( strcmp(finale,"fvs")==0){
                ParserFVS2(f1);
            }
            else ParserFVS(f1);
     */

}

void Parser::ParserMBV(ifstream &f1) {

    int nArchi, da, a, i, j, useless;
    bool esiste;

    //primi due dati numero nodi e numero archi su prima riga
    f1 >> numNodi;
    f1 >> nArchi;
    f1 >> useless;

    // Vado a leggere il peso di ogni nodo
    for (i = 0; i < numNodi; i++) {
        datiNodo *v = (datiNodo*) malloc(sizeof (datiNodo));
        //f1 >> da >> a;
        v->id = i + 1;
        v->peso = 0;
        vetNodi.push_back(v);
    }

    //cout << "Ho letto i pesi" << endl;


    while (!f1.eof() && f1 >> da && f1 >> a && f1 >> useless) {
        if (da != a)//cout << "Seelf Loop" <<endl;
        {
            datiArco *d = (datiArco*) malloc(sizeof (datiArco));

            //                  da--; // -1 per il fatto che l'indice del nodo1 e' 0.
            //                   a--; //come sopra

            d->da = da;
            d->a = a;

            esiste = false;

            for (j = 0; j < vetArchi.size(); j++) {
                if (vetArchi[j]->da == da && vetArchi[j]->a == a) {
                    esiste = true;
                    break;
                }
                if (vetArchi[j]->da == a && vetArchi[j]->a == da) {
                    esiste = true;
                    break;
                }
            }
            if (!esiste)
                vetArchi.push_back(d);
        }

    }

    //cout << "Ho letto gli archi" << endl;

    //       cout << "nArchi:" <<nArchi<<"  vetArchi:"<<vetArchi.size() <<endl;

    //this->adattaNonOrientato();

    // Calcolo e salvo il grado di ogni nodo
    int grado;
    for (i = 0; i < vetNodi.size(); i++) {
        grado = 0;
        for (j = 0; j < vetArchi.size(); j++)
            if (vetArchi[j]->da == vetNodi[i]->id)grado++;

        vetNodi[i]->grado = grado;

    }

    // Info Generali
    //        for(i=0;i<vetNodi.size();i++){
    //            cout << "Nodo: " << vetNodi[i]->id << " di peso: " << vetNodi[i]->peso << ",di grado: " << vetNodi[i]->grado << " e di ratio: " << (double) vetNodi[i]->peso/vetNodi[i]->grado << endl;
    //        }
    //        for(i=0;i<vetArchi.size();i++){
    //            cout << "(" << vetArchi[i]->da << "," << vetArchi[i]->a << ")" << endl;
    //        }
    //        cout << endl <<  "Ho letto il file di input e costruito il grafo!" << endl << endl;

}

void Parser::ParserFVS(ifstream &f1) {

    int nArchi, da, a, i, j;
    bool esiste;

    //primi due dati numero nodi e numero archi su prima riga
    f1 >> numNodi;
    f1 >> nArchi;

    // Vado a leggere il peso di ogni nodo
    for (i = 0; i < numNodi; i++) {
        datiNodo *v = (datiNodo*) malloc(sizeof (datiNodo));
        f1 >> da >> a;
        v->id = da;
        v->peso = a;
        vetNodi.push_back(v);
    }

    //cout << "Ho letto i pesi" << endl;


    while (!f1.eof() && f1 >> da && f1 >> a) {
        if (da != a)//cout << "Seelf Loop" <<endl;
        {
            datiArco *d = (datiArco*) malloc(sizeof (datiArco));

            //                  da--; // -1 per il fatto che l'indice del nodo1 e' 0.
            //                   a--; //come sopra

            d->da = da;
            d->a = a;

            esiste = false;

            for (j = 0; j < vetArchi.size(); j++) {
                if (vetArchi[j]->da == da && vetArchi[j]->a == a) {
                    esiste = true;
                    break;
                }
                if (vetArchi[j]->da == a && vetArchi[j]->a == da) {
                    esiste = true;
                    break;
                }
            }
            if (!esiste)
                vetArchi.push_back(d);
        }

    }

    //cout << "Ho letto gli archi" << endl;

    //       cout << "nArchi:" <<nArchi<<"  vetArchi:"<<vetArchi.size() <<endl;

    //this->adattaNonOrientato();

    // Calcolo e salvo il grado di ogni nodo
    int grado;
    for (i = 0; i < vetNodi.size(); i++) {
        grado = 0;
        for (j = 0; j < vetArchi.size(); j++)
            if (vetArchi[j]->da == vetNodi[i]->id)grado++;

        vetNodi[i]->grado = grado;

    }

    // Info Generali
    //        for(i=0;i<vetNodi.size();i++){
    //            cout << "Nodo: " << vetNodi[i]->id << " di peso: " << vetNodi[i]->peso << ",di grado: " << vetNodi[i]->grado << " e di ratio: " << (double) vetNodi[i]->peso/vetNodi[i]->grado << endl;
    //        }
    //        for(i=0;i<vetArchi.size();i++){
    //            cout << "(" << vetArchi[i]->da << "," << vetArchi[i]->a << ")" << endl;
    //        }
    //        cout << endl <<  "Ho letto il file di input e costruito il grafo!" << endl << endl;

}

void Parser::ParserFVS2(ifstream &f1) {

    // Legge dal file in input i dati riguardo le citt√†, le loro posizioni e la dimensione del problema. Le informazioni
    // vengono salvate nell'array globale City.

    //FILE *fp;
    //char st[300],istanza[300],range[300];
    string st, istanza, range, type, commento;
    int nArchi, da, a, i, j, k, incide, seme;
    bool esiste;

    while (!f1.eof()) {

        f1 >> st;
        //cout << st;

        if (st == "NAME:") {
            f1 >> istanza;
            //cout << istanza << endl;
        } else if (st == "TYPE:") {
            f1 >> type;
            //cout << type << endl;
        } else if (st == "COMMENT:") {
            getline(f1, commento);
            //cout << commento << endl;

        } else if (st == "NODES:") {
            f1 >> numNodi;
            //cout << numNodi << endl;
        } else if (st == "EDGES:") {
            f1 >> nArchi;
            //cout << nArchi << endl;
        } else if (st == "RANGE_WEIGHT:") {
            f1 >> range;
            //cout << range << endl;
        } else if (st == "SEED:") {
            f1 >> seme;
            //cout << seme << endl;
        } else if (st == "NODE_WEIGHT_SECTION") {
            // Vado a leggere il peso di ogni nodo
            for (i = 0; i < numNodi; i++) {
                datiNodo *v = (datiNodo*) malloc(sizeof (datiNodo));
                f1 >> da >> a;
                v->id = da;
                v->peso = a;
                //cout << da << " " << a << endl;
                vetNodi.push_back(v);
            }
        } else if (st == "ADIACENT_LOWER_TRIANGULAR_MATRIX") {


            for (i = 1; i <= numNodi; i++) {
                da = i;
                //printf("\n");
                for (j = 1; j <= i; j++) {
                    f1 >> incide;
                    a = j;
                    //printf("%d ",incide);

                    if (incide != 0 && da != a)//cout << "Seelf Loop" <<endl;
                    {
                        datiArco *d = (datiArco*) malloc(sizeof (datiArco));

                        d->da = da;
                        d->a = a;

                        esiste = false;

                        for (k = 0; k < vetArchi.size(); k++) {
                            if (vetArchi[k]->da == da && vetArchi[k]->a == a) {
                                esiste = true;
                                break;
                            }
                            if (vetArchi[k]->da == a && vetArchi[k]->a == da) {
                                esiste = true;
                                break;
                            }
                        }
                        if (!esiste)
                            vetArchi.push_back(d);
                    }
                }
            }

            f1 >> st;
        } else {
            printf("\n\n[Parser] ERROR: Wrong input file format\n");
            exit(1);
        };
    }

    // Calcolo e salvo il grado di ogni nodo
    int grado;
    for (i = 0; i < vetNodi.size(); i++) {
        grado = 0;
        for (j = 0; j < vetArchi.size(); j++)
            if (vetArchi[j]->da == vetNodi[i]->id)grado++;

        vetNodi[i]->grado = grado;

    }
}

void Parser::ParserCLQ(ifstream &f1) {

    //FILE *fp;
    //char st[300],istanza[300],range[300];
    string st, istanza, commento;
    int nArchi, da, a, i, j, k, incide;
    bool esiste;

    while (!f1.eof()) {

        f1 >> st;
        //cout << st;

        if (st == "NAME:") {
            f1 >> istanza;
            //cout << istanza << endl;
        } else if (st == "NUM_COLORS:") {
            f1 >> numColors;
            //cout << type << endl;
        }
        else if (st == "COMMENT:") {
            getline(f1, commento);
            //cout << commento << endl;

        } else if (st == "NODES:") {
            f1 >> numNodi;
            //cout << numNodi << endl;
        } else if (st == "EDGES:") {
            f1 >> nArchi;
            //cout << nArchi << endl;
        } else if (st == "ADIACENT_LOWER_TRIANGULAR_MATRIX") {


            for (i = 1; i <= numNodi; i++) {
                da = i;
                //printf("\n");
                for (j = 1; j <= i; j++) {
                    f1 >> incide;
                    a = j;
                    //printf("%d ",incide);

                    if (incide != 0 && da != a)//cout << "Seelf Loop" <<endl;
                    {
                        datiArco *d = (datiArco*) malloc(sizeof (datiArco));

                        d->da = da;
                        d->a = a;
                        d->color = incide;

                        esiste = false;

                        for (k = 0; k < vetArchi.size(); k++) {
                            if (vetArchi[k]->da == da && vetArchi[k]->a == a) {
                                esiste = true;
                                break;
                            }
                            if (vetArchi[k]->da == a && vetArchi[k]->a == da) {
                                esiste = true;
                                break;
                            }
                        }
                        if (!esiste)
                            vetArchi.push_back(d);
                    }
                }
            }

            f1 >> st;
        } else {
            printf("\n\n[Parser] ERROR: Wrong input file format\n");
            exit(1);
        };
    }

    for (i = 0; i < numNodi; i++) {
        datiNodo *v = (datiNodo*) malloc(sizeof (datiNodo));
        v->id = i;
        vetNodi.push_back(v);
    }
    // Calcolo e salvo il grado di ogni nodo
    int grado;
    for (i = 0; i < vetNodi.size(); i++) {
        grado = 0;
        for (j = 0; j < vetArchi.size(); j++)
            if (vetArchi[j]->da == vetNodi[i]->id)grado++;

        vetNodi[i]->grado = grado;

    }
}

void Parser::ParserCLQ_BIN(FILE *f1) {
    intestazione info;

    string istanza, commento;
    int nArchi, da, a, i, j, k, incide;
    bool esiste;

    // Per prima cosa leggo e salvo i campi dell'intestazione 
    fread(&info, sizeof (intestazione), 1, f1);
    istanza = info.name;
    //strcpy(istanza,info.name);
    numColors = info.ncolors;
    commento = info.comment;
    //strcpy(commento,info.comment);
    numNodi = info.nnodes;
    nArchi = info.nedges;

    //...poi passo alla matrice di adiacenza
    int **matrice_supporto;

    matrice_supporto = (int**) malloc(sizeof (int*) *numNodi);
        
    
    for (i = 0; i < numNodi; i++) {
        matrice_supporto[i] = (int*) malloc(sizeof (int) *numNodi);
    }

    for (i = 0; i < numNodi; i++) {
        fread(matrice_supporto[i], sizeof (int), i + 1, f1);
    }  
        
    for (i = 0; i < numNodi; i++) {
        da = i + 1;

        for (j = 0; j <= i; j++) {
            incide = matrice_supporto[i][j];
            a = j + 1;
            //printf("%d ",incide);

            if (incide != 0 && da != a)//cout << "Seelf Loop" <<endl;
            {
                datiArco *d = (datiArco*) malloc(sizeof (datiArco));

                d->da = da;
                d->a = a;
                d->color = incide;

                esiste = false;

                for (k = 0; k < vetArchi.size(); k++) {
                    if (vetArchi[k]->da == da && vetArchi[k]->a == a) {
                        esiste = true;
                        break;
                    }
                    if (vetArchi[k]->da == a && vetArchi[k]->a == da) {
                        esiste = true;
                        break;
                    }
                }
                if (!esiste)
                    vetArchi.push_back(d);
            }
        }
    }

    //MODIFICATO DA JACEK FILIPCZUK//
    for (i = 0; i < numNodi; i++) {
        datiNodo *v = (datiNodo*) malloc(sizeof (datiNodo));
        v->id = i;

        vetNodi.push_back(v);
    }
    // Calcolo e salvo il grado di ogni nodo
    int grado;
    
    for (i = 0; i < vetNodi.size(); i++) {
        grado = 0;
        for (j = 0; j < vetArchi.size(); j++)
            if (vetArchi[j]->da == vetNodi[i]->id)grado++;

        vetNodi[i]->grado = grado;
        vetNodi[i]->peso = numNodi - grado;// Assegna un peso pari al numero di amicize nel grafo originale

    }
    
}

void Parser::ParserT1(ifstream &f1) {

    int nArchi, da, a, peso;
    bool esiste;

    //primi due dati numero nodi e numero archi su prima riga
    f1 >> numNodi;
    f1 >> nArchi;

#ifdef Parser2
    f1 >> peso;
#endif

    while (!f1.eof() &&
            f1 >> da &&
            f1 >> a
#ifdef Parser2
            && f1 >> peso
#endif
            ) {
        if (da != a)//cout << "Seelf Loop" <<endl;
        {
            datiArco *d = (datiArco*) malloc(sizeof (datiArco));

            da--; // -1 per il fatto che l'indice del nodo1 e' 0.
            a--; //come sopra

            d->da = da;
            d->a = a;

            esiste = false;

            for (int j = 0; j < vetArchi.size(); j++) {
                if (vetArchi[j]->da == da && vetArchi[j]->a == a) {
                    esiste = true;
                    break;
                }
                if (vetArchi[j]->da == a && vetArchi[j]->a == da) {
                    esiste = true;
                    break;
                }
            }
            if (!esiste)
                vetArchi.push_back(d);
        }

    }
    //       cout << "nArchi:" <<nArchi<<"  vetArchi:"<<vetArchi.size() <<endl;

}

void Parser::OrdinaArchi() {
    int app;
    for (int i = 0; i < vetArchi.size() - 1; i++) {
        for (int j = i + 1; j < vetArchi.size(); j++) {
            //if(vetArchi[i]->da<vetArchi[j]->da || (vetArchi[i]->da==vetArchi[j]->da && vetArchi[i]->a<vetArchi[j]->a))
            if (vetArchi[i]->da < vetArchi[j]->da || (vetArchi[i]->da == vetArchi[j]->da && occorrenzeArchi[vetArchi[i]->a] > occorrenzeArchi[vetArchi[j]->a])) {
                app = vetArchi[i]->da;
                vetArchi[i]->da = vetArchi[j]->da;
                vetArchi[j]->da = app;

                app = vetArchi[i]->a;
                vetArchi[i]->a = vetArchi[j]->a;
                vetArchi[j]->a = app;

            }
        }
    }
}

int Parser::GradoVertice(int v) {
    //return vetNodi[v]->grado;
    int grado = 0;
    for (int i = 0; i < vetArchi.size(); i++)
        if (vetArchi[i]->da == v)grado++;
    return grado;
}

Parser::~Parser() {

    while (vetArchi.size() > 0) {
        free(vetArchi.back());
        vetArchi.pop_back();
        //delete vetArchi[i];
        //free(&(vetArchi[i]));  
    }

    while (vetNodi.size() > 0) {
        free(vetNodi.back());
        vetNodi.pop_back();
        //delete vetArchi[i];
        //free(&(vetArchi[i]));  
    }

    //          delete []vetNodi;
    //          delete []vetArchi;
    //            delete [] occorrenzeArchi;
}

Parser::datiArco Parser::getArco(int i) {
    return *vetArchi[i];
}

Parser::datiNodo Parser::getNodo(int i) {
    return *vetNodi[i];
}


void Parser::adattaNonOrientato()
    {
        int i,j;

        int aux_matrice[numNodi+1][numNodi+1];

        for(i=0;i<=numNodi;i++){
            for(j=0;j<=numNodi;j++){
                aux_matrice[i][j]=0;
            }
        }

        for(i=0;i<vetArchi.size();i++){
            aux_matrice[getArco(i).da][getArco(i).a]=1;
        }

        for(i=0;i<vetArchi.size();i++){
            if(aux_matrice[getArco(i).a][getArco(i).da]==0){
                 datiArco *d=(datiArco*)malloc(sizeof(datiArco));
                 d->da=getArco(i).a;
                 d->a=getArco(i).da;
                 d->color = getArco(i).color;
                 vetArchi.push_back(d);             
                 aux_matrice[getArco(i).a][getArco(i).da]=1;
            }
                
        }

        
        for (int i = 0; i < numNodi; i++) {
            occorrenzeArchi.push_back(GradoVertice(i));
        }
        
//        int grado;
//        for(i=0;i<vetNodi.size();i++){
//            grado=0;
//            for(j=0;j<vetArchi.size();j++)
//                if(vetArchi[j]->da==vetNodi[i]->id)grado++;
//
//            vetNodi[i]->grado = grado;
//        }
//        printf("yuhu3\n"); 
        
        //OrdinaArchi();
       // cout <<"  vetArchi Adattato:"<<vetArchi.size() <<endl;
        
    }
 


 
//void Parser::adattaNonOrientato() {
//    int i, j;
//    j = vetArchi.size();
//
//    for (i = 0; i < j; i++)//creo gli archi (j,i) per tutti gli archi (i,j)
//    {
//        datiArco *d = (datiArco*) malloc(sizeof (datiArco));
//        d->da = getArco(i).a;
//        d->a = getArco(i).da;
//        d->color = getArco(i).color;
//        vetArchi.push_back(d);
//    }
//
//    for (i = 0; i < vetArchi.size() - 1; i++) {
//        for (j = i + 1; j < vetArchi.size(); j++) {
//            if (getArco(i).a == getArco(j).a && getArco(i).da == getArco(j).da) {
//                datiArco *d = vetArchi[j];
//                vetArchi[j] = vetArchi[vetArchi.size() - 1];
//                vetArchi.pop_back();
//                free(d);
//            }
//
//        }
//
//    }
//
//
//    for (int i = 0; i < numNodi; i++) {
//        occorrenzeArchi.push_back(GradoVertice(i));
//    }
//
//    //        int grado;
//    //        for(i=0;i<vetNodi.size();i++){
//    //            grado=0;
//    //            for(int j=0;j<vetArchi.size();j++)
//    //                if(vetArchi[j]->da==vetNodi[i]->id)grado++;
//    //
//    //            vetNodi[i]->grado = grado;
//    //        }
//
//    //OrdinaArchi();
//    // cout <<"  vetArchi Adattato:"<<vetArchi.size() <<endl;
//
//}
