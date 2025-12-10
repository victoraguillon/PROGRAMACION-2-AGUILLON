#ifndef OPS_DOCTORES_HPP
#define OPS_DOCTORES_HPP

#include "../hospital/Hospital.hpp"

void registrarDoctor(Hospital& h);
void listarDoctores();
void buscarDoctorPorID();
void asignarPacienteDoctor();
void verPacientesDeDoctor();
void modificarDoctor();

// Menú de doctores (interfaz)
void menuDoctores(Hospital& h);

#endif