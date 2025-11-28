#include "Paciente.hpp"
#include <cstring>
#include <iostream>
#include <iomanip>

using namespace std;

Paciente::Paciente() {
    id = 0;
    nombre[0] = '\0';
    apellido[0] = '\0';
    cedula[0] = '\0';
    edad = 0;
    sexo = 'M';
    activo = true;
    eliminado = false;
    cantidadConsultas = 0;
    primerConsultaID = -1;
    cantidadCitas = 0;
    fechaRegistro = time(0);
}

void Paciente::setId(int i) { id = i; }
void Paciente::setDatosPersonales(const char* n, const char* a, const char* c, int e, char s) {
    strncpy(nombre, n, 50);
    strncpy(apellido, a, 50);
    strncpy(cedula, c, 20);
    edad = e;
    sexo = s;
}
void Paciente::setContacto(const char* t, const char* mail) {
    strncpy(telefono, t, 15);
    strncpy(email, mail, 50);
}
void Paciente::setEliminado(bool estado) { eliminado = estado; }

int Paciente::getId() const { return id; }
const char* Paciente::getNombre() const { return nombre; }
const char* Paciente::getApellido() const { return apellido; }
const char* Paciente::getCedula() const { return cedula; }
bool Paciente::isEliminado() const { return eliminado; }

bool Paciente::agregarCita(int citaID) {
    if (cantidadCitas >= 20) return false;
    citasIDs[cantidadCitas++] = citaID;
    return true;
}

void Paciente::setPrimerConsulta(int consultaID) { primerConsultaID = consultaID; }
int Paciente::getPrimerConsulta() const { return primerConsultaID; }
int Paciente::getCantidadConsultas() const { return cantidadConsultas; }
void Paciente::incrementarConsultas() { cantidadConsultas++; }
int Paciente::getCantidadCitas() const { return cantidadCitas; }
int Paciente::getCitaID(int index) const { 
    if(index >= 0 && index < cantidadCitas) return citasIDs[index];
    return -1;
}

bool Paciente::validarDatos() const {
    return (strlen(nombre) > 0 && strlen(cedula) > 0 && edad >= 0);
}

void Paciente::mostrarFila() const {
    cout << "| " << setw(3) << id << " | " 
         << setw(15) << nombre << " | "
         << setw(15) << apellido << " | "
         << setw(12) << cedula << " |" << endl;
}

void Paciente::mostrarDetalle() const {
    cout << "\n--- DATOS DEL PACIENTE ---\n";
    cout << "ID: " << id << "\nNombre: " << nombre << " " << apellido;
    cout << "\nCedula: " << cedula << "\nEdad: " << edad << "\nSexo: " << sexo;
    cout << "\nTelefono: " << telefono << "\nEmail: " << email << endl;
}
