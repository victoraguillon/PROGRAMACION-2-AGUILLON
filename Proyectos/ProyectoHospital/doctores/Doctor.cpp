#include  "Doctor.hpp"  
#include  <cstring>  
#include  <iostream>  
#include  <iomanip>  
using   namespace   std ;

Doctor::Doctor() {
    id =  0 ;
    nombre[ 0 ] =  '\0' ; 
    apellido[ 0 ] =  '\0' ; 
    cedula[ 0 ] =  '\0' ;           
    aniosExperiencia =  0 ;          
    especialidad[ 0 ] =  '\0' ;
    costoConsulta =  0 ; 
    disponible =  true ; 
    eliminado =  false ; 
    cantidadPacientes =  0 ; 
    cantidadCitas =  0 ;
} 

void Doctor::setId(int i)  { 
    id = i;
} 

void Doctor::setDatos(const char* n, const char* a, const char* c, const char* esp, float costo, int anios) {
    strncpy (nombre, n,  50 );
    strncpy (apellido, a,  50 );
    strncpy (cedula, c,  20 );        
    aniosExperiencia = anios;       
    strncpy (especialidad, esp,  50 ); 
    costoConsulta = costo;
} 

void Doctor::setDisponible(bool d)  {disponible = d;} 
void Doctor::setEliminado(bool e)  {eliminado = e;} 

int Doctor::getId() const  {return  id;} 
const char* Doctor::getNombre() const  {return  nombre;} 
const char* Doctor::getApellido() const  {return  apellido;}
const char* Doctor::getCedula() const {return cedula;}
int Doctor::getAniosExperiencia() const {return aniosExperiencia;}
const char* Doctor::getEspecialidad() const  {return  especialidad;} 
float Doctor::getCosto() const  {return  costoConsulta;}
bool Doctor::isEliminado() const  {return  eliminado; } 


bool Doctor::agregarPaciente(int idPac) {
    if (cantidadPacientes >= 50) return false;
    for(int i=0; i<cantidadPacientes; i++) if(pacientesAsignadosIDs[i] == idPac) return false;
    pacientesAsignadosIDs[cantidadPacientes++] = idPac;
    return true;
}

bool Doctor::removerPaciente(int idPac) {
    int pos = -1;
    for(int i=0; i<cantidadPacientes; i++) {
        if(pacientesAsignadosIDs[i] == idPac) { pos = i; break; }
    }
    if(pos == -1) return false;
    
    // Compactar array
    for(int i=pos; i<cantidadPacientes-1; i++) {
        pacientesAsignadosIDs[i] = pacientesAsignadosIDs[i+1];
    }
    cantidadPacientes--;
    return true;
}

bool Doctor::agregarCita(int idCita) {
    if (cantidadCitas >= 30) return false;
    citasIDs[cantidadCitas++] = idCita;
    return true;
}
int Doctor::getCantidadPacientes() const { return cantidadPacientes; }
int Doctor::getPacienteID(int index) const { return pacientesAsignadosIDs[index]; }

void Doctor::mostrarFila() const {
    cout << "| " << setw(3) << id << " | " 
         << setw(15) << nombre << " | "
         << setw(15) << especialidad << " | "
         << setw(8) << fixed << setprecision(2) << costoConsulta << " |" << endl;
}