#include "Hospital.hpp"
#include <cstring>
#include <iostream>

using namespace std;

Hospital::Hospital() {
    nombre[0] = '\0';
    totalPacientes = 0;
    totalDoctores = 0;
    totalCitas = 0;
    totalConsultas = 0;
}

Hospital::Hospital(const char* nom, const char* dir, const char* tel, const char* mail) {
    strncpy(nombre, nom, 100);
    strncpy(direccion, dir, 150);
    strncpy(telefono, tel, 15);
    strncpy(email, mail, 50);
    totalPacientes = 0;
    totalDoctores = 0;
    totalCitas = 0;
    totalConsultas = 0;
}

const char* Hospital::getNombre() const { return nombre; }
const char* Hospital::getDireccion() const { return direccion; }
const char* Hospital::getTelefono() const { return telefono; }
const char* Hospital::getEmail() const { return email; }

void Hospital::incrementarPacientes() { totalPacientes++; }
void Hospital::decrementarPacientes() { totalPacientes--; }
void Hospital::incrementarDoctores() { totalDoctores++; }
void Hospital::decrementarDoctores() { totalDoctores--; }
void Hospital::incrementarCitas() { totalCitas++; }
void Hospital::decrementarCitas() { totalCitas--; }
void Hospital::incrementarConsultas() { totalConsultas++; }

void Hospital::mostrarEstadisticas() const {
    cout << "\n=== ESTADISTICAS DEL HOSPITAL ===\n";
    cout << "Pacientes registrados: " << totalPacientes << endl;
    cout << "Doctores registrados:  " << totalDoctores << endl;
    cout << "Citas agendadas:       " << totalCitas << endl;
    cout << "Consultas realizadas:  " << totalConsultas << endl;
}