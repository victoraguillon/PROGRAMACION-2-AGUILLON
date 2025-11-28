#ifndef  DOCTOR_HPP  
#define  DOCTOR_HPP  
class Doctor {  
private :     
    int  id;
    char  nombre[ 50 ];     
    char  apellido[ 50 ];     
    char  cedula[ 20 ];         
    int  aniosExperiencia;       
    char  especialidad[ 50 ];     
    float  costoConsulta;
    bool  disponible;
    bool  eliminado;
    int  pacientesAsignadosIDs[ 50 ];
    int  cantidadPacientes; 
    int  citasIDs[ 30 ]; 
    int  cantidadCitas; 

public :    
    Doctor();
    void setId(int i) ; 
    void setDatos(const char* n, const char* a, const char* c, const char* esp, float costo, int anios); 
    void setDisponible(bool d) ;
    void setEliminado(bool e) ;

    // Getters
    int getId() const ;
    const char* getNombre() const ; 
    const char* getApellido() const ;
    const char* getCedula() const;           
    int getAniosExperiencia() const;         
    const char* getEspecialidad() const ; 
    float getCosto() const ;
    bool isEliminado() const ;
    
    bool agregarPaciente(int pacienteID) ;
    bool removerPaciente(int pacienteID) ;
    bool agregarCita(int citaID) ;
    int getCantidadPacientes() const ;
    int getPacienteID(int index) const ;


    void mostrarFila() const ;
    void mostrarDetalle() const ;
}; 
# endif