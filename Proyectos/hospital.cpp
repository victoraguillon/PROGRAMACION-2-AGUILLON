// ================================================================
// SISTEMA DE GESTIÓN HOSPITALARIA
// TEMA: punteros, memoria dinámica, CRUDs, búsquedas y validaciones
// Bibliotecas usadas: <iostream>, <cstring>, <ctime>, <iomanip>, <locale>
// ================================================================

#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <locale>

using namespace std;

// ----------------------------
// CONSTANTES DE TAMAÑO 
// ----------------------------
const int TAM_NOMBRE = 50;
const int TAM_APELLIDO = 50;
const int TAM_CEDULA = 20;
const int TAM_ESPECIALIDAD = 50;
const int TAM_HORARIO = 50;
const int TAM_TELEFONO = 15;
const int TAM_EMAIL = 50;
const int TAM_TIPO_SANGRE = 5;
const int TAM_DIRECCION = 100;
const int TAM_DIAGNOSTICO = 200;
const int TAM_TRATAMIENTO = 200;
const int TAM_MEDICAMENTOS = 150;
const int TAM_OBSERVACIONES = 200;
const int TAM_ESTADO = 20;
const int TAM_FECHA = 11; // "YYYY-MM-DD" + '\0'
const int TAM_HORA = 6;   // "HH:MM" + '\0'
const int TAM_ALERGIAS = 500;

// ESTRUCTURAS INICIALES
struct HistorialMedico {
    int idConsulta;
    char fecha[TAM_FECHA];
    char hora[TAM_HORA];
    char diagnostico[TAM_DIAGNOSTICO];
    char tratamiento[TAM_TRATAMIENTO];
    char medicamentos[TAM_MEDICAMENTOS];
    int idDoctor;
    float costo;
};

struct Paciente {
    int id;
    char nombre[TAM_NOMBRE];
    char apellido[TAM_APELLIDO];
    char cedula[TAM_CEDULA];
    int edad;
    char sexo; // 'M' o 'F'
    char tipoSangre[TAM_TIPO_SANGRE];
    char telefono[TAM_TELEFONO];
    char direccion[TAM_DIRECCION];
    char email[TAM_EMAIL];
    HistorialMedico* historial;
    int cantidadConsultas;
    int capacidadHistorial;
    int* citasAgendadas; // IDs de citas
    int cantidadCitas;
    int capacidadCitas;
    char alergias[TAM_ALERGIAS];
    char observaciones[TAM_ALERGIAS];
    bool activo;
};

struct Doctor {
    int id;
    char nombre[TAM_NOMBRE];
    char apellido[TAM_APELLIDO];
    char cedula[TAM_CEDULA];
    char especialidad[TAM_ESPECIALIDAD];
    int aniosExperiencia;
    float costoConsulta;
    char horarioAtencion[TAM_HORARIO];
    char telefono[TAM_TELEFONO];
    char email[TAM_EMAIL];
    int* pacientesAsignados;
    int cantidadPacientes;
    int capacidadPacientes;
    int* citasAgendadas;
    int cantidadCitas;
    int capacidadCitas;
    bool disponible;
};

struct Cita {
    int id;
    int idPaciente;
    int idDoctor;
    char fecha[TAM_FECHA];
    char hora[TAM_HORA];
    char motivo[150];
    char estado[TAM_ESTADO]; // "Agendada", "Atendida", "Cancelada"
    char observaciones[TAM_OBSERVACIONES];
    bool atendida;
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

    int siguienteIdPaciente;
    int siguienteIdDoctor;
    int siguienteIdCita;
    int siguienteIdConsulta;
};


// Helpers y validaciones básicas



void toLowerCopy(const char* src, char* dst, int dstSize) {
    int n = (int)strlen(src);
    if (n >= dstSize) n = dstSize - 1;
    for (int i = 0; i < n; i++) {
        char c = src[i];
        if (c >= 'A' && c <= 'Z') c = (char)(c + 32);
        dst[i] = c;
    }
    dst[n] = '\0';
}

// Valida formato YYYY-MM-DD y rangos básicos 
bool validarFecha(const char* fecha) {
    if ((int)strlen(fecha) != 10) return false;
    if (fecha[4] != '-' || fecha[7] != '-') return false;
    int y = (fecha[0]-'0')*1000 + (fecha[1]-'0')*100 + (fecha[2]-'0')*10 + (fecha[3]-'0');
    int m = (fecha[5]-'0')*10 + (fecha[6]-'0');
    int d = (fecha[8]-'0')*10 + (fecha[9]-'0');
    if (y < 1900 || y > 2100) return false;
    if (m < 1 || m > 12) return false;
    int maxd = 31;
    if (m == 4 || m == 6 || m == 9 || m == 11) maxd = 30;
    else if (m == 2) {
        bool leap = (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0));
        maxd = leap ? 29 : 28;
    }
    if (d < 1 || d > maxd) return false;
    return true;
}


bool validarHora(const char* hora) {
    if ((int)strlen(hora) != 5) return false;
    if (hora[2] != ':') return false;
    int h = (hora[0]-'0')*10 + (hora[1]-'0');
    int m = (hora[3]-'0')*10 + (hora[4]-'0');
    if (h < 0 || h > 23) return false;
    if (m < 0 || m > 59) return false;
    return true;
}

int compararFechas(const char* f1, const char* f2) {
    int c = strcmp(f1, f2);
    if (c < 0) return -1;
    if (c > 0) return 1;
    return 0;
}


bool validarCedula(const char* cedula) {
    int len = (int)strlen(cedula);
    return len > 0 && len < TAM_CEDULA;
}


bool validarEmail(const char* email) {
    bool tieneArroba = false, tienePunto = false;
    int len = (int)strlen(email);
    for (int i = 0; i < len; i++) {
        if (email[i] == '@') tieneArroba = true;
        if (email[i] == '.') tienePunto = true;
    }
    return tieneArroba && tienePunto;
}

//INICIALIZACIÓN HOSPITAL

Hospital* inicializarHospital(const char* nombre, int capacidadInicialPacientes = 10,
                              int capacidadInicialDoctores = 10,
                              int capacidadInicialCitas = 20) {
    Hospital* h = new Hospital;
    strncpy(h->nombre, nombre, 100); h->nombre[99] = '\0';
    strncpy(h->direccion, "Sin direccion", 150); h->direccion[149] = '\0';
    strncpy(h->telefono, "000000000000000", 15); h->telefono[14] = '\0';

    h->capacidadPacientes = capacidadInicialPacientes;
    h->capacidadDoctores = capacidadInicialDoctores;
    h->capacidadCitas = capacidadInicialCitas;

    h->cantidadPacientes = 0;
    h->cantidadDoctores = 0;
    h->cantidadCitas = 0;

    h->pacientes = new Paciente[h->capacidadPacientes];
    h->doctores = new Doctor[h->capacidadDoctores];
    h->citas = new Cita[h->capacidadCitas];

    h->siguienteIdPaciente = 1;
    h->siguienteIdDoctor = 1;
    h->siguienteIdCita = 1;
    h->siguienteIdConsulta = 1;

    return h;
}

void redimensionarArrayPacientes(Hospital* h) {
    int nuevaCap = h->capacidadPacientes * 2;
    Paciente* nuevo = new Paciente[nuevaCap];
    for (int i = 0; i < h->cantidadPacientes; i++) nuevo[i] = h->pacientes[i];
    delete[] h->pacientes;
    h->pacientes = nuevo;
    h->capacidadPacientes = nuevaCap;
}
void redimensionarArrayDoctores(Hospital* h) {
    int nuevaCap = h->capacidadDoctores * 2;
    Doctor* nuevo = new Doctor[nuevaCap];
    for (int i = 0; i < h->cantidadDoctores; i++) nuevo[i] = h->doctores[i];
    delete[] h->doctores;
    h->doctores = nuevo;
    h->capacidadDoctores = nuevaCap;
}
void redimensionarArrayCitas(Hospital* h) {
    int nuevaCap = h->capacidadCitas * 2;
    Cita* nuevo = new Cita[nuevaCap];
    for (int i = 0; i < h->cantidadCitas; i++) nuevo[i] = h->citas[i];
    delete[] h->citas;
    h->citas = nuevo;
    h->capacidadCitas = nuevaCap;
}

void destruirHospital(Hospital* h) {
    if (!h) return;

    for (int i = 0; i < h->cantidadPacientes; i++) {
        delete[] h->pacientes[i].historial;
        delete[] h->pacientes[i].citasAgendadas;
    }
    delete[] h->pacientes;

    for (int i = 0; i < h->cantidadDoctores; i++) {
        delete[] h->doctores[i].pacientesAsignados;
        delete[] h->doctores[i].citasAgendadas;
    }
    delete[] h->doctores;
    delete[] h->citas;
    delete h;
}
//MODULO DE PACIENTES

Paciente* crearPaciente(Hospital* h, const char* nombre, const char* apellido,
                        const char* cedula, int edad, char sexo) {
    // Validaciones básicas de entrada antes de tocar memoria
    if (!validarCedula(cedula)) {
        cout << "Error: Cédula inválida." << endl;
        return nullptr;
    }
    if (edad < 0 || edad > 120) {
        cout << "Error: Edad fuera de rango (0-120)." << endl;
        return nullptr;
    }
    if (sexo != 'M' && sexo != 'F') {
        cout << "Error: Sexo inválido. Use 'M' o 'F'." << endl;
        return nullptr;
    }

    // EN CASO DE CÉDULA DÚPLICADA
    for (int i = 0; i < h->cantidadPacientes; i++) {
        char a[TAM_CEDULA], b[TAM_CEDULA];
        toLowerCopy(h->pacientes[i].cedula, a, TAM_CEDULA);
        toLowerCopy(cedula, b, TAM_CEDULA);
        if (strcmp(a, b) == 0) {
            cout << "Error: Ya existe un paciente con cédula " << cedula << endl;
            return nullptr;
        }
    }
    if (h->cantidadPacientes >= h->capacidadPacientes) {
        redimensionarArrayPacientes(h);
    }

    //CREACIÓN Y ASIGNACIÓN DE PACIENTE.

    Paciente& p = h->pacientes[h->cantidadPacientes];
    p.id = h->siguienteIdPaciente++;
    strncpy(p.nombre, nombre, TAM_NOMBRE - 1); p.nombre[TAM_NOMBRE - 1] = '\0';
    strncpy(p.apellido, apellido, TAM_APELLIDO - 1); p.apellido[TAM_APELLIDO - 1] = '\0';
    strncpy(p.cedula, cedula, TAM_CEDULA - 1); p.cedula[TAM_CEDULA - 1] = '\0';
    p.edad = edad;
    p.sexo = sexo;
    p.tipoSangre[0] = '\0';
    p.telefono[0] = '\0';
    p.direccion[0] = '\0';
    p.email[0] = '\0';
    p.activo = true;

    p.capacidadHistorial = 5;
    p.cantidadConsultas = 0;
    p.historial = new HistorialMedico[p.capacidadHistorial];
    p.capacidadCitas = 5;
    p.cantidadCitas = 0;
    p.citasAgendadas = new int[p.capacidadCitas];

    p.alergias[0] = '\0';
    p.observaciones[0] = '\0';

    h->cantidadPacientes++;
    cout << "Paciente creado con ID " << p.id << endl;
    return &p;
}

Paciente* buscarPacientePorCedula(Hospital* h, const char* cedula) {
    char objetivo[TAM_CEDULA]; toLowerCopy(cedula, objetivo, TAM_CEDULA);
    for (int i = 0; i < h->cantidadPacientes; i++) {
        char comp[TAM_CEDULA]; toLowerCopy(h->pacientes[i].cedula, comp, TAM_CEDULA);
        if (strcmp(comp, objetivo) == 0) return &h->pacientes[i];
    }
    return nullptr;
}

Paciente* buscarPacientePorId(Hospital* h, int id) {
    for (int i = 0; i < h->cantidadPacientes; i++) {
        if (h->pacientes[i].id == id) return &h->pacientes[i];
    }
    return nullptr;
}

Paciente** buscarPacientesPorNombre(Hospital* h, const char* nombre, int* cantidad) {
    char objetivo[TAM_NOMBRE]; toLowerCopy(nombre, objetivo, TAM_NOMBRE);
    int contador = 0;
    for (int i = 0; i < h->cantidadPacientes; i++) {
        char comp[TAM_NOMBRE]; toLowerCopy(h->pacientes[i].nombre, comp, TAM_NOMBRE);
        if (strstr(comp, objetivo)) contador++;
    }
    if (contador == 0) { *cantidad = 0; return nullptr; }

    Paciente** resultados = new Paciente*[contador];
    int indice = 0;
    for (int i = 0; i < h->cantidadPacientes; i++) {
        char comp[TAM_NOMBRE]; toLowerCopy(h->pacientes[i].nombre, comp, TAM_NOMBRE);
        if (strstr(comp, objetivo)) resultados[indice++] = &h->pacientes[i];
    }
    *cantidad = contador;
    return resultados;
}

// ACTUALIZACIÓN DE DATOS
bool actualizarPaciente(Hospital* h, int id) {
    Paciente* p = buscarPacientePorId(h, id);
    if (!p) { cout << "Error: Paciente no encontrado." << endl; return false; }

    cout << "Actualizando paciente ID " << p->id << endl;

    cout << "Nuevo teléfono (max 14 chars): ";
    cin.ignore();
    cin.getline(p->telefono, TAM_TELEFONO);

    cout << "Nueva dirección (max 99 chars): ";
    cin.getline(p->direccion, TAM_DIRECCION);

    cout << "Nuevo email (max 49 chars): ";
    cin.getline(p->email, TAM_EMAIL);
    if (!validarEmail(p->email)) {
        cout << "Advertencia: Email posiblemente inválido." << endl;
    }

    cout << "Nueva edad: ";
    int nuevaEdad = p->edad;
    cin >> nuevaEdad;
    if (cin.fail() || nuevaEdad < 0 || nuevaEdad > 120) {
        cin.clear(); cin.ignore(10000, '\n');
        cout << "Error: Edad inválida. No se actualiza." << endl;
        return false;
    }
    p->edad = nuevaEdad;

    cout << "Actualización completada." << endl;
    return true;
}

// ELIMINACIÓN DE PACIENTE.
bool eliminarPaciente(Hospital* h, int id) {
    int indice = -1;
    for (int i = 0; i < h->cantidadPacientes; i++) {
        if (h->pacientes[i].id == id) { indice = i; break; }
    }
    if (indice == -1) { cout << "Error: Paciente no encontrado." << endl; return false; }

    delete[] h->pacientes[indice].historial;
    delete[] h->pacientes[indice].citasAgendadas;

    for (int i = indice; i < h->cantidadPacientes - 1; i++) {
        h->pacientes[i] = h->pacientes[i + 1];
    }
    h->cantidadPacientes--;
    cout << "Paciente eliminado." << endl;
    return true;
}

// Listado de pacientes en tabla ASCII
void listarPacientes(Hospital* h) {
    cout << "+-----+------------------------------+--------------+------+-----------+" << endl;
    cout << "| ID  | Nombre completo              | Cedula       | Edad | Consultas |" << endl;
    cout << "+-----+------------------------------+--------------+------+-----------+" << endl;
    for (int i = 0; i < h->cantidadPacientes; i++) {
        Paciente& p = h->pacientes[i];
        char nombreCompleto[2*TAM_NOMBRE];
        snprintf(nombreCompleto, sizeof(nombreCompleto), "%s %s", p.nombre, p.apellido);
        cout << "| " << setw(3) << right << p.id << " | "
             << setw(28) << left << nombreCompleto << " | "
             << setw(12) << left << p.cedula << " | "
             << setw(4) << right << p.edad << " | "
             << setw(9) << right << p.cantidadConsultas << " |" << endl;
    }
    cout << "+-----+------------------------------+--------------+------+-----------+" << endl;
}

// HISTORIAL MÉDICO (ARRAYS DENTRO DEL PACIENTE)

void agregarConsultaAlHistorial(Paciente* p, HistorialMedico consulta) {
    if (p->cantidadConsultas >= p->capacidadHistorial) {
        int nuevaCap = p->capacidadHistorial * 2;
        HistorialMedico* nuevo = new HistorialMedico[nuevaCap];
        for (int i = 0; i < p->cantidadConsultas; i++) nuevo[i] = p->historial[i];
        delete[] p->historial;
        p->historial = nuevo;
        p->capacidadHistorial = nuevaCap;
    }
    p->historial[p->cantidadConsultas] = consulta;
    p->cantidadConsultas++;
}
HistorialMedico* obtenerHistorialCompleto(Paciente* p, int* cantidad) {
    *cantidad = p->cantidadConsultas;
    return p->historial;
}

void mostrarHistorialMedico(Paciente* p) {
    cout << "+-----+------------+-------+---------------------------+------------+--------+" << endl;
    cout << "| ID  |   Fecha    | Hora  | Diagnostico               | ID Doctor  | Costo  |" << endl;
    cout << "+-----+------------+-------+---------------------------+------------+--------+" << endl;
    for (int i = 0; i < p->cantidadConsultas; i++) {
        HistorialMedico& c = p->historial[i];
        cout << "| " << setw(3) << right << c.idConsulta << " | "
             << setw(10) << left << c.fecha << " | "
             << setw(5) << left << c.hora << " | "
             << setw(25) << left << c.diagnostico << " | "
             << setw(10) << right << c.idDoctor << " | "
             << setw(6) << right << fixed << setprecision(2) << c.costo << " |" << endl;
    }
    cout << "+-----+------------+-------+---------------------------+------------+--------+" << endl;
}

HistorialMedico* obtenerUltimaConsulta(Paciente* p) {
    if (p->cantidadConsultas == 0) return nullptr;
    return &p->historial[p->cantidadConsultas - 1];
}

// MÓDULO DE DOCTORES 
Doctor* crearDoctor(Hospital* h, const char* nombre, const char* apellido,
                    const char* cedula, const char* especialidad,
                    int aniosExperiencia, float costoConsulta) {
    if (!validarCedula(cedula)) {
        cout << "Error: Cédula inválida." << endl;
        return nullptr;
    }
    if (aniosExperiencia < 0) {
        cout << "Error: Años de experiencia no pueden ser negativos." << endl;
        return nullptr;
    }
    if (costoConsulta <= 0.0f) {
        cout << "Error: Costo de consulta debe ser mayor que 0." << endl;
        return nullptr;
    }

    // Validar cédula única
    for (int i = 0; i < h->cantidadDoctores; i++) {
        char a[TAM_CEDULA], b[TAM_CEDULA];
        toLowerCopy(h->doctores[i].cedula, a, TAM_CEDULA);
        toLowerCopy(cedula, b, TAM_CEDULA);
        if (strcmp(a, b) == 0) {
            cout << "Error: Ya existe un doctor con cédula " << cedula << endl;
            return nullptr;
        }
    }

    if (h->cantidadDoctores >= h->capacidadDoctores) {
        redimensionarArrayDoctores(h);
    }

    Doctor& d = h->doctores[h->cantidadDoctores];
    d.id = h->siguienteIdDoctor++;
    strncpy(d.nombre, nombre, TAM_NOMBRE - 1); d.nombre[TAM_NOMBRE - 1] = '\0';
    strncpy(d.apellido, apellido, TAM_APELLIDO - 1); d.apellido[TAM_APELLIDO - 1] = '\0';
    strncpy(d.cedula, cedula, TAM_CEDULA - 1); d.cedula[TAM_CEDULA - 1] = '\0';
    strncpy(d.especialidad, especialidad, TAM_ESPECIALIDAD - 1); d.especialidad[TAM_ESPECIALIDAD - 1] = '\0';
    d.aniosExperiencia = aniosExperiencia;
    d.costoConsulta = costoConsulta;
    d.horarioAtencion[0] = '\0';
    d.telefono[0] = '\0';
    d.email[0] = '\0';
    d.disponible = true;

    d.capacidadPacientes = 5;
    d.cantidadPacientes = 0;
    d.pacientesAsignados = new int[d.capacidadPacientes];

    d.capacidadCitas = 10;
    d.cantidadCitas = 0;
    d.citasAgendadas = new int[d.capacidadCitas];

    h->cantidadDoctores++;
    cout << "Doctor creado con ID " << d.id << endl;
    return &d;
}

Doctor* buscarDoctorPorId(Hospital* h, int id) {
    for (int i = 0; i < h->cantidadDoctores; i++) {
        if (h->doctores[i].id == id) return &h->doctores[i];
    }
    return nullptr;
}

// Búsqueda por especialidad exacta
Doctor** buscarDoctoresPorEspecialidad(Hospital* h, const char* especialidad, int* cantidad) {
    char objetivo[TAM_ESPECIALIDAD]; toLowerCopy(especialidad, objetivo, TAM_ESPECIALIDAD);
    int contador = 0;
    for (int i = 0; i < h->cantidadDoctores; i++) {
        char comp[TAM_ESPECIALIDAD]; toLowerCopy(h->doctores[i].especialidad, comp, TAM_ESPECIALIDAD);
        if (strcmp(comp, objetivo) == 0) contador++;
    }
    if (contador == 0) { *cantidad = 0; return nullptr; }

    Doctor** resultados = new Doctor*[contador];
    int indice = 0;
    for (int i = 0; i < h->cantidadDoctores; i++) {
        char comp[TAM_ESPECIALIDAD]; toLowerCopy(h->doctores[i].especialidad, comp, TAM_ESPECIALIDAD);
        if (strcmp(comp, objetivo) == 0) resultados[indice++] = &h->doctores[i];
    }
    *cantidad = contador;
    return resultados;
}

// Asignar paciente a doctor
bool asignarPacienteADoctor(Doctor* d, int idPaciente) {
    if (!d) return false;
    for (int i = 0; i < d->cantidadPacientes; i++) {
        if (d->pacientesAsignados[i] == idPaciente) {
            cout << "Advertencia: Paciente ya asignado a este doctor." << endl;
            return false;
        }
    }
    if (d->cantidadPacientes >= d->capacidadPacientes) {
        int nuevaCap = d->capacidadPacientes * 2;
        int* nuevo = new int[nuevaCap];
        for (int i = 0; i < d->cantidadPacientes; i++) nuevo[i] = d->pacientesAsignados[i];
        delete[] d->pacientesAsignados;
        d->pacientesAsignados = nuevo;
        d->capacidadPacientes = nuevaCap;
    }
    d->pacientesAsignados[d->cantidadPacientes++] = idPaciente;
    return true;
}

// Remover paciente del doctor
bool removerPacienteDeDoctor(Doctor* d, int idPaciente) {
    if (!d) return false;
    int indice = -1;
    for (int i = 0; i < d->cantidadPacientes; i++) {
        if (d->pacientesAsignados[i] == idPaciente) { indice = i; break; }
    }
    if (indice == -1) return false;
    for (int i = indice; i < d->cantidadPacientes - 1; i++) d->pacientesAsignados[i] = d->pacientesAsignados[i + 1];
    d->cantidadPacientes--;
    return true;
}

void listarDoctores(Hospital* h) {
    cout << "+-----+----------------------+----------------------+----------------+------+----------+" << endl;
    cout << "| ID  | Nombre               | Apellido             | Especialidad   | Años | Disponible|" << endl;
    cout << "+-----+----------------------+----------------------+----------------+------+----------+" << endl;
    for (int i = 0; i < h->cantidadDoctores; i++) {
        Doctor& d = h->doctores[i];
        cout << "| " << setw(3) << right << d.id << " | "
             << setw(20) << left << d.nombre << " | "
             << setw(20) << left << d.apellido << " | "
             << setw(14) << left << d.especialidad << " | "
             << setw(4) << right << d.aniosExperiencia << " | "
             << setw(8) << left << (d.disponible ? "Si" : "No") << " |" << endl;
    }
    cout << "+-----+----------------------+----------------------+----------------+------+----------+" << endl;
}

bool eliminarDoctor(Hospital* h, int id) {
    int indice = -1;
    for (int i = 0; i < h->cantidadDoctores; i++) {
        if (h->doctores[i].id == id) { indice = i; break; }
    }
    if (indice == -1) { cout << "Error: Doctor no encontrado." << endl; return false; }

    delete[] h->doctores[indice].pacientesAsignados;
    delete[] h->doctores[indice].citasAgendadas;

    for (int i = indice; i < h->cantidadDoctores - 1; i++) h->doctores[i] = h->doctores[i + 1];
    h->cantidadDoctores--;
    cout << "Doctor eliminado." << endl;
    return true;
}

// Listar pacientes asignados a un doctor 
void listarPacientesDeDoctor(Hospital* h, int idDoctor) {
    Doctor* d = buscarDoctorPorId(h, idDoctor);
    if (!d) { cout << "Error: Doctor no encontrado." << endl; return; }

    cout << "+-----+------------------------------+--------------+------+-----------+" << endl;
    cout << "| ID  | Nombre completo              | Cedula       | Edad | Consultas |" << endl;
    cout << "+-----+------------------------------+--------------+------+-----------+" << endl;
    for (int i = 0; i < d->cantidadPacientes; i++) {
        Paciente* p = buscarPacientePorId(h, d->pacientesAsignados[i]);
        if (!p) continue; // EN CASO DE ELIMINACIÓN PREVIA
        char nombreCompleto[2*TAM_NOMBRE];
        snprintf(nombreCompleto, sizeof(nombreCompleto), "%s %s", p->nombre, p->apellido);
        cout << "| " << setw(3) << right << p->id << " | "
             << setw(28) << left << nombreCompleto << " | "
             << setw(12) << left << p->cedula << " | "
             << setw(4) << right << p->edad << " | "
             << setw(9) << right << p->cantidadConsultas << " |" << endl;
    }
    cout << "+-----+------------------------------+--------------+------+-----------+" << endl;
}

// MÓDULO DE CITAS


