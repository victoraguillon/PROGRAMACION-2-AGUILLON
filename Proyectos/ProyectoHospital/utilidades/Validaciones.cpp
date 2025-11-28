#include "Validaciones.hpp"
#include "Formatos.hpp"
#include <iostream>
#include <cstring>

using namespace std;

int Validaciones::leerEntero(const char* mensaje, int min, int max) {
    int val;
    while (true) {
        cout << mensaje;
        if (cin >> val) {
            if (val >= min && val <= max) {
                Formatos::limpiarBuffer();
                return val;
            }
            cout << "Error: Valor fuera de rango (" << min << "-" << max << ").\n";
        } else {
            cout << "Error: Entrada no valida.\n";
            Formatos::limpiarBuffer();
        }
    }
}

float Validaciones::leerFloat(const char* mensaje, float min, float max) {
    float val;
    while (true) {
        cout << mensaje;
        if (cin >> val) {
            if (val >= min && val <= max) {
                Formatos::limpiarBuffer();
                return val;
            }
            cout << "Error: Valor fuera de rango.\n";
        } else {
            cout << "Error: Entrada no valida.\n";
            Formatos::limpiarBuffer();
        }
    }
}

void Validaciones::leerCadena(const char* mensaje, char* destino, int tamMax, bool permitirVacio) {
    while (true) {
        cout << mensaje;
        cin.getline(destino, tamMax);
        if (cin.fail()) {
            cout << "Error: Texto muy largo.\n";
            cin.clear(); 
            continue;
        }
        if (!permitirVacio && strlen(destino) == 0) {
            cout << "Error: El campo no puede estar vacio.\n";
            continue;
        }
        return;
    }
}

char Validaciones::leerSexo(const char* mensaje) {
    while (true) {
        cout << mensaje;
        char s;
        if (cin >> s) {
            Formatos::limpiarBuffer();
            if (s == 'M' || s == 'F' || s == 'm' || s == 'f') {
                return (s == 'm' ? 'M' : (s == 'f' ? 'F' : s));
            }
            cout << "Error: Use 'M' o 'F'.\n";
        } else {
            Formatos::limpiarBuffer();
        }
    }
}

bool Validaciones::validarFecha(const char* fecha) {
    if ((int)strlen(fecha) != 10) return false;
    if (fecha[4] != '-' || fecha[7] != '-') return false;
    int y = (fecha[0]-'0')*1000 + (fecha[1]-'0')*100 + (fecha[2]-'0')*10 + (fecha[3]-'0');
    int m = (fecha[5]-'0')*10 + (fecha[6]-'0');
    int d = (fecha[8]-'0')*10 + (fecha[9]-'0');
    if (y < 1900 || y > 2100) return false;
    if (m < 1 || m > 12) return false;
    return (d >= 1 && d <= 31);
}

void Validaciones::leerFecha(const char* mensaje, char* destino) {
    while(true) {
        leerCadena(mensaje, destino, 11);
        if(validarFecha(destino)) return;
        cout << "Formato invalido (YYYY-MM-DD).\n";
    }
}

bool Validaciones::validarHora(const char* hora) {
    if ((int)strlen(hora) != 5) return false;
    if (hora[2] != ':') return false;
    int h = (hora[0]-'0')*10 + (hora[1]-'0');
    int m = (hora[3]-'0')*10 + (hora[4]-'0');
    return (h >= 0 && h <= 23 && m >= 0 && m <= 59);
}

void Validaciones::leerHora(const char* mensaje, char* destino) {
    while(true) {
        leerCadena(mensaje, destino, 6);
        if(validarHora(destino)) return;
        cout << "Formato invalido (HH:MM).\n";
    }
}

bool Validaciones::validarEmail(const char* email) {
    if(strlen(email) == 0) return true; 
    bool arroba = false, punto = false;
    for(int i=0; email[i]; i++) {
        if(email[i] == '@') arroba = true;
        if(email[i] == '.') punto = true;
    }
    return arroba && punto;
}

bool Validaciones::confirmarAccion(const char* mensaje) {
    cout << mensaje << " (S/N): ";
    char c;
    cin >> c;
    Formatos::limpiarBuffer();
    return (c == 'S' || c == 's');
}