#ifndef GESTOR_ARCHIVOS_HPP
#define GESTOR_ARCHIVOS_HPP

#include <fstream>
#include <iostream>
#include "Constantes.hpp"


class GestorArchivos {
public:
    static bool inicializarSistemaArchivos();
    

    static bool guardarRegistro(void* registro, size_t tamano, const char* archivo);
    static bool leerRegistroPorID(int id, void* registro, size_t tamano, const char* archivo, int offsetID = 0);
    static bool actualizarRegistro(int id, void* registro, size_t tamano, const char* archivo, int offsetID = 0);
    static bool eliminarRegistroLogico(int id, size_t tamano, const char* archivo, int offsetID = 0, int offsetEliminado = -1);
    
    static ArchivoHeader leerHeader(const char* archivo);
    static bool leerRegistroPorPosicion(int indice, void* registro, size_t tamano, const char* archivo);
    static bool actualizarHeader(const char* nombreArchivo, const ArchivoHeader& header);
    static std::streamoff calcularPosicion(int indice, size_t tamanoRegistro);

private:
    static bool inicializarArchivo(const char* archivo);
};

#endif