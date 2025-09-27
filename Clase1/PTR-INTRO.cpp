#include <iostream>
using namespace std;

int main () {

    string nombre = "Victor Aguillon";
string *nombre_p = &nombre;


int edad = 18;
int *edad_p = &edad;


cout << "Mi nombre es: " << *nombre_p << endl;
cout << "La direccion en memoria de mi nombre es: " << nombre_p << endl;
cout << endl;
cout << "Tengo: " << *edad_p <<" años de edad" << endl;
cout << "La direccion en memoria de mi edad es: " << edad_p << endl;


    return 0;
}