

#ifndef PARSER_S
#define PARSER_S

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#define Parser2 1

#define MBV 0
#define FVS 1
#define CLQ 2
#define CLQ_BIN 3

class Parser {
  
public:
       typedef struct{
               int da;
               int a;
               int color;
       }datiArco;

       typedef struct{
               int id;
               int grado;
               int peso;
       }datiNodo;

        Parser(std::string);
       Parser(std::string, short);
       
       ~Parser();
       datiArco getArco(int i);
       datiNodo getNodo(int i);
       int getNumArchi(){return vetArchi.size();}
       int getNumNodi(){return numNodi;}
		 int getNumColors(){return numColors;}
       void adattaNonOrientato();


private:
        int numNodi;
		  int numColors;
        std::vector<datiArco*> vetArchi;
        std::vector<datiNodo*> vetNodi;
        std::vector<int> occorrenzeArchi;
        void OrdinaArchi();
        int GradoVertice(int v);
        void ParserT1(std::ifstream&);
        void ParserFVS(std::ifstream&);
        void ParserFVS2(std::ifstream&);
		  void ParserCLQ(std::ifstream&);
			void ParserCLQ_BIN(std::FILE*);
        void ParserT2(std::ifstream&);
        void ParserMBV(std::ifstream&);	
        
};


#endif
