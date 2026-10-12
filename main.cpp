#include <iostream>
#include <unistd.h>
#include <limits.h>

#include "include/punto1.h"
#include "include/punto2.h"
#include "include/punto3.h"
#include "include/punto4.h"

using namespace std;

// Cada punto fue escrito para ejecutarse desde su propia carpeta
// (rutas relativas "results/..." o "../results/..."). Esta funcion entra
// a esa carpeta, ejecuta el punto y vuelve a la carpeta original.
int ejecutarEn(const char* carpeta, int (*punto)()) {
    char origen[PATH_MAX];
    if(getcwd(origen, sizeof(origen)) == nullptr || chdir(carpeta) != 0) {
        cout << "No se pudo entrar a la carpeta " << carpeta << endl;
        return 1;
    }
    int estado = punto();
    if(chdir(origen) != 0) cout << "No se pudo volver a " << origen << endl;
    return estado;
}

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
                ejecutarEn("1.Esferas2DAdiabaticas/src", ejecutarPunto1); 
                break;
            case 2: 
                cout << "\n--- Ejecutando Oscilador de Morse Forzado ---" << endl;
                ejecutarEn("2.MorseForzado", ejecutarPunto2); 
                break;
            case 3: 
                cout << "\n--- Ejecutando Oscilador de Mathieu-Duffing con Ruido ---" << endl;
                ejecutarEn("3.MathieuDuffingRuido", ejecutarPunto3); 
                break;
            case 4: 
                cout << "\n--- Ejecutando Modelo de Ising 2D ---" << endl;
                ejecutarEn("4.Ising2D/src", ejecutarPunto4); 
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