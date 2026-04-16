#include "parser.h"
#include "Tokens.h"
#include <iostream>
#include <cstdlib>
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
case LLAVE_ABRE:
return parsearBloque();
  break;

case KW_CLASE:
  break;
case KW_MIENTRAS: 
  return parsearMientras();
  break;
case KW_IMPRIMIR:
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
    //x==1){}
    if (Actual().tipo != ASIGNACION) {
        errores.push_back("Error: valor invalido o se esperaba '=' en linea "+to_string(Actual().linea));
        int lineaE=Actual().linea;
        while(Actual().tipo!=FIN && Actual().tipo!= LLAVE_ABRE && Actual().linea==lineaE){
          nextToken();

        }
        return nullptr;
    }
    nextToken();
    //->2
    if(Actual().tipo == DECIMAL || Actual().tipo == ENTERO || Actual().tipo == ID){
      
    Nodo* der = nullptr;
    //puntero llamado der sin apuntar a ningun lado
    // cout<<"lexema: "<<Actual().tipo<<endl;
    if(Actual().tipo == CADENA){
      Nodo* temp = new Nodo("CADENA",Actual().lexema);
      // cout<<"Es cadena"<<endl;
    nextToken();
    cout<<"lexema error loop: "<<Actual().lexema<<endl;
    cin.get();
    // x=1asd
    // 1 - asd(ID)
    der = temp;
    }else{
    der = parsearExpresion();
    //x=a+1
    
    }
    //crea un puntero llamdo expresion 
    //--->0x91283 = retorna = nodo

    return new Nodo("ASIGNACION", "=",
        new Nodo("ID", id.lexema),
       der 
    );
    }else{
      errores.push_back("Error: valor invalido despues en la asignacion, linea: "+ to_string(Actual().linea));
      int lineaE = Actual().linea;
      while(Actual().linea==lineaE && Actual().tipo != LLAVE_ABRE){
        nextToken();
      }
      return nullptr;
    }
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
        if(Actual().tipo==FIN){
          return izq;
        }                                  //x=2+10
        //-
        //nextToken =  me da 2 y avanza +1
    } else {//x=
        errores.push_back("Error: valor invalido en linea: "+to_string(Actual().linea));
        int lineaE=Actual().linea;
        //saltar a la siguiente linea
        
        while(Actual().tipo!= FIN && Actual().linea==lineaE){
        nextToken();
        }
        return nullptr;
    }

    if (Actual().tipo == MAS      || Actual().tipo == MENOS ||
        Actual().tipo == MULTIPLICACION || Actual().tipo == DIVISION) {

        Token operador = nextToken(); //+ avanzo 1
        Nodo* der = parsearExpresion();
        //x=2+10 :  +
        //der = 
        return new Nodo(operador.lexema, "", izq, der);
    }else{
      errores.push_back("Error: valor o simbolo invalido en la linea: "+to_string(Actual().linea));
      int lineaE=Actual().linea;
      while(Actual().tipo!=FIN && Actual().linea==lineaE){
        nextToken();
      }
      return nullptr;
    }

    return izq;
}
    Nodo* Parser::parsearCondicion(){
        //->(
      if(Actual().tipo != PAREN_ABRE){
        errores.push_back("Error: se esperaba un '(' en linea: "  + to_string(Actual().linea));
        int lineaE=Actual().linea;

        while(Actual().tipo!=PAREN_CIERRA && Actual().tipo != FIN && Actual().tipo != LLAVE_ABRE && Actual().linea==lineaE){
          nextToken();
        }
        cout<<"Toke de salida:"<<Actual().tipo<<endl;
        return nullptr;
      }
      //si(){}
      //x==j, x<2
      nextToken();
      // id o constante || operador de condicion || id constante
      Nodo* izq = nullptr;
      
        if(Actual().tipo==PAREN_CIERRA){
          errores.push_back("Error: condicion vacia '()', invalida en linea: "+to_string(Actual().linea));
          return nullptr;
        }
        //(...x || 10 ||10.2)
      if (!(Actual().tipo == ID || Actual().tipo==ENTERO || Actual().tipo==DECIMAL)){
        //no es valido
        errores.push_back("Error: valor invalido en la condicion, linea: "+to_string(Actual().linea));
        int lineaE=Actual().linea;
        while(Actual().tipo != FIN && Actual().tipo != PAREN_CIERRA && Actual().tipo != LLAVE_ABRE && Actual().linea == lineaE){
        nextToken();
        }
        return nullptr;
        }else{
        string tipoNodo;
        if (Actual().tipo == ENTERO)       tipoNodo = "ENTERO"; //ENUM toString(enum)
        else if (Actual().tipo == DECIMAL) tipoNodo = "DECIMAL";
        else if (Actual().tipo == KW_IMPRIMIR)  tipoNodo="IMPRIMIR";
        else                               tipoNodo = "ID";
        // cout << "tipoNodo asignado: " << tipoNodo << endl;  
        izq = new Nodo(tipoNodo, nextToken().lexema); //Entero , 2  -> -+/
      }
      if (Actual().tipo == MENOR_QUE      || Actual().tipo == MAYOR_QUE || Actual().tipo == IGUALDAD || Actual().tipo == OP_AND || Actual().tipo == OP_OR || Actual().tipo == DESIGUALDAD || Actual().tipo == MAYOR_IGUAL || Actual().tipo == MENOR_IGUAL) {        
        Token operador = nextToken(); //+ avanzo 1
        Nodo* der = parsearExpresion();
        //x=2+10 :  +
        //der = 
        return new Nodo("CONDICION",operador.lexema,izq, der);
      }else{
        // cout<<"no es asignacion"<<endl;
        errores.push_back("Error: operador invalido en linea: "+to_string(Actual().linea));
        //encuentra error en ejemplo 
        //x=2
        //  ^
        while(Actual().tipo!=PAREN_CIERRA && Actual().tipo != FIN && Actual().tipo != LLAVE_ABRE){
          nextToken();
        }
        return nullptr;
    }
      //else{}
      //x<2
      //  ^ 
    return izq;
}
      


    Nodo* Parser::parsearBloque(){
    //validar si se inicia en con }
    //solo coherente dentro de un if o algo asi xd
    if (Actual().tipo != LLAVE_ABRE){
      errores.push_back("Error: se esperaba un '{' en linea: " +to_string(Actual().linea));

      cout<<"Error loop"<<endl;
      cin.get();
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
//parseo de si
    Nodo* Parser::parsearSi(){
      //si(){}
      Nodo* si_parsed = new Nodo("SENTENCIA_SI","<>");
      // Nodo* default_else= nulltptr
      // Nodo* sent_condicion = new Nodo("SENT_CONDICION","");
      // si -> (
      nextToken();
      Nodo* izq = parsearCondicion();
      // if(Actual().tipo==PAREN_CIERRA){
      //   nextToken();
      // }
      if(izq==nullptr){
        cout<<"resultado: nullptr"<<Actual().tipo<<endl;
        // cin.get();
        // return nullptr;
      }else{


      if(Actual().tipo != PAREN_CIERRA){
        errores.push_back("Error: se esperaba un ')' en linea: "+ to_string(Actual().tipo));
        return nullptr;
      }else{

      si_parsed->hijos.push_back(izq);
      }
      }
      // cout<<"Token con el que cierra el if: "<<Actual().tipo<<endl;
      //nextToken();
      //token con el que retorna = ) = 20
      // cout<<"si cierra bien"<<endl;
      nextToken();
      Nodo* der = parsearBloque();

      if (der == nullptr){ 
        // cout<<"dio null"<<endl;
        return nullptr;
      }
      cout<<"Actual"<<Actual().tipo<<endl;
      si_parsed->hijos.push_back(der);
      if(Actual().tipo==KW_SINO){
        nextToken();
        Nodo* bloque = parsearBloque();
        bloque->tipo="SINO_BLOQUE";


        si_parsed->hijos.push_back(bloque);
        
      }

        return si_parsed;
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

        while(Actual().tipo!=PAREN_CIERRA && Actual().tipo != FIN && Actual().tipo !=LLAVE_ABRE){
          nextToken();
        }
        return nullptr;
      }
      Nodo* izq = parsearCondicion();
      if(izq==nullptr){
        return nullptr;
      }
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
