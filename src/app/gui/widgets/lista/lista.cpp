#include "lista.h"
#include "Fonts/FreeSans9pt7b.h"

Lista::Lista(MCUFRIEND_kbv &display_ref,const char** items,uint8_t total_items,const uint16_t color_texto,const uint16_t color_texto_seleccionado,const uint16_t color_fondo,const uint16_t color_fondo_seleccionado,uint8_t max_filas_por_col,uint8_t ancho_col,uint8_t alto_fil):
    display(display_ref),color_texto(color_texto), 
    color_texto_seleccionado(color_texto_seleccionado), 
    color_fondo(color_fondo), 
    color_fondo_seleccionado(color_fondo_seleccionado),
    max_filas_por_col(max_filas_por_col),
    ancho_columna(ancho_col),
    alto_fila(alto_fil)
{
    this->lista_items = items;
    this->total_items = total_items;
    this->indice_items = 0;
}

void Lista::mostrar_item(uint8_t id_item, bool item_seleccionado) {
    //determinar la fila y columna en la que se posiciona el item
    uint8_t fila = id_item % max_filas_por_col;
    uint8_t columna = id_item / max_filas_por_col;

    uint16_t posicion_x_item = posicion_x + columna * ancho_columna;
    uint16_t posicion_y_item = posicion_y + fila * alto_fila;

    uint16_t fondo = item_seleccionado ? color_fondo_seleccionado : color_fondo;
    uint16_t texto = item_seleccionado ? color_texto_seleccionado : color_texto;

    display.fillRect(posicion_x_item - offset_x, posicion_y_item - offset_y, ancho_columna - 5, alto_fila, fondo);

    display.setCursor(posicion_x_item, posicion_y_item);
    display.setTextColor(texto);
    display.setFont(&FreeSans9pt7b);
    display.print(lista_items[id_item]);
}

bool Lista::mostrar_lista() {
    
    if(total_items > 1){
        display.setCursor(posicion_x, posicion_y);
        for (uint8_t i = 0; i < total_items; i++) {
            mostrar_item(i, i == indice_items);
        }
        return true;
    }
    else{
        //la lista no tiene elementos
        return false;
    }
    
}

bool Lista::navegar_previo() {
    if (indice_items > 0) {

        uint8_t anterior = indice_items;
        indice_items--;
        mostrar_item(anterior, false);
        mostrar_item(indice_items, true);
        return true;
    }
    else{
        //no se puede acceder al item previo
        return false;
    }
}

bool Lista::navegar_siguiente() {
    if (indice_items < total_items - 1) {

        uint8_t anterior = indice_items;
        indice_items++;
        mostrar_item(anterior, false);
        mostrar_item(indice_items, true);
        return true;
    }
    else{
        //no se puede acceder al sigueinte item
        return false;
    }
}
