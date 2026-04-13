#include "parser.h"
#include "Tokens.h"
#include <iostream>
using namespace std;
//si ( x > 0 ) { Imprimir("positivo") }
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
        if(sentencia !=nullptr){
        raiz->hijos.push_back(sentencia);
        }
    }

    return raiz;
}

Nodo* Parser::parsearSentencia() {
    switch(Actual().tipo){
case ID:
  return parsearAsignacion();
  break;
case KW_CLASE:
  break;
case KW_MIENTRAS: 
  return parsearMientras();
  break;
case FUNCION_PROPIA:
  break;
case KW_SI: 
  return parsearSi();
  break;
case KW_CASO: 
  break;
case KW_ENTERO: 
  break;
case KW_DOBLE:
  break;
// case KW_CADENA_TIPO: 
  // parsearCadenaTipo();
default: //se ejecuta solo si al parsearla sentencia 
    cout<<"debug 1"<<endl;
    errores.push_back("Error: Inicio de sentencia invalido en linea: "+to_string(Actual().linea));
    int lineaE = Actual().linea;
          //si encuentra un error en la linea x, avanza al token de la linea diferente a esa
    while(Actual().tipo!=FIN && Actual().linea==lineaE && Actual().tipo != LLAVE_CIERRA){
    nextToken();
    }

    return nullptr;
}

}

Nodo* Parser::parsearAsignacion() {
    // cout<<"tipo debug"<<Actual().tipo<<endl;
   // if(Actual().tipo == ENTERO || Actual().tipo == DECIMAL){
   //   cout<<"error: sintaxis invalida no se permite una constante como inicio de sentencia"<<endl;
   //   return nullptr;
   // } 
    Token id = nextToken();
    //x=2-2
    //x++
    
    if (Actual().tipo != ASIGNACION) {
        errores.push_back("Error: se esperaba '=' en linea "+to_string(Actual().linea));
        return nullptr;
    }
    
    nextToken();
    //->2
    Nodo* der = nullptr;
    //puntero llamado der sin apuntar a ningun lado
    // cout<<"lexema: "<<Actual().tipo<<endl;
    if(Actual().tipo == CADENA){
      Nodo* temp = new Nodo("CADENA",Actual().lexema);
      // cout<<"Es cadena"<<endl;
    nextToken();
    der = temp;
    }else{
    der = parsearExpresion();
    }
    //crea un puntero llamdo expresion 
    //--->0x91283 = retorna = nodo

    return new Nodo("ASIGNACION", "=",
        new Nodo("ID", id.lexema),
       der 
    );
}

Nodo* Parser::parsearExpresion() {
    // cout << "parsearExpresion: tipo=" << Actual().tipo << " lexema=" << Actual().lexema << endl;
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
        errores.push_back("Error: valor invalido en linea: "+to_string(Actual().linea));
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
    Nodo* Parser::parsearCondicion(){
      if(Actual().tipo != PAREN_ABRE){
        errores.push_back("Error: se esperaba un '(' en linea: "  + to_string(Actual().linea));
        return nullptr;
      }
      nextToken();
      // id o constante || operador de condicion || id constante
      Nodo* izq = nullptr;
      if (!(Actual().tipo == ID || Actual().tipo==ENTERO || Actual().tipo==DECIMAL)){
        //no es valido
        errores.push_back("Error: valor invalido en la condicion, linea: "+to_string(Actual().linea));
        return nullptr;
      }else{
        string tipoNodo;
        if (Actual().tipo == ENTERO)       tipoNodo = "ENTERO"; //ENUM toString(enum)
        else if (Actual().tipo == DECIMAL) tipoNodo = "DECIMAL";
        else if (Actual().tipo == FUNCION_PROPIA)  tipoNodo="FUNCION_PROPIA";
        else                               tipoNodo = "ID";
        // cout << "tipoNodo asignado: " << tipoNodo << endl;  
        izq = new Nodo(tipoNodo, nextToken().lexema); //Entero , 2  -> -+/
      }
      if (Actual().tipo == MENOR_QUE      || Actual().tipo == MAYOR_QUE || Actual().tipo == IGUALDAD || Actual().tipo == AND_LOGICO || Actual().tipo == OR_LOGICO || Actual().tipo == DESIGUALDAD || Actual().tipo == MAYOR_IGUAL || Actual().tipo == MENOR_IGUAL) {        
        Token operador = nextToken(); //+ avanzo 1
        Nodo* der = parsearExpresion();
        //x=2+10 :  +
        //der = 
        return new Nodo("CONDICION",operador.lexema,izq, der);
    }//else{}
      //x<2
      //  ^ 
    return izq;
}
      


    Nodo* Parser::parsearBloque(){
    //validar si se inicia en con }
    //solo coherente dentro de un if o algo asi xd
    if (Actual().tipo != LLAVE_ABRE){
      errores.push_back("Error: se esperaba un '{' en linea: " +to_string(Actual().linea));
      return nullptr;
    }
    nextToken();
    //recorrer las sentencias posteriores y anexarlas a un nodo llamado BLOQUE    
    Nodo* bloque =new Nodo("BLOQUE","{}");
    while(Actual().tipo != LLAVE_CIERRA && Actual().tipo != FIN){
      Nodo* s = parsearSentencia();
      //si en algun momento s es nullptr quiere decir, que en la
      //sentencia hay un error
      if(s != nullptr){
      bloque ->hijos.push_back(s);
      }
      
     }
    if (Actual().tipo != LLAVE_CIERRA){
      errores.push_back("Error: se esperaba un '}' en linea: "+ to_string(Actual().linea));
      return nullptr;
    }
      nextToken();
      return bloque;
    }
    Nodo* Parser::parsearSi(){
      
      // Nodo* sent_condicion = new Nodo("SENT_CONDICION","");
      nextToken();
      Nodo* izq = parsearCondicion();
      // cout<<"Token con el que cierra el if: "<<Actual().tipo<<endl;
      //nextToken();
      if(Actual().tipo != PAREN_CIERRA){
        errores.push_back("Error: se esperaba un ')' en linea: "+ to_string(Actual().tipo));
        return nullptr;
      }
      //token con el que retorna = ) = 20
      // cout<<"si cierra bien"<<endl;
      nextToken();
      Nodo* der = parsearBloque();
      if (der == nullptr){ 
        cout<<"dio null"<<endl;
        return nullptr;
      }

        return new Nodo("SENT_CONDICION", "", izq, der);
    }
//ejemplo input
//x=0
//mientras(x<10){
//x=x+1
//}
    Nodo* Parser::parsearMientras(){
      //entra en mientras 
      if(Actual().tipo!=KW_MIENTRAS){
        errores.push_back("Error: inicio de sentencia invalido, en linea: "+to_string(Actual().linea));
        return nullptr;
      }
      nextToken();
      if(Actual().tipo != PAREN_ABRE){
        errores.push_back("Error: se esperaba un '(' en linea: "+to_string(Actual().tipo));
        return nullptr;
      }
      Nodo* izq = parsearCondicion();
      //validacion si hay )
      if(Actual().tipo != PAREN_CIERRA){
        errores.push_back("Error: se esperaba un ')' en linea: "+ to_string(Actual().tipo));
        return nullptr;
      }
      nextToken();
      Nodo* der = parsearBloque();
      return new Nodo("SENT_MIENTRAS","",izq,der);
    }
      // Nodo* Parser::parsear
    // Token* parsearCadena duda si esto se parsea
    // Nodo* parsearCondicion();
    // Nodo* parsearBloque();
    // Nodo* llamadaFuncion();
