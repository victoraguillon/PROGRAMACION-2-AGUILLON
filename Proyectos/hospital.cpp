
// SISTEMA DE GESTIÓN HOSPITALARIA
// TEMA: punteros, memoria dinámica, CRUDs, búsquedas y validaciones
// Bibliotecas usadas: <iostream>, <cstring>, <ctime>, <iomanip>, <locale>
// Hecho por: Víctor M. Aguillón.
// ================================================================

#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <clocale>
#include <stdlib.h>

using namespace std;


// ----------------------------
// CONSTANTES DE TAMAÑO 
// ----------------------------
const int tamanio_NOMBRE = 50;
const int tamanio_APELLIDO = 50;
const int tamanio_CEDULA = 20;
const int tamanio_ESPECIALIDAD = 50;
const int tamanio_HORARIO = 50;
const int tamanio_TELEFONO = 15;
const int tamanio_EMAIL = 50;
const int tamanio_TIPO_SANGRE = 5;
const int tamanio_DIRECCION = 100;
const int tamanio_DIAGNOSTICO = 200;
const int tamanio_TRATAMIENTO = 200;
const int tamanio_MEDICAMENTOS = 150;
const int tamanio_OBSERVACIONES = 200;
const int tamanio_ESTADO = 20;
const int tamanio_FECHA = 11; // "YYYY-MM-DD" + '\0'
const int tamanio_HORA = 6;   // "HH:MM" + '\0'
const int tamanio_ALERGIAS = 500;

// ESTRUCTURAS INICIALES
struct HistorialMedico {
    int idConsulta;
    char fecha[tamanio_FECHA];
    char hora[tamanio_HORA];
    char diagnostico[tamanio_DIAGNOSTICO];
    char tratamiento[tamanio_TRATAMIENTO];
    char medicamentos[tamanio_MEDICAMENTOS];
    int idDoctor;
    float costo;
};

struct Paciente {
    int id;
    char nombre[tamanio_NOMBRE];
    char apellido[tamanio_APELLIDO];
    char cedula[tamanio_CEDULA];
    int edad;
    char sexo; // 'M' o 'F'
    char tipoSangre[tamanio_TIPO_SANGRE];
    char telefono[tamanio_TELEFONO];
    char direccion[tamanio_DIRECCION];
    char email[tamanio_EMAIL];
    HistorialMedico* historial;
    int cantidadConsultas;
    int capacidadHistorial;
    int* citasAgendadas; // IDs de citas
    int cantidadCitas;
    int capacidadCitas;
    char alergias[tamanio_ALERGIAS];
    char observaciones[tamanio_ALERGIAS];
    bool activo;
};

struct Doctor {
    int id;
    char nombre[tamanio_NOMBRE];
    char apellido[tamanio_APELLIDO];
    char cedula[tamanio_CEDULA];
    char especialidad[tamanio_ESPECIALIDAD];
    int aniosExperiencia;
    float costoConsulta;
    char horarioAtencion[tamanio_HORARIO];
    char telefono[tamanio_TELEFONO];
    char email[tamanio_EMAIL];
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
    char fecha[tamanio_FECHA];
    char hora[tamanio_HORA];
    char motivo[150];
    char estado[tamanio_ESTADO]; // "Agendada", "Atendida", "Cancelada"
    char observaciones[tamanio_OBSERVACIONES];
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

// Valida formato YYYY-MM-DD 
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

bool validarCedula(Hospital* h, const char* cedula) {
    int longitud = (int)strlen(cedula);

    if (longitud == 0) {
        cout << "Error: La cédula no puede estar vacía." << endl;
        return false;
    }

    if (longitud > 8) {
        cout << "Error: La cédula debe tener máximo 8 dígitos." << endl;
        return false;
    }

    for (int i = 0; i < longitud; i++) {
        if (cedula[i] < '0' || cedula[i] > '9') {
            cout << "Error: La cédula solo debe contener dígitos." << endl;
            return false;
        }
    }
    for (int i = 0; i < h->cantidadPacientes; i++) {
        if (strcmp(h->pacientes[i].cedula, cedula) == 0) {
            cout << "Error: Ya existe un paciente con la cédula " << cedula << endl;
            return false;
        }
    }
    for (int i = 0; i < h->cantidadDoctores; i++) {
        if (strcmp(h->doctores[i].cedula, cedula) == 0) {
            cout << "Error: Ya existe un doctor con la cédula " << cedula << endl;
            return false;
        }
    }

    return true;
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

Hospital* inicializarHospital(const char* nombre, int capacidadInicialPacientes = 10, int capacidadInicialDoctores = 10, int capacidadInicialCitas = 20) { 
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
    for (int i = 0; i < h->cantidadPacientes; i++) {
        nuevo[i] = h->pacientes[i];}
    delete[] h->pacientes;
    h->pacientes = nuevo;
    h->capacidadPacientes = nuevaCap;
}
void redimensionarArrayDoctores(Hospital* h) {
    int nuevaCap = h->capacidadDoctores * 2;
    Doctor* nuevo = new Doctor[nuevaCap];
    for (int i = 0; i < h->cantidadDoctores; i++) {
        nuevo[i] = h->doctores[i];
    }
    delete[] h->doctores;
    h->doctores = nuevo;
    h->capacidadDoctores = nuevaCap;
}
void redimensionarArrayCitas(Hospital* h) {
    int nuevaCap = h->capacidadCitas * 2;
    Cita* nuevo = new Cita[nuevaCap];
    for (int i = 0; i < h->cantidadCitas; i++){
        nuevo[i] = h->citas[i];
    }
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


void limpiarEntrada() {
    cin.clear();
    cin.ignore(10000, '\n');
}

int leerEnteroSeguro(const char* prompt, int minVal, int maxVal) {
    int val;
    while (true) {
        cout << prompt;
        if (cin >> val) {
            if (val >= minVal && val <= maxVal) {
                limpiarEntrada();
                return val;
            }
            cout << "Error: valor fuera de rango (" << minVal << "-" << maxVal << "). Intente de nuevo.\n";
        } else {
            cout << "Error: entrada no válida. Intente de nuevo.\n";
            limpiarEntrada();
        }
    }
}

float leerFloatSeguro(const char* prompt, float minVal, float maxVal) {
    float val;
    while (true) {
        cout << prompt;
        if (cin >> val) {
            if (val >= minVal && val <= maxVal) {
                limpiarEntrada();
                return val;
            }
            cout << "Error: valor fuera de rango (" << minVal << "-" << maxVal << "). Intente de nuevo.\n";
        } else {
            cout << "Error: entrada no válida. Intente de nuevo.\n";
            limpiarEntrada();
        }
    }
}

void leerCadenaSeguro(const char* prompt, char* destino, int tamMax, bool permitirVacio = false) {
    while (true) {
        cout << prompt;
        cin.getline(destino, tamMax);
        if (cin.fail()) {
            cout << "Error: longitud excedida. Intente de nuevo.\n";
            limpiarEntrada();
            continue;
        }
        if (!permitirVacio && destino[0] == '\0') {
            cout << "Error: no puede estar vacío. Intente de nuevo.\n";
            continue;
        }
        return;
    }
}

char leerSexoSeguro(const char* prompt) {
    while (true) {
        cout << prompt;
        char s;
        if (cin >> s) {
            limpiarEntrada();
            if (s == 'M' || s == 'F') return s;
            cout << "Error: sexo inválido. Use 'M' o 'F'.\n";
        } else {
            cout << "Error: entrada no válida.\n";
            limpiarEntrada();
        }
    }
}

void leerFechaSegura(const char* prompt, char* destino, int tamMax) {
    while (true) {
        cout << prompt;
        cin.getline(destino, tamMax);
        if (cin.fail()) {
            cout << "Error: longitud excedida. Intente de nuevo.\n";
            limpiarEntrada();
            continue;
        }
        if (!validarFecha(destino)) {
            cout << "Error: Fecha inválida. Formato YYYY-MM-DD.\n";
            continue;
        }
        return;
    }
}

void leerHoraSegura(const char* prompt, char* destino, int tamMax) {
    while (true) {
        cout << prompt;
        cin.getline(destino, tamMax);
        if (cin.fail()) {
            cout << "Error: longitud excedida. Intente de nuevo.\n";
            limpiarEntrada();
            continue;
        }
        if (!validarHora(destino)) {
            cout << "Error: Hora inválida. Formato HH:MM.\n";
            continue;
        }
        return;
    }
}

bool leerConfirmacion(const char* prompt) {
    while (true) {
        cout << prompt << " (S/N): ";
        char c;
        if (cin >> c) {
            limpiarEntrada();
            if (c == 'S' || c == 's') return true;
            if (c == 'N' || c == 'n') return false;
            cout << "Responda 'S' o 'N'.\n";
        } else {
            limpiarEntrada();
            cout << "Entrada no válida.\n";
        }
    }
}
//MODULO DE PACIENTES

Paciente* crearPaciente(Hospital* h, const char* nombre, const char* apellido, const char* cedula, int edad, char sexo) {
    // Validaciones básicas de entrada antes de tocar memoria
  
  validarCedula (h, cedula);

    if (!validarCedula(h, cedula)) {
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
        char a[tamanio_CEDULA], b[tamanio_CEDULA];
        toLowerCopy(h->pacientes[i].cedula, a, tamanio_CEDULA);
        toLowerCopy(cedula, b, tamanio_CEDULA);
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
        strncpy(p.nombre, nombre, tamanio_NOMBRE - 1); p.nombre[tamanio_NOMBRE - 1] = '\0';
        strncpy(p.apellido, apellido, tamanio_APELLIDO - 1); p.apellido[tamanio_APELLIDO - 1] = '\0';
        strncpy(p.cedula, cedula, tamanio_CEDULA - 1); p.cedula[tamanio_CEDULA - 1] = '\0';
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
    char objetivo[tamanio_CEDULA]; toLowerCopy(cedula, objetivo, tamanio_CEDULA);
    for (int i = 0; i < h->cantidadPacientes; i++) {
        char comp[tamanio_CEDULA]; toLowerCopy(h->pacientes[i].cedula, comp, tamanio_CEDULA);
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
    char objetivo[tamanio_NOMBRE]; toLowerCopy(nombre, objetivo, tamanio_NOMBRE);
    int contador = 0;
    for (int i = 0; i < h->cantidadPacientes; i++) {
        char comp[tamanio_NOMBRE]; toLowerCopy(h->pacientes[i].nombre, comp, tamanio_NOMBRE);
        if (strstr(comp, objetivo)) contador++;
    }
    if (contador == 0) { *cantidad = 0; return nullptr; }

    Paciente** resultados = new Paciente*[contador];
    int indice = 0;
    for (int i = 0; i < h->cantidadPacientes; i++) {
        char comp[tamanio_NOMBRE]; toLowerCopy(h->pacientes[i].nombre, comp, tamanio_NOMBRE);
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
    cin.getline(p->telefono, tamanio_TELEFONO);

    cout << "Nueva dirección (max 99 chars): ";
    cin.getline(p->direccion, tamanio_DIRECCION);

    cout << "Nuevo email (max 49 chars): ";
    cin.getline(p->email, tamanio_EMAIL);
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

// ELIMINACIÓN DE PACIENTE.1

bool eliminarPaciente(Hospital* h, int id) {
    int indice = -1;
    for (int i = 0; i < h->cantidadPacientes; i++) {
        if (h->pacientes[i].id == id) { indice = i; break; }
    }
    if (indice == -1) { cout << "Error: Paciente no encontrado." << endl; 
        
        return false;
        system ("PAUSE");
     }

    delete[] h->pacientes[indice].historial;
    delete[] h->pacientes[indice].citasAgendadas;

    for (int i = indice; i < h->cantidadPacientes - 1; i++) {
        h->pacientes[i] = h->pacientes[i + 1];
    }
    h->cantidadPacientes--;
    cout << "Paciente eliminado." << endl;
    return true;
}

// Listado de pacientes en tabla
void listarPacientes(Hospital* h) {
    cout << "+-----+------------------------------+--------------+------+-----------+" << endl;
    cout << "| ID  | Nombre completo              | Cedula       | Edad | Consultas |" << endl;
    cout << "+-----+------------------------------+--------------+------+-----------+" << endl;
    for (int i = 0; i < h->cantidadPacientes; i++) {
        Paciente& p = h->pacientes[i];
        char nombreCompleto[2*tamanio_NOMBRE];
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

    if (!validarCedula(h, cedula)) {
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
        char a[tamanio_CEDULA], b[tamanio_CEDULA];
        toLowerCopy(h->doctores[i].cedula, a, tamanio_CEDULA);
        toLowerCopy(cedula, b, tamanio_CEDULA);
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
    strncpy(d.nombre, nombre, tamanio_NOMBRE - 1); d.nombre[tamanio_NOMBRE - 1] = '\0';
    strncpy(d.apellido, apellido, tamanio_APELLIDO - 1); d.apellido[tamanio_APELLIDO - 1] = '\0';
    strncpy(d.cedula, cedula, tamanio_CEDULA - 1); d.cedula[tamanio_CEDULA - 1] = '\0';
    strncpy(d.especialidad, especialidad, tamanio_ESPECIALIDAD - 1); d.especialidad[tamanio_ESPECIALIDAD - 1] = '\0';
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
    char objetivo[tamanio_ESPECIALIDAD]; toLowerCopy(especialidad, objetivo, tamanio_ESPECIALIDAD);
    int contador = 0;
    for (int i = 0; i < h->cantidadDoctores; i++) {
        char comp[tamanio_ESPECIALIDAD]; toLowerCopy(h->doctores[i].especialidad, comp, tamanio_ESPECIALIDAD);
        if (strcmp(comp, objetivo) == 0) contador++;
    }
    if (contador == 0) { *cantidad = 0; return nullptr; }

    Doctor** resultados = new Doctor*[contador];
    int indice = 0;
    for (int i = 0; i < h->cantidadDoctores; i++) {
        char comp[tamanio_ESPECIALIDAD]; toLowerCopy(h->doctores[i].especialidad, comp, tamanio_ESPECIALIDAD);
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
        char nombreCompleto[2*tamanio_NOMBRE];
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
bool verificarDisponibilidad(Hospital* h, int idDoctor, const char* fecha, const char* hora) {
    for (int i = 0; i < h->cantidadCitas; i++) {
        Cita& c = h->citas[i];
        if (c.idDoctor == idDoctor && strcmp(c.fecha, fecha) == 0 && strcmp(c.hora, hora) == 0 && strcmp(c.estado, "Agendada") == 0) {
            return false;
        }
    }
    return true;
}

Cita* agendarCita(Hospital* h, int idPaciente, int idDoctor, const char* fecha, const char* hora, const char* motivo) {
    Paciente* p = buscarPacientePorId(h, idPaciente);
    Doctor* d = buscarDoctorPorId(h, idDoctor);
    if (!p) { cout << "Error: Paciente no existe." << endl; return nullptr; }
    if (!d) { cout << "Error: Doctor no existe." << endl; return nullptr; }
    if (!validarFecha(fecha)) { cout << "Error: Fecha inválida (YYYY-MM-DD)." << endl; return nullptr; }
    if (!validarHora(hora)) { cout << "Error: Hora inválida (HH:MM)." << endl; return nullptr; }
    if (!verificarDisponibilidad(h, idDoctor, fecha, hora)) {
        cout << "Error: Doctor no disponible en esa fecha/hora." << endl; return nullptr;
    }

    if (h->cantidadCitas >= h->capacidadCitas) {
        redimensionarArrayCitas(h);
    }

    Cita& c = h->citas[h->cantidadCitas];
    c.id = h->siguienteIdCita++;
    c.idPaciente = idPaciente;
    c.idDoctor = idDoctor;
    strncpy(c.fecha, fecha, tamanio_FECHA - 1); c.fecha[tamanio_FECHA - 1] = '\0';
    strncpy(c.hora, hora, tamanio_HORA - 1); c.hora[tamanio_HORA - 1] = '\0';
    strncpy(c.motivo, motivo, 149); c.motivo[149] = '\0';
    strncpy(c.estado, "Agendada", tamanio_ESTADO - 1); c.estado[tamanio_ESTADO - 1] = '\0';
    c.observaciones[0] = '\0';
    c.atendida = false;

    // Registrar ID de cita en paciente y doctor 
    if (p->cantidadCitas >= p->capacidadCitas) {
        int nuevaCap = p->capacidadCitas * 2;
        int* nuevo = new int[nuevaCap];
        for (int i = 0; i < p->cantidadCitas; i++) nuevo[i] = p->citasAgendadas[i];
        delete[] p->citasAgendadas;
        p->citasAgendadas = nuevo;
        p->capacidadCitas = nuevaCap;
    }
    p->citasAgendadas[p->cantidadCitas++] = c.id;

    if (d->cantidadCitas >= d->capacidadCitas) {
        int nuevaCap = d->capacidadCitas * 2;
        int* nuevo = new int[nuevaCap];
        for (int i = 0; i < d->cantidadCitas; i++) nuevo[i] = d->citasAgendadas[i];
        delete[] d->citasAgendadas;
        d->citasAgendadas = nuevo;
        d->capacidadCitas = nuevaCap;
    }
    d->citasAgendadas[d->cantidadCitas++] = c.id;

    h->cantidadCitas++;
    cout << "Cita agendada con ID " << c.id << endl;
    return &c;
}

bool cancelarCita(Hospital* h, int idCita) {
    int indice = -1;
    for (int i = 0; i < h->cantidadCitas; i++) {
        if (h->citas[i].id == idCita) { indice = i; break; }
    }
    if (indice == -1) { cout << "Error: Cita no encontrada." << endl; return false; }
    Cita& c = h->citas[indice];
    strncpy(c.estado, "Cancelada", tamanio_ESTADO - 1); c.estado[tamanio_ESTADO - 1] = '\0';
    c.atendida = false;

    // Remover de arrays del paciente y doctor (compactando)
    Paciente* p = buscarPacientePorId(h, c.idPaciente);
    Doctor* d = buscarDoctorPorId(h, c.idDoctor);

    if (p) {
        int j = -1;
        for (int i = 0; i < p->cantidadCitas; i++) if (p->citasAgendadas[i] == c.id) { j = i; break; }
        if (j != -1) {
            for (int i = j; i < p->cantidadCitas - 1; i++) p->citasAgendadas[i] = p->citasAgendadas[i + 1];
            p->cantidadCitas--;
        }
    }
    if (d) {
        int j = -1;
        for (int i = 0; i < d->cantidadCitas; i++) if (d->citasAgendadas[i] == c.id) { j = i; break; }
        if (j != -1) {
            for (int i = j; i < d->cantidadCitas - 1; i++) d->citasAgendadas[i] = d->citasAgendadas[i + 1];
            d->cantidadCitas--;
        }
    }
    cout << "Cita cancelada." << endl;
    return true;
}

bool atenderCita(Hospital* h, int idCita, const char* diagnostico, const char* tratamiento, const char* medicamentos) {
    int indice = -1;
    for (int i = 0; i < h->cantidadCitas; i++) {
        if (h->citas[i].id == idCita) { indice = i; break; }
    }
    if (indice == -1) { cout << "Error: Cita no encontrada." << endl; return false; }
    Cita& c = h->citas[indice];
    if (strcmp(c.estado, "Agendada") != 0) { cout << "Error: Solo citas agendadas pueden ser atendidas." << endl; return false; }

    Paciente* p = buscarPacientePorId(h, c.idPaciente);
    Doctor* d = buscarDoctorPorId(h, c.idDoctor);
    if (!p || !d) { cout << "Error: Paciente o Doctor inválido." << endl; return false; }

    // Crear consulta y agregar al historial médico del paciente
    HistorialMedico consulta;
        consulta.idConsulta = h->siguienteIdConsulta++;
        strncpy(consulta.fecha, c.fecha, tamanio_FECHA - 1); consulta.fecha[tamanio_FECHA - 1] = '\0';
        strncpy(consulta.hora, c.hora, tamanio_HORA - 1); consulta.hora[tamanio_HORA - 1] = '\0';
        strncpy(consulta.diagnostico, diagnostico, tamanio_DIAGNOSTICO - 1); consulta.diagnostico[tamanio_DIAGNOSTICO - 1] = '\0';
        strncpy(consulta.tratamiento, tratamiento, tamanio_TRATAMIENTO - 1); consulta.tratamiento[tamanio_TRATAMIENTO - 1] = '\0';
        strncpy(consulta.medicamentos, medicamentos, tamanio_MEDICAMENTOS - 1); consulta.medicamentos[tamanio_MEDICAMENTOS - 1] = '\0';
        consulta.idDoctor = d->id;
        consulta.costo = d->costoConsulta;

    agregarConsultaAlHistorial(p, consulta);

    // Actualiza estado de cita
    strncpy(c.estado, "Atendida", tamanio_ESTADO - 1); c.estado[tamanio_ESTADO - 1] = '\0';
    c.atendida = true;

    cout << "Cita atendida y registrada en historial (ID consulta " << consulta.idConsulta << ")." << endl;
    return true;
}

// Obtener citas de paciente
Cita** obtenerCitasDePaciente(Hospital* h, int idPaciente, int* cantidad) {
    int contador = 0;
        for (int i = 0; i < h->cantidadCitas; i++) if (h->citas[i].idPaciente == idPaciente) contador++;
            if (contador == 0) { *cantidad = 0; return nullptr; }
    Cita** arr = new Cita*[contador];
        int indice = 0;
        for (int i = 0; i < h->cantidadCitas; i++) if (h->citas[i].idPaciente == idPaciente) arr[indice++] = &h->citas[i];
    *cantidad = contador;
    return arr;
}

// Obtener citas de doctor
Cita** obtenerCitasDeDoctor(Hospital* h, int idDoctor, int* cantidad) {
    int contador = 0;
        for (int i = 0; i < h->cantidadCitas; i++) if (h->citas[i].idDoctor == idDoctor) contador++;
            if (contador == 0) { *cantidad = 0; return nullptr; }
    Cita** arr = new Cita*[contador];
        int indice = 0;
        for (int i = 0; i < h->cantidadCitas; i++) if (h->citas[i].idDoctor == idDoctor) arr[indice++] = &h->citas[i];
    *cantidad = contador;
    return arr;
}

// Obtener citas por fecha
Cita** obtenerCitasPorFecha(Hospital* h, const char* fecha, int* cantidad) {
    int contador = 0;
        for (int i = 0; i < h->cantidadCitas; i++) if (strcmp(h->citas[i].fecha, fecha) == 0) contador++;
            if (contador == 0) { *cantidad = 0; return nullptr; }
    Cita** arr = new Cita*[contador];
        int indice = 0;
        for (int i = 0; i < h->cantidadCitas; i++) if (strcmp(h->citas[i].fecha, fecha) == 0) arr[indice++] = &h->citas[i];
    *cantidad = contador;
    return arr;
}

void listarCitasPendientes(Hospital* h) {
    cout << "+-----+----------+----------+------------+-------+---------------------------+-------------+" << endl;
    cout << "| ID  | Paciente | Doctor   | Fecha      | Hora  | Motivo                    | Estado      |" << endl;
    cout << "+-----+----------+----------+------------+-------+---------------------------+-------------+" << endl;
    for (int i = 0; i < h->cantidadCitas; i++) {
        Cita& c = h->citas[i];
        if (strcmp(c.estado, "Agendada") == 0) {
            cout << "| " << setw(3) << right << c.id << " | "
                 << setw(8) << right << c.idPaciente << " | "
                 << setw(8) << right << c.idDoctor << " | "
                 << setw(10) << left << c.fecha << " | "
                 << setw(5) << left << c.hora << " | "
                 << setw(27) << left << c.motivo << " | "
                 << setw(11) << left << c.estado << " |" << endl;
        }
    }
    cout << "+-----+----------+----------+------------+-------+---------------------------+-------------+" << endl;
}

