# 🏥 Sistema de Gestión Hospitalaria
## Realizado por: Victor Aguillón

## 1. Descripción del Proyecto

### 🎯 Objetivo General
Desarrollar un sistema de gestión hospitalaria en C++ que permita administrar pacientes, doctores y citas médicas, integrando funcionalidades CRUD, validaciones, historial médico y manejo dinámico de memoria.

### ⚙️ Funcionalidades Principales
- Registro, búsqueda, actualización y eliminación de pacientes y doctores.
- Agendamiento, cancelación y atención de citas médicas.
- Registro de historial médico por paciente.
- Validaciones de entrada (cédula, email, fecha, hora).
- Redimensionamiento automático de arrays dinámicos.
- Visualización tabular de datos.

---

## 2. Instrucciónes de compilación.
- Identificar ruta en consola.
- Colocar la siguiente expresión: g++ -std=c++11 hospital.cpp -o hospital 

## 3. Instrucciones de ejecución:
 - Identificar ruta en consola
 - Colocar la expresión ./hospital.exe para ejecutar

## 4. Estructuras de Datos

### 📦 `HistorialMedico`
Contiene información de una consulta médica:
- Fecha y hora
- Diagnóstico
- Tratamiento
- Medicamentos
- ID del doctor
- Costo

### 👤 `Paciente`
Almacena datos personales del paciente:
- Historial médico dinámico (`HistorialMedico[]`)
- Citas agendadas (`int[]`)
- Alergias y observaciones
- Estado activo

### 🩺 `Doctor`
Contiene datos del profesional médico:
- Especialidad y experiencia
- Citas agendadas (`int[]`)
- Pacientes asignados (`int[]`)
- Disponibilidad

### 📅 `Cita`
Representa una cita médica entre paciente y doctor:
- Fecha y hora
- Motivo
- Estado (`Agendada`, `Atendida`, `Cancelada`)
- Observaciones

### 🏥 `Hospital`
Estructura principal que agrupa:
- Arrays dinámicos de `Paciente`, `Doctor`, `Cita`
- Control de IDs automáticos
- Capacidades y contadores

### 🔗 Relaciones entre Estructuras
- `Paciente` contiene historial y citas.
- `Doctor` contiene pacientes y citas.
- `Cita` enlaza `Paciente` y `Doctor` por ID.
- `Hospital` centraliza todas las estructuras.

---

## 5. Funciones Principales

### 👤 Módulo de Pacientes

| Función | Descripción |
|--------|-------------|
| `crearPaciente` | Crea y valida un nuevo paciente. |
| `buscarPacientePorId` / `PorCedula` / `PorNombre` | Búsqueda por distintos criterios. |
| `actualizarPaciente` | Modifica datos como edad, email, dirección. |
| `eliminarPaciente` | Libera memoria y compacta el array. |
| `listarPacientes` | Muestra tabla con datos clave. |
| `agregarConsultaAlHistorial` | Añade una consulta médica al historial. |
| `mostrarHistorialMedico` | Muestra todas las consultas del paciente. |

### 🩺 Módulo de Doctores

| Función | Descripción |
|--------|-------------|
| `crearDoctor` | Registra un nuevo doctor con validaciones. |
| `buscarDoctorPorId` / `PorEspecialidad` | Búsqueda por ID o especialidad. |
| `asignarPacienteADoctor` / `removerPacienteDeDoctor` | Gestión de pacientes asignados. |
| `listarDoctores` | Muestra tabla con datos clave. |
| `eliminarDoctor` | Libera memoria y compacta el array. |
| `listarPacientesDeDoctor` | Muestra pacientes asignados a un doctor. |

### 📅 Módulo de Citas

| Función | Descripción |
|--------|-------------|
| `agendarCita` | Crea una cita validando disponibilidad. |
| `cancelarCita` | Cambia estado y elimina referencias. |
| `atenderCita` | Registra diagnóstico y actualiza historial. |
| `verificarDisponibilidad` | Verifica si el doctor está libre. |
| `obtenerCitasDePaciente` / `DeDoctor` | Devuelve array de citas por ID. |

### 🏥 Inicialización y Memoria

| Función | Descripción |
|--------|-------------|
| `inicializarHospital` | Crea hospital con capacidades iniciales. |
| `redimensionarArrayPacientes` / `Doctores` / `Citas` | Duplica capacidad cuando se alcanza el límite. |
| `destruirHospital` | Libera toda la memoria dinámica. |

---

## 6. Decisiones de Diseño

### 📌 Uso de Arrays Dinámicos
Se eligieron arrays dinámicos para:
- Controlar el crecimiento de datos sin límite fijo.
- Evitar uso de STL para mantener compatibilidad académica.
- Permitir redimensionamiento manual y control de memoria.

### 🔁 Estrategia de Redimensionamiento
- Cada array se duplica cuando se alcanza su capacidad.
- Se copian los elementos existentes al nuevo array.
- Se libera la memoria anterior con `delete[]`.

### 🧠 Manejo de Memoria
- Se usa `new` y `delete[]` para asignación y liberación.
- Cada paciente y doctor tiene arrays internos que también se liberan.
- La función `destruirHospital` garantiza limpieza completa al finalizar.

---

