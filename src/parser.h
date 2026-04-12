#include "Tokens.h"
#include <vector>
#include <iostream>
#include "Lexer.h"
//creacion de la estructura de parsers
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
};

class Parser {
  public :
    // void printTree();
    Parser(vector<Token> toks);
    Nodo* ParseAsignacion();
    Nodo* parsearPrograma();
  private:
    Nodo* parsearSentencia();
    Nodo* parsearAsignacion();
    Nodo* parsearExpresion();
    Token Actual();
    Token nextToken();
    
    vector <Token> tokens;
    int posicion = 0;
};
   


