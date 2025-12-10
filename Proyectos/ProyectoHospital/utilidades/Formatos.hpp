#ifndef FORMATOS_HPP
#define FORMATOS_HPP

#include <string>

// Definición de colores 
#define RESET   "\033[0m"
#define ROJO    "\033[31m"
#define VERDE   "\033[32m"
#define AZUL    "\033[34m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"
#define AMARILLO "\033[33m"
#define BLANCO  "\033[37m"
#define NEGRITA "\033[1m"

class Formatos {
public:
    static void limpiarPantalla();
    static void pausar();
    static void limpiarBuffer();
    static void toLowerCopy(const char* src, char* dst, int dstSize);
    static void imprimirLineaSeparadora();
    static void imprimirEncabezado(const char* titulo);
    static void printCentrado(const char* texto, const char* color = BLANCO);
    static void printError(const char* mensaje);
    static void printExito(const char* mensaje);
};

#endif