#ifndef OPS_PACIENTES_HPP
#define OPS_PACIENTES_HPP

#include "../hospital/Hospital.hpp"

void registrarPaciente(Hospital& h);
void listarPacientes();
void buscarPacientePorCedula();
void buscarPacientePorID();
void eliminarPaciente(Hospital& h);
void modificarPaciente();


#endif