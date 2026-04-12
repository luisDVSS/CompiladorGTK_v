#pragma once
#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>

// ── Debe coincidir con tu definicion en parser.h ─────────────────────────────

// ════════════════════════════════════════════════════════════════════════════
//  COLORES POR TIPO DE NODO
// ════════════════════════════════════════════════════════════════════════════
static std::map<std::string, std::tuple<double,double,double>> AST_COLORES = {
    // Nodos estructurales
    {"PROGRAMA",     {0.15, 0.40, 0.70}},
    {"ASIGNACION",   {0.75, 0.42, 0.10}},
    // Operadores aritmeticos
    {"+",            {0.65, 0.20, 0.20}},
    {"-",            {0.65, 0.20, 0.20}},
    {"*",            {0.65, 0.20, 0.20}},
    {"/",            {0.65, 0.20, 0.20}},
    // Tipos de datos / hojas
    {"ENTERO",       {0.18, 0.55, 0.35}},
    {"DECIMAL",      {0.18, 0.55, 0.35}},
    {"ID",           {0.38, 0.35, 0.65}},
    {"CADENA",       {0.18, 0.52, 0.52}},
    // Por si agregas mas nodos despues
    {"IF",           {0.20, 0.52, 0.28}},
    {"WHILE",        {0.20, 0.52, 0.28}},
    {"FUNCION",      {0.16, 0.55, 0.52}},
    {"LLAMADA",      {0.45, 0.22, 0.62}},
    {"RETORNO",      {0.60, 0.22, 0.40}},
};

static std::tuple<double,double,double> ast_color_para(const std::string& tipo) {
    auto it = AST_COLORES.find(tipo);
    if (it != AST_COLORES.end()) return it->second;
    return {0.28, 0.28, 0.42};
}

// ════════════════════════════════════════════════════════════════════════════
//  DIMENSIONES
// ════════════════════════════════════════════════════════════════════════════
static const double AST_NODE_W   = 100.0;
static const double AST_NODE_H   =  42.0;
static const double AST_NIVEL_SEP=  80.0;
static const double AST_H_PAD    =   8.0;  // espacio horizontal entre subarboles

// ════════════════════════════════════════════════════════════════════════════
//  HELPERS: obtener hijos unificados (izq/der + vector hijos)
// ════════════════════════════════════════════════════════════════════════════
static std::vector<Nodo*> ast_hijos(Nodo* n) {
    std::vector<Nodo*> res;
    if (!n) return res;
    if (n->izq) res.push_back(n->izq);
    if (n->der) res.push_back(n->der);
    for (Nodo* h : n->hijos) res.push_back(h);
    return res;
}

// ════════════════════════════════════════════════════════════════════════════
//  CALCULO DE ANCHO (recursivo, incluye padding entre hijos)
// ════════════════════════════════════════════════════════════════════════════
static double ast_ancho(Nodo* n) {
    if (!n) return 0;
    auto hijos = ast_hijos(n);
    if (hijos.empty()) return AST_NODE_W + AST_H_PAD;

    double total = 0;
    for (Nodo* h : hijos) total += ast_ancho(h);
    return std::max(total, AST_NODE_W + AST_H_PAD);
}

// ════════════════════════════════════════════════════════════════════════════
//  DIBUJO RECURSIVO
// ════════════════════════════════════════════════════════════════════════════
static void ast_dibujar(cairo_t* cr, Nodo* n, double cx, double y) {
    if (!n) return;

    auto hijos = ast_hijos(n);

    // ── Lineas a hijos (primero, quedan detras) ──────────────────────────
    if (!hijos.empty()) {
        double total_w = 0;
        for (Nodo* h : hijos) total_w += ast_ancho(h);

        double hx = cx - total_w / 2.0;
        for (Nodo* h : hijos) {
            double hw  = ast_ancho(h);
            double hcx = hx + hw / 2.0;

            cairo_set_source_rgba(cr, 0.65, 0.65, 0.65, 0.8);
            cairo_set_line_width(cr, 1.3);
            cairo_move_to(cr, cx,  y + AST_NODE_H);
            cairo_line_to(cr, hcx, y + AST_NIVEL_SEP);
            cairo_stroke(cr);

            ast_dibujar(cr, h, hcx, y + AST_NIVEL_SEP);
            hx += hw;
        }
    }

    // ── Rectangulo del nodo ──────────────────────────────────────────────
    double rx = cx - AST_NODE_W / 2.0;
    auto [r, g, b] = ast_color_para(n->tipo);

    // Sombra
    cairo_set_source_rgba(cr, 0, 0, 0, 0.18);
    cairo_rectangle(cr, rx + 2, y + 2, AST_NODE_W, AST_NODE_H);
    cairo_fill(cr);

    // Fondo
    cairo_set_source_rgb(cr, r, g, b);
    cairo_rectangle(cr, rx, y, AST_NODE_W, AST_NODE_H);
    cairo_fill(cr);

    // Borde claro
    cairo_set_source_rgba(cr, 1, 1, 1, 0.25);
    cairo_set_line_width(cr, 1.0);
    cairo_rectangle(cr, rx, y, AST_NODE_W, AST_NODE_H);
    cairo_stroke(cr);

    // ── Texto: tipo (gris, 9px, arriba) ─────────────────────────────────
    cairo_select_font_face(cr, "Monospace",
                           CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 9);
    cairo_set_source_rgba(cr, 1, 1, 1, 0.60);
    cairo_move_to(cr, rx + 5, y + 13);
    cairo_show_text(cr, n->tipo.c_str());

    // ── Texto: valor (blanco, 12px, abajo) ──────────────────────────────
    std::string val = n->valor;
    if (val.length() > 11) val = val.substr(0, 10) + "…";

    cairo_select_font_face(cr, "Monospace",
                           CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 12);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_move_to(cr, rx + 5, y + 30);
    cairo_show_text(cr, val.c_str());
}

// ════════════════════════════════════════════════════════════════════════════
//  PROFUNDIDAD (para calcular alto del canvas)
// ════════════════════════════════════════════════════════════════════════════
static int ast_profundidad(Nodo* n) {
    if (!n) return 0;
    int mx = 0;
    for (Nodo* h : ast_hijos(n))
        mx = std::max(mx, ast_profundidad(h));
    return mx + 1;
}

// ════════════════════════════════════════════════════════════════════════════
//  DATOS DEL VISOR
// ════════════════════════════════════════════════════════════════════════════
typedef struct {
    Nodo*      raiz;
    double     zoom;
    GtkWidget* area;
} ASTViewerData;

// ════════════════════════════════════════════════════════════════════════════
//  CALLBACKS
// ════════════════════════════════════════════════════════════════════════════
static gboolean ast_on_draw(GtkWidget* widget, cairo_t* cr, gpointer data) {
    ASTViewerData* d = (ASTViewerData*)data;

    // Fondo oscuro
    cairo_set_source_rgb(cr, 0.10, 0.10, 0.14);
    cairo_paint(cr);

    if (!d->raiz) {
        cairo_set_source_rgba(cr, 1, 1, 1, 0.4);
        cairo_select_font_face(cr, "Sans",
                               CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(cr, 14);
        cairo_move_to(cr, 20, 40);
        cairo_show_text(cr, "No hay arbol generado aun.");
        return FALSE;
    }

    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);

    cairo_scale(cr, d->zoom, d->zoom);
    double centro = (alloc.width / d->zoom) / 2.0;
    ast_dibujar(cr, d->raiz, centro, 24);
    return FALSE;
}

static gboolean ast_on_scroll(GtkWidget* widget, GdkEventScroll* ev, gpointer data) {
    ASTViewerData* d = (ASTViewerData*)data;
    if      (ev->direction == GDK_SCROLL_UP)   d->zoom = std::min(d->zoom * 1.12, 4.0);
    else if (ev->direction == GDK_SCROLL_DOWN)  d->zoom = std::max(d->zoom / 1.12, 0.15);
    gtk_widget_queue_draw(widget);
    return TRUE;
}

// ════════════════════════════════════════════════════════════════════════════
//  FUNCION PUBLICA: abrir (o refrescar) la ventana del AST
//
//  Llama esta funcion desde IDC_BTN_ANALIZAR despues de obtener el arbol:
//
//      Nodo* arbol = parser.parsearPrograma();
//      ast_viewer_mostrar(arbol);
//
// ════════════════════════════════════════════════════════════════════════════
static ASTViewerData* _ast_global = nullptr;

static void ast_viewer_mostrar(Nodo* raiz) {
    // Calcular tamano del canvas
    int ancho  = (int)ast_ancho(raiz) + 120;
    int alto   = ast_profundidad(raiz) * (int)AST_NIVEL_SEP + 80;
    if (ancho < 600) ancho = 600;
    if (alto  < 400) alto  = 400;

    if (_ast_global == nullptr) {
        // Primera vez: crear ventana nueva
        _ast_global = g_new(ASTViewerData, 1);
        _ast_global->zoom = 1.0;

        GtkWidget* ventana = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(ventana), "AST Viewer");
        gtk_window_set_default_size(GTK_WINDOW(ventana), 900, 600);
        // Cerrar solo oculta, no destruye (para poder reusar)
        g_signal_connect(ventana, "delete-event",
                         G_CALLBACK(gtk_widget_hide_on_delete), NULL);

        GtkWidget* scroll = gtk_scrolled_window_new(NULL, NULL);
        gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                       GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

        GtkWidget* area = gtk_drawing_area_new();
        _ast_global->area = area;

        gtk_widget_add_events(area, GDK_SCROLL_MASK);
        g_signal_connect(area, "draw",         G_CALLBACK(ast_on_draw),   _ast_global);
        g_signal_connect(area, "scroll-event", G_CALLBACK(ast_on_scroll), _ast_global);

        gtk_container_add(GTK_CONTAINER(scroll), area);
        gtk_container_add(GTK_CONTAINER(ventana), scroll);
        gtk_widget_show_all(ventana);
    }

    // Actualizar datos y tamano del canvas
    _ast_global->raiz = raiz;
    _ast_global->zoom = 1.0;
    gtk_widget_set_size_request(_ast_global->area, ancho, alto);
    gtk_widget_queue_draw(_ast_global->area);

    // Traer la ventana al frente si ya existia
    GtkWidget* top = gtk_widget_get_toplevel(_ast_global->area);
    gtk_window_present(GTK_WINDOW(top));
}
