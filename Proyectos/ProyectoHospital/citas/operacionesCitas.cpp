#include "operacionesCitas.hpp"
#include "Cita.hpp"
#include "../pacientes/Paciente.hpp"
#include "../doctores/Doctor.hpp"
#include "../persistencia/GestorArchivos.hpp"
#include "../utilidades/Validaciones.hpp"
#include "../utilidades/Formatos.hpp"
#include <iostream>

using namespace std;

void agendarCita(Hospital& h) {
    int idPac = Validaciones::leerEntero("ID Paciente: ", 1, 99999);
    int idDoc = Validaciones::leerEntero("ID Doctor: ", 1, 99999);
    
    Paciente p; Doctor d;
    
    if(!GestorArchivos::leerRegistroPorID(idPac, &p, sizeof(Paciente), Rutas::PACIENTES) || p.isEliminado()) {
        cout << "Paciente invalido.\n"; Formatos::pausar(); return;
    }
    if(!GestorArchivos::leerRegistroPorID(idDoc, &d, sizeof(Doctor), Rutas::DOCTORES) || d.isEliminado()) {
        cout << "Doctor invalido.\n"; Formatos::pausar(); return;
    }
    
    char fecha[11], hora[6];
    Validaciones::leerFecha("Fecha (YYYY-MM-DD): ", fecha);
    Validaciones::leerHora("Hora (HH:MM): ", hora);
    
    ArchivoHeader head = GestorArchivos::leerHeader(Rutas::CITAS);
    Cita c;
    c.setDatos(head.proximoID, idPac, idDoc, fecha, hora);
    
    if(GestorArchivos::guardarRegistro(&c, sizeof(Cita), Rutas::CITAS)) {
        p.agregarCita(c.getId());
        d.agregarCita(c.getId());
        GestorArchivos::actualizarRegistro(idPac, &p, sizeof(Paciente), Rutas::PACIENTES);
        GestorArchivos::actualizarRegistro(idDoc, &d, sizeof(Doctor), Rutas::DOCTORES);
        
        h.incrementarCitas();
        cout << "Cita agendada ID: " << c.getId() << endl;
    }
    Formatos::pausar();
}

void listarCitas() {
    Formatos::imprimirEncabezado("CITAS REGISTRADAS");
    ArchivoHeader h = GestorArchivos::leerHeader(Rutas::CITAS);
    Cita c;
    cout << "+-----+----------+----------+-------------+------------+" << endl;
    cout << "| ID  | ID Pac.  | ID Doc.  | Fecha       | Estado     |" << endl;
    cout << "+-----+----------+----------+-------------+------------+" << endl;
    for(int i=0; i<h.cantidadRegistros; i++) {
        GestorArchivos::leerRegistroPorPosicion(i, &c, sizeof(Cita), Rutas::CITAS);
        if(!c.isEliminado()) c.mostrarFila();
    }
    cout << "+-----+----------+----------+-------------+------------+" << endl;
    Formatos::pausar();
}
void modificarCita() {
    Formatos::imprimirEncabezado("MODIFICAR CITA COMPLETA");
    int idCita = Validaciones::leerEntero("Ingrese ID de la Cita a modificar: ", 1, 99999);
    
    Cita c;
    if (GestorArchivos::leerRegistroPorID(idCita, &c, sizeof(Cita), Rutas::CITAS) && !c.isEliminado()) {
        c.mostrarFila();
        
        if (Validaciones::confirmarAccion("\n¿Desea reasignar TODOS los datos (Pac/Doc/Fecha)?")) {
            int nuevoIdPac, nuevoIdDoc;
            char nuevaFecha[11], nuevaHora[6];
            
            cout << "\n--- Ingrese los NUEVOS datos ---\n";
            
            // 1. Validar nuevo paciente
            nuevoIdPac = Validaciones::leerEntero("Nuevo ID Paciente: ", 1, 99999);
            Paciente pTest;
            if (!GestorArchivos::leerRegistroPorID(nuevoIdPac, &pTest, sizeof(Paciente), Rutas::PACIENTES) || pTest.isEliminado()) {
                cout << "Error: El Paciente indicado no existe.\n";
                Formatos::pausar();
                return;
            }

            // 2. Validar nuevo doctor
            nuevoIdDoc = Validaciones::leerEntero("Nuevo ID Doctor: ", 1, 99999);
            Doctor dTest;
            if (!GestorArchivos::leerRegistroPorID(nuevoIdDoc, &dTest, sizeof(Doctor), Rutas::DOCTORES) || dTest.isEliminado()) {
                cout << "Error: El Doctor indicado no existe.\n";
                Formatos::pausar();
                return;
            }

            // 3. Fecha y Hora
            Validaciones::leerFecha("Nueva Fecha (YYYY-MM-DD): ", nuevaFecha);
            Validaciones::leerHora("Nueva Hora (HH:MM): ", nuevaHora);

            // Actualizamos la Cita
            c.setDatos(c.getId(), nuevoIdPac, nuevoIdDoc, nuevaFecha, nuevaHora);

            if (GestorArchivos::actualizarRegistro(idCita, &c, sizeof(Cita), Rutas::CITAS)) {
                cout << "\nCita actualizada exitosamente.\n";
                cout << "(Nota: Si cambio de Paciente/Doctor, los historiales podrian desvincularse en reportes antiguos).\n";
            } else {
                cout << "\nError al guardar cambios.\n";
            }
        }
    } else {
        cout << "Cita no encontrada.\n";
    }
    Formatos::pausar();
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
