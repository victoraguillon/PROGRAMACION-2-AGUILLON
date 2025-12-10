#include "Mantenimiento.hpp"
#include "GestorArchivos.hpp"
#include "Constantes.hpp"
#include "../utilidades/Formatos.hpp"
#include "../utilidades/Validaciones.hpp"
#include "../pacientes/Paciente.hpp"
#include "../doctores/Doctor.hpp"
#include "../citas/Cita.hpp"
#include "../historial/HistorialMedico.hpp"
#include <filesystem>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctime>
#include <limits>

using namespace std;

namespace Mantenimiento {

void menu(Hospital& h) {
    (void)h;
    namespace fs = filesystem;
    int op;
    do {
        Formatos::imprimirEncabezado("MANTENIMIENTO DE ARCHIVOS");
        Formatos::printCentrado("1. Verificar integridad", BLANCO);
        Formatos::printCentrado("2. Compactar archivos (eliminar registros borrados)", BLANCO);
        Formatos::printCentrado("3. Respaldar datos", BLANCO);
        Formatos::printCentrado("4. Restaurar desde respaldo", BLANCO);
        Formatos::printCentrado("5. Estadisticas de uso de archivos", BLANCO);
        Formatos::printCentrado("6. Volver", AMARILLO);

        cout << endl;
        Formatos::imprimirLineaSeparadora();

        op = Validaciones::leerEntero("\n   >>> Opcion: ", 1, 6);

        auto getTamano = [&](const char* ruta)->size_t {
            
            if (string(ruta) == Rutas::PACIENTES) return sizeof(Paciente);
            if (string(ruta) == Rutas::DOCTORES) return sizeof(Doctor);
            if (string(ruta) == Rutas::CITAS) return sizeof(Cita);
            if (string(ruta) == Rutas::HISTORIAL) return sizeof(HistorialMedico);
            if (string(ruta) == Rutas::HOSPITAL) return sizeof(Hospital);
            return 0;
        };

        auto esperarEnter = [&]() {
            Formatos::printCentrado("Presione [ENTER] para volver al menu de mantenimiento...", AMARILLO);
           
            cin.clear();
            string _tmp;

            if (getline(cin, _tmp) && _tmp.size() == 0) {
                Formatos::printCentrado("Presione [ENTER] para continuar...", AMARILLO);
                getline(cin, _tmp);
            } else if (!_tmp.empty()) {
                Formatos::printCentrado("Presione [ENTER] para continuar...", AMARILLO);
                getline(cin, _tmp);
            } else {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        };

        auto verificar = [&](){
            Formatos::imprimirEncabezado("VERIFICAR INTEGRIDAD");
            for (int i = 0; i < Rutas::CANTIDAD_ARCHIVOS; i++) {
                const char* ruta = Rutas::ARCHIVOS_DATOS[i];
                ArchivoHeader hd = GestorArchivos::leerHeader(ruta);
                size_t tam = getTamano(ruta);
                int activos = 0;
                for (int r = 0; r < hd.cantidadRegistros; r++) {
                    if (tam == sizeof(Paciente)) {
                        Paciente p; GestorArchivos::leerRegistroPorPosicion(r, &p, tam, ruta);
                        if (!p.isEliminado()) activos++;
                    } else if (tam == sizeof(Doctor)) {
                        Doctor d; GestorArchivos::leerRegistroPorPosicion(r, &d, tam, ruta);
                        if (!d.isEliminado()) activos++;
                    } else if (tam == sizeof(Cita)) {
                        Cita c; GestorArchivos::leerRegistroPorPosicion(r, &c, tam, ruta);
                        if (!c.isEliminado()) activos++;
                    } else if (tam == sizeof(HistorialMedico)) {
                        HistorialMedico hm; GestorArchivos::leerRegistroPorPosicion(r, &hm, tam, ruta);
                        activos++; // best-effort
                    } else if (tam == sizeof(Hospital)) {
                        Hospital hh; GestorArchivos::leerRegistroPorPosicion(r, &hh, tam, ruta);
                        activos++;
                    }
                }
                cout << "Archivo: " << ruta << " | Header registros: " << hd.cantidadRegistros
                     << " | Header activos: " << hd.registrosActivos << " | Contados activos: " << activos << endl;
            }
            esperarEnter();
        };

        auto compactar = [&](){
            Formatos::imprimirEncabezado("COMPACTAR ARCHIVOS");
            for (int i = 0; i < Rutas::CANTIDAD_ARCHIVOS; i++) {
                const char* ruta = Rutas::ARCHIVOS_DATOS[i];
                size_t tam = getTamano(ruta);
                if (tam == 0) continue;

                ArchivoHeader hd = GestorArchivos::leerHeader(ruta);
                string tmp = string(ruta) + ".tmp";
                ofstream out(tmp, ios::binary);
                if (!out) { cout << "Error creando temporal para " << ruta << endl; continue; }

                ArchivoHeader newH = {0, hd.proximoID, 0, 0};
                out.write((char*)&newH, sizeof(ArchivoHeader));

                for (int r = 0; r < hd.cantidadRegistros; r++) {
                    if (tam == sizeof(Paciente)) {
                        Paciente p; GestorArchivos::leerRegistroPorPosicion(r, &p, tam, ruta);
                        if (!p.isEliminado()) { out.write((char*)&p, static_cast<streamsize>(tam)); newH.cantidadRegistros++; newH.registrosActivos++; }
                    } else if (tam == sizeof(Doctor)) {
                        Doctor d; GestorArchivos::leerRegistroPorPosicion(r, &d, tam, ruta);
                        if (!d.isEliminado()) { out.write((char*)&d, static_cast<streamsize>(tam)); newH.cantidadRegistros++; newH.registrosActivos++; }
                    } else if (tam == sizeof(Cita)) {
                        Cita c; GestorArchivos::leerRegistroPorPosicion(r, &c, tam, ruta);
                        if (!c.isEliminado()) { out.write((char*)&c, static_cast<streamsize>(tam)); newH.cantidadRegistros++; newH.registrosActivos++; }
                    } else if (tam == sizeof(HistorialMedico)) {
                        HistorialMedico hm; GestorArchivos::leerRegistroPorPosicion(r, &hm, tam, ruta);
                        out.write((char*)&hm, static_cast<streamsize>(tam)); newH.cantidadRegistros++; newH.registrosActivos++; }
                    else if (tam == sizeof(Hospital)) {
                        Hospital hh; GestorArchivos::leerRegistroPorPosicion(r, &hh, tam, ruta);
                        out.write((char*)&hh, static_cast<streamsize>(tam)); newH.cantidadRegistros++; newH.registrosActivos++; }
                }

                out.seekp(0);
                out.write((char*)&newH, sizeof(ArchivoHeader));
                out.close();

                try {
                    fs::remove(ruta);
                    fs::rename(tmp, ruta);
                    cout << "Compactado: " << ruta << " (registros: " << newH.cantidadRegistros << ")" << endl;
                } catch(...) {
                    cout << "Error al reemplazar " << ruta << endl;
                }
            }
            esperarEnter();
        };

        auto respaldar = [&](){
            Formatos::imprimirEncabezado("RESPALDAR DATOS");
            fs::path backupBase(Rutas::BACKUP_PATH);
            try { fs::create_directories(backupBase); } catch(...) {}
            auto t = time(nullptr);
            char buf[64]; strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", localtime(&t));
            fs::path dest = backupBase / buf;
            try { fs::create_directories(dest); } catch(...) { cout << "Error creando directorio de backup" << endl; esperarEnter(); return; }

            for (int i = 0; i < Rutas::CANTIDAD_ARCHIVOS; i++) {
                const char* ruta = Rutas::ARCHIVOS_DATOS[i];
                fs::path src(ruta);
                fs::path dst = dest / src.filename();
                try { fs::copy_file(src, dst, fs::copy_options::overwrite_existing); cout << "Respaldado: " << src << " -> " << dst << endl; } catch(...) { cout << "Fallo respaldo: " << ruta << endl; }
            }
            esperarEnter();
        };

        auto restaurar = [&](){
            Formatos::imprimirEncabezado("RESTAURAR RESPALDO");
            fs::path backupBase(Rutas::BACKUP_PATH);
            if (!fs::exists(backupBase)) { cout << "No hay respaldos disponibles." << endl; esperarEnter(); return; }

            vector<fs::path> dirs;
            for (auto &p : fs::directory_iterator(backupBase)) if (fs::is_directory(p.path())) dirs.push_back(p.path());
            if (dirs.empty()) { cout << "No hay respaldos disponibles." << endl; esperarEnter(); return; }

            cout << "Respaldos encontrados:" << endl;
            for (size_t i = 0; i < dirs.size(); i++) cout << i+1 << ") " << dirs[i].filename().string() << endl;
            int sel = Validaciones::leerEntero("Seleccione respaldo: ", 1, (int)dirs.size());
            size_t idx = static_cast<size_t>(sel - 1);
            fs::path pick = dirs[idx];

            for (auto &entry : fs::directory_iterator(pick)) {
                fs::path src = entry.path();
                fs::path dst = fs::path("datos") / src.filename();
                try { fs::copy_file(src, dst, fs::copy_options::overwrite_existing); cout << "Restaurado: " << dst << endl; } catch(...) { cout << "Error restaurando: " << src << endl; }
            }
            esperarEnter();
        };

        auto estadisticas = [&](){
            Formatos::imprimirEncabezado("ESTADISTICAS DE ARCHIVOS");
            for (int i = 0; i < Rutas::CANTIDAD_ARCHIVOS; i++) {
                const char* ruta = Rutas::ARCHIVOS_DATOS[i];
                ArchivoHeader hd = GestorArchivos::leerHeader(ruta);
                try { auto bytes = fs::file_size(ruta); cout << ruta << " | size: " << bytes << " bytes | registros: " << hd.cantidadRegistros << " | activos: " << hd.registrosActivos << endl; } catch(...) { cout << ruta << " | (no disponible)" << endl; }
            }
            esperarEnter();
        };

        switch(op) {
            case 1: verificar(); break;
            case 2: compactar(); break;
            case 3: respaldar(); break;
            case 4: restaurar(); break;
            case 5: estadisticas(); break;
            case 6: break;
            default: Formatos::printError("Opcion invalida."); break;
        }

    } while(op != 6);
}

} // namespace Mantenimiento
