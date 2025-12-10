#include "operacionesPacientes.hpp"
#include "Paciente.hpp"
#include "../persistencia/GestorArchivos.hpp"
#include "../utilidades/Validaciones.hpp"
#include "../utilidades/Formatos.hpp"
#include "../historial/operacionesHistorial.hpp"
#include <iostream>
#include <cstring>
using namespace std;

void registrarPaciente(Hospital& h) {
    char nombre[50], apellido[50], cedula[20], telefono[15], email[50];
    int edad;
    char sexo;

    Formatos::imprimirEncabezado("REGISTRAR NUEVO PACIENTE");
    Validaciones::leerCadena("Nombre: ", nombre, 50);
    Validaciones::leerCadena("Apellido: ", apellido, 50);
    Validaciones::leerCadena("Cedula: ", cedula, 20);
  
    
    edad = Validaciones::leerEntero("Edad: ", 0, 120);
    sexo = Validaciones::leerSexo("Sexo (M/F): ");
    Validaciones::leerCadena("Telefono: ", telefono, 15);
    Validaciones::leerCadena("Email (opcional): ", email, 50, true);

    // Crear objeto
    ArchivoHeader header = GestorArchivos::leerHeader(Rutas::PACIENTES);
    Paciente p;
    p.setId(header.proximoID);
    p.setDatosPersonales(nombre, apellido, cedula, edad, sexo);
    p.setContacto(telefono, email);

    // Guardar
    if (GestorArchivos::guardarRegistro(&p, sizeof(Paciente), Rutas::PACIENTES)) {
        cout << "\nPaciente registrado con ID: " << p.getId() << endl;
        h.incrementarPacientes();
    } else {
        cout << "\nError al guardar en archivo.\n";
    }
    Formatos::pausar();
}

void listarPacientes() {
    Formatos::imprimirEncabezado("LISTADO DE PACIENTES");
    
    ArchivoHeader h = GestorArchivos::leerHeader(Rutas::PACIENTES);
    if (h.registrosActivos == 0) {
        cout << "No hay pacientes registrados.\n";
        Formatos::pausar();
        return;
    }

    Paciente p;
    cout << "+-----+-----------------+-----------------+--------------+" << endl;
    cout << "| ID  | Nombre          | Apellido        | Cedula       |" << endl;
    cout << "+-----+-----------------+-----------------+--------------+" << endl;

    for (int i = 0; i < h.cantidadRegistros; i++) {
        GestorArchivos::leerRegistroPorPosicion(i, &p, sizeof(Paciente), Rutas::PACIENTES);
        if (!p.isEliminado()) {
            p.mostrarFila();
        }
    }
    cout << "+-----+-----------------+-----------------+--------------+" << endl;
    Formatos::pausar();
}

void buscarPacientePorCedula() {
    char cedula[20];
    Validaciones::leerCadena("Ingrese Cedula a buscar: ", cedula, 20);
    
    ArchivoHeader h = GestorArchivos::leerHeader(Rutas::PACIENTES);
    Paciente p;
    bool encontrado = false;
    
    for (int i = 0; i < h.cantidadRegistros; i++) {
        GestorArchivos::leerRegistroPorPosicion(i, &p, sizeof(Paciente), Rutas::PACIENTES);
        if (!p.isEliminado() && strcmp(p.getCedula(), cedula) == 0) {
            p.mostrarDetalle();
            encontrado = true;
            break;
        }
    }
    
    if (!encontrado) cout << "Paciente no encontrado.\n";
    Formatos::pausar();
}

void buscarPacientePorID() {
    int id = Validaciones::leerEntero("Ingrese ID: ", 1, 999999);
    Paciente p;
    if (GestorArchivos::leerRegistroPorID(id, &p, sizeof(Paciente), Rutas::PACIENTES) && !p.isEliminado()) {
        p.mostrarDetalle();
    } else {
        cout << "Paciente no encontrado o eliminado.\n";
    }
    Formatos::pausar();
}

void eliminarPaciente(Hospital& h) {
    int id = Validaciones::leerEntero("ID del paciente a eliminar: ", 1, 999999);
    Paciente p;
    
    if (GestorArchivos::leerRegistroPorID(id, &p, sizeof(Paciente), Rutas::PACIENTES) && !p.isEliminado()) {
        p.mostrarDetalle();
        if (Validaciones::confirmarAccion("Esta seguro de eliminar este paciente?")) {
            p.setEliminado(true);
            GestorArchivos::actualizarRegistro(id, &p, sizeof(Paciente), Rutas::PACIENTES);
            GestorArchivos::eliminarRegistroLogico(id, sizeof(Paciente), Rutas::PACIENTES);
            h.decrementarPacientes();
            cout << "Paciente eliminado.\n";
        }
    } else {
        cout << "Paciente no encontrado.\n";
    }
    Formatos::pausar();
}

void modificarPaciente() {
    Formatos::imprimirEncabezado("MODIFICAR DATOS DE PACIENTE");
    int id = Validaciones::leerEntero("Ingrese ID del paciente a modificar: ", 1, 999999);
    
    Paciente p;
    // Buscamos el paciente
    if (GestorArchivos::leerRegistroPorID(id, &p, sizeof(Paciente), Rutas::PACIENTES) && !p.isEliminado()) {
        p.mostrarDetalle();
        
        if (Validaciones::confirmarAccion("\n¿Desea sobrescribir TODOS los datos de este paciente?")) {
            char nombre[50], apellido[50], cedula[20], telefono[15], email[50];
            int edad;
            char sexo;

            cout << "\n--- Ingrese los NUEVOS datos ---\n";
            
            // 1. Datos Personales
            Validaciones::leerCadena("Nuevo Nombre: ", nombre, 50);
            Validaciones::leerCadena("Nuevo Apellido: ", apellido, 50);
            Validaciones::leerCadena("Nueva Cedula: ", cedula, 20);
            edad = Validaciones::leerEntero("Nueva Edad: ", 0, 120);
            sexo = Validaciones::leerSexo("Nuevo Sexo (M/F): ");
            
            // 2. Datos de Contacto
            Validaciones::leerCadena("Nuevo Telefono: ", telefono, 15);
            Validaciones::leerCadena("Nuevo Email (opcional): ", email, 50, true);
            
            p.setDatosPersonales(nombre, apellido, cedula, edad, sexo);
            p.setContacto(telefono, email);

            // Guardamos en el archivo binario
            if (GestorArchivos::actualizarRegistro(id, &p, sizeof(Paciente), Rutas::PACIENTES)) {
                cout << "\nPaciente actualizado exitosamente.\n";
            } else {
                cout << "\nError crítico al escribir en el archivo.\n";
            }
        }
    } else {
        cout << "Paciente no encontrado o eliminado.\n";
    }
    Formatos::pausar();
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
        Formatos::printCentrado("7. Modificar Datos      ", CYAN);
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

