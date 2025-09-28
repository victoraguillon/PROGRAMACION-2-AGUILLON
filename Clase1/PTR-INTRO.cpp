#include <iostream>
#include <string>
#include <locale>
using namespace std;

int main () {
    setlocale(LC_ALL, "spanish");


// UN PUNTERO ES UNA VARIABLE QUE GUARDA LA DIRECCION EN MEMORIA DE OTRA VARIABLE
// Un puntero puede declararse, si y solo si, se hace declarandolo del mismo tipo que la variable a la que apunta.
// EJEMPLOS:
string nombre = "Victor Aguillon";
string *nombre_p = &nombre; // El asterisco significa "Desreferencia" y el ampersand significa "Referencia", siendo esta referencia la direccion en memoria.

int edad = 18;
int *edad_p = &edad;


cout << "Mi nombre es: " << *nombre_p << endl; // Al usarse * se muestra el valor de la variable nombre
cout << "La direccion en memoria de mi nombre es: " << nombre_p << endl; // Al no usarse el * se muestra la direccion en memoria
cout << endl;
cout << "Tengo: " << *edad_p <<" años de edad" << endl;
cout << "La direccion en memoria de mi edad es: " << edad_p << endl;


// 2. MODIFICACION DE VALORES USANDO PUNTEROS

int HR_Judge = 52; // Jonrones de Judge esta temporada hasta ayer
int* punteroHRJudge = &HR_Judge; // Puntero que almacena la direccion de esos HR
cout << "Los jonrones de Aaron Judge hasta ayer: " << HR_Judge << endl;
*punteroHRJudge = 53; // Cambiamos el valor al que apunta el puntero en cuestion
cout << "Los jonrones de Aaron Judge hasta el dia de hoy (modificado usando el puntero): " << HR_Judge << endl;

cout << "\n";
    return 0;
}