#pragma once
#include "Tokens.h"
#include <vector>
#include <iostream>
#include "Lexer.h"
//creacion de la estructura de parsers
//cambio
using namespace std;
//structura nodo del ast
struct Nodo{
string tipo;
string valor;
vector<Nodo*> hijos;
Nodo* izq;
Nodo* der;
//constructor de la struct
 Nodo(string t, string v, Nodo* i=nullptr, Nodo* d=nullptr):tipo(t),valor(v),izq(i),der(d){}
 ~Nodo(){
        delete izq;
        delete der;
        for(Nodo* hijo : hijos)
            delete hijo;
    }
};

class Parser {
  public :
    // void printTree();
    vector <string> errores;
    Parser(vector<Token> toks);
    Nodo* ParseAsignacion();
    Nodo* parsearPrograma();
  private :
    Nodo* Imprimir();
    Nodo* parsearSi();
    Nodo* parsearMientras();
    Nodo* parsearCondicion();
    Nodo* parsearBloque();
    Nodo* llamadaFuncion();
    Nodo* parsearSentencia();
    Nodo* parsearAsignacion();
    Nodo* parsearExpresion();
    Token Actual();
    Token nextToken();
    
    vector <Token> tokens;
    int posicion = 0;
};
   


