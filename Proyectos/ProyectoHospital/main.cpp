
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

using namespace std;

// Declaracion de menus locales
void menuPacientes(Hospital& h);
void menuDoctores(Hospital& h);
void menuCitas(Hospital& h);

int main() {
    // 1. Inicializar sistema
    if (!GestorArchivos::inicializarSistemaArchivos()) {
        Formatos::printError("Error critico al inicializar sistema de archivos.");
        return 1;
    }

    // 2. Cargar datos del Hospital
    Hospital hospital;
    ifstream fileH(Rutas::HOSPITAL, ios::binary);
    
    if (fileH) {
        fileH.read((char*)&hospital, sizeof(Hospital));
        fileH.close();
        
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
        // --- MENÚ PRINCIPAL CON ESTÉTICA ---
        Formatos::imprimirEncabezado(hospital.getNombre());
        
        Formatos::printCentrado("1. Gestion de Pacientes", BLANCO);
        Formatos::printCentrado("2. Gestion de Doctores ", BLANCO);
        Formatos::printCentrado("3. Gestion de Citas    ", BLANCO);
        Formatos::printCentrado("4. Realizar Consulta   ", BLANCO);
        Formatos::printCentrado("5. Ver Estadisticas    ", BLANCO);
        Formatos::printCentrado("6. Salir               ", ROJO);
        
        cout << endl;
        Formatos::imprimirLineaSeparadora();
        
        // Input alineado
        opcion = Validaciones::leerEntero("\n   >>> Seleccione una opcion: ", 1, 6);

        switch (opcion) {
            case 1: menuPacientes(hospital); break;
            case 2: menuDoctores(hospital); break;
            case 3: menuCitas(hospital); break;
            case 4: realizarConsulta(hospital); break;
            case 5:
                hospital.mostrarEstadisticas();
                Formatos::pausar();
                break;
            case 6: {
                ofstream out(Rutas::HOSPITAL, ios::binary);
                out.write((char*)&hospital, sizeof(Hospital));
                Formatos::printCentrado("Guardando y saliendo...", AMARILLO);
            } break;
            default:
                Formatos::printError("Opcion invalida.");
                break;
        }
    } while (opcion != 6);

    return 0;
}

void menuPacientes(Hospital& h) {
    int op;
    do {
        Formatos::imprimirEncabezado("GESTION DE PACIENTES");
        
        Formatos::printCentrado("1. Registrar Paciente   ", BLANCO);
        Formatos::printCentrado("2. Listar Todos         ", BLANCO);
        Formatos::printCentrado("3. Buscar por ID        ", BLANCO);
        Formatos::printCentrado("4. Buscar por Cedula    ", BLANCO);
        Formatos::printCentrado("5. Ver Historial Medico ", BLANCO);
        Formatos::printCentrado("6. Eliminar Paciente    ", BLANCO);
        Formatos::printCentrado("7. Modificar Datos      ", CYAN); // Opción nueva destacada
        Formatos::printCentrado("8. Volver               ", AMARILLO);
        
        cout << endl;
        Formatos::imprimirLineaSeparadora();

        op = Validaciones::leerEntero("\n   >>> Opcion: ", 1, 8);
        
        switch (op) {
            case 1: registrarPaciente(h); break;
            case 2: listarPacientes(); break;
            case 3: buscarPacientePorID(); break;
            case 4: buscarPacientePorCedula(); break;
            case 5: verHistorialPaciente(); break;
            case 6: eliminarPaciente(h); break;
            case 7: modificarPaciente(); break;
            case 8: break; // volver
            default: Formatos::printError("Opcion invalida."); break;
        }
    } while(op != 8);
}

void menuDoctores(Hospital& h) {
    int op;
    do {
        Formatos::imprimirEncabezado("GESTION DE DOCTORES");
        
        Formatos::printCentrado("1. Registrar Doctor       ", BLANCO);
        Formatos::printCentrado("2. Listar Doctores        ", BLANCO);
        Formatos::printCentrado("3. Buscar por ID          ", BLANCO);
        Formatos::printCentrado("4. Asignar Paciente       ", BLANCO);
        Formatos::printCentrado("5. Ver Pacientes Asignados", BLANCO); 
        Formatos::printCentrado("6. Modificar Doctor       ", CYAN);
        Formatos::printCentrado("7. Volver                 ", AMARILLO);
        
        cout << endl;
        Formatos::imprimirLineaSeparadora();
        
        op = Validaciones::leerEntero("\n   >>> Opcion: ", 1, 7);
        
        switch(op) {
            case 1: registrarDoctor(h); break;
            case 2: listarDoctores(); break;
            case 3: buscarDoctorPorID(); break;
            case 4: asignarPacienteDoctor(); break;
            case 5: verPacientesDeDoctor(); break;
            case 6: modificarDoctor(); break;
            case 7: break;
            default: Formatos::printError("Opcion invalida."); break;
        }
    } while(op != 7);
}

void menuCitas(Hospital& h) {
    int op;
    do {
        Formatos::imprimirEncabezado("GESTION DE CITAS");
        
        Formatos::printCentrado("1. Agendar Cita            ", BLANCO);
        Formatos::printCentrado("2. Listar Citas            ", BLANCO);
        Formatos::printCentrado("3. Modificar/Cancelar Cita ", CYAN);
        Formatos::printCentrado("4. Volver                  ", AMARILLO);
        
        cout << endl;
        Formatos::imprimirLineaSeparadora();
        
        op = Validaciones::leerEntero("\n   >>> Opcion: ", 1, 4);
        
        switch(op) {
            case 1: agendarCita(h); break;
            case 2: listarCitas(); break;
            case 3: modificarCita(); break;
            case 4: break;
            default: Formatos::printError("Opcion invalida."); break;
        }
    } while(op != 4);
}