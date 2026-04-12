#include "parser.h"
#include <iostream>
using namespace std;

Parser::Parser(vector<Token> t) : tokens(t), posicion(0) {}

Token Parser::Actual() {
 if (posicion >= tokens.size()) {
        return tokens.back();  // regresa el último token (FIN)
    }
    return tokens[posicion];
}

Token Parser::nextToken() {
 if (posicion >= tokens.size()) {
        return tokens.back();  // regresa el último token (FIN)
    }
    return tokens[posicion++]; //retorna token y despues de retornarlo suma 1
}

Nodo* Parser::parsearPrograma() {
    Nodo* raiz = new Nodo("PROGRAMA", "");

    while (Actual().tipo != FIN) {
        Nodo* sentencia = parsearSentencia();
        raiz->hijos.push_back(sentencia);
    }

    return raiz;
}

Nodo* Parser::parsearSentencia() {
    if (Actual().tipo == ID) {
        return parsearAsignacion();
    }
    return nullptr;
}

Nodo* Parser::parsearAsignacion() {
   if(Actual().tipo == ENTERO || Actual().tipo == DECIMAL){
     cout<<"error: sintaxis invalida no se permite una constante como inicio de sentencia"<<endl;
     return nullptr;
   } 
    Token id = nextToken();
    //x=2-2
    //x++
    
    if (Actual().tipo != ASIGNACION) {
        cout << "Error: se esperaba '=' en linea " << Actual().linea << endl;
        return nullptr;
    }
    
    nextToken();
    //->2

    Nodo* expresion = parsearExpresion();
    //crea un puntero llamdo expresion 
    //--->0x91283 = retorna = nodo

    return new Nodo("ASIGNACION", "",
        new Nodo("ID", id.lexema),
        expresion
    );
}

Nodo* Parser::parsearExpresion() {
    cout << "parsearExpresion: tipo=" << Actual().tipo << " lexema=" << Actual().lexema << endl;
    Nodo* izq = nullptr;
//x=j-2
//2
//nodo = tipo valor izq derecha --- 
    if (Actual().tipo == ENTERO || Actual().tipo == DECIMAL || Actual().tipo == ID) {
 
        string tipoNodo;
        if (Actual().tipo == ENTERO)       tipoNodo = "ENTERO"; //ENUM toString(enum)
                                                                //string 
        else if (Actual().tipo == DECIMAL) tipoNodo = "DECIMAL";
        else                               tipoNodo = "ID";
        // cout << "tipoNodo asignado: " << tipoNodo << endl;  
        izq = new Nodo(tipoNodo, nextToken().lexema); //Entero , 2  -> -+/
                                                      //x=2+10
        //-
        //nextokent =  me da 2 y avanza +1
    } else {
        cout << "Error: valor invalido en linea " << Actual().linea << endl;
        return nullptr;
    }

    if (Actual().tipo == MAS      || Actual().tipo == MENOS ||
        Actual().tipo == MULTIPLICACION || Actual().tipo == DIVISION) {

        Token operador = nextToken(); //+ avanzo 1
        Nodo* der = parsearExpresion();
        //x=2+10 :  +
        //der = 
        return new Nodo(operador.lexema, "", izq, der);
    }//else{}

    return izq;
}

    // Nodo* Parser::parsearSi(){
    //   Actual().tipo == 
    // }
    // Nodo* parsearMientras();
    // Nodo* parsearCondicion();
    // Nodo* parsearBloque();
    // Nodo* llamadaFuncion();
