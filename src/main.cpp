#include "Lexer.h"
#include "Tokens.h"
#include "parser.h"
#include <string>
#include <vector>
#include "ast_viewer.h"
#include <sstream>
#include <stdexcept>
#include <gtk/gtk.h>
#include <iostream>
//metodo estativo de activacion de la app
//
using namespace std;
vector<Token> listaTokens;
Nodo* arbol=nullptr;
void parsear(){
   Parser parser (listaTokens);
   arbol = parser.parsearPrograma(); 
}
void imprimirArbol(Nodo* nodo, int nivel = 0) {
ast_viewer_mostrar(arbol);
}
string LimpiarLexemaParaDisplay(const string& lexema) {
    string resultado = "";
    for (char c : lexema) {
        if (c == '\n') {
            resultado += "\\n"; // Muestra explícitamente el salto de línea
        }
        else if (c == '\r') {
            resultado += "\\r"; // Muestra el retorno de carro (típico en Windows)
        }
        else if (c == '\t') {
            resultado += "\\t"; // Muestra las tabulaciones
        }
        else {
            resultado += c;     // Deja los caracteres normales intactos
        }
    }
    return resultado;
}
typedef struct {
  GtkWidget *EntryCode;
  GtkWidget *OutputText;
}AppWidgets;
void IDC_BTN_ANALIZAR(GtkWidget *btn,gpointer data){
  AppWidgets *widgets = (AppWidgets *)data;

  GtkTextBuffer *bufferOutput = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->OutputText));
  GtkTextBuffer *bufferCode = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->EntryCode));
  gint len = gtk_text_buffer_get_char_count(bufferCode);
  
    if (len > 0 ){
      vector<gchar> buffer(len +1);
      //creo los puunteros(sin valor) start, end
      GtkTextIter start,end;
      // a start le pongo el inicio del bufferCode y a end su final
      gtk_text_buffer_get_bounds(bufferCode, &start, &end);
      gchar *codigo = gtk_text_buffer_get_text(bufferCode, &start, &end, FALSE);
      //parseo el buffer de codigo  (gchar) a string 
      string CodigoFuente=string(codigo);
      //libero el buffer de codigo
      g_free(codigo);
      try {
        Lexer miAnalizador(
            CodigoFuente,
            "matriz_transiciones.csv",
            "nombres_tokens.csv",
            "estados_tokens.csv",
            "char_columnas.csv",
            "keywords.csv"
            );

   listaTokens = miAnalizador.generarListaTokens();
   listaTokens.push_back({FIN, "EOF",0,0});
for (const auto& t : listaTokens) {
    cout << "tipo=" << t.tipo << " lexema=" << t.lexema << endl;
}
   // Parser parser (listaTokens);
   //  arbol = parser.parsearPrograma(); 
   //  // ast_viewer_mostrar(arbol);

     std::stringstream salida;
     for (const auto& token : listaTokens) {
       const std::string& nombre = NOMBRES_TOKENS[token.tipo];
       salida << "Línea: " << token.linea << " \t| Col: " << token.columna << " \t| ";
       salida << "Tipo: [" << nombre << "]";
       salida << (nombre.length() < 7 ? "\t\t" : "\t");
       salida << "| Lexema: [" << LimpiarLexemaParaDisplay(token.lexema) << "]\r\n";
       }
     gtk_text_buffer_set_text(bufferOutput,salida.str().c_str(),-1);
      }catch(exception& e){
        string msg = e.what(); 
        gtk_text_buffer_set_text(bufferOutput,msg.c_str(),-1);
      }
    } 
//      catch (const std::exception& e) {
//        //cambiar a gtk
//        SetWindowText(hwndOutput, ConvertirHaciaWide(e.what()).c_str());
//      }
//
}
static void activate (GtkApplication* app, gpointer user_data){
  //window(contenedor/ventana principal)
  GtkWidget*window;
  window=gtk_application_window_new(app);
  gtk_window_set_title (GTK_WINDOW (window), "Analizador Lexico FIM");
  gtk_window_set_default_size (GTK_WINDOW(window),1100,700);
  //layout
  GtkWidget*box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  GtkWidget*box_btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
  GtkWidget*lblinput=gtk_label_new("Codigo fuente");  
  GtkWidget*lbloutput=gtk_label_new("Salida del lexer");
  gtk_container_set_border_width(GTK_CONTAINER(box),15);
  //añado a la ventana las dos boxes
  gtk_container_add(GTK_CONTAINER(window),box);
  //El box_btns tiene que estar dentro del box (en medio)
// gtk_container_add(GTK_CONTAINER(window),box_btns);
  //widgets
  //ventana de scroll para el "Edit text"
  GtkWidget*btnCompilar = gtk_button_new_with_label("Ejecutar Analizador");
  GtkWidget*btnMostrarAST = gtk_button_new_with_label("Mostrar AST");
  GtkWidget*btnGenerarArbol = gtk_button_new_with_label("Generar Arbol");


  GtkWidget *scrollCode = gtk_scrolled_window_new(NULL,NULL);
  
  GtkWidget *scrollOutput = gtk_scrolled_window_new(NULL,NULL);

  gtk_widget_set_size_request(scrollCode,-1,150);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrollCode),200);
gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scrollCode),1100);

  gtk_widget_set_size_request(scrollOutput,-1,150);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrollOutput),200);

  GtkWidget*OutputText = gtk_text_view_new();
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(OutputText),GTK_WRAP_WORD);
  gtk_container_add(GTK_CONTAINER(scrollOutput),OutputText);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(OutputText),FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(OutputText),FALSE);
    
  GtkWidget*EntryCode=gtk_text_view_new();
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(EntryCode),GTK_WRAP_WORD);
  gtk_container_add(GTK_CONTAINER(scrollCode),EntryCode);
  // gtk_container_add(GTK_CONTAINER(gtk_bin_get_child(GTK_BIN(scrollCode))),EntryCode);

  AppWidgets *widgets = g_new(AppWidgets,1);
  widgets->EntryCode=EntryCode;
  widgets->OutputText=OutputText;


  // GtkTextBuffer *codeBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(EntryCode),)
  // gtk_entry_set_max_length(GTK_ENTRY(EntryCodigo),50);//max length pues xD
  //poner dentro de la box: box el componente btnCompilar y el btnMostrarAST
  //
  gtk_box_pack_start(GTK_BOX(box_btns),btnGenerarArbol,FALSE,TRUE,0);
  gtk_box_pack_start(GTK_BOX(box_btns),btnMostrarAST,FALSE,TRUE,0);
  gtk_box_pack_start(GTK_BOX(box_btns),btnCompilar,FALSE,TRUE,0);
 //aqui se muestran todos los los cosos dentro de box (box ya se muestra como box principal ya que es la unica que agrego a window)
gtk_box_pack_start(GTK_BOX(box),box_btns,TRUE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(box),lblinput,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(box),scrollCode,TRUE,TRUE,0);

  gtk_box_pack_start(GTK_BOX(box),lbloutput,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(box),scrollOutput,TRUE,TRUE,0);
  //signals- lo que el boton o cosa que tenga accionador, va a hacer .D
  
  g_signal_connect(btnMostrarAST, "clicked",G_CALLBACK(imprimirArbol),NULL);
  g_signal_connect(btnGenerarArbol, "clicked",G_CALLBACK(parsear),NULL);
  g_signal_connect(btnCompilar, "clicked",G_CALLBACK(IDC_BTN_ANALIZAR),widgets);
//inicializacion de window
  gtk_widget_show_all(window);
}
int main(int argc, char* argv[]){
  gtk_init(&argc,&argv);
  GtkApplication *app;
  int status;
  app = gtk_application_new("org.compiler.app",G_APPLICATION_DEFAULT_FLAGS);
  gtk_application_window_new(app);
  g_signal_connect(app,"activate",G_CALLBACK(activate), NULL);
  status = g_application_run (G_APPLICATION(app),argc,argv);
  g_object_unref(app);
  return status;

}

