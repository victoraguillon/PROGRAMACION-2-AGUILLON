#include "Formatos.hpp"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>

using namespace std;


const int ANCHO_CONSOLA = 80;

void Formatos::limpiarPantalla() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void Formatos::pausar() {
    cout << "\n";
    printCentrado("Presione [ENTER] para continuar...", AMARILLO);
    cin.get();
}

void Formatos::limpiarBuffer() {
    cin.clear();
    char c;
    while ((c = cin.get()) != '\n' && c != EOF) {}
}

void Formatos::toLowerCopy(const char* src, char* dst, int dstSize) {
    int n = (int)strlen(src);
    if (n >= dstSize) n = dstSize - 1;
    for (int i = 0; i < n; i++) {
        char c = src[i];
        if (c >= 'A' && c <= 'Z') c = (char)(c + 32);
        dst[i] = c;
    }
    dst[n] = '\0';
}


void Formatos::printCentrado(const char* texto, const char* color) {
    int len = (int)strlen(texto);
    int padding = (ANCHO_CONSOLA - len) / 2;
    
    if (padding < 0) padding = 0;

    for(int i = 0; i < padding; i++) cout << " ";
    cout << color << texto << RESET << endl;
}

void Formatos::imprimirLineaSeparadora() {
    cout << CYAN;
    for(int i=0; i<ANCHO_CONSOLA; i++) cout << "=";
    cout << RESET << endl;
}

void Formatos::imprimirEncabezado(const char* titulo) {
    limpiarPantalla();
    cout << endl;
    imprimirLineaSeparadora();
    cout << endl;
    printCentrado(titulo, NEGRITA CYAN);
    cout << endl;
    imprimirLineaSeparadora();
    cout << endl;
}

void Formatos::printError(const char* mensaje) {
    string msg = "X Error: ";
    msg += mensaje;
    printCentrado(msg.c_str(), ROJO);
}

void Formatos::printExito(const char* mensaje) {
    string msg = "! Exito: ";
    msg += mensaje;
    printCentrado(msg.c_str(), VERDE);
}