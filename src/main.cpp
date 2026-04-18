#include "Lexer.h"
#include "Tokens.h"
#include "ast_viewer.h"
#include "glib-object.h"
#include "glib.h"
#include "gtk/gtkcssprovider.h"
#include "parser.h"
#include <fstream>
#include <gtk/gtk.h>
#include <iostream>
#include <sstream>
// #include <stdexcept>
#include <string>
#include <vector>
// metodo estatico de activacion de la app

using namespace std;
// variables globales

typedef struct {
  GtkWidget *EntryCode;
  GtkWidget *OutputText;
  GtkWidget *ThemeColorBtn;
  GtkCssProvider *Provider;
  GtkCssProvider *ProviderFont;
} AppWidgets;

int sizeFont = 16;
// false = day true = night
bool color_state = false;
// clor de window y botones
char cssWindowColorNight[800] =
    "window,  scrolledwindow, viewport { background-color: #2B2B2B; } "
    "textview { background-color: #1E1E1E; color: #EBE7E6; } "
    "textview text { background-color: #1E1E1E; color: #EBE7E6; } "
    "button { background: #3C3C3C; background-image: none; "
    "         border: none; border-radius: 4px; } "
    "button:hover { background: #505050; background-image: none; } "
    "button label { color: #EBE7E6; }";
char cssWindowColorDay[800] =
    "window,  scrolledwindow, viewport { background-color: #EBEBEB; } "
    "textview { background-color: #FFFFFF; color: #000000; } "
    "textview text { background-color: #FFFFFF; color: #000000; } "
    "button { background: #EFEFEF; background-image: none; "
    "         border: none; border-radius: 4px; } "
    "button:hover { background: #E3D8D5; background-image: none; } "
    "button label { color: #121111; }";

char cssBuffer[200];
vector<Token> listaTokens;
vector<string> errores;
Nodo *arbol = nullptr;
// declaracion de buffers
GtkTextBuffer *bufferCode;
GtkTextBuffer *bufferOutput;

void parsear(GtkWidget *, gpointer data) {
  vector<string> *errores = (vector<string> *)data;
  errores->clear();
  Parser parser(listaTokens);
  arbol = parser.parsearPrograma();
  *errores = parser.errores;
  if (!errores->empty()) {
    for (string e : *errores) {
      cout << e << endl;
    }
  }
}
void ZoomM(GtkWidget *widget, gpointer data) {
  AppWidgets *widgets = (AppWidgets *)data;
  GtkCssProvider *provider = widgets->ProviderFont;
  if (sizeFont < 30) {
    sizeFont = sizeFont + 1;

    sprintf(cssBuffer,
            ".text_output { font-family: 'Monospace'; font-size: %dpt; }"
            ".text_entry {font-family : 'Monospace'; font-size: %dpt;}",
            sizeFont, sizeFont);

    gtk_css_provider_load_from_data(provider, cssBuffer, -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);
  } else {
    cout << "Fuente demasiado grande" << endl;
  }
}
void ZoomL(GtkWidget *widget, gpointer data) {
  AppWidgets *widgets = (AppWidgets *)data;
  GtkCssProvider *provider = widgets->ProviderFont;
  if (sizeFont > 10) {
    sizeFont = sizeFont - 1;

    sprintf(cssBuffer,
            ".text_output { font-family: 'Monospace'; font-size: %dpt; }"
            ".text_entry {font-family : 'Monospace'; font-size: %dpt;}",
            sizeFont, sizeFont);

    gtk_css_provider_load_from_data(provider, cssBuffer, -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);
  } else {
    cout << "fuente demasiado pequeña" << endl;
  }
}
void imprimirArbol(Nodo *nodo, int nivel = 0) { ast_viewer_mostrar(arbol); }
string LimpiarLexemaParaDisplay(const string &lexema) {
  string resultado = "";
  for (char c : lexema) {
    if (c == '\n') {
      resultado += "\\n"; // Muestra explícitamente el salto de línea
    } else if (c == '\r') {
      resultado += "\\r"; // Muestra el retorno de carro (típico en Windows)
    } else if (c == '\t') {
      resultado += "\\t"; // Muestra las tabulaciones
    } else {
      resultado += c; // Deja los caracteres normales intactos
    }
  }
  return resultado;
}
void setBufferCodeBytxt(string buff_C) {
  gtk_text_buffer_set_text(GTK_TEXT_BUFFER(bufferCode), buff_C.c_str(), -1);
}
// funcion de importar el codigo de un txt
//// recive en data el widget de entrycode y outputtext

void ImportarCode(GtkWidget *widget, gpointer data) {
  // recivo la struct mandada(con input y output dentro)
  AppWidgets *widgets = (AppWidgets *)data;
  // uso entrycode de la estructura (referencia del
  bufferCode = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->EntryCode));
  // text widget entrycode)
  ifstream inputFile("code_ejemplo.txt");
  if (!inputFile.is_open()) {
    cout << "Error: no se pudo abrir el archivo" << endl;
    // cin.get();
  }
  string linea;
  string bufferC;
  bufferC.reserve(300);
  while (getline(inputFile, linea)) {
    cout << linea << endl;
    bufferC.append(linea);
    bufferC.append("\n");
  }
  setBufferCodeBytxt(bufferC);
  cout << bufferC << endl;
}

void changeColor(GtkWidget *widget, gpointer data) {
  AppWidgets *widgets = (AppWidgets *)data;

  GtkCssProvider *provider = widgets->Provider;
  GtkWidget *ThemeColorBtn = widgets->ThemeColorBtn;
  // es niht
  if (color_state) {
    gtk_css_provider_load_from_data(provider, cssWindowColorNight, -1, NULL);
    gtk_button_set_label(GTK_BUTTON(ThemeColorBtn), "Night");
  } else {
    // es day
    gtk_css_provider_load_from_data(provider, cssWindowColorDay, -1, NULL);

    gtk_button_set_label(GTK_BUTTON(ThemeColorBtn), "Day");
  }
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                            GTK_STYLE_PROVIDER(provider),
                                            GTK_STYLE_PROVIDER_PRIORITY_USER);
  color_state = !color_state;
}
void IDC_BTN_ANALIZAR(GtkWidget *btn, gpointer data) {
  AppWidgets *widgets = (AppWidgets *)data;

  bufferOutput = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->OutputText));
  bufferCode = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widgets->EntryCode));
  gint len = gtk_text_buffer_get_char_count(bufferCode);

  if (len > 0) {
    vector<gchar> buffer(len + 1);
    // creo los puunteros(sin valor) start, end
    GtkTextIter start, end;
    // a start le pongo el inicio del bufferCode y a end su final
    gtk_text_buffer_get_bounds(bufferCode, &start, &end);
    gchar *codigo = gtk_text_buffer_get_text(bufferCode, &start, &end, FALSE);
    // parseo el buffer de codigo  (gchar) a string
    string CodigoFuente = string(codigo);
    // libero el buffer de codigo
    g_free(codigo);
    try {
      Lexer miAnalizador(CodigoFuente, "matriz_transiciones.csv",
                         "nombres_tokens.csv", "estados_tokens.csv",
                         "char_columnas.csv", "keywords.csv");

      listaTokens = miAnalizador.generarListaTokens();
      listaTokens.push_back({FIN, "EOF", 0, 0});
      for (const auto &t : listaTokens) {
        cout << "tipo=" << t.tipo << " lexema=" << t.lexema << endl;
      }
      // Parser parser (listaTokens);
      //  arbol = parser.parsearPrograma();
      //  // ast_viewer_mostrar(arbol);

      std::stringstream salida;
      for (const auto &token : listaTokens) {
        const std::string &nombre = NOMBRES_TOKENS[token.tipo];
        salida << "Línea: " << token.linea << " \t| Col: " << token.columna
               << " \t| ";
        salida << "Tipo: [" << nombre << "]";
        salida << (nombre.length() < 7 ? "\t\t" : "\t");
        salida << "| Lexema: [" << LimpiarLexemaParaDisplay(token.lexema)
               << "]\r\n";
      }
      gtk_text_buffer_set_text(bufferOutput, salida.str().c_str(), -1);
    } catch (exception &e) {
      string msg = e.what();
      gtk_text_buffer_set_text(bufferOutput, msg.c_str(), -1);
    }
  }
  //      catch (const std::exception& e) {
  //        //cambiar a gtk
  //        SetWindowText(hwndOutput, ConvertirHaciaWide(e.what()).c_str());
  //      }
  //
}
static void activate(GtkApplication *app, gpointer user_data) {
  // window(contenedor/ventana principal)
  GtkWidget *window;
  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Analizador Lexico FIM");
  gtk_window_set_default_size(GTK_WINDOW(window), 1100, 700);
  // layout
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  GtkWidget *box_btns = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  GtkWidget *box_header_src = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  GtkWidget *lblinput = gtk_label_new("Codigo fuente");
  GtkWidget *lbloutput = gtk_label_new("Salida del lexer");
  gtk_container_set_border_width(GTK_CONTAINER(box), 15);
  // añado a la ventana las dos boxes
  gtk_container_add(GTK_CONTAINER(window), box);
  // El box_btns tiene que estar dentro del box (en medio)
  // gtk_container_add(GTK_CONTAINER(window),box_btns);
  // widgets
  // ventana de scroll para el "Edit text"
  GtkWidget *btnZoomM = gtk_button_new_with_label("+");
  GtkWidget *btnZoomL = gtk_button_new_with_label("-");
  GtkWidget *btnCompilar = gtk_button_new_with_label("Ejecutar Analizador");
  GtkWidget *btnMostrarAST = gtk_button_new_with_label("Mostrar AST");
  GtkWidget *btnGenerarArbol = gtk_button_new_with_label("Generar Arbol");
  GtkWidget *btnGetText = gtk_button_new_with_label("Importar");
  GtkWidget *btnChangeColor = gtk_button_new_with_label("Day");
  GtkWidget *scrollCode = gtk_scrolled_window_new(NULL, NULL);

  GtkWidget *scrollOutput = gtk_scrolled_window_new(NULL, NULL);

  gtk_widget_set_size_request(scrollCode, -1, 150);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrollCode),
                                             200);
  gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scrollCode),
                                            1100);

  gtk_widget_set_size_request(scrollOutput, -1, 150);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrollOutput),
                                             200);

  GtkWidget *OutputText = gtk_text_view_new();
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(OutputText), GTK_WRAP_WORD);
  gtk_container_add(GTK_CONTAINER(scrollOutput), OutputText);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(OutputText), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(OutputText), FALSE);

  GtkWidget *EntryCode = gtk_text_view_new();
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(EntryCode), GTK_WRAP_WORD);
  gtk_container_add(GTK_CONTAINER(scrollCode), EntryCode);
  // obtener el contexto de los output y del entry

  // GtkStyleContext *context_btn_theme =
  //     gtk_widget_get_style_context(btnChangeColor);
  GtkStyleContext *context_out = gtk_widget_get_style_context(OutputText);
  GtkStyleContext *context_entry = gtk_widget_get_style_context(EntryCode);
  // gtk_style_context_add_class(context_btn_theme, "btnTheme");
  gtk_style_context_add_class(context_out, "text_output");
  gtk_style_context_add_class(context_entry, "text_entry");
  GtkCssProvider *provider = gtk_css_provider_new();
  GtkCssProvider *providerFont = gtk_css_provider_new();

  AppWidgets *widgets = g_new(AppWidgets, 1);
  widgets->EntryCode = EntryCode;
  widgets->OutputText = OutputText;
  widgets->ThemeColorBtn = btnChangeColor;
  widgets->Provider = provider;
  widgets->ProviderFont = providerFont;
  // inicializacion del provider de font(fuente por defecto)
  // para que no se note el cambio de fuente o de cosos
  gtk_css_provider_load_from_data(
      providerFont,
      ".text_output { font-family: 'Monospace'; font-size: 16pt; }"
      ".text_entry {font-family : 'Monospace'; font-size: 16pt;}",
      -1, NULL);
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                            GTK_STYLE_PROVIDER(providerFont),
                                            GTK_STYLE_PROVIDER_PRIORITY_USER);
  // seteo de tema day por defecto
  gtk_css_provider_load_from_data(provider, cssWindowColorDay, -1, NULL);
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                            GTK_STYLE_PROVIDER(provider),
                                            GTK_STYLE_PROVIDER_PRIORITY_USER);

  gtk_box_pack_start(GTK_BOX(box_btns), btnGenerarArbol, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box_btns), btnMostrarAST, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box_btns), btnCompilar, FALSE, TRUE, 0);

  gtk_box_pack_end(GTK_BOX(box_btns), btnChangeColor, FALSE, TRUE, 0);
  gtk_box_pack_end(GTK_BOX(box_btns), btnZoomM, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(box_header_src), btnGetText, FALSE, TRUE, 0);
  gtk_box_pack_end(GTK_BOX(box_btns), btnZoomL, FALSE, TRUE, 0);

  // aqui se muestran todos los los cosos dentro de box (box ya se muestra como
  // box principal ya que es la unica que agrego a window)
  gtk_box_pack_start(GTK_BOX(box), box_btns, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(box), box_header_src, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(box_header_src), lblinput, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(box), scrollCode, TRUE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(box), lbloutput, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(box), scrollOutput, TRUE, TRUE, 0);
  // signals- lo que el boton o cosa que tenga accionador, va a hacer .D
  g_signal_connect(btnZoomL, "clicked", G_CALLBACK(ZoomL), widgets);
  g_signal_connect(btnZoomM, "clicked", G_CALLBACK(ZoomM), widgets);

  g_signal_connect(btnGetText, "clicked", G_CALLBACK(ImportarCode), widgets);
  g_signal_connect(btnMostrarAST, "clicked", G_CALLBACK(imprimirArbol), NULL);
  g_signal_connect(btnGenerarArbol, "clicked", G_CALLBACK(parsear), &errores);
  g_signal_connect(btnCompilar, "clicked", G_CALLBACK(IDC_BTN_ANALIZAR),
                   widgets);
  g_signal_connect(btnChangeColor, "clicked", G_CALLBACK(changeColor), widgets);
  // inicializacion de window
  gtk_widget_show_all(window);
}
int main(int argc, char *argv[]) {
  gtk_init(&argc, &argv);
  GtkApplication *app;
  int status;
  app = gtk_application_new("org.compiler.app", G_APPLICATION_DEFAULT_FLAGS);
  // gtk_application_window_new(app);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}
