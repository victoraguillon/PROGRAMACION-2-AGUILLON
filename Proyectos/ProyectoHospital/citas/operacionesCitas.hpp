#ifndef OPS_CITAS_HPP
#define OPS_CITAS_HPP

#include "../hospital/Hospital.hpp"

void agendarCita(Hospital& h);
void listarCitas();
void modificarCita();

// Menú de citas (interfaz)
void menuCitas(Hospital& h);

#endif