# ifndef CONSTANTES_HPP
# define CONSTANTES_HPP

namespace Rutas {
    const char* const BASE_PATH = "datos/";
    const char* const HOSPITAL = "datos/hospital.bin";
    const char* const DOCTORES = "datos/doctores.bin";
    const char* const PACIENTES = "datos/pacientes.bin";
    const char* const CITAS = "datos/citas.bin";
    const char* const HISTORIAL = "datos/historiales.bin";

    const char* const BACKUP_PATH = "datos/backup/";
    
    const char* const ARCHIVOS_DATOS[] = {
        DOCTORES,
        PACIENTES,
        CITAS,
        HISTORIAL,
        HOSPITAL
    };
    const int CANTIDAD_ARCHIVOS = 5;
}

// Encabezado de archivo usado para administrar registros en archivos binarios
struct ArchivoHeader {
    int cantidadRegistros;
    int proximoID;
    int registrosActivos;
    int registrosEliminados;
};

const int VERSION_SISTEMA = 1;

# endif