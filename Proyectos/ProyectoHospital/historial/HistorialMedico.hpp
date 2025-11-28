#ifndef HISTORIAL_MEDICO_HPP
#define HISTORIAL_MEDICO_HPP

class HistorialMedico {
private:
    int id;
    int idPaciente;
    int idDoctor;
    char fecha[11];
    char diagnostico[200];
    char tratamiento[200];
    int siguienteConsultaID; 
    bool eliminado;

public:
    HistorialMedico();
    void setDatos(int id, int idPac, int idDoc, const char* f);
    void setInfoMedica(const char* diag, const char* trat);
    void setSiguienteID(int idSig);
    
    int getId() const;
    int getSiguienteID() const;
    int getPacienteID() const;
    
    void mostrarDetalle() const;
};

#endif