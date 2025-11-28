#ifndef CITA_HPP
#define CITA_HPP

#include <cstddef>

class Cita {
private:
    int id;
    int idPaciente;
    int idDoctor;
    char fecha[11];
    char hora[6];
    char estado[20];
    bool eliminado;

public:
    Cita();
    void setId(int i);
    void setDatos(int i, int idPac, int idDoc, const char* f, const char* h);
    void setEstado(const char* e);

    int getId() const;
    int getPacienteID() const;
    int getDoctorID() const;
    const char* getFecha() const;
    const char* getHora() const;
    const char* getEstado() const;
    bool isEliminado() const;
    bool isAtendida() const;

    void mostrarFila() const;
    void mostrarDetalle() const;
};

#endif 