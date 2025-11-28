#include "HistorialMedico.hpp"
#include <iostream>
#include <cstring>

using namespace std;

HistorialMedico::HistorialMedico() {
    id=0; idPaciente=0; idDoctor=0; siguienteConsultaID=-1; eliminado=false;
    fecha[0]=0; diagnostico[0]=0; tratamiento[0]=0;
}

void HistorialMedico::setDatos(int i, int idPac, int idDoc, const char* f) {
    id=i; idPaciente=idPac; idDoctor=idDoc;
    strncpy(fecha, f, 11);
}

void HistorialMedico::setInfoMedica(const char* diag, const char* trat) {
    strncpy(diagnostico, diag, 200);
    strncpy(tratamiento, trat, 200);
}

void HistorialMedico::setSiguienteID(int idSig) { siguienteConsultaID = idSig; }

int HistorialMedico::getId() const { return id; }
int HistorialMedico::getSiguienteID() const { return siguienteConsultaID; }
int HistorialMedico::getPacienteID() const { return idPaciente; }

void HistorialMedico::mostrarDetalle() const {
    cout << "  Consulta ID: " << id << " | Fecha: " << fecha << endl;
    cout << "  Diagnostico: " << diagnostico << endl;
    cout << "  Tratamiento: " << tratamiento << endl;
    cout << "  -------------------------------" << endl;
}