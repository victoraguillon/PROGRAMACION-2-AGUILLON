#ifndef HOSPITAL_HPP
#define HOSPITAL_HPP

class Hospital {
private:
    char nombre[100];
    char direccion[150];
    char telefono[15];
    char email[50];

    int totalPacientes;
    int totalDoctores;
    int totalCitas;
    int totalConsultas;

public:
    Hospital();
    Hospital(const char* nom, const char* dir, const char* tel, const char* mail);
    
    // Getters
    const char* getNombre() const;
    const char* getDireccion() const;
    const char* getTelefono() const;
    const char* getEmail() const;
    

    void incrementarPacientes();
    void decrementarPacientes();
    void incrementarDoctores();
    void decrementarDoctores();
    void incrementarCitas();
    void decrementarCitas();
    void incrementarConsultas();
    
    void mostrarEstadisticas() const;
};

#endif