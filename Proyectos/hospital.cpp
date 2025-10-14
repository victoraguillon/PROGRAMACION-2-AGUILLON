#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>

using namespace std;

// ESTRUCTURA 1: Historial Médico

struct HistorialMedico {
    int idConsulta;
    char fecha[11];         // Formato fijo: "YYYY-MM-DD" (10 + '\0')
    char hora[6];           // Formato fijo: "HH:MM" (5 + '\0')
    char diagnostico[200];
    char tratamiento[200];
    char medicamentos[150];
    int idDoctor;
    float costo;
};


// ESTRUCTURA 2: Paciente

struct Paciente {
    int id;
    char nombre[50];
    char apellido[50];
    char cedula[20];        // Debe ser única en el sistema
    int edad;
    char sexo;              // Solo 'M' o 'F'
    char tipoSangre[5];     // Ejemplo: "O+", "AB-"
    char telefono[15];
    char direccion[100];
    char email[50];
    HistorialMedico* historial;
    int cantidadConsultas;
    int capacidadHistorial;
    int* citasAgendadas;
    int cantidadCitas;
    int capacidadCitas;
    char alergias[500];     // Formato: "Alergia1, Alergia2, ..."
    char observaciones[500];
    bool activo;
};


// ESTRUCTURA 3: Doctor

struct Doctor {
    int id;
    char nombre[50];
    char apellido[50];
    char cedula[20];        // Debe ser única en el sistema
    char especialidad[50];
    int aniosExperiencia;   // Validar que sea >= 0
    float costoConsulta;    // Validar que sea > 0
    char horarioAtencion[50]; // Ejemplo: "Lun-Vie 8:00-16:00"
    char telefono[15];
    char email[50];

    int* pacientesAsignados;
    int cantidadPacientes;
    int capacidadPacientes;

    int* citasAgendadas;
    int cantidadCitas;
    int capacidadCitas;

    bool disponible;
};

// ESTRUCTURA 1.5: Cita

struct Cita {
    int id;
    int idPaciente;
    int idDoctor;
    char fecha[11];         // Formato fijo: "YYYY-MM-DD"
    char hora[6];           // Formato fijo: "HH:MM"
    char motivo[150];
    char estado[20];        // Valores válidos: "Agendada", "Atendida", "Cancelada"
    char observaciones[200];
    bool atendida;          // true solo si la cita fue atendida
};


struct Hospital {
    char nombre[100];
    char direccion[150];
    char telefono[15];

    Paciente* pacientes;
    int cantidadPacientes;
    int capacidadPacientes;

    Doctor* doctores;
    int cantidadDoctores;
    int capacidadDoctores;

    Cita* citas;
    int cantidadCitas;
    int capacidadCitas;

    // Los siguientes contadores permiten generar IDs únicos
    int siguienteIdPaciente;   
    int siguienteIdDoctor;     
    int siguienteIdCita;       
    int siguienteIdConsulta;   
};
