#pragma once
#include "parser.h"
#include <algorithm>
#include <gtk/gtk.h>
#include <map>
#include <string>
#include <vector>

// ════════════════════════════════════════════════════════════════════════════
//  COLORES POR TIPO DE NODO
// ════════════════════════════════════════════════════════════════════════════
static std::map<std::string, std::tuple<double, double, double>> AST_COLORES = {
    {"PROGRAMA", {0.15, 0.40, 0.70}}, {"ASIGNACION", {0.75, 0.42, 0.10}},
    {"+", {0.65, 0.20, 0.20}},        {"-", {0.65, 0.20, 0.20}},
    {"*", {0.65, 0.20, 0.20}},        {"/", {0.65, 0.20, 0.20}},
    {"ENTERO", {0.18, 0.55, 0.35}},   {"DECIMAL", {0.18, 0.55, 0.35}},
    {"ID", {0.38, 0.35, 0.65}},       {"CADENA", {0.18, 0.52, 0.52}},
    {"IF", {0.20, 0.52, 0.28}},       {"WHILE", {0.20, 0.52, 0.28}},
    {"FUNCION", {0.16, 0.55, 0.52}},  {"LLAMADA", {0.45, 0.22, 0.62}},
    {"RETORNO", {0.60, 0.22, 0.40}},
};

static std::tuple<double, double, double>
ast_color_para(const std::string &tipo) {
  auto it = AST_COLORES.find(tipo);
  if (it != AST_COLORES.end())
    return it->second;
  return {0.28, 0.28, 0.42};
}

// ════════════════════════════════════════════════════════════════════════════
//  DIMENSIONES
// ════════════════════════════════════════════════════════════════════════════
static const double AST_NODE_W = 100.0;
static const double AST_NODE_H = 42.0;
static const double AST_NIVEL_SEP = 80.0;
static const double AST_H_PAD = 8.0;

// ════════════════════════════════════════════════════════════════════════════
//  HELPERS
// ════════════════════════════════════════════════════════════════════════════
static std::vector<Nodo *> ast_hijos(Nodo *n) {
  std::vector<Nodo *> res;
  if (!n)
    return res;
  if (n->izq)
    res.push_back(n->izq);
  if (n->der)
    res.push_back(n->der);
  for (Nodo *h : n->hijos)
    res.push_back(h);
  return res;
}

static double ast_ancho(Nodo *n) {
  if (!n)
    return 0;
  auto hijos = ast_hijos(n);
  if (hijos.empty())
    return AST_NODE_W + AST_H_PAD;
  double total = 0;
  for (Nodo *h : hijos)
    total += ast_ancho(h);
  return std::max(total, AST_NODE_W + AST_H_PAD);
}

static int ast_profundidad(Nodo *n) {
  if (!n)
    return 0;
  int mx = 0;
  for (Nodo *h : ast_hijos(n))
    mx = std::max(mx, ast_profundidad(h));
  return mx + 1;
}

// ════════════════════════════════════════════════════════════════════════════
//  DIBUJO RECURSIVO
// ════════════════════════════════════════════════════════════════════════════
static void ast_dibujar(cairo_t *cr, Nodo *n, double cx, double y) {
  if (!n)
    return;

  auto hijos = ast_hijos(n);

  if (!hijos.empty()) {
    double total_w = 0;
    for (Nodo *h : hijos)
      total_w += ast_ancho(h);

    double hx = cx - total_w / 2.0;
    for (Nodo *h : hijos) {
      double hw = ast_ancho(h);
      double hcx = hx + hw / 2.0;

      cairo_set_source_rgba(cr, 0.65, 0.65, 0.65, 0.8);
      cairo_set_line_width(cr, 1.3);
      cairo_move_to(cr, cx, y + AST_NODE_H);
      cairo_line_to(cr, hcx, y + AST_NIVEL_SEP);
      cairo_stroke(cr);

      ast_dibujar(cr, h, hcx, y + AST_NIVEL_SEP);
      hx += hw;
    }
  }

  double rx = cx - AST_NODE_W / 2.0;
  auto [r, g, b] = ast_color_para(n->tipo);

  cairo_set_source_rgba(cr, 0, 0, 0, 0.18);
  cairo_rectangle(cr, rx + 2, y + 2, AST_NODE_W, AST_NODE_H);
  cairo_fill(cr);

  cairo_set_source_rgb(cr, r, g, b);
  cairo_rectangle(cr, rx, y, AST_NODE_W, AST_NODE_H);
  cairo_fill(cr);

  cairo_set_source_rgba(cr, 1, 1, 1, 0.25);
  cairo_set_line_width(cr, 1.0);
  cairo_rectangle(cr, rx, y, AST_NODE_W, AST_NODE_H);
  cairo_stroke(cr);

  cairo_select_font_face(cr, "Monospace", CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 9);
  cairo_set_source_rgba(cr, 1, 1, 1, 0.60);
  cairo_move_to(cr, rx + 5, y + 13);
  cairo_show_text(cr, n->tipo.c_str());

  std::string val = n->valor;
  if (val.length() > 11)
    val = val.substr(0, 10) + "…";

  cairo_select_font_face(cr, "Monospace", CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, 12);
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_move_to(cr, rx + 5, y + 30);
  cairo_show_text(cr, val.c_str());
}

// ════════════════════════════════════════════════════════════════════════════
//  DATOS DEL VISOR
//  - zoom     : factor de escala actual
//  - offset_x/y: desplazamiento en píxeles (pan)
//  - drag_*   : estado del arrastre con botón central / botón 1
// ════════════════════════════════════════════════════════════════════════════
typedef struct {
  Nodo *raiz;
  double zoom;
  double offset_x;
  double offset_y;
  // Pan con ratón
  gboolean dragging;
  double drag_start_x;
  double drag_start_y;
  double drag_origin_x;
  double drag_origin_y;
  GtkWidget *area;
} ASTViewerData;

// ════════════════════════════════════════════════════════════════════════════
//  DRAW
// ════════════════════════════════════════════════════════════════════════════
static gboolean ast_on_draw(GtkWidget *widget, cairo_t *cr, gpointer data) {
  ASTViewerData *d = (ASTViewerData *)data;

  cairo_set_source_rgb(cr, 0.10, 0.10, 0.14);
  cairo_paint(cr);

  if (!d->raiz) {
    cairo_set_source_rgba(cr, 1, 1, 1, 0.4);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 14);
    cairo_move_to(cr, 20, 40);
    cairo_show_text(cr, "No hay arbol generado aun.");
    return FALSE;
  }

  GtkAllocation alloc;
  gtk_widget_get_allocation(widget, &alloc);

  // Aplicar translación + zoom centrados en el widget
  cairo_translate(cr, d->offset_x, d->offset_y);
  cairo_scale(cr, d->zoom, d->zoom);

  double centro = (alloc.width / d->zoom) / 2.0;
  ast_dibujar(cr, d->raiz, centro, 24);
  return FALSE;
}

// ════════════════════════════════════════════════════════════════════════════
//  SCROLL  — maneja rueda de ratón Y touchpad (smooth + pinch)
//
//  Reglas:
//    • GDK_SCROLL_SMOOTH + Ctrl  →  zoom (pinch de touchpad o Ctrl+scroll)
//    • GDK_SCROLL_SMOOTH sin Ctrl→  pan (scroll de dos dedos en touchpad)
//    • GDK_SCROLL_UP/DOWN + Ctrl →  zoom (Ctrl + rueda de ratón clásico)
//    • GDK_SCROLL_UP/DOWN sin Ctrl→  pan vertical (rueda de ratón clásico)
// ════════════════════════════════════════════════════════════════════════════
static gboolean ast_on_scroll(GtkWidget *widget, GdkEventScroll *ev,
                              gpointer data) {
  ASTViewerData *d = (ASTViewerData *)data;
  gboolean ctrl_held = (ev->state & GDK_CONTROL_MASK) != 0;

  if (ev->direction == GDK_SCROLL_SMOOTH) {
    if (ctrl_held) {
      // ── Pinch / Ctrl+scroll suave → zoom centrado en el cursor ──────
      // delta_y > 0  ≡  alejar;  delta_y < 0  ≡  acercar
      double factor = (ev->delta_y > 0) ? 1.0 / (1.0 + ev->delta_y * 0.12)
                                        : (1.0 + (-ev->delta_y) * 0.12);

      double old_zoom = d->zoom;
      d->zoom = std::clamp(d->zoom * factor, 0.10, 6.0);
      double real_factor = d->zoom / old_zoom;

      // Ajustar offset para que el punto bajo el cursor no se mueva
      d->offset_x = ev->x - real_factor * (ev->x - d->offset_x);
      d->offset_y = ev->y - real_factor * (ev->y - d->offset_y);
    } else {
      // ── Scroll de dos dedos → pan ─────────────────────────────────
      d->offset_x -= ev->delta_x * 4.0;
      d->offset_y -= ev->delta_y * 4.0;
    }
    gtk_widget_queue_draw(widget);
    return TRUE;
  }

  // Eventos clásicos de rueda
  const double STEP = 0.12;
  if (ctrl_held) {
    double factor =
        (ev->direction == GDK_SCROLL_UP) ? (1.0 + STEP) : (1.0 - STEP);
    double old_zoom = d->zoom;
    d->zoom = std::clamp(d->zoom * factor, 0.10, 6.0);
    double real_factor = d->zoom / old_zoom;
    d->offset_x = ev->x - real_factor * (ev->x - d->offset_x);
    d->offset_y = ev->y - real_factor * (ev->y - d->offset_y);
  } else {
    const double PAN = 40.0;
    if (ev->direction == GDK_SCROLL_UP)
      d->offset_y += PAN;
    if (ev->direction == GDK_SCROLL_DOWN)
      d->offset_y -= PAN;
    if (ev->direction == GDK_SCROLL_LEFT)
      d->offset_x += PAN;
    if (ev->direction == GDK_SCROLL_RIGHT)
      d->offset_x -= PAN;
  }

  gtk_widget_queue_draw(widget);
  return TRUE;
}

// ════════════════════════════════════════════════════════════════════════════
//  PAN CON RATÓN (botón 1 o botón central)
// ════════════════════════════════════════════════════════════════════════════
static gboolean ast_on_button_press(GtkWidget *, GdkEventButton *ev,
                                    gpointer data) {
  ASTViewerData *d = (ASTViewerData *)data;
  if (ev->button == 1 || ev->button == 2) {
    d->dragging = TRUE;
    d->drag_start_x = ev->x;
    d->drag_start_y = ev->y;
    d->drag_origin_x = d->offset_x;
    d->drag_origin_y = d->offset_y;
    return TRUE;
  }
  return FALSE;
}

static gboolean ast_on_button_release(GtkWidget *, GdkEventButton *ev,
                                      gpointer data) {
  ASTViewerData *d = (ASTViewerData *)data;
  if (ev->button == 1 || ev->button == 2) {
    d->dragging = FALSE;
    return TRUE;
  }
  return FALSE;
}

static gboolean ast_on_motion(GtkWidget *widget, GdkEventMotion *ev,
                              gpointer data) {
  ASTViewerData *d = (ASTViewerData *)data;
  if (d->dragging) {
    d->offset_x = d->drag_origin_x + (ev->x - d->drag_start_x);
    d->offset_y = d->drag_origin_y + (ev->y - d->drag_start_y);
    gtk_widget_queue_draw(widget);
    return TRUE;
  }
  return FALSE;
}

// Tecla R → resetear vista
static gboolean ast_on_key_press(GtkWidget *widget, GdkEventKey *ev,
                                 gpointer data) {
  ASTViewerData *d = (ASTViewerData *)data;
  if (ev->keyval == GDK_KEY_r || ev->keyval == GDK_KEY_R) {
    d->zoom = 1.0;
    d->offset_x = 0.0;
    d->offset_y = 0.0;
    gtk_widget_queue_draw(widget);
    return TRUE;
  }
  return FALSE;
}

// ════════════════════════════════════════════════════════════════════════════
//  FUNCIÓN PÚBLICA
// ════════════════════════════════════════════════════════════════════════════
static ASTViewerData *_ast_global = nullptr;

static void ast_viewer_mostrar(Nodo *raiz) {
  int ancho = (int)ast_ancho(raiz) + 120;
  int alto = ast_profundidad(raiz) * (int)AST_NIVEL_SEP + 80;
  if (ancho < 600)
    ancho = 600;
  if (alto < 400)
    alto = 400;

  if (_ast_global == nullptr) {
    _ast_global = g_new0(ASTViewerData, 1);
    _ast_global->zoom = 1.0;
    _ast_global->offset_x = 0.0;
    _ast_global->offset_y = 0.0;

    GtkWidget *ventana = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(
        GTK_WINDOW(ventana),
        "AST Viewer  [scroll=pan · Ctrl+scroll/pinch=zoom · R=reset]");
    gtk_window_set_default_size(GTK_WINDOW(ventana), 900, 600);
    g_signal_connect(ventana, "delete-event",
                     G_CALLBACK(gtk_widget_hide_on_delete), NULL);

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    // Desactivamos las barras nativas: manejamos el scroll nosotros
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER, GTK_POLICY_NEVER);

    GtkWidget *area = gtk_drawing_area_new();
    _ast_global->area = area;

    // Eventos necesarios
    gtk_widget_add_events(
        area,
        GDK_SCROLL_MASK |
            GDK_SMOOTH_SCROLL_MASK | // ← imprescindible para touchpad
            GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
            GDK_POINTER_MOTION_MASK | GDK_KEY_PRESS_MASK);

    gtk_widget_set_can_focus(area, TRUE);

    g_signal_connect(area, "draw", G_CALLBACK(ast_on_draw), _ast_global);
    g_signal_connect(area, "scroll-event", G_CALLBACK(ast_on_scroll),
                     _ast_global);
    g_signal_connect(area, "button-press-event",
                     G_CALLBACK(ast_on_button_press), _ast_global);
    g_signal_connect(area, "button-release-event",
                     G_CALLBACK(ast_on_button_release), _ast_global);
    g_signal_connect(area, "motion-notify-event", G_CALLBACK(ast_on_motion),
                     _ast_global);
    g_signal_connect(area, "key-press-event", G_CALLBACK(ast_on_key_press),
                     _ast_global);

    gtk_container_add(GTK_CONTAINER(scroll), area);
    gtk_container_add(GTK_CONTAINER(ventana), scroll);
    gtk_widget_show_all(ventana);
  }

  _ast_global->raiz = raiz;
  _ast_global->zoom = 1.0;
  _ast_global->offset_x = 0.0;
  _ast_global->offset_y = 0.0;
  gtk_widget_set_size_request(_ast_global->area, ancho, alto);
  gtk_widget_queue_draw(_ast_global->area);

  GtkWidget *top = gtk_widget_get_toplevel(_ast_global->area);
  gtk_window_present(GTK_WINDOW(top));
  gtk_widget_grab_focus(_ast_global->area);
}
