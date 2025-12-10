#include "Cita.hpp"
#include <cstring>
#include <iostream>
#include <iomanip>

using namespace std;

Cita::Cita() {
    id=0; idPaciente=0; idDoctor=0;
    fecha[0]=0; hora[0]=0;
    strcpy(estado, "Agendada");
    eliminado = false;
}

void Cita::setDatos(int i, int idPac, int idDoc, const char* f, const char* h) {
    id = i; idPaciente = idPac; idDoctor = idDoc;
    strncpy(fecha, f, 11);
    strncpy(hora, h, 6);
}

void Cita::setEstado(const char* e) { strncpy(estado, e, 20); }

void Cita::setId(int i) { id = i; }

int Cita::getId() const { return id; }
int Cita::getPacienteID() const { return idPaciente; }
int Cita::getDoctorID() const { return idDoctor; }
const char* Cita::getFecha() const { return fecha; }
const char* Cita::getHora() const { return hora; }
const char* Cita::getEstado() const { return estado; }
bool Cita::isEliminado() const { return eliminado; }
bool Cita::isAtendida() const { return strcmp(estado, "Atendida") == 0; }

void Cita::mostrarFila() const {
    cout << "| " << setw(3) << id << " | "
         << setw(8) << idPaciente << " | "
         << setw(8) << idDoctor << " | "
         << setw(11) << fecha << " | "
         << setw(10) << estado << " |" << endl;
}

void Cita::mostrarDetalle() const {
    cout << "ID: " << id << endl;
    cout << "Paciente ID: " << idPaciente << endl;
    cout << "Fecha: " << fecha << "\nHora: " << hora << endl;
    cout << "Estado: " << estado << endl;
}