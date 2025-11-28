#include "operacionesDoctores.hpp"
#include "Doctor.hpp"
#include "../pacientes/Paciente.hpp"
#include "../persistencia/GestorArchivos.hpp"
#include "../utilidades/Validaciones.hpp"
#include "../utilidades/Formatos.hpp"
#include <iostream>

using namespace std;

# include  "operacionesDoctores.hpp"  
# include  "Doctor.hpp"  
# include  "../pacientes/Paciente.hpp"  
# include  "../persistencia/GestorArchivos.hpp"  
# include  "../utilidades/Validaciones.hpp"  
# include  "../utilidades/Formatos.hpp"  
# include  <iostream>  
using   namespace   std ;

void registrarDoctor(Hospital& h)  { 
    char  nombre[ 50 ], apellido[ 50 ], especialidad[ 50 ]; 
    char  cedula[ 20 ];                  
    float  costo; 
    int  aniosExperiencia;             
    
    Formatos::imprimirEncabezado( "REGISTRAR DOCTOR" );
    Validaciones::leerCadena( "Nombre: " , nombre,  50 ); 
    Validaciones::leerCadena( "Apellido: " , apellido,  50 );
    Validaciones::leerCadena( "Cedula: " , cedula,  20 );
    Validaciones::leerCadena( "Especialidad: " , especialidad,  50 ); 
    costo = Validaciones::leerFloat( "Costo Consulta: " ,  0 ,  10000 );
    aniosExperiencia = Validaciones::leerEntero( "Anios de Experiencia: " ,  0 ,  80 );
    ArchivoHeader header = GestorArchivos::leerHeader(Rutas::DOCTORES); 
    Doctor d; 
    d.setId(header.proximoID); 
    d.setDatos(nombre, apellido, cedula, especialidad, costo, aniosExperiencia);
    if  (GestorArchivos::guardarRegistro(&d,  sizeof (Doctor), Rutas::DOCTORES)) { 
        cout  <<  "Doctor registrado con ID: "  << d.getId() <<  endl ; 
        h.incrementarDoctores();
    } else {
        cout << "\nError al guardar en archivo.\n";
    }

    Formatos::pausar();
}

void listarDoctores() {
    Formatos::imprimirEncabezado("LISTA DE DOCTORES");
    ArchivoHeader h = GestorArchivos::leerHeader(Rutas::DOCTORES);
    Doctor d;
    cout << "+-----+-----------------+-----------------+----------+" << endl;
    cout << "| ID  | Nombre          | Especialidad    | Costo    |" << endl;
    cout << "+-----+-----------------+-----------------+----------+" << endl;
    for(int i=0; i<h.cantidadRegistros; i++) {
        GestorArchivos::leerRegistroPorPosicion(i, &d, sizeof(Doctor), Rutas::DOCTORES);
        if(!d.isEliminado()) d.mostrarFila();
    }
    cout << "+-----+-----------------+-----------------+----------+" << endl;
    Formatos::pausar();
}

void buscarDoctorPorID() {
    int id = Validaciones::leerEntero("ID Doctor: ", 1, 99999);
    Doctor d;
    if(GestorArchivos::leerRegistroPorID(id, &d, sizeof(Doctor), Rutas::DOCTORES) && !d.isEliminado()) {
        cout << "Dr. " << d.getNombre() << " " << d.getApellido() << " - " << d.getEspecialidad() << endl;
    } else {
        cout << "Doctor no encontrado.\n";
    }
    Formatos::pausar();
}

void asignarPacienteDoctor() {
    int idDoc = Validaciones::leerEntero("ID Doctor: ", 1, 99999);
    int idPac = Validaciones::leerEntero("ID Paciente: ", 1, 99999);
    
    Doctor d;
    Paciente p;
    
    bool existeDoc = GestorArchivos::leerRegistroPorID(idDoc, &d, sizeof(Doctor), Rutas::DOCTORES);
    bool existePac = GestorArchivos::leerRegistroPorID(idPac, &p, sizeof(Paciente), Rutas::PACIENTES);
    
    if(existeDoc && existePac && !d.isEliminado() && !p.isEliminado()) {
        if(d.agregarPaciente(idPac)) {
            GestorArchivos::actualizarRegistro(idDoc, &d, sizeof(Doctor), Rutas::DOCTORES);
            cout << "Asignacion exitosa.\n";
        } else {
            cout << "No se pudo asignar (Ya asignado o cupo lleno).\n";
        }
    } else {
        cout << "Doctor o Paciente no validos.\n";
    }
    Formatos::pausar();
}

void verPacientesDeDoctor() {
    int idDoc = Validaciones::leerEntero("ID Doctor: ", 1, 99999);
    Doctor d;
    if(GestorArchivos::leerRegistroPorID(idDoc, &d, sizeof(Doctor), Rutas::DOCTORES)) {
        cout << "Pacientes asignados al Dr. " << d.getNombre() << ": " << d.getCantidadPacientes() << endl;
        for(int i=0; i<d.getCantidadPacientes(); i++) {
            Paciente p;
            if(GestorArchivos::leerRegistroPorID(d.getPacienteID(i), &p, sizeof(Paciente), Rutas::PACIENTES)) {
                cout << "- " << p.getNombre() << " " << p.getApellido() << endl;
            }
        }
    }
    Formatos::pausar();
}
void modificarDoctor() {
    Formatos::imprimirEncabezado("MODIFICAR DATOS DE DOCTOR");
    int id = Validaciones::leerEntero("Ingrese ID del Doctor a modificar: ", 1, 99999);
    
    Doctor d;
    if (GestorArchivos::leerRegistroPorID(id, &d, sizeof(Doctor), Rutas::DOCTORES) && !d.isEliminado()) {
        d.mostrarFila();
        
        if (Validaciones::confirmarAccion("\n¿Desea sobrescribir TODOS los datos de este doctor?")) {
            char nombre[50], apellido[50], especialidad[50];
            float costo;

            cout << "\n--- Ingrese los NUEVOS datos ---\n";

            Validaciones::leerCadena("Nuevo Nombre: ", nombre, 50);
            Validaciones::leerCadena("Nuevo Apellido: ", apellido, 50);
            Validaciones::leerCadena("Nueva Especialidad: ", especialidad, 50);
            costo = Validaciones::leerFloat("Nuevo Costo Consulta: ", 0, 10000);

            // Actualizamos objeto
            // Mantener la cedula y años si no se solicitó cambio
            d.setDatos(nombre, apellido, d.getCedula(), especialidad, costo, d.getAniosExperiencia());

            // Guardamos
            if (GestorArchivos::actualizarRegistro(id, &d, sizeof(Doctor), Rutas::DOCTORES)) {
                cout << "\nDoctor actualizado exitosamente.\n";
            } else {
                cout << "\nError al guardar cambios.\n";
            }
        }
    } else {
        cout << "Doctor no encontrado.\n";
    }
    Formatos::pausar();
}
