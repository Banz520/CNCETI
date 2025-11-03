#ifndef LISTA_H
#define LISTA_H

#include "MCUFRIEND_kbv.h"

class Lista {
private:
    MCUFRIEND_kbv &display;
    static const uint16_t posicion_x = 12, posicion_y = 60;
    const uint16_t color_texto, color_texto_seleccionado, color_fondo, color_fondo_seleccionado;
    static const uint8_t offset_x = 2, offset_y = 15;
    const uint8_t max_filas_por_col, ancho_columna, alto_fila;
    uint8_t total_items, indice_items;
    const char** lista_items;
    bool inicializada;

    void mostrar_item(uint8_t id_item, bool item_seleccionado);

public:
    // Constructor sin los items - responsabilidad única
    Lista(MCUFRIEND_kbv &display_ref,
          const uint16_t color_texto,
          const uint16_t color_texto_seleccionado,
          const uint16_t color_fondo,
          const uint16_t color_fondo_seleccionado,
          uint8_t max_filas_por_col,
          uint8_t ancho_col,
          uint8_t alto_fil);
    
    // Método para inicializar con datos (puede ser llamado después)
    void inicializar(const char** items, uint8_t total_items);
    
    bool mostrar_lista();
    bool navegar_previo();
    bool navegar_siguiente();
    bool esta_inicializada() const { return inicializada; }
};

#endif