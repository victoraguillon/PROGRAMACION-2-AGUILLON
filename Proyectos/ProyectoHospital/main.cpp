
// SISTEMA DE GESTION HOSPITALARIA - VERSIÓN 3.0 (FINAL)
// TEMA: MODULARIZACIÓN, PROGRAMACIÓN ORIENTADA A OBJETOS, CREACIÓN Y MANEJO DE LIBRERÍAS EN C++
// Hecho por: Victor M. Aguillon.
// Bibliotecas usadas: <iostream>, <fstream>, <string>, <limits>, <cstdio>, <cstddef>, <cstdlib>, <cstring>, <iomanip>, <ctime> y las bibliotecas propias del proyecto.
// ================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include "hospital/Hospital.hpp"
#include "persistencia/GestorArchivos.hpp"
#include "utilidades/Formatos.hpp"
#include "utilidades/Validaciones.hpp"
#include "pacientes/operacionesPacientes.hpp"
#include "doctores/operacionesDoctores.hpp"
#include "citas/operacionesCitas.hpp"
#include "historial/operacionesHistorial.hpp"
#include "pacientes/Paciente.hpp"
#include "doctores/Doctor.hpp"
#include "citas/Cita.hpp"
#include "historial/HistorialMedico.hpp"
#include <filesystem>
#include <vector>
#include "persistencia/Mantenimiento.hpp"

using namespace std;

// Declaracion de menus locales (implementaciones en sus módulos)
void menuMantenimiento(Hospital& h);

int main() {
    // 1. Inicializar sistema
    if (!GestorArchivos::inicializarSistemaArchivos()) {
        Formatos::printError("Error critico al inicializar sistema de archivos.");
        return 1;
    }

    // 2. Cargar datos del Hospital
    Hospital hospital;
    ifstream archivoHospital(Rutas::HOSPITAL, ios::binary);
    
    if (archivoHospital) {
        archivoHospital.read((char*)&hospital, sizeof(Hospital));
        archivoHospital.close();
        
        // --- Bloque de Edición Inicial con Estética ---
        Formatos::limpiarPantalla();
        Formatos::printCentrado("--- INICIO DEL SISTEMA ---", CYAN);
        cout << endl;
        Formatos::printCentrado("Datos cargados correctamente.", VERDE);
        
        // Mostramos el nombre actual centrado
        string msg = "Hospital Actual: " + string(hospital.getNombre());
        Formatos::printCentrado(msg.c_str(), BLANCO);
        cout << endl;

        Formatos::printCentrado("¿Desea editar los datos del hospital? (S/N)", AMARILLO);
        cout << "   >>> "; // Prompt simple
        
        string resp;
        if (cin.peek() == '\n') cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, resp);

        if (!resp.empty() && (resp[0] == 'S' || resp[0] == 's')) {
            Formatos::imprimirEncabezado("EDITAR CONFIGURACION HOSPITAL");
            
            string nombre, direccion, telefono, correo;
            
            // Usamos colores para los prompts
            cout << AMARILLO << "   Ingrese el nuevo nombre (Enter para mantener): " << RESET;
            getline(cin, nombre);
            
            cout << AMARILLO << "   Ingrese la nueva direccion: " << RESET;
            getline(cin, direccion);
            
            cout << AMARILLO << "   Ingrese el nuevo telefono: " << RESET;
            getline(cin, telefono);
            
            cout << AMARILLO << "   Ingrese el nuevo correo: " << RESET;
            getline(cin, correo);


            if (nombre.empty()) nombre = hospital.getNombre();
          
            
            hospital = Hospital(nombre.c_str(), direccion.c_str(), telefono.c_str(), correo.c_str());
            
            ofstream out(Rutas::HOSPITAL, ios::binary);
            out.write((char*)&hospital, sizeof(Hospital));
            out.close();
            
            Formatos::printExito("Datos actualizados correctamente.");
            Formatos::pausar();
        }
    } else {
     
        Formatos::imprimirEncabezado("CONFIGURACION INICIAL");
        Formatos::printCentrado("No se encontro archivo de datos. Configure el sistema:", ROJO);
        cout << endl;

        string nombre, direccion, telefono, correo;
        if (cin.peek() == '\n') cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        cout << "   Nombre del Hospital: ";
        getline(cin, nombre);
        cout << "   Direccion: ";
        getline(cin, direccion);
        cout << "   Telefono: ";
        getline(cin, telefono);
        cout << "   Correo: ";
        getline(cin, correo);

        if (nombre.empty()) nombre = "HOSPITAL GENERAL";
        if (direccion.empty()) direccion = "Av. Milagro";
        if (telefono.empty()) telefono = "0414-1234567";
        if (correo.empty()) correo = "info@hospital.com";

        hospital = Hospital(nombre.c_str(), direccion.c_str(), telefono.c_str(), correo.c_str());
        ofstream out(Rutas::HOSPITAL, ios::binary);
        out.write((char*)&hospital, sizeof(Hospital));
        out.close();
        
        Formatos::printExito("Configuracion guardada.");
        Formatos::pausar();
    }

    int opcion;
    do {
        // --- MENÚ PRINCIPAL BONITO
        Formatos::imprimirEncabezado(hospital.getNombre());
        
        Formatos::printCentrado("1. Gestion de Pacientes", BLANCO);
        Formatos::printCentrado("2. Gestion de Doctores ", BLANCO);
        Formatos::printCentrado("3. Gestion de Citas    ", BLANCO);
        Formatos::printCentrado("4. Realizar Consulta   ", BLANCO);
        Formatos::printCentrado("5. Ver Estadisticas    ", BLANCO);
            Formatos::printCentrado("6. Mantenimiento Archivos", CYAN);
            Formatos::printCentrado("7. Salir               ", ROJO);
        
        cout << endl;
        Formatos::imprimirLineaSeparadora();
        
        // Input alineado
        opcion = Validaciones::leerEntero("\n   >>> Seleccione una opcion: ", 1, 7);

        switch (opcion) {
            case 1: menuPacientes(hospital); break;
            case 2: menuDoctores(hospital); break;
            case 3: menuCitas(hospital); break;
            case 4: realizarConsulta(hospital); break;
            case 5:
                hospital.mostrarEstadisticas();
                Formatos::pausar();
                break;
            case 6: menuMantenimiento(hospital); break;
            case 7: {
                ofstream out(Rutas::HOSPITAL, ios::binary);
                out.write((char*)&hospital, sizeof(Hospital));
                Formatos::printCentrado("Guardando y saliendo...", AMARILLO);
            } break;
            default:
                Formatos::printError("Opcion invalida.");
                break;
        }
    } while (opcion != 7);

    return 0;
}
void menuMantenimiento(Hospital& h) { Mantenimiento::menu(h); }