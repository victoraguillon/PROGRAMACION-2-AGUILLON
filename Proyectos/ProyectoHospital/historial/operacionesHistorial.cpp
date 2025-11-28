#include "operacionesHistorial.hpp"
#include "HistorialMedico.hpp"
#include "../citas/Cita.hpp"
#include "../pacientes/Paciente.hpp"
#include "../persistencia/GestorArchivos.hpp"
#include "../utilidades/Validaciones.hpp"
#include "../utilidades/Formatos.hpp"
#include <iostream>

using namespace std;

void realizarConsulta(Hospital& h) {
    int idCita = Validaciones::leerEntero("ID Cita a atender: ", 1, 99999);
    Cita c;
    
    if(!GestorArchivos::leerRegistroPorID(idCita, &c, sizeof(Cita), Rutas::CITAS) || c.isEliminado()) {
        cout << "Cita no encontrada.\n"; return;
    }
    
    if(c.isAtendida()) { cout << "Cita ya atendida.\n"; return; }
    
    char diag[200], trat[200];
    Validaciones::leerCadena("Diagnostico: ", diag, 200);
    Validaciones::leerCadena("Tratamiento: ", trat, 200);
    
    ArchivoHeader hh = GestorArchivos::leerHeader(Rutas::HISTORIAL);
    HistorialMedico hm;
    hm.setDatos(hh.proximoID, c.getPacienteID(), c.getDoctorID(), c.getFecha());
    hm.setInfoMedica(diag, trat);

    GestorArchivos::guardarRegistro(&hm, sizeof(HistorialMedico), Rutas::HISTORIAL);

    Paciente p;
    GestorArchivos::leerRegistroPorID(c.getPacienteID(), &p, sizeof(Paciente), Rutas::PACIENTES);
    
    if(p.getPrimerConsulta() == -1) {
        p.setPrimerConsulta(hm.getId());
    } else {
 
        int idActual = p.getPrimerConsulta();
        HistorialMedico aux;
        while(true) {
            GestorArchivos::leerRegistroPorID(idActual, &aux, sizeof(HistorialMedico), Rutas::HISTORIAL);
            if(aux.getSiguienteID() == -1) {
                aux.setSiguienteID(hm.getId());
                GestorArchivos::actualizarRegistro(aux.getId(), &aux, sizeof(HistorialMedico), Rutas::HISTORIAL);
                break;
            }
            idActual = aux.getSiguienteID();
        }
    }
    p.incrementarConsultas();
    GestorArchivos::actualizarRegistro(p.getId(), &p, sizeof(Paciente), Rutas::PACIENTES);
  
    c.setEstado("Atendida");
    GestorArchivos::actualizarRegistro(c.getId(), &c, sizeof(Cita), Rutas::CITAS);
    
    h.incrementarConsultas();
    cout << "Consulta registrada con exito.\n";
    Formatos::pausar();
}

void verHistorialPaciente() {
    int idPac = Validaciones::leerEntero("ID Paciente: ", 1, 99999);
    Paciente p;
    if(!GestorArchivos::leerRegistroPorID(idPac, &p, sizeof(Paciente), Rutas::PACIENTES)) {
        cout << "Paciente no existe.\n"; return;
    }
    
    Formatos::imprimirEncabezado("HISTORIAL MEDICO");
    cout << "Paciente: " << p.getNombre() << " " << p.getApellido() << endl;
    
    int idActual = p.getPrimerConsulta();
    if(idActual == -1) {
        cout << "Sin historial previo.\n";
    }
    
    HistorialMedico hm;
    while(idActual != -1) {
        if(GestorArchivos::leerRegistroPorID(idActual, &hm, sizeof(HistorialMedico), Rutas::HISTORIAL)) {
            hm.mostrarDetalle();
            idActual = hm.getSiguienteID();
        } else {
            break; 
        }
    }
    Formatos::pausar();
}