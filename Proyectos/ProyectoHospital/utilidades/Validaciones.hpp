#ifndef VALIDACIONES_HPP
#define VALIDACIONES_HPP

class Validaciones {
public:
    static int leerEntero(const char* mensaje, int min, int max);
    static float leerFloat(const char* mensaje, float min, float max);
    static void leerCadena(const char* mensaje, char* destino, int tamMax, bool permitirVacio = false);
    static char leerSexo(const char* mensaje);
    static bool validarFecha(const char* fecha);
    static void leerFecha(const char* mensaje, char* destino);
    static bool validarHora(const char* hora);
    static void leerHora(const char* mensaje, char* destino);
    static bool validarEmail(const char* email);
    static bool confirmarAccion(const char* mensaje);
};

#endif