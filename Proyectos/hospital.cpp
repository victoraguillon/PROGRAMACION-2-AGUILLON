
// SISTEMA DE GESTION HOSPITALARIA - VERSIÓN 2.0
// TEMA: PERSISTENCIA, ARCHIVOS BINARIOS, ACCESO ALEATORIO
// Hecho por: Victor M. Aguillon.
// Bibliotecas usadas: <iostream>, <cstring>, <ctime>, <iomanip>, <fstream>, <cstdio>, <direct.h>
// ================================================================

#include <iostream>
#include <fstream>      // Para fstream (lectura/escritura de archivos)
#include <cstring>      
#include <ctime>        
#include <cstdlib>      
#include <iomanip>      
#include <cstdio>       
#include <direct.h>     
using namespace std;

// ----------------------------
// CONSTANTES GLOBALES
// ----------------------------

// Nombres de los archivos binarios
const char* HOSPITAL_BIN = "hospital.bin";
const char* PACIENTES_BIN = "pacientes.bin";
const char* DOCTORES_BIN = "doctores.bin";
const char* CITAS_BIN = "citas.bin";
const char* HISTORIALES_BIN = "historiales.bin";

// Versión del formato de archivos 
const int VERSION_ARCHIVOS = 1;

// Constantes de tamaño
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
const int tamanio_FECHA = 11; // "YYYY-MM-DD"
const int tamanio_HORA = 6;   // "HH:MM"
const int tamanio_ALERGIAS = 500;

// ----------------------------
// ESTRUCTURAS
// ----------------------------

// Header 
struct ArchivoHeader {
    int cantidadRegistros;      // Cantidad total de registros escritos
    int proximoID;              // Siguiente ID a asignar
    int registrosActivos;       // Registros que NO están marcados como eliminados
    int version;                // Versión del formato (ej: 1)
};

// Estructura Hospital (SE MANTIENE EN MEMORIA)
struct Hospital {
    char nombre[100];
    char direccion[150];
    char telefono[15];
    char email[tamanio_EMAIL];
    int siguienteIDPaciente;
    int siguienteIDDoctor;
    int siguienteIDCita;
    int siguienteIDConsulta;
    int totalPacientesRegistrados;
    int totalDoctoresRegistrados;
    int totalCitasAgendadas;
    int totalConsultasRealizadas;
};

// Estructura de Historial Médico 
struct HistorialMedico {
    int id;
    int pacienteID;                
    char fecha[tamanio_FECHA];
    char hora[tamanio_HORA];
    char diagnostico[tamanio_DIAGNOSTICO];
    char tratamiento[tamanio_TRATAMIENTO];
    char medicamentos[tamanio_MEDICAMENTOS];
    int doctorID;
    float costo;
    int siguienteConsultaID;           
    // Metadata: Datos de los datos
    bool eliminado;
    time_t fechaRegistro;
};

// Estructura de Paciente
struct Paciente {
    int id;
    char nombre[tamanio_NOMBRE];
    char apellido[tamanio_APELLIDO];
    char cedula[tamanio_CEDULA];
    int edad;
    char sexo;
    char tipoSangre[tamanio_TIPO_SANGRE];
    char telefono[tamanio_TELEFONO];
    char direccion[tamanio_DIRECCION];
    char email[tamanio_EMAIL];
    char alergias[tamanio_ALERGIAS];
    char observaciones[tamanio_ALERGIAS]; 
    bool activo; 
    int cantidadConsultas;
    int primerConsultaID;          
    int cantidadCitas;
    int citasIDs[20];               
    
    // Metadata de registro
    bool eliminado;                 // Para borrado lógico
    time_t fechaCreacion;
    time_t fechaModificacion;
};

// Estructura de Doctor
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
    bool disponible;
    int cantidadPacientes;
    int pacientesIDs[50];           // Máximo 50 pacientes
    int cantidadCitas;
    int citasIDs[30];               // Máximo 30 citas
    
    // Metadata
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaModificacion;
};

// Estructura de Cita
struct Cita {
    int id;
    int pacienteID;
    int doctorID;
    char fecha[tamanio_FECHA];
    char hora[tamanio_HORA];
    char motivo[150];
    char estado[tamanio_ESTADO];    
    char observaciones[tamanio_OBSERVACIONES];
    bool atendida;
    int consultaID;                
    
    // Metadata
    bool eliminado;
    time_t fechaCreacion;
    time_t fechaModificacion;
};


// ---------------------------------
// HELPERS Y VALIDACIONES (PROYECTO 1)
// ---------------------------------

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

bool validarEmail(const char* email) {
    bool tieneArroba = false, tienePunto = false;
    int len = (int)strlen(email);
    if (len == 0) return true; // Permitir email vacío
    for (int i = 0; i < len; i++) {
        if (email[i] == '@') tieneArroba = true;
        if (email[i] == '.') tienePunto = true;
    }
    return tieneArroba && tienePunto;
}

// ---------------------------------
// HELPERS DE ENTRADA 
// ---------------------------------

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
            cout << "Error: entrada no valida. Intente de nuevo.\n";
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
                cout << "Error: entrada no valida. Intente de nuevo.\n";
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
            cout << "Error: no puede estar vacio. Intente de nuevo.\n";
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
                if (s == 'M' || s == 'F' || s == 'm' || s == 'f') {
                    return (s == 'm' ? 'M' : (s == 'f' ? 'F' : s));
                }
                cout << "Error: sexo invalido. Use 'M' o 'F'.\n";
        } else {
            cout << "Error: entrada no valida.\n";
            limpiarEntrada();
        }
    }
}

void leerFechaSegura(const char* prompt, char* destino, int tamMax) {
    while (true) {
        leerCadenaSeguro(prompt, destino, tamMax);
        if (!validarFecha(destino)) {
            cout << "Error: Fecha invalida. Formato YYYY-MM-DD.\n";
            continue;
        }
        return;
    }
}

void leerHoraSegura(const char* prompt, char* destino, int tamMax) {
    while (true) {
        leerCadenaSeguro(prompt, destino, tamMax);
        if (!validarHora(destino)) {
            cout << "Error: Hora invalida. Formato HH:MM.\n";
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
            cout << "Entrada no valida.\n";
        }
    }
}


// ------------------------------------------
// NUEVAS FUNCIONES DE ARCHIVOS (PROYECTO 2)
// ------------------------------------------
bool leerHeader(const char* nombreArchivo, ArchivoHeader& header) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        cout << "Error critico: No se pudo abrir " << nombreArchivo << " para leer header.\n";
        return false;
    }
    
    archivo.read((char*)&header, sizeof(ArchivoHeader));
    if (archivo.fail()) {
        cout << "Error critico: No se pudo leer el header de " << nombreArchivo << ".\n";
        archivo.close();
        return false;
    }
    
    archivo.close();
    return true;
}


bool actualizarHeader(const char* nombreArchivo, const ArchivoHeader& header) {
    fstream archivo(nombreArchivo, ios::binary | ios::in | ios::out);
    if (!archivo) {
        cout << "Error critico: No se pudo abrir " << nombreArchivo << " para actualizar header.\n";
        return false;
    }
    archivo.seekp(0, ios::beg); 
    archivo.write((char*)&header, sizeof(ArchivoHeader));
    if (archivo.fail()) {
        cout << "Error critico: No se pudo escribir el header de " << nombreArchivo << ".\n";
        archivo.close();
        return false;
    }
    
    archivo.close();
    return true;
}

long calcularPosicion(int indice, int sizeRegistro) {
    return (long)sizeof(ArchivoHeader) + ((long)indice * sizeRegistro);
}
// ------------------------------------------
// VALIDACIÓN DE CÉDULA (PROYECTO 2)
// ------------------------------------------

bool validarCedulaUnica(const char* cedula) {
    ifstream archivoPac(PACIENTES_BIN, ios::binary);
    if (archivoPac) {
        ArchivoHeader hPac;
        archivoPac.read((char*)&hPac, sizeof(ArchivoHeader));
        
        Paciente p;
        for (int i = 0; i < hPac.cantidadRegistros; i++) {
            archivoPac.read((char*)&p, sizeof(Paciente));
            if (!p.eliminado && strcmp(p.cedula, cedula) == 0) {
                cout << "Error: Ya existe un PACIENTE con la cedula " << cedula << endl;
                archivoPac.close();
                return false;
            }
        }
        archivoPac.close();
    }


    ifstream archivoDoc(DOCTORES_BIN, ios::binary);
    if (archivoDoc) {
        ArchivoHeader hDoc;
        archivoDoc.read((char*)&hDoc, sizeof(ArchivoHeader));
        
        Doctor d;
        for (int i = 0; i < hDoc.cantidadRegistros; i++) {
            archivoDoc.read((char*)&d, sizeof(Doctor));
            if (!d.eliminado && strcmp(d.cedula, cedula) == 0) {
                cout << "Error: Ya existe un DOCTOR con la cedula " << cedula << endl;
                archivoDoc.close();
                return false;
            }
        }
        archivoDoc.close();
    }
        return true;
}

// ------------------------------------------
// FUNCIONES CRUD PACIENTES (PROYECTO 2)
// ------------------------------------------

int buscarIndicePaciente(int id) {
    ifstream archivo(PACIENTES_BIN, ios::binary);
    if (!archivo) return -1;
    
    ArchivoHeader h;
    archivo.read((char*)&h, sizeof(ArchivoHeader));
    
    Paciente p;
    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read((char*)&p, sizeof(Paciente));
        if (archivo.fail()) break;
        
        if (p.id == id && !p.eliminado) {
            archivo.close();
            return i; 
        }
    }
    
    archivo.close();
    return -1; 
}


bool leerPacientePorIndice(int indice, Paciente& p) {
    ifstream archivo(PACIENTES_BIN, ios::binary);
    if (!archivo) return false;
    
    long pos = calcularPosicion(indice, sizeof(Paciente));
    archivo.seekg(pos);
    
    archivo.read((char*)&p, sizeof(Paciente));
    if (archivo.fail()) {
        archivo.close();
        return false;
    }
    
    archivo.close();
    return true;
}

bool actualizarPacientePorIndice(int indice, const Paciente& p) {
    fstream archivo(PACIENTES_BIN, ios::binary | ios::in | ios::out);
    if (!archivo) return false;
    
    long pos = calcularPosicion(indice, sizeof(Paciente));
    archivo.seekp(pos);
    
    archivo.write((char*)&p, sizeof(Paciente));
    if (archivo.fail()) {
        archivo.close();
        return false;
    }
    
    archivo.close();
    return true;
}


bool buscarPacientePorID(int id, Paciente& p) {
    int indice = buscarIndicePaciente(id);
    if (indice == -1) {
        return false;
    }
    return leerPacientePorIndice(indice, p);
}


bool buscarPacientePorCedula(const char* cedula, Paciente& p) {
    ifstream archivo(PACIENTES_BIN, ios::binary);
    if (!archivo) return false;
    
    ArchivoHeader h;
    archivo.read((char*)&h, sizeof(ArchivoHeader));
    
    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read((char*)&p, sizeof(Paciente));
        if (archivo.fail()) break;
        
        if (strcmp(p.cedula, cedula) == 0 && !p.eliminado) {
            archivo.close();
            return true; 
        }
    }
    
    archivo.close();
    return false; 
}

bool guardarNuevoPaciente(const Paciente& p) {
    ArchivoHeader h;
    if (!leerHeader(PACIENTES_BIN, h)) return false;

    fstream archivo(PACIENTES_BIN, ios::binary | ios::in | ios::out);
    if (!archivo) return false;
    archivo.seekp(0, ios::end);
    archivo.write((char*)&p, sizeof(Paciente));
    if (archivo.fail()) {
        cout << "Error: Falla al escribir nuevo paciente en .bin\n";
        archivo.close();
        return false;
    }
    h.cantidadRegistros++;
    h.registrosActivos++;
    h.proximoID = p.id + 1;
    

    archivo.close();
    if (!actualizarHeader(PACIENTES_BIN, h)) {
        cout << "Error: Falla al actualizar header de pacientes.bin\n";
        return false;
    }

    return true;
}

void listarPacientes() {
    ifstream archivo(PACIENTES_BIN, ios::binary);
    if (!archivo) {
        cout << "No se pudo abrir " << PACIENTES_BIN << ". ¿Archivo no inicializado?\n";
        return;
    }
    
    ArchivoHeader h;
    archivo.read((char*)&h, sizeof(ArchivoHeader));
    if (archivo.fail() || h.version != VERSION_ARCHIVOS) {
        cout << "Archivo de pacientes invalido o version incorrecta.\n";
        archivo.close();
        return;
    }
    
    cout << "\n--- LISTA DE PACIENTES ACTIVOS (" << h.registrosActivos << ") ---\n";
    cout << "+-----+------------------------------+--------------+------+-----------+" << endl;
    cout << "| ID  | Nombre completo              | Cedula       | Sexo | Consultas |" << endl;
    cout << "+-----+------------------------------+--------------+------+-----------+" << endl;
    
    Paciente p;
    int contadorActivos = 0;
    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read((char*)&p, sizeof(Paciente));
        if (archivo.fail()) break;
        
        if (!p.eliminado) {
            contadorActivos++;
            char nombreCompleto[101];
            snprintf(nombreCompleto, 101, "%s %s", p.nombre, p.apellido);
            
            cout << "| " << setw(3) << right << p.id << " | "
                 << setw(28) << left << nombreCompleto << " | "
                 << setw(12) << left << p.cedula << " | "
                 << setw(4) << left << p.sexo << " | "
                 << setw(9) << right << p.cantidadConsultas << " |" << endl;
        }
    }
    
    cout << "+-----+------------------------------+--------------+------+-----------+" << endl;
    if (contadorActivos != h.registrosActivos) {
        cout << "Advertencia: Inconsistencia en conteo de registros activos.\n";
    }
    
    archivo.close();
}


// ------------------------------------------
// FUNCIONES CRUD DOCTORES (PROYECTO 2)
// ------------------------------------------

int buscarIndiceDoctor(int id) {
    ifstream archivo(DOCTORES_BIN, ios::binary);
    if (!archivo) return -1;
    ArchivoHeader h;
    archivo.read((char*)&h, sizeof(ArchivoHeader));
    Doctor d;
    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read((char*)&d, sizeof(Doctor));
        if (archivo.fail()) break;
        if (d.id == id && !d.eliminado) {
            archivo.close();
            return i;
        }
    }
    archivo.close();
    return -1;
}

bool leerDoctorPorIndice(int indice, Doctor& d) {
    ifstream archivo(DOCTORES_BIN, ios::binary);
    if (!archivo) return false;
    long pos = calcularPosicion(indice, sizeof(Doctor));
    archivo.seekg(pos);
    archivo.read((char*)&d, sizeof(Doctor));
    bool exito = !archivo.fail();
    archivo.close();
    return exito;
}

bool actualizarDoctorPorIndice(int indice, const Doctor& d) {
    fstream archivo(DOCTORES_BIN, ios::binary | ios::in | ios::out);
    if (!archivo) return false;
    long pos = calcularPosicion(indice, sizeof(Doctor));
    archivo.seekp(pos);
    archivo.write((char*)&d, sizeof(Doctor));
    bool exito = !archivo.fail();
    archivo.close();
    return exito;
}

bool buscarDoctorPorID(int id, Doctor& d) {
    int indice = buscarIndiceDoctor(id);
    if (indice == -1) return false;
    return leerDoctorPorIndice(indice, d);
}

bool guardarNuevoDoctor(const Doctor& d) {
    ArchivoHeader h;
    if (!leerHeader(DOCTORES_BIN, h)) return false;
    fstream archivo(DOCTORES_BIN, ios::binary | ios::in | ios::out);
    if (!archivo) return false;
    archivo.seekp(0, ios::end);
    archivo.write((char*)&d, sizeof(Doctor));
    if (archivo.fail()) {
        archivo.close();
        return false;
    }
    h.cantidadRegistros++;
    h.registrosActivos++;
    h.proximoID = d.id + 1;
    archivo.close();
    return actualizarHeader(DOCTORES_BIN, h);
}

void listarDoctores() {
    ifstream archivo(DOCTORES_BIN, ios::binary);
    if (!archivo) {
        cout << "No se pudo abrir " << DOCTORES_BIN << ".\n";
        return;
    }
    
    ArchivoHeader h;
    archivo.read((char*)&h, sizeof(ArchivoHeader));
    if (archivo.fail() || h.version != VERSION_ARCHIVOS) {
        cout << "Archivo de doctores invalido.\n";
        archivo.close();
        return;
    }
    
    cout << "\n--- LISTA DE DOCTORES ACTIVOS (" << h.registrosActivos << ") ---\n";
    cout << "+-----+------------------------------+----------------------+------------+" << endl;
    cout << "| ID  | Nombre completo              | Especialidad         | Costo      |" << endl;
    cout << "+-----+------------------------------+----------------------+------------+" << endl;
    
    Doctor d;
    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read((char*)&d, sizeof(Doctor));
        if (archivo.fail()) break;
        
        if (!d.eliminado) {
            char nombreCompleto[101];
            snprintf(nombreCompleto, 101, "%s %s", d.nombre, d.apellido);
            
            cout << "| " << setw(3) << right << d.id << " | "
                 << setw(28) << left << nombreCompleto << " | "
                 << setw(20) << left << d.especialidad << " | "
                 << setw(10) << right << fixed << setprecision(2) << d.costoConsulta << " |" << endl;
        }
    }
    
    cout << "+-----+------------------------------+----------------------+------------+" << endl;
    archivo.close();
}


// ------------------------------------------
// FUNCIONES CRUD CITAS (PROYECTO 2)
// ------------------------------------------

int buscarIndiceCita(int id) {
    ifstream archivo(CITAS_BIN, ios::binary);
    if (!archivo) return -1;
    ArchivoHeader h;
    archivo.read((char*)&h, sizeof(ArchivoHeader));
    Cita c;
    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read((char*)&c, sizeof(Cita));
        if (archivo.fail()) break;
        if (c.id == id && !c.eliminado) {
            archivo.close();
            return i;
        }
    }
    archivo.close();
    return -1;
}

bool leerCitaPorIndice(int indice, Cita& c) {
    ifstream archivo(CITAS_BIN, ios::binary);
    if (!archivo) return false;
    long pos = calcularPosicion(indice, sizeof(Cita));
    archivo.seekg(pos);
    archivo.read((char*)&c, sizeof(Cita));
    bool exito = !archivo.fail();
    archivo.close();
    return exito;
}

bool actualizarCitaPorIndice(int indice, const Cita& c) {
    fstream archivo(CITAS_BIN, ios::binary | ios::in | ios::out);
    if (!archivo) return false;
    long pos = calcularPosicion(indice, sizeof(Cita));
    archivo.seekp(pos);
    archivo.write((char*)&c, sizeof(Cita));
    bool exito = !archivo.fail();
    archivo.close();
    return exito;
}

bool buscarCitaPorID(int id, Cita& c) {
    int indice = buscarIndiceCita(id);
    if (indice == -1) return false;
    return leerCitaPorIndice(indice, c);
}

bool guardarNuevaCita(const Cita& c) {
    ArchivoHeader h;
    if (!leerHeader(CITAS_BIN, h)) return false;
    fstream archivo(CITAS_BIN, ios::binary | ios::in | ios::out);
    if (!archivo) return false;
    archivo.seekp(0, ios::end);
    archivo.write((char*)&c, sizeof(Cita));
    if (archivo.fail()) {
        archivo.close();
        return false;
    }
    h.cantidadRegistros++;
    h.registrosActivos++;
    h.proximoID = c.id + 1;
    archivo.close();
    return actualizarHeader(CITAS_BIN, h);
}


bool verificarDisponibilidadDoctor(int idDoctor, const char* fecha, const char* hora) {
    ifstream archivo(CITAS_BIN, ios::binary);
    if (!archivo) return true; 
    
    ArchivoHeader h;
    archivo.read((char*)&h, sizeof(ArchivoHeader));
    
    Cita c;
    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read((char*)&c, sizeof(Cita));
        if (archivo.fail()) break;
        
        if (!c.eliminado && c.doctorID == idDoctor &&
            strcmp(c.fecha, fecha) == 0 &&
            strcmp(c.hora, hora) == 0 &&
            strcmp(c.estado, "Agendada") == 0) 
        {
            archivo.close();
            return false; 
        }
    }
    
    archivo.close();
    return true; 
}

void listarCitasPendientes() {
    ifstream archivo(CITAS_BIN, ios::binary);
    if (!archivo) {
        cout << "No se pudo abrir " << CITAS_BIN << ".\n";
        return;
    }
    
    ArchivoHeader h;
    archivo.read((char*)&h, sizeof(ArchivoHeader));
    if (archivo.fail()) {
        archivo.close();
        return;
    }
    
    cout << "\n--- LISTA DE CITAS PENDIENTES (Agendadas) ---\n";
    cout << "+-----+----------+----------+------------+-------+---------------------------+" << endl;
    cout << "| ID  | ID Pac.  | ID Doc.  | Fecha      | Hora  | Motivo                    |" << endl;
    cout << "+-----+----------+----------+------------+-------+---------------------------+" << endl;
    
    Cita c;
    int contador = 0;
    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read((char*)&c, sizeof(Cita));
        if (archivo.fail()) break;
        
        if (!c.eliminado && strcmp(c.estado, "Agendada") == 0) {
            contador++;
            cout << "| " << setw(3) << right << c.id << " | "
                 << setw(8) << right << c.pacienteID << " | "
                 << setw(8) << right << c.doctorID << " | "
                 << setw(10) << left << c.fecha << " | "
                 << setw(5) << left << c.hora << " | "
                 << setw(25) << left << c.motivo << " |" << endl;
        }
    }
    
    cout << "+-----+----------+----------+------------+-------+---------------------------+" << endl;
    if (contador == 0) {
        cout << "No hay citas pendientes.\n";
    }
    archivo.close();
}


// ------------------------------------------
// FUNCIONES CRUD HISTORIAL 
// ------------------------------------------

int buscarIndiceConsulta(int id) {
    ifstream archivo(HISTORIALES_BIN, ios::binary);
    if (!archivo) return -1;
    ArchivoHeader h;
    archivo.read((char*)&h, sizeof(ArchivoHeader));
    HistorialMedico hm;
    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read((char*)&hm, sizeof(HistorialMedico));
        if (archivo.fail()) break;
        if (hm.id == id && !hm.eliminado) {
            archivo.close();
            return i;
        }
    }
    archivo.close();
    return -1;
}

bool leerConsultaPorIndice(int indice, HistorialMedico& hm) {
    ifstream archivo(HISTORIALES_BIN, ios::binary);
    if (!archivo) return false;
    long pos = calcularPosicion(indice, sizeof(HistorialMedico));
    archivo.seekg(pos);
    archivo.read((char*)&hm, sizeof(HistorialMedico));
    bool exito = !archivo.fail();
    archivo.close();
    return exito;
}

bool actualizarConsultaPorIndice(int indice, const HistorialMedico& hm) {
    fstream archivo(HISTORIALES_BIN, ios::binary | ios::in | ios::out);
    if (!archivo) return false;
    long pos = calcularPosicion(indice, sizeof(HistorialMedico));
    archivo.seekp(pos);
    archivo.write((char*)&hm, sizeof(HistorialMedico));
    bool exito = !archivo.fail();
    archivo.close();
    return exito;
}

bool buscarConsultaPorID(int id, HistorialMedico& hm) {
    int indice = buscarIndiceConsulta(id);
    if (indice == -1) return false;
    return leerConsultaPorIndice(indice, hm);
}

bool guardarNuevaConsulta(const HistorialMedico& hm) {
    ArchivoHeader h;
    if (!leerHeader(HISTORIALES_BIN, h)) return false;
    fstream archivo(HISTORIALES_BIN, ios::binary | ios::in | ios::out);
    if (!archivo) return false;
    archivo.seekp(0, ios::end);
    archivo.write((char*)&hm, sizeof(HistorialMedico));
    if (archivo.fail()) {
        archivo.close();
        return false;
    }
    h.cantidadRegistros++;
    h.registrosActivos++;
    h.proximoID = hm.id + 1;
    archivo.close();
    return actualizarHeader(HISTORIALES_BIN, h);
}

void mostrarHistorialMedico(int pacienteID) {
    Paciente p;
    if (!buscarPacientePorID(pacienteID, p)) {
        cout << "Error: Paciente no encontrado.\n";
        return;
    }
    
    cout << "\n--- HISTORIAL MEDICO DE " << p.nombre << " " << p.apellido << " (" << p.cantidadConsultas << " consultas) ---\n";
    
    if (p.primerConsultaID == -1) {
        cout << "El paciente no tiene consultas registradas.\n";
        return;
    }
    
    cout << "+-----+------------+-------+---------------------------+------------+--------+" << endl;
    cout << "| ID  |   Fecha    | Hora  | Diagnostico               | ID Doctor  | Costo  |" << endl;
    cout << "+-----+------------+-------+---------------------------+------------+--------+" << endl;
    
    int consultaActualID = p.primerConsultaID;
    HistorialMedico hm;
    
    while (consultaActualID != -1) {
        if (buscarConsultaPorID(consultaActualID, hm)) {
            if (!hm.eliminado) {
                cout << "| " << setw(3) << right << hm.id << " | "
                     << setw(10) << left << hm.fecha << " | "
                     << setw(5) << left << hm.hora << " | "
                     << setw(25) << left << hm.diagnostico << " | "
                     << setw(10) << right << hm.doctorID << " | "
                     << setw(6) << right << fixed << setprecision(2) << hm.costo << " |" << endl;
            }
            consultaActualID = hm.siguienteConsultaID;
        } else {
            cout << "Error: Se rompio la cadena del historial. ID " << consultaActualID << " no encontrado.\n";
            break;
        }
    }
    
    cout << "+-----+------------+-------+---------------------------+------------+--------+" << endl;
}

/**
 * Registra un nuevo paciente, actualiza contadores y guarda en disco.
 */
void registrarPaciente(Hospital& h) {
    char nombre[tamanio_NOMBRE], apellido[tamanio_APELLIDO], cedula[tamanio_CEDULA];
    int edad;
    char sexo;
    
    leerCadenaSeguro("Nombre: ", nombre, tamanio_NOMBRE);
    leerCadenaSeguro("Apellido: ", apellido, tamanio_APELLIDO);
    
    while (true) {
        leerCadenaSeguro("Cedula: ", cedula, tamanio_CEDULA);
        if (strlen(cedula) > 8) {
            cout << "Error: La cedula debe tener maximo 8 digitos.\n";
            continue;
        }
        if (validarCedulaUnica(cedula)) {
            break;
        }
        if (!leerConfirmacion("La cedula ya existe. Desea intentar con otra?")) {
            cout << "Registro cancelado.\n";
            return;
        }
    }
    
    edad = leerEnteroSeguro("Edad (0-120): ", 0, 120);
    sexo = leerSexoSeguro("Sexo (M/F): ");

    Paciente p;
    p.id = h.siguienteIDPaciente; 
    strncpy(p.nombre, nombre, tamanio_NOMBRE);
    strncpy(p.apellido, apellido, tamanio_APELLIDO);
    strncpy(p.cedula, cedula, tamanio_CEDULA);
    p.edad = edad;
    p.sexo = sexo;
    p.activo = true;
    p.eliminado = false;
    p.fechaCreacion = time(0);
    p.fechaModificacion = time(0);
    

    leerCadenaSeguro("Telefono: ", p.telefono, tamanio_TELEFONO);

 
    while (true) {
        leerCadenaSeguro("Email (opcional): ", p.email, tamanio_EMAIL, true);
        if (validarEmail(p.email)) break;
        cout << "Email invalido. Ingrese un email valido o deje vacio.\n";
    }

    p.tipoSangre[0] = '\0';
    p.direccion[0] = '\0';
    p.alergias[0] = '\0';
    p.observaciones[0] = '\0';
    
    p.cantidadConsultas = 0;
    p.primerConsultaID = -1;
    p.cantidadCitas = 0;
    

    if (guardarNuevoPaciente(p)) {
        cout << "Paciente registrado exitosamente con ID " << p.id << ".\n";
        h.siguienteIDPaciente++;
        h.totalPacientesRegistrados++;
    } else {
        cout << "Error: No se pudo guardar el paciente en el archivo.\n";
    }
}

void registrarDoctor(Hospital& h) {
    char nombre[tamanio_NOMBRE], apellido[tamanio_APELLIDO], cedula[tamanio_CEDULA], especialidad[tamanio_ESPECIALIDAD];
    int anios;
    float costo;
    
    leerCadenaSeguro("Nombre: ", nombre, tamanio_NOMBRE);
    leerCadenaSeguro("Apellido: ", apellido, tamanio_APELLIDO);
    
    while (true) {
        leerCadenaSeguro("Cedula: ", cedula, tamanio_CEDULA);
        if (strlen(cedula) > 8) {
            cout << "Error: La cedula debe tener maximo 8 digitos.\n";
            continue;
        }
        if (validarCedulaUnica(cedula)) {
            break;
        }
        if (!leerConfirmacion("La cedula ya existe. Desea intentar con otra?")) {
            cout << "Registro cancelado.\n";
            return;
        }
    }
    
    leerCadenaSeguro("Especialidad: ", especialidad, tamanio_ESPECIALIDAD);
    anios = leerEnteroSeguro("Anios de experiencia: ", 0, 100);
    costo = leerFloatSeguro("Costo consulta (>0): ", 0.01f, 100000.0f);
    
    Doctor d;
    d.id = h.siguienteIDDoctor;
    strncpy(d.nombre, nombre, tamanio_NOMBRE);
    strncpy(d.apellido, apellido, tamanio_APELLIDO);
    strncpy(d.cedula, cedula, tamanio_CEDULA);
    strncpy(d.especialidad, especialidad, tamanio_ESPECIALIDAD);
    d.aniosExperiencia = anios;
    d.costoConsulta = costo;
    
    d.disponible = true;
    d.eliminado = false;
    d.fechaCreacion = time(0);
    d.fechaModificacion = time(0);
    
    d.horarioAtencion[0] = '\0';
    d.telefono[0] = '\0';
    d.email[0] = '\0';
    
    d.cantidadPacientes = 0;
    d.cantidadCitas = 0;
    
    if (guardarNuevoDoctor(d)) {
        cout << "Doctor registrado exitosamente con ID " << d.id << ".\n";
        h.siguienteIDDoctor++;
        h.totalDoctoresRegistrados++;
    } else {
        cout << "Error: No se pudo guardar el doctor en el archivo.\n";
    }
}

void agendarCita(Hospital& h) {
    // Usamos un rango amplio para la entrada y validamos existencia después.
    int idPaciente = leerEnteroSeguro("ID paciente: ", 1, 1000000);
    int idDoctor = leerEnteroSeguro("ID doctor: ", 1, 1000000);
    
    Paciente p;
    Doctor d;
    
    if (!buscarPacientePorID(idPaciente, p)) {
        cout << "Error: Paciente con ID " << idPaciente << " no existe o fue eliminado.\n";
        return;
    }
    if (!buscarDoctorPorID(idDoctor, d)) {
        cout << "Error: Doctor con ID " << idDoctor << " no existe o fue eliminado.\n";
        return;
    }
    
    // Verificar espacio en arrays fijos
    if (p.cantidadCitas >= 20) {
        cout << "Error: El paciente ya tiene el maximo de citas (20).\n";
        return;
    }
    if (d.cantidadCitas >= 30) {
        cout << "Error: El doctor ya tiene el maximo de citas (30).\n";
        return;
    }
    
    char fecha[tamanio_FECHA], hora[tamanio_HORA], motivo[150];
    leerFechaSegura("Fecha (YYYY-MM-DD): ", fecha, tamanio_FECHA);
    
    while (true) {
        leerHoraSegura("Hora (HH:MM): ", hora, tamanio_HORA);
        if (verificarDisponibilidadDoctor(idDoctor, fecha, hora)) {
            break;
        }
        cout << "Error: El doctor no esta disponible en esa fecha/hora.\n";
        if (!leerConfirmacion("Desea intentar con otra hora?")) {
            cout << "Agendamiento cancelado.\n";
            return;
        }
    }
    
    leerCadenaSeguro("Motivo: ", motivo, 150);
    
    Cita c;
    c.id = h.siguienteIDCita;
    c.pacienteID = idPaciente;
    c.doctorID = idDoctor;
    strncpy(c.fecha, fecha, tamanio_FECHA);
    strncpy(c.hora, hora, tamanio_HORA);
    strncpy(c.motivo, motivo, 150);
    strncpy(c.estado, "Agendada", tamanio_ESTADO);
    c.observaciones[0] = '\0';
    c.atendida = false;
    c.consultaID = -1;
    c.eliminado = false;
    c.fechaCreacion = time(0);
    c.fechaModificacion = time(0);
    
    if (guardarNuevaCita(c)) {
        cout << "Cita agendada exitosamente con ID " << c.id << ".\n";
        h.siguienteIDCita++;
        h.totalCitasAgendadas++;
        
        // Actualizar relaciones en Paciente y Doctor
        p.citasIDs[p.cantidadCitas] = c.id;
        p.cantidadCitas++;
        
        d.citasIDs[d.cantidadCitas] = c.id;
        d.cantidadCitas++;
        
        int indiceP = buscarIndicePaciente(p.id);
        int indiceD = buscarIndiceDoctor(d.id);
        
        actualizarPacientePorIndice(indiceP, p);
        actualizarDoctorPorIndice(indiceD, d);
        
    } else {
        cout << "Error: No se pudo guardar la cita en el archivo.\n";
    }
}

/**
 * Atiende una cita, crea la consulta, y enlaza todo en disco.
 */
void atenderCita(Hospital& h) {
    int idCita = leerEnteroSeguro("ID de la cita a atender: ", 1, h.siguienteIDCita - 1);
    
    Cita c;
    int indiceCita = buscarIndiceCita(idCita);
    if (indiceCita == -1) {
        cout << "Error: Cita no encontrada.\n";
        return;
    }
    leerCitaPorIndice(indiceCita, c);
    
    if (strcmp(c.estado, "Atendida") == 0) {
        cout << "Error: Esta cita ya fue atendida.\n";
        return;
    }
    if (strcmp(c.estado, "Cancelada") == 0) {
        cout << "Error: Esta cita fue cancelada.\n";
        return;
    }
    
    Paciente p;
    Doctor d;
    if (!buscarPacientePorID(c.pacienteID, p) || !buscarDoctorPorID(c.doctorID, d)) {
        cout << "Error: No se encontro el paciente o doctor asociado.\n";
        return;
    }
    
    cout << "Atendiendo Cita ID " << c.id << " (Paciente: " << p.nombre << ", Doctor: " << d.nombre << ")\n";
    char diag[tamanio_DIAGNOSTICO], trat[tamanio_TRATAMIENTO], meds[tamanio_MEDICAMENTOS];
    leerCadenaSeguro("Diagnostico: ", diag, tamanio_DIAGNOSTICO);
    leerCadenaSeguro("Tratamiento: ", trat, tamanio_TRATAMIENTO);
    leerCadenaSeguro("Medicamentos: ", meds, tamanio_MEDICAMENTOS, true);
    
    // 1. Crear la nueva consulta
    HistorialMedico hm;
    hm.id = h.siguienteIDConsulta;
    hm.pacienteID = p.id;
    strncpy(hm.fecha, c.fecha, tamanio_FECHA);
    strncpy(hm.hora, c.hora, tamanio_HORA);
    strncpy(hm.diagnostico, diag, tamanio_DIAGNOSTICO);
    strncpy(hm.tratamiento, trat, tamanio_TRATAMIENTO);
    strncpy(hm.medicamentos, meds, tamanio_MEDICAMENTOS);
    hm.doctorID = d.id;
    hm.costo = d.costoConsulta;
    hm.siguienteConsultaID = -1; // Es la última por ahora
    hm.eliminado = false;
    hm.fechaRegistro = time(0);
    
    // 2. Guardar la nueva consulta en historiales.bin
    if (!guardarNuevaConsulta(hm)) {
        cout << "Error critico: No se pudo guardar la consulta.\n";
        return;
    }
    
    // 3. Enlazar la consulta con el paciente (y consultas anteriores)
    if (p.primerConsultaID == -1) {
        // Es la primera consulta
        p.primerConsultaID = hm.id;
    } else {
        // Buscar la última consulta para enlazarla
        int idActual = p.primerConsultaID;
        HistorialMedico consultaAnterior;
        while (idActual != -1) {
            if (buscarConsultaPorID(idActual, consultaAnterior)) {
                if (consultaAnterior.siguienteConsultaID == -1) {
                    // Encontramos la última, la actualizamos
                    consultaAnterior.siguienteConsultaID = hm.id;
                    int indiceAnt = buscarIndiceConsulta(consultaAnterior.id);
                    actualizarConsultaPorIndice(indiceAnt, consultaAnterior);
                    break;
                }
                idActual = consultaAnterior.siguienteConsultaID;
            } else {
                cout << "Error: Se rompio la cadena del historial al buscar la ultima consulta.\n";
                break; // Salir del bucle si hay un error
            }
        }
    }
    
    // 4. Actualizar el estado de la cita
    strncpy(c.estado, "Atendida", tamanio_ESTADO);
    c.atendida = true;
    c.consultaID = hm.id; // Enlazar cita con la consulta
    c.fechaModificacion = time(0);
    actualizarCitaPorIndice(indiceCita, c);
    
    // 5. Actualizar el paciente (contador de consultas)
    p.cantidadConsultas++;
    int indiceP = buscarIndicePaciente(p.id);
    actualizarPacientePorIndice(indiceP, p);
    
    // 6. Actualizar contadores del hospital
    h.siguienteIDConsulta++;
    h.totalConsultasRealizadas++;
    
    cout << "Cita atendida y registrada en historial (ID consulta " << hm.id << ")." << endl;
}

/**
 * Marca un paciente como eliminado (borrado lógico).
 */
void eliminarPaciente(Hospital& h) {
    int id = leerEnteroSeguro("ID del paciente a eliminar: ", 1, 1000000);

    int indice = buscarIndicePaciente(id);
    if (indice == -1) {
        cout << "Error: Paciente no encontrado.\n";
        return;
    }

    Paciente p;
    leerPacientePorIndice(indice, p);

    // Mostrar confirmación con nombre y ID
    char msg[200];
    snprintf(msg, sizeof(msg), "Esta seguro que desea eliminar a %s %s (ID %d)?", p.nombre, p.apellido, p.id);
    if (leerConfirmacion(msg)) {
        p.eliminado = true;
        p.activo = false;
        p.fechaModificacion = time(0);
        
        if (actualizarPacientePorIndice(indice, p)) {
            cout << "Paciente eliminado logicamente.\n";
            h.totalPacientesRegistrados--;
            
            // Actualizar header de pacientes
            ArchivoHeader hPac;
            if (leerHeader(PACIENTES_BIN, hPac)) {
                hPac.registrosActivos--;
                actualizarHeader(PACIENTES_BIN, hPac);
            }
        } else {
            cout << "Error: No se pudo actualizar el registro del paciente.\n";
        }
    } else {
        cout << "Eliminacion cancelada.\n";
    }
}


// ------------------------------------------
// FUNCIONES DE INICIALIZACIÓN Y GUARDADO
// ------------------------------------------


bool inicializarArchivo(const char* nombreArchivo, int version) {
    ofstream archivo(nombreArchivo, ios::binary | ios::trunc);
    if (!archivo) {
        cout << "Error fatal: No se pudo crear el archivo " << nombreArchivo << endl;
        return false;
    }
    
    ArchivoHeader h;
    h.cantidadRegistros = 0;
    h.proximoID = 1;
    h.registrosActivos = 0;
    h.version = version;
    
    archivo.write((char*)&h, sizeof(ArchivoHeader));
    if (archivo.fail()) {
        cout << "Error fatal: No se pudo escribir el header en " << nombreArchivo << endl;
        archivo.close();
        return false;
    }
    
    archivo.close();
    cout << "Archivo " << nombreArchivo << " inicializado.\n";
    return true;
}

/**
 * Carga los datos de hospital.bin en la variable 'h'.
 * Si los archivos no existen, los inicializa.
 */
bool cargarDatosHospital(Hospital& h) {
    ifstream archivo(HOSPITAL_BIN, ios::binary);
    
    if (!archivo) {
        cout << "Advertencia: No se encontro " << HOSPITAL_BIN << ". Iniciando configuracion...\n";
        
      
        leerCadenaSeguro("Ingrese el nombre del hospital: ", h.nombre, 100);
        leerCadenaSeguro("Ingrese la direccion: ", h.direccion, 150);
    leerCadenaSeguro("Ingrese el telefono: ", h.telefono, 15);
    leerCadenaSeguro("Ingrese el email (opcional): ", h.email, sizeof(h.email), true);
        
      
        h.siguienteIDPaciente = 1;
        h.siguienteIDDoctor = 1;
        h.siguienteIDCita = 1;
        h.siguienteIDConsulta = 1;
        h.totalPacientesRegistrados = 0;
        h.totalDoctoresRegistrados = 0;
        h.totalCitasAgendadas = 0;
        h.totalConsultasRealizadas = 0;
        
        ofstream archivoOut(HOSPITAL_BIN, ios::binary | ios::trunc);
        if (!archivoOut) {
            cout << "Error fatal: No se pudo crear " << HOSPITAL_BIN << endl;
            return false;
        }
        archivoOut.write((char*)&h, sizeof(Hospital));
        archivoOut.close();
        
         if (!inicializarArchivo(PACIENTES_BIN, VERSION_ARCHIVOS) ||
            !inicializarArchivo(DOCTORES_BIN, VERSION_ARCHIVOS) ||
            !inicializarArchivo(CITAS_BIN, VERSION_ARCHIVOS) ||
            !inicializarArchivo(HISTORIALES_BIN, VERSION_ARCHIVOS))
        {
            cout << "Error fatal: Falla al inicializar archivos .bin.\n";
            return false;
        }
        
        cout << "Sistema inicializado correctamente.\n";
        return true;
        
    } else {
        // --- EJECUCIÓN NORMAL: CARGAR DATOS ---
        archivo.read((char*)&h, sizeof(Hospital));
        if (archivo.fail()) {
            cout << "Error fatal: " << HOSPITAL_BIN << " esta corrupto.\n";
            archivo.close();
            return false;
        }
        archivo.close();
        
       
        ArchivoHeader testHeader;
        if (!leerHeader(PACIENTES_BIN, testHeader) || testHeader.version != VERSION_ARCHIVOS) {
            cout << "Error fatal: " << PACIENTES_BIN << " esta corrupto o es de una version incompatible.\n";
            return false;
        }
        
        cout << "Datos del hospital cargados.\n";
        return true;
    }
}

bool guardarDatosHospital(const Hospital& h) {
    fstream archivo(HOSPITAL_BIN, ios::binary | ios::in | ios::out);
    if (!archivo) {
        cout << "Error fatal: No se pudo abrir " << HOSPITAL_BIN << " para guardar.\n";
        return false;
    }
    
    archivo.seekp(0, ios::beg);
    archivo.write((char*)&h, sizeof(Hospital));
    if (archivo.fail()) {
        cout << "Error fatal: No se pudo guardar " << HOSPITAL_BIN << ".\n";
        archivo.close();
        return false;
    }
    
    archivo.close();
    cout << "Datos del hospital y contadores guardados.\n";
    return true;
}

// FUNCIONES NUEVAS AÑADIDAS (COMPLEMENTO - PROYECTO 1)
void buscarDoctoresPorEspecialidad() {
    char especialidad[tamanio_ESPECIALIDAD];
    leerCadenaSeguro("Ingrese especialidad: ", especialidad, tamanio_ESPECIALIDAD);
    
    char espBuscada[tamanio_ESPECIALIDAD];
    toLowerCopy(especialidad, espBuscada, tamanio_ESPECIALIDAD);
    
    ifstream archivo(DOCTORES_BIN, ios::binary);
    if (!archivo) {
        cout << "No se pudo abrir el archivo de doctores.\n";
        return;
    }
    
    ArchivoHeader h;
    archivo.read((char*)&h, sizeof(ArchivoHeader));
    Doctor d;
    int encontrados = 0;
    
    cout << "\n--- Doctores encontrados con especialidad '" << especialidad << "' ---\n";
    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read((char*)&d, sizeof(Doctor));
        if (archivo.fail()) break;
        
        char espActual[tamanio_ESPECIALIDAD];
        toLowerCopy(d.especialidad, espActual, tamanio_ESPECIALIDAD);
        
        // strstr busca si la cadena buscada está contenida
        if (!d.eliminado && strstr(espActual, espBuscada) != NULL) {
            cout << "ID " << d.id << ": " << d.nombre << " " << d.apellido 
                 << " (Costo: " << d.costoConsulta << ")\n";
            encontrados++;
        }
    }
    
    if (encontrados == 0) {
        cout << "No se encontraron doctores con esa especialidad.\n";
    }
    archivo.close();
}


void asignarPacienteADoctor() {
    int idDoctor = leerEnteroSeguro("ID del doctor: ", 1, 1000000);
    int idPaciente = leerEnteroSeguro("ID del paciente: ", 1, 1000000);
    
    Doctor d;
    Paciente p;
    
    int indiceD = buscarIndiceDoctor(idDoctor);
    // Valida que ambos existan
    if (indiceD == -1 || !buscarPacientePorID(idPaciente, p)) {
        cout << "Error: Doctor o Paciente no encontrado.\n";
        return;
    }
    leerDoctorPorIndice(indiceD, d);
    
    if (d.cantidadPacientes >= 50) {
        cout << "Error: El doctor ya tiene el maximo de pacientes (50).\n";
        return;
    }
    
    for (int i = 0; i < d.cantidadPacientes; i++) {
        if (d.pacientesIDs[i] == idPaciente) {
            cout << "Advertencia: El paciente ya estaba asignado a este doctor.\n";
            return;
        }
    }
    
    // Asignar
    d.pacientesIDs[d.cantidadPacientes] = idPaciente;
    d.cantidadPacientes++;
    d.fechaModificacion = time(0);
    
    if (actualizarDoctorPorIndice(indiceD, d)) {
        cout << "Paciente " << p.nombre << " asignado al Dr. " << d.nombre << ".\n";
    } else {
        cout << "Error al actualizar el archivo del doctor.\n";
    }
}

void removerPacienteDeDoctor() {
    int idDoctor = leerEnteroSeguro("ID del doctor: ", 1, 1000000);
    int idPaciente = leerEnteroSeguro("ID del paciente: ", 1, 1000000);
    
    Doctor d;
    int indiceD = buscarIndiceDoctor(idDoctor);
    if (indiceD == -1) {
        cout << "Error: Doctor no encontrado.\n";
        return;
    }
    leerDoctorPorIndice(indiceD, d);

    int indiceP = -1; // Índice *dentro* del array pacientesIDs
    for (int i = 0; i < d.cantidadPacientes; i++) {
        if (d.pacientesIDs[i] == idPaciente) {
            indiceP = i;
            break;
        }
    }
    
    if (indiceP == -1) {
        cout << "Error: El paciente no estaba asignado a este doctor.\n";
        return;
    }
    
    // Compactar el array
    for (int i = indiceP; i < d.cantidadPacientes - 1; i++) {
        d.pacientesIDs[i] = d.pacientesIDs[i + 1];
    }
    
    d.cantidadPacientes--;
    d.fechaModificacion = time(0);
    
    if (actualizarDoctorPorIndice(indiceD, d)) {
        cout << "Paciente removido del doctor.\n";
    } else {
        cout << "Error al actualizar el archivo del doctor.\n";
    }
}

void listarPacientesDeDoctor() {
    int idDoctor = leerEnteroSeguro("ID del doctor: ", 1, 1000000);
    Doctor d;
    if (!buscarDoctorPorID(idDoctor, d)) {
        cout << "Error: Doctor no encontrado.\n";
        return;
    }
    
    cout << "\n--- Pacientes del Dr. " << d.nombre << " " << d.apellido << " (" << d.cantidadPacientes << ") ---\n";
    if (d.cantidadPacientes == 0) {
        cout << "No hay pacientes asignados.\n";
        return;
    }
    
    for (int i = 0; i < d.cantidadPacientes; i++) {
        Paciente p;
        if (buscarPacientePorID(d.pacientesIDs[i], p)) {
            cout << "  ID " << p.id << ": " << p.nombre << " " << p.apellido << " (Cedula: " << p.cedula << ")\n";
        } else {
            cout << "  ID " << d.pacientesIDs[i] << ": (Registro de paciente no encontrado o eliminado)\n";
        }
    }
}


void eliminarDoctor(Hospital& h) {
    int id = leerEnteroSeguro("ID del doctor a eliminar: ", 1, h.siguienteIDDoctor - 1);
    
    int indice = buscarIndiceDoctor(id);
    if (indice == -1) {
        cout << "Error: Doctor no encontrado.\n";
        return;
    }
    
    Doctor d;
    leerDoctorPorIndice(indice, d);
    char msg[200];
    snprintf(msg, sizeof(msg), "Seguro que desea eliminar al Dr. %s %s (ID %d)?", d.nombre, d.apellido, d.id);
    if (leerConfirmacion(msg)) {
        d.eliminado = true;
        d.disponible = false;
        d.fechaModificacion = time(0);
        
        if (actualizarDoctorPorIndice(indice, d)) {
            cout << "Doctor eliminado logicamente.\n";
            h.totalDoctoresRegistrados--; // Actualiza contador en RAM
            
            // Actualizar header de doctores
            ArchivoHeader hDoc;
            if (leerHeader(DOCTORES_BIN, hDoc)) {
                hDoc.registrosActivos--;
                actualizarHeader(DOCTORES_BIN, hDoc);
            }
        } else {
            cout << "Error: No se pudo actualizar el registro del doctor.\n";
        }
    } else {
        cout << "Eliminacion cancelada.\n";
    }
}

/**
 * Cambia el estado de una cita a "Cancelada".
 */
void cancelarCita(Hospital& h) {
    int id = leerEnteroSeguro("ID de la cita a suspender/cancelar: ", 1, h.siguienteIDCita - 1);
    
    int indice = buscarIndiceCita(id);
    if (indice == -1) {
        cout << "Error: Cita no encontrada.\n";
        return;
    }
    
    Cita c;
    leerCitaPorIndice(indice, c);
    
    if (strcmp(c.estado, "Atendida") == 0) {
        cout << "Error: No se puede cancelar una cita que ya fue atendida.\n";
        return;
    }
    if (strcmp(c.estado, "Cancelada") == 0) {
        cout << "La cita ya estaba cancelada.\n";
        return;
    }
    
    strncpy(c.estado, "Cancelada", tamanio_ESTADO);
    c.fechaModificacion = time(0);
   
    
    if (actualizarCitaPorIndice(indice, c)) {
        cout << "Cita ID " << c.id << " ha sido suspendida (Cancelada).\n";
        // Actualizamos estadística en RAM
        h.totalCitasAgendadas--; 
    } else {
        cout << "Error al actualizar el archivo de citas.\n";
    }
}


void listarCitasDePaciente() {
    int idPaciente = leerEnteroSeguro("ID del paciente: ", 1, 1000000);
    Paciente p;
    if (!buscarPacientePorID(idPaciente, p)) {
        cout << "Error: Paciente no encontrado.\n";
        return;
    }
    
    cout << "\n--- Citas de " << p.nombre << " " << p.apellido << " (" << p.cantidadCitas << ") ---\n";
    if (p.cantidadCitas == 0) {
        cout << "No hay citas registradas para este paciente.\n";
        return;
    }
    
    cout << "+-----+------------+-------+------------+-------------+" << endl;
    cout << "| ID  | Fecha      | Hora  | ID Doctor  | Estado      |" << endl;
    cout << "+-----+------------+-------+------------+-------------+" << endl;
    
    int citasMostradas = 0;
    for (int i = 0; i < p.cantidadCitas; i++) {
        Cita c;
        // Leemos Cita por ID desde el array fijo del paciente
        if (buscarCitaPorID(p.citasIDs[i], c)) {
            cout << "| " << setw(3) << right << c.id << " | "
                 << setw(10) << left << c.fecha << " | "
                 << setw(5) << left << c.hora << " | "
                 << setw(10) << right << c.doctorID << " | "
                 << setw(11) << left << c.estado << " |" << endl;
            citasMostradas++;
        }
    }
    cout << "+-----+------------+-------+------------+-------------+" << endl;
    if (citasMostradas == 0 && p.cantidadCitas > 0) {
         cout << "No se encontraron registros de citas (pueden estar corruptos o eliminados).\n";
    }
}

void listarCitasDeDoctor() {
    int idDoctor = leerEnteroSeguro("ID del doctor: ", 1, 1000000);
    Doctor d;
    if (!buscarDoctorPorID(idDoctor, d)) {
        cout << "Error: Doctor no encontrado.\n";
        return;
    }
    
    cout << "\n--- Citas del Dr. " << d.nombre << " " << d.apellido << " (" << d.cantidadCitas << ") ---\n";
    if (d.cantidadCitas == 0) {
        cout << "No hay citas registradas para este doctor.\n";
        return;
    }
    
    cout << "+-----+------------+-------+-------------+-------------+" << endl;
    cout << "| ID  | Fecha      | Hora  | ID Paciente | Estado      |" << endl;
    cout << "+-----+------------+-------+-------------+-------------+" << endl;
    
    int citasMostradas = 0;
    for (int i = 0; i < d.cantidadCitas; i++) {
        Cita c;
        if (buscarCitaPorID(d.citasIDs[i], c)) {
            cout << "| " << setw(3) << right << c.id << " | "
                 << setw(10) << left << c.fecha << " | "
                 << setw(5) << left << c.hora << " | "
                 << setw(11) << right << c.pacienteID << " | "
                 << setw(11) << left << c.estado << " |" << endl;
            citasMostradas++;
        }
    }
    cout << "+-----+------------+-------+-------------+-------------+" << endl;
    if (citasMostradas == 0 && d.cantidadCitas > 0) {
         cout << "No se encontraron registros de citas (pueden estar corruptos o eliminados).\n";
    }
}

void listarCitasPorFecha() {
    char fecha[tamanio_FECHA];
    leerFechaSegura("Ingrese fecha (YYYY-MM-DD): ", fecha, tamanio_FECHA);
    
    ifstream archivo(CITAS_BIN, ios::binary);
    if (!archivo) {
        cout << "No se pudo abrir el archivo de citas.\n";
        return;
    }
    
    ArchivoHeader h;
    archivo.read((char*)&h, sizeof(ArchivoHeader));
    
    cout << "\n--- Citas para la fecha " << fecha << " ---\n";
    cout << "+-----+----------+----------+-------+-------------+" << endl;
    cout << "| ID  | ID Pac.  | ID Doc.  | Hora  | Estado      |" << endl;
    cout << "+-----+----------+----------+-------+-------------+" << endl;
    
    Cita c;
    int encontrados = 0;
    for (int i = 0; i < h.cantidadRegistros; i++) {
        archivo.read((char*)&c, sizeof(Cita));
        if (archivo.fail()) break;
        
        // Lee todas las citas (inclu-so canceladas) de esa fecha
        if (strcmp(c.fecha, fecha) == 0 && !c.eliminado) {
            cout << "| " << setw(3) << right << c.id << " | "
                 << setw(8) << right << c.pacienteID << " | "
                 << setw(8) << right << c.doctorID << " | "
                 << setw(5) << left << c.hora << " | "
                 << setw(11) << left << c.estado << " |" << endl;
            encontrados++;
        }
    }
    
    cout << "+-----+----------+----------+-------+-------------+" << endl;
    if (encontrados == 0) {
        cout << "No se encontraron citas para esa fecha.\n";
    }
    archivo.close();
}


// ------------------------------------------
// FUNCIONES DE MANTENIMIENTO
// ------------------------------------------

/**
 * 1. Verificar integridad de archivos
 * Comprueba si los contadores en los headers coinciden con los datos reales.*/

void verificarIntegridadArchivos() {
    cout << "\n--- VERIFICANDO INTEGRIDAD DE ARCHIVOS ---\n";
    bool integridadOK = true;

    // Verificar Pacientes
    ifstream filePac(PACIENTES_BIN, ios::binary);
    if (filePac) {
        ArchivoHeader h;
        filePac.read((char*)&h, sizeof(ArchivoHeader));
        int activosReales = 0;
        Paciente p;
        for (int i = 0; i < h.cantidadRegistros; i++) {
            filePac.read((char*)&p, sizeof(Paciente));
            if (filePac.fail()) break;
            if (!p.eliminado) activosReales++;
        }
        filePac.close();
        if (activosReales != h.registrosActivos) {
            cout << "Inconsistencia en " << PACIENTES_BIN << ": "
                 << "Header dice " << h.registrosActivos << " activos, "
                 << "conteo real " << activosReales << endl;
            integridadOK = false;
        } else {
            cout << PACIENTES_BIN << ": OK\n";
        }
    } else {
        cout << "Error: No se pudo abrir " << PACIENTES_BIN << endl;
        integridadOK = false;
    }

    // Verificar Doctores
    ifstream fileDoc(DOCTORES_BIN, ios::binary);
    if (fileDoc) {
        ArchivoHeader h;
        fileDoc.read((char*)&h, sizeof(ArchivoHeader));
        int activosReales = 0;
        Doctor d;
        for (int i = 0; i < h.cantidadRegistros; i++) {
            fileDoc.read((char*)&d, sizeof(Doctor));
            if (fileDoc.fail()) break;
            if (!d.eliminado) activosReales++;
        }
        fileDoc.close();
        if (activosReales != h.registrosActivos) {
            cout << "Inconsistencia en " << DOCTORES_BIN << ": "
                 << "Header dice " << h.registrosActivos << " activos, "
                 << "conteo real " << activosReales << endl;
            integridadOK = false;
        } else {
            cout << DOCTORES_BIN << ": OK\n";
        }
    } else {
        cout << "Error: No se pudo abrir " << DOCTORES_BIN << endl;
        integridadOK = false;
    }

    // Verificar Citas
    ifstream fileCita(CITAS_BIN, ios::binary);
    if (fileCita) {
        ArchivoHeader h;
        fileCita.read((char*)&h, sizeof(ArchivoHeader));
        int activosReales = 0;
        Cita c;
        for (int i = 0; i < h.cantidadRegistros; i++) {
            fileCita.read((char*)&c, sizeof(Cita));
            if (fileCita.fail()) break;
            if (!c.eliminado) activosReales++;
        }
        fileCita.close();
        if (activosReales != h.registrosActivos) {
            cout << "Inconsistencia en " << CITAS_BIN << ": "
                 << "Header dice " << h.registrosActivos << " activos, "
                 << "conteo real " << activosReales << endl;
            integridadOK = false;
        } else {
            cout << CITAS_BIN << ": OK\n";
        }
    } else {
        cout << "Error: No se pudo abrir " << CITAS_BIN << endl;
        integridadOK = false;
    }

    // Verificar Historiales
    ifstream fileHist(HISTORIALES_BIN, ios::binary);
    if (fileHist) {
        ArchivoHeader h;
        fileHist.read((char*)&h, sizeof(ArchivoHeader));
        int activosReales = 0;
        HistorialMedico hm;
        for (int i = 0; i < h.cantidadRegistros; i++) {
            fileHist.read((char*)&hm, sizeof(HistorialMedico));
            if (fileHist.fail()) break;
            if (!hm.eliminado) activosReales++;
        }
        fileHist.close();
        if (activosReales != h.registrosActivos) {
            cout << "Inconsistencia en " << HISTORIALES_BIN << ": "
                 << "Header dice " << h.registrosActivos << " activos, "
                 << "conteo real " << activosReales << endl;
            integridadOK = false;
        } else {
            cout << HISTORIALES_BIN << ": OK\n";
        }
    } else {
        cout << "Error: No se pudo abrir " << HISTORIALES_BIN << endl;
        integridadOK = false;
    }
    
    if (integridadOK) {
        cout << "\nVerificacion completada. Todos los archivos consistentes.\n";
    } else {
        cout << "\nVerificacion completada. Se encontraron inconsistencias.\n";
        cout << "Se recomienda compactar los archivos (Opcion 2) para recalcular los contadores.\n";
    }
}


/**
 * 2. Compactar archivos (Helpers)
 * Funciones específicas para reescribir cada archivo sin los registros eliminados.
 */
bool compactarArchivoPacientes() {
    ifstream original("pacientes.bin", ios::binary);
    ofstream temporal("pacientes_temp.bin", ios::binary);

    if (!original.is_open() || !temporal.is_open()) {
        cerr << "ERROR: No se pudo abrir archivos para compactar.\n";
        return false;
    }

    ArchivoHeader header;
    original.read((char*)&header, sizeof(ArchivoHeader));

    ArchivoHeader nuevoHeader = {0, 1, 0, header.version};
    temporal.write((char*)&nuevoHeader, sizeof(ArchivoHeader));

    Paciente p;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        original.read((char*)&p, sizeof(Paciente));
        if (!p.eliminado) {
            p.id = nuevoHeader.proximoID++;
            temporal.write((char*)&p, sizeof(Paciente));
            nuevoHeader.cantidadRegistros++;
            nuevoHeader.registrosActivos++;
        }
    }

    temporal.seekp(0);
    temporal.write((char*)&nuevoHeader, sizeof(ArchivoHeader));

    original.close();
    temporal.close();

    remove("pacientes.bin");
    rename("pacientes_temp.bin", "pacientes.bin");

    cout << "Archivo compactado exitosamente.\n";
    return true;
}

bool compactarArchivoDoctores() {
    ifstream original(DOCTORES_BIN, ios::binary);
    ofstream temporal("doctores_temp.bin", ios::binary);

    if (!original.is_open() || !temporal.is_open()) {
        cerr << "ERROR: No se pudo abrir archivos para compactar doctores.\n";
        return false;
    }

    ArchivoHeader header;
    original.read((char*)&header, sizeof(ArchivoHeader));

    ArchivoHeader nuevoHeader = {0, 1, 0, header.version};
    temporal.write((char*)&nuevoHeader, sizeof(ArchivoHeader));

    Doctor d;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        original.read((char*)&d, sizeof(Doctor));
        if (!d.eliminado) {
            d.id = nuevoHeader.proximoID++; // Reasigna ID
            temporal.write((char*)&d, sizeof(Doctor));
            nuevoHeader.cantidadRegistros++;
            nuevoHeader.registrosActivos++;
        }
    }

    temporal.seekp(0);
    temporal.write((char*)&nuevoHeader, sizeof(ArchivoHeader));

    original.close();
    temporal.close();

    remove(DOCTORES_BIN);
    rename("doctores_temp.bin", DOCTORES_BIN);

    cout << "Archivo de doctores compactado exitosamente.\n";
    return true;
}

bool compactarArchivoCitas() {
    ifstream original(CITAS_BIN, ios::binary);
    ofstream temporal("citas_temp.bin", ios::binary);

    if (!original.is_open() || !temporal.is_open()) {
        cerr << "ERROR: No se pudo abrir archivos para compactar citas.\n";
        return false;
    }

    ArchivoHeader header;
    original.read((char*)&header, sizeof(ArchivoHeader));

    ArchivoHeader nuevoHeader = {0, 1, 0, header.version};
    temporal.write((char*)&nuevoHeader, sizeof(ArchivoHeader));

    Cita c;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        original.read((char*)&c, sizeof(Cita));
        if (!c.eliminado) {
            c.id = nuevoHeader.proximoID++; // Reasigna ID
            temporal.write((char*)&c, sizeof(Cita));
            nuevoHeader.cantidadRegistros++;
            nuevoHeader.registrosActivos++;
        }
    }

    temporal.seekp(0);
    temporal.write((char*)&nuevoHeader, sizeof(ArchivoHeader));

    original.close();
    temporal.close();

    remove(CITAS_BIN);
    rename("citas_temp.bin", CITAS_BIN);

    cout << "Archivo de citas compactado exitosamente.\n";
    return true;
}

bool compactarArchivoHistoriales() {
    ifstream original(HISTORIALES_BIN, ios::binary);
    ofstream temporal("historiales_temp.bin", ios::binary);

    if (!original.is_open() || !temporal.is_open()) {
        cerr << "ERROR: No se pudo abrir archivos para compactar historiales.\n";
        return false;
    }

    ArchivoHeader header;
    original.read((char*)&header, sizeof(ArchivoHeader));

    ArchivoHeader nuevoHeader = {0, 1, 0, header.version};
    temporal.write((char*)&nuevoHeader, sizeof(ArchivoHeader));

    HistorialMedico hm;
    for (int i = 0; i < header.cantidadRegistros; i++) {
        original.read((char*)&hm, sizeof(HistorialMedico));
        if (!hm.eliminado) {
            hm.id = nuevoHeader.proximoID++; // Reasigna ID
            temporal.write((char*)&hm, sizeof(HistorialMedico));
            nuevoHeader.cantidadRegistros++;
            nuevoHeader.registrosActivos++;
        }
    }

    temporal.seekp(0);
    temporal.write((char*)&nuevoHeader, sizeof(ArchivoHeader));

    original.close();
    temporal.close();

    remove(HISTORIALES_BIN);
    rename("historiales_temp.bin", HISTORIALES_BIN);

    cout << "Archivo de historiales compactado exitosamente.\n";
    return true;
}
/**
 * 2. Compactar archivos (Función principal)
 * Elimina físicamente los registros marcados como 'eliminado = true'.
 */
void compactarArchivos() {
    cout << "\n--- COMPACTANDO ARCHIVOS ---\n";
    cout << "Esto eliminara permanentemente los registros borrados y re-calculara los contadores.\n";
    if (!leerConfirmacion("Desea continuar?")) {
        cout << "Compactacion cancelada.\n";
        return;
    }

    cout << "Compactando " << PACIENTES_BIN << "... ";
    if (compactarArchivoPacientes()) cout << "OK.\n";
    else cout << "FALLO.\n";
    
    cout << "Compactando " << DOCTORES_BIN << "... ";
    if (compactarArchivoDoctores()) cout << "OK.\n";
    else cout << "FALLO.\n";
    
    cout << "Compactando " << CITAS_BIN << "... ";
    if (compactarArchivoCitas()) cout << "OK.\n";
    else cout << "FALLO.\n";

    cout << "Compactando " << HISTORIALES_BIN << "... ";
    if (compactarArchivoHistoriales()) cout << "OK.\n";
    else cout << "FALLO.\n";
    
    cout << "\nCompactacion finalizada.\n";
    cout << "NOTA: Las relaciones (ej. ID de Pacientes en Doctores) no se actualizan,\n";
    cout << "pero el sistema ya ignora IDs no encontrados o eliminados.\n";
}


/**
 * 3. y 4. Helper para copiar archivos (Backup/Restore)
 */
bool copiarArchivo(const char* src, const char* dst) {
    ifstream in(src, ios::binary);
    if (!in) {
        cout << "Error: No se encontro el archivo fuente " << src << endl;
        return false;
    }
    ofstream out(dst, ios::binary | ios::trunc);
    if (!out) {
        in.close();
        cout << "Error: No se pudo crear el archivo destino " << dst << endl;
        return false;
    }
    // Copia rápida de búfer
    out << in.rdbuf();
    
    in.close();
    out.close();
    return true;
}

/**
 * 3. Hacer respaldo de datos
 * Copia todos los .bin a una carpeta 'backup/'.
 */
void hacerRespaldoDatos() {
    const char* backupDir = "backup";
    cout << "\n--- HACER RESPALDO DE DATOS ---\n";
    cout << "Se creara una copia de los datos en la carpeta '" << backupDir << "/'\n";
    
    // Crear directorio (ignora error si ya existe)
    _mkdir(backupDir);
    
    bool exito = true;
    cout << "Respaldando " << HOSPITAL_BIN << "... ";
    exito &= copiarArchivo(HOSPITAL_BIN, "backup/hospital.bin");
    cout << (exito ? "OK.\n" : "FALLO.\n");
    
    cout << "Respaldando " << PACIENTES_BIN << "... ";
    exito &= copiarArchivo(PACIENTES_BIN, "backup/pacientes.bin");
    cout << (exito ? "OK.\n" : "FALLO.\n");
    
    cout << "Respaldando " << DOCTORES_BIN << "... ";
    exito &= copiarArchivo(DOCTORES_BIN, "backup/doctores.bin");
    cout << (exito ? "OK.\n" : "FALLO.\n");
    
    cout << "Respaldando " << CITAS_BIN << "... ";
    exito &= copiarArchivo(CITAS_BIN, "backup/citas.bin");
    cout << (exito ? "OK.\n" : "FALLO.\n");
    
    cout << "Respaldando " << HISTORIALES_BIN << "... ";
    exito &= copiarArchivo(HISTORIALES_BIN, "backup/historiales.bin");
    cout << (exito ? "OK.\n" : "FALLO.\n");

    if (exito) {
        cout << "\nRespaldo completado exitosamente.\n";
    } else {
        cout << "\nError: El respaldo fallo para uno o mas archivos.\n";
    }
}

/**
 * 4. Restaurar desde respaldo
 * Copia los archivos de 'backup/' a la carpeta principal.
 */
void restaurarDesdeRespaldo(Hospital& h_actual) {
    cout << "\n--- RESTAURAR DESDE RESPALDO ---\n";
    cout << "ADVERTENCIA: Esto sobreescribira todos los datos actuales con los del ultimo respaldo.\n";
    cout << "Cualquier cambio no guardado se perdera.\n";
    
    if (!leerConfirmacion("Desea continuar con la restauracion?")) {
        cout << "Restauracion cancelada.\n";
        return;
    }

    bool exito = true;
    cout << "Restaurando " << HOSPITAL_BIN << "... ";
    exito &= copiarArchivo("backup/hospital.bin", HOSPITAL_BIN);
    cout << (exito ? "OK.\n" : "FALLO.\n");

    cout << "Restaurando " << PACIENTES_BIN << "... ";
    exito &= copiarArchivo("backup/pacientes.bin", PACIENTES_BIN);
    cout << (exito ? "OK.\n" : "FALLO.\n");

    cout << "Restaurando " << DOCTORES_BIN << "... ";
    exito &= copiarArchivo("backup/doctores.bin", DOCTORES_BIN);
    cout << (exito ? "OK.\n" : "FALLO.\n");

    cout << "Restaurando " << CITAS_BIN << "... ";
    exito &= copiarArchivo("backup/citas.bin", CITAS_BIN);
    cout << (exito ? "OK.\n" : "FALLO.\n");

    cout << "Restaurando " << HISTORIALES_BIN << "... ";
    exito &= copiarArchivo("backup/historiales.bin", HISTORIALES_BIN);
    cout << (exito ? "OK.\n" : "FALLO.\n");

    if (exito) {
        cout << "\nRestauracion completada. Recargando datos del hospital en memoria...\n";
        ifstream archivo(HOSPITAL_BIN, ios::binary);
        if (archivo) {
            archivo.read((char*)&h_actual, sizeof(Hospital));
            archivo.close();
            cout << "Datos recargados. El sistema continuara con los datos restaurados.\n";
        } else {
            cout << "Error critico: No se pudo recargar " << HOSPITAL_BIN << "!\n";
            cout << "Se recomienda reiniciar el programa.\n";
        }
    } else {
        cout << "\nError: No se pudieron restaurar los archivos (falta el backup en 'backup/'?).\n";
    }
}


/**
 * 5. Helper para obtener tamaño de archivo
 */
long getFileSize(const char* nombre) {
    ifstream in(nombre, ios::binary | ios::ate);
    if (!in) return 0;
    long size = (long)in.tellg();
    in.close();
    return size;
}

/**
 * 5. Estadísticas de uso de archivos
 * Muestra un resumen de los headers y tamaños de archivo. */
void mostrarEstadisticasArchivos() {
    cout << "\n--- ESTADISTICAS DE USO DE ARCHIVOS ---\n";
    cout << left;
    cout << "+-----------------+----------+----------+------------+----------+-------------+\n";
    cout << "| Archivo         | Total Reg| Activos  | Eliminados | Sig. ID  | Tamano (KB) |\n";
    cout << "+-----------------+----------+----------+------------+----------+-------------+\n";

    ArchivoHeader h;
    
    // Pacientes
    if (leerHeader(PACIENTES_BIN, h)) {
        cout << "| " << setw(15) << PACIENTES_BIN << " | "
             << setw(8) << h.cantidadRegistros << " | "
             << setw(8) << h.registrosActivos << " | "
             << setw(10) << (h.cantidadRegistros - h.registrosActivos) << " | "
             << setw(8) << h.proximoID << " | "
             << setw(11) << fixed << setprecision(2) << (getFileSize(PACIENTES_BIN) / 1024.0) << " |\n";
    }
    // Doctores
    if (leerHeader(DOCTORES_BIN, h)) {
        cout << "| " << setw(15) << DOCTORES_BIN << " | "
             << setw(8) << h.cantidadRegistros << " | "
             << setw(8) << h.registrosActivos << " | "
             << setw(10) << (h.cantidadRegistros - h.registrosActivos) << " | "
             << setw(8) << h.proximoID << " | "
             << setw(11) << fixed << setprecision(2) << (getFileSize(DOCTORES_BIN) / 1024.0) << " |\n";
    }
    // Citas
    if (leerHeader(CITAS_BIN, h)) {
        cout << "| " << setw(15) << CITAS_BIN << " | "
             << setw(8) << h.cantidadRegistros << " | "
             << setw(8) << h.registrosActivos << " | "
             << setw(10) << (h.cantidadRegistros - h.registrosActivos) << " | "
             << setw(8) << h.proximoID << " | "
             << setw(11) << fixed << setprecision(2) << (getFileSize(CITAS_BIN) / 1024.0) << " |\n";
    }
    // Historiales
    if (leerHeader(HISTORIALES_BIN, h)) {
        cout << "| " << setw(15) << HISTORIALES_BIN << " | "
             << setw(8) << h.cantidadRegistros << " | "
             << setw(8) << h.registrosActivos << " | "
             << setw(10) << (h.cantidadRegistros - h.registrosActivos) << " | "
             << setw(8) << h.proximoID << " | "
             << setw(11) << fixed << setprecision(2) << (getFileSize(HISTORIALES_BIN) / 1024.0) << " |\n";
    }
    
    cout << "+-----------------+----------+----------+------------+----------+-------------+\n";
    // Hospital.bin (no tiene header)
    cout << "| " << setw(15) << HOSPITAL_BIN << " | "
         << setw(8) << "N/A" << " | " << setw(8) << "N/A" << " | " << setw(10) << "N/A" << " | "
         << setw(8) << "N/A" << " | " << setw(11) << fixed << setprecision(2) << (getFileSize(HOSPITAL_BIN) / 1024.0) << " |\n";
    cout << "+-----------------+----------+----------+------------+----------+-------------+\n";
}



// ------------------------------------------
// FUNCIÓN PRINCIPAL (MAIN) FINAL (GRACIAS SEÑOR GRACIAS)
// ------------------------------------------

int main() {
    Hospital hospital; 
    
    if (!cargarDatosHospital(hospital)) {
        cout << "No se pudo inicializar el sistema. Saliendo.\n";
        system("pause");
        return 1;
    }

    int opcionPrincipal;
    do {
        system("cls");
        cout << "\t\t+----------------------------------------+\n";
        cout << "\t\t+       " << setw(30) << left << hospital.nombre << " +\n";
        cout << "\t\t+----------------------------------------+\n";
    cout << "\t\t+  Direccion: " << setw(26) << left << hospital.direccion << " +\n";
    cout << "\t\t+  Email: " << setw(32) << left << hospital.email << " +\n";
        cout << "\t\t+----------------------------------------+\n";
        cout << "\t\t+          SELECCIONE UNA OPCION         +\n";
        cout << "\t\t+----------------------------------------+\n";
        cout << "\t\t+  1. Gestion de Pacientes               +\n";
        cout << "\t\t+  2. Gestion de Doctores                +\n";
        cout << "\t\t+  3. Gestion de Citas                   +\n";
        cout << "\t\t+  4. Mantenimiento de Archivos          +\n"; 
        cout << "\t\t+  5. Guardar y Salir                    +\n"; 
        cout << "\t\t+----------------------------------------+\n";
        opcionPrincipal = leerEnteroSeguro("Opcion: ", 1, 5); 

        switch (opcionPrincipal) {
            case 1: { // Gestion de Pacientes 
                int opcionPacientes;
                do {
                    system("cls");
                    cout << "\n--- GESTION DE PACIENTES ---\n";
                    cout << "1. Registrar paciente\n";
                    cout << "2. Buscar paciente por cedula\n";
                    cout << "3. Buscar paciente por ID\n";
                    cout << "4. Ver historial medico\n";
                    cout << "5. Lista de pacientes\n"; 
                    cout << "6. Eliminar paciente (Logico)\n"; 
                    cout << "7. Volver\n"; 
                    opcionPacientes = leerEnteroSeguro("Seleccione una opcion: ", 1, 7);

                    switch (opcionPacientes) {
                        case 1: {
                            registrarPaciente(hospital); 
                            system("pause");
                            break;
                        }
                        case 2: {
                            char cedula[tamanio_CEDULA];
                            leerCadenaSeguro("Ingrese cedula: ", cedula, tamanio_CEDULA);
                            Paciente p;
                            if (buscarPacientePorCedula(cedula, p)) {
                                cout << "Encontrado: ID " << p.id << " - " << p.nombre << " " << p.apellido << "\n";
                                cout << "  Telefono: " << p.telefono << " | Email: " << p.email << "\n";
                            } else {
                                cout << "Paciente no encontrado.\n";
                            }
                            system("pause");
                            break;
                        }
                        case 3: {
                            int id = leerEnteroSeguro("ID paciente: ", 1, 1000000);
                            Paciente p;
                            if (buscarPacientePorID(id, p)) {
                                cout << "Encontrado: " << p.nombre << " " << p.apellido << " (Cedula: " << p.cedula << ")\n";
                            } else {
                                cout << "Paciente no encontrado.\n";
                            }
                            system("pause");
                            break;
                        }
                        case 4: {
                            int id = leerEnteroSeguro("ID paciente: ", 1, 1000000);
                            mostrarHistorialMedico(id);
                            system("pause");
                            break;
                        }
                        case 5: {
                            listarPacientes();
                            system("pause");
                            break;
                        }
                        case 6: {
                            eliminarPaciente(hospital);
                            system("pause");
                            break;
                        }
                    }
                } while (opcionPacientes != 7);
                break;
            }

            case 2: { // Gestion de Doctores 
                int opcionDoctores;
                do {
                    system("cls");
                    cout << "\n--- GESTION DE DOCTORES ---\n";
                    cout << "1. Ingresar nuevo doctor\n";
                    cout << "2. Buscar doctor por ID\n";
                    cout << "3. Buscar doctor por especialidad\n";
                    cout << "4. Asignar paciente a doctor\n";
                    cout << "5. Remover paciente de doctor\n";
                    cout << "6. Lista de pacientes asignados a un doctor\n";
                    cout << "7. Ver lista de doctores\n";
                    cout << "8. Eliminar doctor\n";
                    cout << "9. Volver\n";
                    opcionDoctores = leerEnteroSeguro("Seleccione una opcion: ", 1, 9);

                    switch (opcionDoctores) {
                        case 1: {
                            registrarDoctor(hospital);
                            system("pause");
                            break;
                        }
                        case 2: {
                            int id = leerEnteroSeguro("ID doctor: ", 1, 1000000);
                            Doctor d;
                            if (buscarDoctorPorID(id, d)) {
                                cout << "Encontrado: Dr. " << d.nombre << " " << d.apellido << "\n";
                                cout << "  Especialidad: " << d.especialidad << " | Costo: " << d.costoConsulta << "\n";
                            } else {
                                cout << "Doctor no encontrado.\n";
                            }
                            system("pause");
                            break;
                        }
                        case 3: {
                            buscarDoctoresPorEspecialidad();
                            system("pause");
                            break;
                        }
                        case 4: {
                            asignarPacienteADoctor();
                            system("pause");
                            break;
                        }
                        case 5: {
                            removerPacienteDeDoctor();
                            system("pause");
                            break;
                        }
                        case 6: {
                            listarPacientesDeDoctor();
                            system("pause");
                            break;
                        }
                        case 7: {
                            listarDoctores();
                            system("pause");
                            break;
                        }
                        case 8: {
                            eliminarDoctor(hospital);
                            system("pause");
                            break;
                        }
                    }
                } while (opcionDoctores != 9);
                break;
            }

            case 3: { // Gestion de Citas 
                int opcionCitas;
                do {
                    system("cls");
                    cout << "\n--- GESTION DE CITAS ---\n";
                    cout << "1. Agendar cita\n";
                    cout << "2. Suspender cita\n";
                    cout << "3. Atender cita\n";
                    cout << "4. Ver citas de un paciente\n";
                    cout << "5. Ver citas de un doctor\n";
                    cout << "6. Ver citas en una fecha\n";
                    cout << "7. Ver citas pendientes\n";
                    cout << "8. Volver\n";
                    opcionCitas = leerEnteroSeguro("Seleccione una opcion: ", 1, 8);

                    switch (opcionCitas) {
                        case 1: {
                            agendarCita(hospital);
                            system("pause");
                            break;
                        }
                        case 2: {
                            cancelarCita(hospital);
                            system("pause");
                            break;
                        }
                        case 3: {
                            atenderCita(hospital);
                            system("pause");
                            break;
                        }
                        case 4: {
                            listarCitasDePaciente();
                            system("pause");
                            break;
                        }
                        case 5: {
                            listarCitasDeDoctor();
                            system("pause");
                            break;
                        }
                        case 6: {
                            listarCitasPorFecha();
                            system("pause");
                            break;
                        }
                        case 7: {
                            listarCitasPendientes();
                            system("pause");
                            break;
                        }
                    }
                } while (opcionCitas != 8);
                break;
            }
            
            case 4: { // (NUEVO) Gestion de Mantenimiento
                int opcionMantenimiento;
                do {
                    system("cls");
                    cout << "\n--- MANTENIMIENTO DE ARCHIVOS ---\n";
                    cout << "1. Verificar integridad de archivos\n";
                    cout << "2. Compactar archivos (eliminar registros borrados)\n";
                    cout << "3. Hacer respaldo de datos\n";
                    cout << "4. Restaurar desde respaldo\n";
                    cout << "5. Estadisticas de uso de archivos\n";
                    cout << "6. Volver al menu principal\n";
                    opcionMantenimiento = leerEnteroSeguro("Seleccione una opcion: ", 1, 6);

                    switch (opcionMantenimiento) {
                        case 1: {
                            verificarIntegridadArchivos();
                            system("pause");
                            break;
                        }
                        case 2: {
                            compactarArchivos();
                            system("pause");
                            break;
                        }
                        case 3: {
                            hacerRespaldoDatos();
                            system("pause");
                            break;
                        }
                        case 4: {
                            // Pasa 'hospital' por referencia 
                            restaurarDesdeRespaldo(hospital);
                            system("pause");
                            break;
                        }
                        case 5: {
                            mostrarEstadisticasArchivos();
                            system("pause");
                            break;
                        }
                    }
                } while (opcionMantenimiento != 6);
                break;
            }

            case 5: { // Salir
                if (guardarDatosHospital(hospital)) {
                    cout << "Todos los datos han sido guardados. Saliendo...\n";
                } else {
                    cout << "ERROR: No se pudieron guardar los datos. Saliendo de todas formas...\n";
                }
                break;
            }
        }
    } while (opcionPrincipal != 5); 

    // Ya no se llama a destruirHospital() porque no hay memoria dinámica que liberar.
    // 'hospital' es una variable local y se destruye automáticamente.
    
    system("pause");
    return 0;
}