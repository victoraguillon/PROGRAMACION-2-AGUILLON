#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <locale>

using namespace std;

// ESTRUCTURA 1: Historial Médico

struct HistorialMedico {
    int idConsulta;
    char fecha[11];         // Formato fijo: "YYYY-MM-DD"
    char hora[6];           // Formato fijo: "HH:MM"
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

Paciente* crearPaciente (Hospital* hospital, const char* nombre, const char* apellido, 
const char* cedula, int edad, char sexo){


for (int i = 0; i < hospital->cantidadPacientes; i++) {
    if (strcmp (hospital-> pacientes[i].cedula, cedula) == 0) {
        return nullptr;
    }
}

if (hospital->cantidadPacientes >= hospital->capacidadPacientes){
    int nuevaCapacidad = hospital ->cantidadPacientes * 2;
    Paciente* nuevoArray = new Paciente [nuevaCapacidad];
    for (int i = 0; i < hospital ->cantidadPacientes; i++) {
    nuevoArray [i] = hospital ->pacientes[i];
    }
    delete[] hospital->pacientes;
    hospital->pacientes = nuevoArray;
    hospital->capacidadPacientes = nuevaCapacidad;
}

Paciente& nuevo = hospital -> pacientes[hospital->cantidadPacientes];
    nuevo.id = hospital->siguienteIdPaciente++;
    strncpy (nuevo.nombre, nombre, 50);
    strncpy (nuevo.apellido, apellido, 50);
    strncpy (nuevo.cedula, cedula, 20);
    nuevo.edad = edad;
    nuevo.sexo = sexo;
    nuevo.activo = true;
    nuevo.historial = new HistorialMedico[5];
    nuevo.cantidadConsultas = 0;
    nuevo.capacidadHistorial = 5;
    nuevo.citasAgendadas = new int[5];
    nuevo.cantidadCitas = 0;
    nuevo.capacidadCitas = 5;
    nuevo.alergias[0] = '\0';
    nuevo.observaciones[0] = '\0';
    hospital->cantidadPacientes++;
return &nuevo;
}

Paciente* buscarPacienteporCedula(Hospital* hospital, const char* cedula){
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if  (strcmp(hospital->pacientes[i].cedula, cedula) == 0) {
            return &hospital->pacientes[i];
        }
    }
return nullptr;
}

Paciente* buscarPacienteporId(Hospital* hospital, int id){
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if  (hospital->pacientes[i].id == id) {
            return &hospital->pacientes[i];
        }
    }
return nullptr;
}


Paciente** buscarPacientesPorNombre(Hospital* hospital, const char* nombre, int* cantidad) {
    int contador = 0;
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (strstr(hospital->pacientes[i].nombre, nombre)) {
            contador++;
        }
    }

    if (contador == 0) {
        *cantidad = 0;
        return nullptr;
    }

    Paciente** resultados = new Paciente*[contador];
    int index = 0;
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (strstr(hospital->pacientes[i].nombre, nombre)) {
            resultados[index++] = &hospital->pacientes[i];
        }
    }

    *cantidad = contador;
    return resultados;
}

bool actualizarPaciente(Hospital* hospital, int id) {
    Paciente* paciente = buscarPacienteporId(hospital, id);
    if (!paciente) return false;


    return true;
}

bool eliminarPaciente(Hospital* hospital, int id) {
    int indice = -1;
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (hospital->pacientes[i].id == id) {
            indice = i;
            break;
        }
    }
if (indice == -1) return false;

    Paciente& paciente = hospital->pacientes[indice];
    delete[] paciente.historial;
    delete[] paciente.citasAgendadas;

    for (int i = indice; i < hospital->cantidadPacientes - 1; i++) {
        hospital->pacientes[i] = hospital->pacientes[i + 1];
    }

    hospital->cantidadPacientes--;
    return true;
}
void listarPacientes(Hospital* hospital) {
    cout << "+-----+------------------------------+--------------+------+-----------+" << endl;
    cout << "| ID  | Nombre completo              | Cedula       | Edad | Consultas |" << endl;
    cout << "+-----+------------------------------+--------------+------+-----------+" << endl;

    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        Paciente& p = hospital->pacientes[i];
        char nombreCompleto[51];
        snprintf(nombreCompleto, 51, "%s %s", p.nombre, p.apellido);

        cout << "| " << setw(3) << right << p.id << " | "
             << setw(28) << left << nombreCompleto << " | "
             << setw(12) << left << p.cedula << " | "
             << setw(4) << right << p.edad << " | "
             << setw(9) << right << p.cantidadConsultas << " |" << endl;
    }
    cout << "+-----+------------------------------+--------------+------+-----------+" << endl;
}


// AGREGAR CONSULTA AL HISTORIAL
void agregarConsultaAlHistorial(Paciente* paciente, HistorialMedico nuevaConsulta) {
    // Verificar si el array está lleno
    if (paciente->cantidadConsultas >= paciente->capacidadHistorial) {
        int nuevaCapacidad = paciente->capacidadHistorial * 2;
        HistorialMedico* nuevoHistorial = new HistorialMedico[nuevaCapacidad];

        // Copiar datos existentes al nuevo array
        for (int i = 0; i < paciente->cantidadConsultas; i++) {
            nuevoHistorial[i] = paciente->historial[i];
        }

        // Liberar memoria antigua y actualizar puntero
        delete[] paciente->historial;
        paciente->historial = nuevoHistorial;
        paciente->capacidadHistorial = nuevaCapacidad;
    }

    // Agregar nueva consulta
    paciente->historial[paciente->cantidadConsultas] = nuevaConsulta;
    paciente->cantidadConsultas++;
}
// PARA OBTENER EL HISTORIAL COMPLETO
HistorialMedico* obtenerHistorialCompleto(Paciente* paciente, int* cantidad) {
    *cantidad = paciente->cantidadConsultas;
    return paciente->historial;
}

void mostrarHistorialMedico(Paciente* paciente) {
    cout << "+-----+------------+-------+---------------------------+------------+--------+" << endl;
    cout << "| ID  |   Fecha    | Hora  | Diagnostico               | ID Doctor  | Costo  |" << endl;
    cout << "+-----+------------+-------+---------------------------+------------+--------+" << endl;

    for (int i = 0; i < paciente->cantidadConsultas; i++) {
        HistorialMedico& consulta = paciente->historial[i];
        cout << "| " << setw(3) << consulta.idConsulta << " | "
             << setw(10) << consulta.fecha << " | "
             << setw(5) << consulta.hora << " | "
             << setw(25) << consulta.diagnostico << " | "
             << setw(10) << consulta.idDoctor << " | "
             << setw(6) << fixed << setprecision(2) << consulta.costo << " |" << endl;
    }

    cout << "+-----+------------+-------+---------------------------+------------+--------+" << endl;
}


int main() {

    setlocale(LC_ALL, "spanish");

    // Inicializar hospital con capacidad inicial de 10 pacientes
    Hospital hospital;
    strncpy(hospital.nombre, "Hospital Central", 100);
    strncpy(hospital.direccion, "Av. Principal, Maracaibo", 150);
    strncpy(hospital.telefono, "0261-1234567", 15);

    hospital.capacidadPacientes = 10;
    hospital.pacientes = new Paciente[hospital.capacidadPacientes];
    hospital.cantidadPacientes = 0;

    hospital.capacidadDoctores = 10;
    hospital.doctores = new Doctor[hospital.capacidadDoctores];
    hospital.cantidadDoctores = 0;

    hospital.capacidadCitas = 20;
    hospital.citas = new Cita[hospital.capacidadCitas];
    hospital.cantidadCitas = 0;

    hospital.siguienteIdPaciente = 1;
    hospital.siguienteIdDoctor = 1;
    hospital.siguienteIdCita = 1;
    hospital.siguienteIdConsulta = 1;

    // Crear paciente
    Paciente* p1 = crearPaciente(&hospital, "Juan", "Perez", "V-12345678", 35, 'M');
    if (p1) {
        cout << "Paciente creado: " << p1->nombre << " " << p1->apellido << endl;
    }

    // Buscar paciente por cédula
    Paciente* encontrado = buscarPacienteporCedula(&hospital, "V-12345678");
    if (encontrado) {
        cout << "Paciente encontrado: " << encontrado->nombre << " " << encontrado->apellido << endl;
    }

    // Agregar consulta al historial
    HistorialMedico consulta;
    consulta.idConsulta = hospital.siguienteIdConsulta++;
    strncpy(consulta.fecha, "2025-10-14", 11);
    strncpy(consulta.hora, "09:30", 6);
    strncpy(consulta.diagnostico, "Gripe comun", 200);
    strncpy(consulta.tratamiento, "Reposo y liquidos", 200);
    strncpy(consulta.medicamentos, "Paracetamol", 150);
    consulta.idDoctor = 1;
    consulta.costo = 15.0;

    agregarConsultaAlHistorial(p1, consulta);

    // Mostrar historial médico
    mostrarHistorialMedico(p1);

    // Listar todos los pacientes
    listarPacientes(&hospital);

    // Liberar memoria al final
    for (int i = 0; i < hospital.cantidadPacientes; i++) {
        delete[] hospital.pacientes[i].historial;
        delete[] hospital.pacientes[i].citasAgendadas;
    }
    delete[] hospital.pacientes;
    delete[] hospital.doctores;
    delete[] hospital.citas;

    return 0;
}

   