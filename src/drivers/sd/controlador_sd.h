#ifndef CONTROLADOR_SD_H
#define CONTROLADOR_SD_H

#include "SD.h"

class ControladorSD : SDClass{
private:
    

public:
    File directorio_raiz;
    ~ControladorSD();
    bool iniciarSD();
    const char** leerDirectorio(File dir, int numTabs);
};

#endif