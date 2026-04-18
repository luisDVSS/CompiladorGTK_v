#include "parser.h"
#include "Tokens.h"
// #include <cstdlib>
#include <iostream>
#include <iterator>
using namespace std;
// entrada = si(x==){}
//  debugin
//   salida: )
//   Entrada :)
//   debug 1
//   Token: }
//   Error: valor invalido en linea: 1
//   Error: se esperaba un ')' en linea: 23
//   Error: Inicio de sentencia invalido en linea: 2
//   si ( x > 0 ) { Imprimir("positivo") }
Parser::Parser(vector<Token> t) : tokens(t), posicion(0) {}

Token Parser::Actual() {
  if (posicion >= tokens.size()) {
    return tokens.back(); // regresa el último token (FIN)
  }
  return tokens[posicion];
}

Token Parser::nextToken() {
  if (posicion >= tokens.size()) {
    return tokens.back(); // regresa el último token (FIN)
  }
  return tokens[posicion++]; // retorna token y despues de retornarlo suma 1
}

Nodo *Parser::parsearPrograma() {
  Nodo *raiz = new Nodo("PROGRAMA", "");

  while (Actual().tipo != FIN) {
    Nodo *sentencia = parsearSentencia();
    if (sentencia != nullptr) {
      raiz->hijos.push_back(sentencia);
    }
  }

  return raiz;
}

Nodo *Parser::parsearSentencia() {
  switch (Actual().tipo) {
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
    return parsearImprimir();
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
  default: // se ejecuta solo si al parsearla sentencia

    cout << "debug 1: " << Actual().tipo << "- " << Actual().lexema << endl;
    errores.push_back("Error: Insasdicio de sentencia invalido en linea: " +
                      to_string(Actual().linea));
    // cout << "Token: " << Actual().lexema << endl;
    int lineaE = Actual().linea;
    // si encuentra un error en la linea x, avanza al token de la linea
    // diferente a esa
    while (Actual().tipo != FIN && Actual().linea == lineaE) {
      nextToken();
    }

    return nullptr;
  }
  return nullptr;
}

Nodo *Parser::parsearAsignacion() {
  // cout<<"tipo debug"<<Actual().tipo<<endl;
  // if(Actual().tipo == ENTERO || Actual().tipo == DECIMAL){
  //   cout<<"error: sintaxis invalida no se permite una constante como inicio
  //   de sentencia"<<endl; return nullptr;
  // }
  Token id = nextToken();
  // x==1){}
  if (Actual().tipo != ASIGNACION) {
    errores.push_back("Error: valor invalido o se esperaba '=' en linea " +
                      to_string(Actual().linea));
    int lineaE = Actual().linea;
    while (Actual().tipo != FIN && Actual().tipo != LLAVE_ABRE &&
           Actual().linea == lineaE) {
      nextToken();
    }
    return nullptr;
  }
  nextToken();
  //->2
  if (Actual().tipo == DECIMAL || Actual().tipo == ENTERO ||
      Actual().tipo == ID || Actual().tipo == CADENA) {
    Nodo *der = nullptr;
    // puntero llamado der sin apuntar a ningun lado
    //  cout<<"lexema: "<<Actual().tipo<<endl;
    if (Actual().tipo == CADENA) {
      Nodo *temp = new Nodo("CADENA", Actual().lexema);
      // cout<<"Es cadena"<<endl;
      nextToken();
      // cout<<"lexema error loop: "<<Actual().lexema<<endl;
      // cin.get();
      // x=1asd
      // 1 - asd(ID)
      der = temp;
    } else {
      der = parsearExpresion();
      // x=a+1
    }
    // crea un puntero llamdo expresion
    //--->0x91283 = retorna = nodo
    return new Nodo("ASIGNACION", "=", new Nodo("ID", id.lexema), der);
  } else {
    errores.push_back(
        "Error: valor invalido despues en la asignacion, linea: " +
        to_string(Actual().linea));
    int lineaE = Actual().linea;
    while (Actual().linea == lineaE && Actual().tipo != LLAVE_ABRE &&
           Actual().tipo != FIN) {
      nextToken();
    }
    return nullptr;
  }
}

Nodo *Parser::parsearExpresion() {
  // entra con ')'
  // cout << "Entrada :" << Actual().lexema << endl;
  // aqui entra una cadena =="cadena"

  int lineaE = Actual().linea;
  Nodo *izq = nullptr;
  // cout << "tipo: " << Actual().tipo << endl;
  if (Actual().tipo == ENTERO || Actual().tipo == DECIMAL ||
      Actual().tipo == ID || Actual().tipo == CADENA) {

    string tipoNodo;
    if (Actual().tipo == ENTERO)
      tipoNodo = "ENTERO"; // ENUM toString(enum)
                           // string
    else if (Actual().tipo == DECIMAL)
      tipoNodo = "DECIMAL";
    else if (Actual().tipo == CADENA)
      tipoNodo = "CADENA";
    else
      tipoNodo = "ID";
    // cout << "tipoNodo asignado: " << tipoNodo << endl;
    izq = new Nodo(tipoNodo, nextToken().lexema); // Entero , 2  -> -+/
    if (Actual().tipo == FIN || Actual().tipo == CADENA ||
        Actual().linea != lineaE) {
      return izq;
    } // aqui ya paso cadena y es el ')'
  } else { // x=
    errores.push_back("Error: valor invalido en linea: " +
                      to_string(Actual().linea));
    int lineaE = Actual().linea;
    // saltar a la siguiente linea

    while (Actual().tipo != FIN && Actual().linea == lineaE &&
           Actual().tipo != LLAVE_ABRE && Actual().tipo != PAREN_CIERRA) {
      nextToken();
    }
    return nullptr;
  }

  if (Actual().tipo == MAS || Actual().tipo == MENOS ||
      Actual().tipo == MULTIPLICACION || Actual().tipo == DIVISION) {

    Token operador = nextToken(); //+ avanzo 1
    Nodo *der = parsearExpresion();
    // x=2+10 :  +
    // der =
    return new Nodo(operador.lexema, "", izq, der);
  } else if (Actual().tipo == PAREN_CIERRA) {
    return izq;
  } else {
    errores.push_back("Error: valor o simbolo invalido en la linea: " +
                      to_string(Actual().linea));
    int lineaE = Actual().linea;
    while (Actual().tipo != FIN && Actual().linea == lineaE) {
      nextToken();
    }
    return nullptr;
  }

  return izq;
}
Nodo *Parser::parsearCondicion() {
  // debugin si(x==){} errores
  if (Actual().tipo != PAREN_ABRE) {
    errores.push_back("Error: se esperaba un '(' en linea: " +
                      to_string(Actual().linea));
    int lineaE = Actual().linea;
    while (Actual().tipo != PAREN_CIERRA && Actual().tipo != FIN &&
           Actual().tipo != LLAVE_ABRE && Actual().linea == lineaE) {
      nextToken();
    }
    return nullptr;
  }
  nextToken();
  Nodo *izq = nullptr;

  if (Actual().tipo == PAREN_CIERRA) {
    errores.push_back("Error: condicion vacia '()', invalida en linea: " +
                      to_string(Actual().linea));
    return nullptr;
  }
  if (!(Actual().tipo == ID || Actual().tipo == ENTERO ||
        Actual().tipo == DECIMAL)) {
    errores.push_back("Error: valor invalido en la condicion, linea: " +
                      to_string(Actual().linea));
    int lineaE = Actual().linea;
    while (Actual().tipo != FIN && Actual().tipo != PAREN_CIERRA &&
           Actual().tipo != LLAVE_ABRE && Actual().linea == lineaE) {
      nextToken();
    }
    return nullptr;
  } else {
    string tipoNodo;
    if (Actual().tipo == ENTERO)
      tipoNodo = "ENTERO"; // ENUM toString(enum)
    else if (Actual().tipo == DECIMAL)
      tipoNodo = "DECIMAL";
    // else if (Actual().tipo == CADENA) tipoNodo = "CADENA";
    else
      tipoNodo = "ID";
    // cout << "tipoNodo asignado: " << tipoNodo << endl;
    izq = new Nodo(tipoNodo, nextToken().lexema); // Entero , 2  -> -+/
  }
  if (Actual().tipo == MENOR_QUE || Actual().tipo == MAYOR_QUE ||
      Actual().tipo == IGUALDAD || Actual().tipo == OP_AND ||
      Actual().tipo == OP_OR || Actual().tipo == DESIGUALDAD ||
      Actual().tipo == MAYOR_IGUAL || Actual().tipo == MENOR_IGUAL) {
    Token operador = nextToken(); //+ avanzo valo
    Nodo *der = parsearExpresion();
    return new Nodo("CONDICION", operador.lexema, izq, der);
  } else {

    errores.push_back("Error: operador invalido en linea: " +
                      to_string(Actual().linea));
    while (Actual().tipo != PAREN_CIERRA && Actual().tipo != FIN &&
           Actual().tipo != LLAVE_ABRE) {
      nextToken();
    }
    return nullptr;
  }
  return izq;
}

Nodo *Parser::parsearBloque() {
  // validar si se inicia en con }
  // solo coherente dentro de un if o algo asi xd
  if (Actual().tipo != LLAVE_ABRE) {
    errores.push_back("Error: se esperaba un '{' en linea: " +
                      to_string(Actual().linea));
    return nullptr;
  }
  nextToken(); // validacion de que un bloque no sea vacio
  // if(Actual().tipo == LLAVE_CIERRA){
  //   int lineaE=Actual().linea;
  //   errores.push_back("Error: bloque de codigo vacio en linea: "+
  //   to_string(Actual().linea));
  // while(Actual().linea == lineaE && Actual().tipo != FIN){
  //   nextToken();
  //   }
  //   return nullptr;
  // }
  // recorrer las sentencias posteriores y anexarlas a un nodo llamado BLOQUE
  Nodo *bloque = new Nodo("BLOQUE", "{}");
  while (Actual().tipo != LLAVE_CIERRA && Actual().tipo != FIN) {
    Nodo *s = parsearSentencia();
    // si en algun momento s es nullptr quiere decir, que en la
    // sentencia hay un error
    if (s != nullptr) {
      bloque->hijos.push_back(s);
    }
  }
  if (Actual().tipo != LLAVE_CIERRA) {
    errores.push_back("Error: se esperaba un '}' en linea: " +
                      to_string(Actual().linea));

    return nullptr;
  }
  nextToken();
  return bloque;
}
// parseo de si
Nodo *Parser::parsearSi() {
  // si(){
  // }
  Nodo *si_parsed = new Nodo("SENTENCIA_SI", "<>");
  // Nodo* default_else= nulltptr
  // Nodo* sent_condicion = new Nodo("SENT_CONDICION","");
  // si -> (
  nextToken();
  Nodo *izq = parsearCondicion();
  // if(Actual().tipo==PAREN_CIERRA){
  //   nextToken();
  // }
  if (izq == nullptr) {
    // cout<<"resultado: nullptr"<<Actual().tipo<<endl;
    // cin.get();
    // return nullptr;
  } else {
    if (Actual().tipo != PAREN_CIERRA) {
      errores.push_back("Error: se esperaba un ')' en linea: " +
                        to_string(Actual().linea));
      int lineaE = Actual().linea;
      while (Actual().tipo != FIN && Actual().tipo != LLAVE_ABRE &&
             Actual().tipo == lineaE) {
        nextToken();
      }
      return nullptr;
    } else {

      si_parsed->hijos.push_back(izq);
    }
  }
  // cout<<"Token con el que cierra el if: "<<Actual().tipo<<endl;
  // nextToken();
  // token con el que retorna = ) = 20
  // cout<<"si cierra bien"<<endl;
  nextToken();
  Nodo *der = parsearBloque();

  if (der == nullptr) {
    // cout<<"dio null"<<endl;
    return nullptr;
  }
  // cout << "Actual" << Actual().tipo << endl;
  si_parsed->hijos.push_back(der);
  if (Actual().tipo == KW_SINO) {
    nextToken();
    Nodo *bloque = parsearBloque();
    bloque->tipo = "SINO_BLOQUE";

    si_parsed->hijos.push_back(bloque);
  }

  return si_parsed;
}
// ejemplo input
// x=0
// mientras(x<10){
// x=x+1
// }
Nodo *Parser::parsearMientras() {
  // entra en mientras
  if (Actual().tipo != KW_MIENTRAS) {
    errores.push_back("Error: inicio de sentencia invalido, en linea: " +
                      to_string(Actual().linea));
    return nullptr;
  }
  nextToken();
  if (Actual().tipo != PAREN_ABRE) {
    errores.push_back("Error: se esperaba un '(' en linea: " +
                      to_string(Actual().tipo));
    while (Actual().tipo != PAREN_CIERRA && Actual().tipo != FIN &&
           Actual().tipo != LLAVE_ABRE) {
      nextToken();
    }
    return nullptr;
  }
  Nodo *izq = parsearCondicion();
  if (izq == nullptr) {
    return nullptr;
  }
  // validacion si hay )
  if (Actual().tipo != PAREN_CIERRA) {
    errores.push_back("Error: se esperaba un ')' en linea: " +
                      to_string(Actual().tipo));
    return nullptr;
  }
  nextToken();
  Nodo *der = parsearBloque();
  return new Nodo("SENT_MIENTRAS", "", izq, der);
}
Nodo *Parser::parsearImprimir() {
  // cout << "Entro con: " << Actual().tipo << endl;
  // cin.get();
  Nodo *N_print = new Nodo("IMPRIMIR", "");
  if (Actual().tipo == KW_IMPRIMIR) {
    nextToken();

    // cout << "Entra al if: " << Actual().tipo << endl;

    // cin.get();
    if (Actual().tipo != PAREN_ABRE) {
      errores.push_back("Error: se esperaba un '(' en linea: " +
                        to_string(Actual().linea));
      int lineaE = Actual().linea;
      while (Actual().tipo != FIN && Actual().linea == lineaE) {
        nextToken();
      }
      return nullptr;
    }
    nextToken();
    N_print->izq = parsearExpresion();

    if (Actual().tipo != PAREN_CIERRA) {
      errores.push_back("Error: se esperaba un ')' en linea: " +
                        to_string(Actual().linea));
      int lineaE = Actual().linea;
      while (Actual().tipo != FIN && Actual().linea == lineaE) {
        nextToken();
      }
    } else {
      nextToken();
    }
  } else {
    int lineaE = Actual().linea;
    while (Actual().tipo != FIN && Actual().linea == lineaE) {
      nextToken();
    }
    return nullptr;
  }
  // cout << N_print->izq->tipo << endl;
  return N_print;
}
