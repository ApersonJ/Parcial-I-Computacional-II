#include <iostream>

#include "include/punto1.h"
#include "include/punto2.h"
#include "include/punto3.h"
#include "include/punto4.h"

using namespace std;

int main() {
    int opcion;
    do {
        cout << "\n=== Fenomenologias Disponibles ===" << endl;
        cout << "1. Gas de Esferas 2D con Paredes Moviles" << endl;
        cout << "2. Oscilador de Morse Forzado" << endl;
        cout << "3. Oscilador de Mathieu-Duffing con Ruido" << endl;
        cout << "4. Modelo de Ising 2D" << endl;
        cout << "0. Salir" << endl;
        cout << "Seleccione una opcion para ejecutar: ";
        cin >> opcion;

        switch(opcion) {
            case 1: 
                cout << "\n--- Ejecutando Gas de Esferas 2D con Paredes Moviles ---" << endl;
                ejecutarPunto1(); 
                break;
            case 2: 
                cout << "\n--- Ejecutando Oscilador de Morse Forzado ---" << endl;
                ejecutarPunto2(); 
                break;
            case 3: 
                cout << "\n--- Ejecutando Oscilador de Mathieu-Duffing con Ruido ---" << endl;
                ejecutarPunto3(); 
                break;
            case 4: 
                cout << "\n--- Ejecutando Modelo de Ising 2D ---" << endl;
                ejecutarPunto4(); 
                break;
            case 0:
                cout << "Saliendo del programa..." << endl;
                break;
            default:
                cout << "Opcion no valida. Intente de nuevo." << endl;
        }
    } while(opcion != 0);

    return 0;
}