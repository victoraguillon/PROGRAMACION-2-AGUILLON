# Documentación de funciones y diseño (ProyectoHospital)

Este documento describe qué hace cada función/Clase importante del proyecto, qué responsabilidades tiene, y qué librerías usa. Está orientado a preparar la defensa del proyecto, con énfasis en Programación Orientada a Objetos y manejo de librerías en cada función.

**Nota:** el proyecto usa serialización binaria directa (lectura/escritura con `fstream` y `sizeof`) y estructuras con arreglos de `char[]`. Esto funciona localmente pero tiene limitaciones de portabilidad y seguridad (ver recomendaciones al final).

**Índice**
- **persistencia**
  - `GestorArchivos` (funciones clave)
  - `Constantes` / `ArchivoHeader`
  - `Mantenimiento::menu` y sub-operaciones
- **modelos (clases / OOP)**
  - `Paciente`
  - `Doctor`
  - `Cita`
  - `HistorialMedico`
  - `Hospital`
- **operaciones (módulos de negocio)**
  - `operacionesPacientes`, `operacionesDoctores`, `operacionesCitas`, `operacionesHistorial`
- **utilidades**
  - `Formatos` (UI helpers)
  - `Validaciones` (lectura/validación de entrada)
- **main.cpp** (coordinador / menú)
- **Recomendaciones para la defensa**

**persistencia**

- **Clase / Módulo: `GestorArchivos`**
  - ¿Qué es?: conjunto de funciones estáticas que centralizan la lectura/escritura de registros binarios en archivos bajo `datos/`.
  - Funciones clave:
    - **`inicializarSistemaArchivos()`**: crea la carpeta `datos/` y archivos si no existen. Usa `stat`/`mkdir` (o `_mkdir` en Windows). Librerías: `<sys/stat.h>`/`<direct.h>`, `<fstream>`.
      - OOP: no es una clase de instancia, se usa como módulo utilitario (estático). Mantiene separación entre lógica de negocio y persistencia.
    - **`inicializarArchivo(const char* archivo)`** (privada): crea un archivo binario y escribe el `ArchivoHeader` inicial.
    - **`leerHeader(const char* archivo)`**: abre con `ifstream` y lee el `ArchivoHeader` (estructura que almacena metadatos: `cantidadRegistros`, `proximoID`, `registrosActivos`, `tamRegistro`). Librerías: `<fstream>`.
    - **`actualizarHeader(const char*, const ArchivoHeader&)`**: abre en modo `in|out` y sobrescribe los primeros bytes con el nuevo header.
    - **`calcularPosicion(int indice, size_t tamanoRegistro)`**: calcula `sizeof(ArchivoHeader) + indice * tamanoRegistro` y retorna `std::streamoff`. Se usa para posicionar `seekg/seekp`.
    - **`guardarRegistro(void* registro, size_t tamano, const char* archivo)`**: agrega al final del archivo (escribe bytes) y actualiza el header con `cantidadRegistros` y `registrosActivos`. Usa `fstream` y `static_cast<std::streamsize>(tamano)` para evitar warnings de conversión.
    - **`leerRegistroPorPosicion(int indice, void* registro, size_t tamano, const char* archivo)`**: posiciona con `seekg(calcularPosicion(...))` y lee `tamano` bytes en el buffer/objeto destino.
    - **`leerRegistroPorID(int id, void* registro, size_t tamano, const char* archivo, int offsetID = 0)`**: recorre registros, lee a un buffer temporal y compara el `int` ubicado en `offsetID` bytes (donde empieza el id en la struct). Si coincide copia al `registro` destino.
    - **`actualizarRegistro(int id, ...)`**: similar a lectura por ID, pero reescribe los bytes en la posición encontrada.
    - **`eliminarRegistroLogico(int id, ...)`**: *implemetación actual simplificada*: decrementa `registrosActivos` en el header y guarda. Idealmente marcaría el campo `eliminado` dentro del registro; actualmente el código actualiza sólo el header para mantener compatibilidad con diseño del proyecto.
  - Librerías usadas: `<fstream>`, `<iostream>`, `<sys/stat.h>` / `<direct.h>`.
  - Consideraciones OOP: `GestorArchivos` actúa como una capa de persistencia separada, lo que favorece el principio de responsabilidad única (SRP) — las clases de dominio no hacen IO directamente.

- **Constantes / `ArchivoHeader`**
  - ¿Qué es?: `persistencia/Constantes.hpp` declara rutas (`Rutas::PACIENTES`, `Rutas::DOCTORES`, etc.), el arreglo `ARCHIVOS_DATOS[]` y la estructura `ArchivoHeader` con metadatos.
  - Uso: todas las funciones de persistencia y mantenimiento leen el header para conocer `cantidadRegistros` y `proximoID`.
  - Librerías: ninguna especial, solo definiciones/constantes.

- **Módulo: `Mantenimiento` (`Mantenimiento::menu`)**
  - ¿Qué hace?: provee un menú (verificar integridad, compactar, respaldar, restaurar, estadísticas) que opera sobre los archivos en `datos/`.
  - Sub-funciones implícitas:
    - **`verificar()`**: lee headers y recorre registros contando `activos` comparándolos con `header.registrosActivos`. Para cada tipo (Paciente, Doctor, Cita, Historial, Hospital) usa `GestorArchivos::leerRegistroPorPosicion(...)` y el método `isEliminado()` cuando exista. Librerías: `<filesystem>` (solo para navegación de backups), `<iostream>`.
    - **`compactar()`**: crea un archivo temporal, copia solo los registros no eliminados y escribe un header nuevo actualizado; luego reemplaza el archivo original por el temporal (usa `std::filesystem::remove/rename`). Este método usa `ofstream` en modo binario y `std::streamsize` para escribir.
    - **`respaldar()`**: crea un directorio `datos/backup/<timestamp>/` y copia archivo por archivo usando `std::filesystem::copy_file`.
    - **`restaurar()`**: lista backups disponibles y copia los archivos seleccionados de vuelta a `datos/`.
    - **`estadisticas()`**: usa `std::filesystem::file_size` y `GestorArchivos::leerHeader` para imprimir tamaño y conteos.
  - Librerías: `<filesystem>`, `<fstream>`, `<ctime>`, `<iostream>`.
  - OOP: `Mantenimiento` es un namespace con funciones; su responsabilidad es la gestión de persistencia consistente. Trabaja con clases de dominio (Paciente, Doctor, etc.) a bajo nivel (lectura/escritura binaria), respetando separación de capas.

**modelos (clases / OOP)**

Cada clase sigue el patrón típico OOP: atributos `private`, métodos `public` para manipular estado (setters/getters), métodos de utilidad para mostrar (`mostrarFila`, `mostrarDetalle`) y validaciones. Se usan arreglos fijos `char[]` y tipos primitivos por simplicidad.

- **`Paciente` (archivo `pacientes/Paciente.hpp`)**
  - Atributos importantes: `id`, `nombre`, `apellido`, `cedula`, `edad`, `sexo`, `telefono`, `email`, `cantidadConsultas`, `primerConsultaID`, `citasIDs[]`, `activo`, `eliminado`, `fechaRegistro`.
  - Métodos clave:
    - **`setId(int)`**, **`setDatosPersonales(...)`**, **`setContacto(...)`**, **`setEliminado(bool)`**: encapsulan asignación de atributos.
    - **`isEliminado()`**: booleano para saber si el registro está lógicamente borrado.
    - **`agregarCita(int)`**, **`getCitaID(int)`**, etc.: gestionan relaciones con citas por IDs.
    - **`validarDatos()`**: método que verifica integridad de los campos del paciente antes de persistir.
  - Librerías usadas: `<ctime>` para `time_t`.
  - OOP: la clase encapsula estado y comportamiento; las operaciones sobre pacientes (crear, listar, modificar) se realizan a través de la API pública de la clase.

- **`Doctor` (archivo `doctores/Doctor.hpp`)**
  - Atributos: `id`, `nombre`, `apellido`, `cedula`, `aniosExperiencia`, `especialidad`, `costoConsulta`, `disponible`, `eliminado`, arrays `pacientesAsignadosIDs[]`, `citasIDs[]`.
  - Métodos clave:
    - **`setDatos(...)`**, **`setDisponible(bool)`**, **`setEliminado(bool)`**, **`getId()`, getNombre()`**, etc.
    - **`agregarPaciente(int)`**, **`removerPaciente(int)`**, **`agregarCita(int)`**: mantienen integridad de relaciones.
  - OOP: composición de arrays de IDs para representar relaciones uno-a-muchos sin usar punteros ni contenedores dinámicos (elección simple pero limitada). Los métodos garantizan límites al agregar/remover.

- **`Cita` (archivo `citas/Cita.hpp`)**
  - Atributos: `id`, `idPaciente`, `idDoctor`, `fecha[11]`, `hora[6]`, `estado[20]`, `eliminado`.
  - Métodos:
    - **`setDatos(int id, int idPac, int idDoc, const char* fecha, const char* hora)`**: inicializa la cita.
    - **`setEstado(const char*)`**, **`isAtendida()`**, **`isEliminado()`**, **`mostrarFila()`**, **`mostrarDetalle()`**.
  - Librerías: usa `<cstring>` indirectamente en implementación para copiar `char[]`.
  - OOP: clase simple que representa una entidad con comportamiento mínimo (mostrar/consultar estado).

- **`HistorialMedico`**
  - Estructura similar: registros con campos, métodos para mostrar y posiblemente `isEliminado()` según implementación.
  - En el mantenimiento, el proyecto actualmente trata historiales como `best-effort` (no siempre chequea `eliminado`). Revisa `HistorialMedico.hpp` si necesitas lógica de borrado lógico.

- **`Hospital` (archivo `hospital/Hospital.hpp`)**
  - Atributos: `nombre`, `direccion`, `telefono`, `email`, totales (pacientes, doctores, citas, consultas).
  - Métodos públicos: getters para datos y métodos para incrementar/decrementar contadores (`incrementarPacientes()`, `decrementarPacientes()`, etc.) y `mostrarEstadisticas()`.
  - Uso en `main`: el usuario puede editar los datos del hospital y el programa mantiene contadores que informan en la vista.
  - OOP: clase de entidad global (configuración/sistema) que expone una interfaz clara para modificar y consultar estadísticas.

**operaciones (capas de negocio)**

- Los módulos `operacionesPacientes.cpp`, `operacionesDoctores.cpp`, `operacionesCitas.cpp`, `operacionesHistorial.cpp` implementan las acciones del usuario (CRUD) sobre las entidades.
- Patrón:
  - Validan entrada (usando `Validaciones`), crean/actualizan instancias de las clases de dominio, y llaman a `GestorArchivos` para persistir o recuperar registros.
  - Ejemplos de funciones (típicas): `agregarPaciente()`, `listarPacientes()`, `modificarPaciente()`, `eliminarPaciente()`.
- OOP: separación de responsabilidades entre:
  - Clases de dominio (estado y comportamiento de la entidad),
  - Gestor de persistencia (lectura/escritura binaria),
  - Módulos de operaciones (lógica de negocio que combina ambas capas y coordina la UI).

**utilidades**

- **`Formatos` (`utilidades/Formatos.hpp/.cpp`)**
  - Responsabilidad: helpers para limpiar pantalla (`limpiarPantalla()`), pausar/esperar (`pausar()`), limpiar buffer (`limpiarBuffer()`), impresión centrada y encabezados.
  - Librerías: `<iostream>`, `system("cls")`/`system("clear")` según plataforma para limpiar pantalla.
  - Puntos importantes: `limpiarBuffer()` consume hasta el `\n` para sincronizar `operator>>` con `getline`, `pausar()` usa `cin.get()` para bloquear.

- **`Validaciones` (`utilidades/Validaciones.hpp/.cpp`)**
  - Funciones para leer y validar entradas del usuario: `leerEntero`, `leerFloat`, `leerCadena`, `leerSexo`, `leerFecha`, `leerHora`, `validarEmail`, `confirmarAccion`.
  - Uso de `cin >>` y `cin.getline()` combinado con `Formatos::limpiarBuffer()` para evitar lecturas residuales.
  - Importante para la defensa: explicar por qué `leerEntero` hace `Formatos::limpiarBuffer()` después de `cin >>` — para evitar que `getline` posterior lea un `\n` dejado en el buffer.

**`main.cpp`**

- Actúa como coordinador de la aplicación: muestra menús, instancia o carga el `Hospital`, y llama a los sub-menús: pacientes, doctores, citas, historial, mantenimiento. Contiene `menuMantenimiento(Hospital&)` que delega a `Mantenimiento::menu`.
- Conexión de capas: `main` no hace persistencia directa (buen diseño); invoca los módulos de operaciones que usan `GestorArchivos`.

**Importante: uso de librerías en funciones**

- `<fstream>`: lectura/escritura binaria en `GestorArchivos`. Funciones como `guardarRegistro`, `leerRegistroPorPosicion`, `actualizarHeader` dependen de `seekg/seekp` y `read/write`.
- `<iostream>`: IO en consola (menús, mensajes). Usada ampliamente en `Formatos`, `Validaciones`, `Mantenimiento`.
- `<filesystem>`: en `Mantenimiento` para listar respaldos, crear directorios y copiar/renombrar archivos (`fs::create_directories`, `fs::copy_file`, `fs::remove`, `fs::rename`, `fs::file_size`). Requiere C++17.
- `<ctime>`: para generar timestamps de respaldos (`std::time`, `std::strftime`).
- `<cstring>` / `<string>`: manipulación de `char[]` y comparaciones para fijar fechas/horas.
- `<limits>`: en `Mantenimiento` y `Validaciones` para `std::numeric_limits<std::streamsize>::max()` al limpiar buffers.

**Puntos críticos que podrías explicar en la defensa**
- Diseño por capas: UI (`main` / Formatos), validación (`Validaciones`), lógica de negocio (`operaciones*`), dominio (clases), persistencia (`GestorArchivos`). Explica cómo cada capa tiene una única responsabilidad y cómo se comunican.
- Serialización binaria con `sizeof`:
  - Ventajas: simple y rápido para este proyecto educativo.
  - Desventajas: dependiente de alineación, endianness, cambios en las clases. No es portable a otras arquitecturas ni seguro frente a cambios de diseño.
- Uso de arrays fijos (`char[]`) frente a `std::string`:
  - Elección didáctica y más sencillo de volcar con `write((char*)&obj, sizeof(obj))`.
  - Riesgos: desbordamientos, manejo manual de tamaños, y dificultad para redimensionar.
- Estrategia de borrado lógico vs físico:
  - Borrado lógico: marca `eliminado` y decrementa `registrosActivos` en header; compactación física posterior mediante `compactar()`.
  - Ventaja: evita reescrituras constantes y permite restauración simple.
- `Mantenimiento::compactar()` como ejemplo de migración/compaction:
  - Lee registro por registro, copia los no eliminados a un archivo temporal, reescribe header y reemplaza el archivo — patrón seguro para evitar pérdida al escribir.
- Sincronización de entrada (buffer): la combinación `cin >>` y `getline` requiere limpiar buffer; explica `Formatos::limpiarBuffer()` y por qué implementaste la espera por ENTER robusta en `Mantenimiento`.

**Recomendaciones rápidas para la defensa**
- Lleva ejemplos concretos (por ejemplo: flujo de `agregarPaciente()`): validar datos → crear `Paciente` → `GestorArchivos::guardarRegistro` → actualizar header.
- Explica un caso de compactación: eliminas registros (marcar `eliminado`), `compactar()` crea un `.tmp` y reemplaza el archivo para recuperar espacio.
- Si te preguntan sobre mejoras: sugiere usar `std::vector`/`std::string`, cambiar a serialización portable (JSON/Protobuf) o una base de datos ligera (SQLite) para mayor robustez.

---

Si quieres, genero ahora un anexo que liste *cada* función pública (nombre exacto y firma) en un formato tabla para imprimir en la defensa, o puedo ampliar la sección de riesgos y mejoras técnicas.

Archivo generado: `PROJECT_FUNCTIONS.md` (en la raíz del proyecto).

