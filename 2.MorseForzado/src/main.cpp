#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "osc_morse_forzado.h"
#include "acoplados.h" 

using namespace std;


void solicitarDatos(double &x0, double &v0,
                     double &dt, double &tmax,
                     double &D, double &a, double &xe, double &m, int &metodo,
                     double &gamma, double &A, double &omega,
                     int &opcion_acople, double &x2_0, double &v2_0, double &kappa);

// --------------------------- funcion principal ------------------------------
int main() {
    Osc_Morse_Forzado Oscilador;

    double x0, v0;
    double t, dt, tmax;
    // Parametros del sistema.
    double D, a, xe, m;
    double gamma, A, omega;
    int metodo;
    
   
    int opcion_acople;
    double x2_0, v2_0, kappa;

    solicitarDatos(x0, v0, dt, tmax, D, a, xe, m, metodo, gamma, A, omega, opcion_acople, x2_0, v2_0, kappa);

    if (opcion_acople == 1) {

        ejecutarAcoplados(x0, v0, x2_0, v2_0, D, a, xe, m, gamma, A, omega, kappa);
        
    } else {
    
        if (metodo == 1)
            cout << "\nMetodo de integracion          = Euler" << endl;
        else if (metodo == 2)
            cout << "\nMetodo de integracion          = Euler-Cromer" << endl;
        else if (metodo == 3)
            cout << "\nMetodo de integracion          = Runge-Kutta 2do Orden (punto medio)" << endl;
        else if (metodo == 4)
            cout << "\nMetodo de integracion          = Runge-Kutta 3er Orden" << endl;
        else
            cout << "\nMetodo de integracion          = Runge-Kutta 4to Orden" << endl;

        Oscilador.Inicie(x0, v0, D, a, xe);

        // Energia inicial, usada para el error relativo (E(t)-E0)/E0.
        double E0 = Oscilador.GetEnergia(m);

        ofstream file("results/datos/trayectoria.dat");

        file << fixed << setprecision(6);
        file << setw(12) << "t"
             << setw(15) << "x"
             << setw(15) << "v"
             << setw(15) << "P"
             << setw(15) << "E"
             << setw(18) << "dE/E0" << endl;

        for (t = 0.0; t < tmax; t += dt) {
            Oscilador.calcularPotencial(Oscilador.Getx(), D, a, xe);

            double E = Oscilador.GetEnergia(m);
            double dE_rel = (E0 != 0.0) ? (E - E0) / E0 : 0.0;

            file << setw(12) << t
                 << setw(15) << Oscilador.Getx()
                 << setw(15) << Oscilador.Getv()
                 << setw(15) << Oscilador.GetP()
                 << setw(15) << E
                 << setw(18) << dE_rel << endl;

            if (metodo == 1)
                Oscilador.MuevaseEuler(t, dt, D, a, xe, m, gamma, A, omega);
            else if (metodo == 2)
                Oscilador.MuevaseEulerCromer(t, dt, D, a, xe, m, gamma, A, omega);
            else if (metodo == 3)
                Oscilador.MuevaseRK2(t, dt, D, a, xe, m, gamma, A, omega);
            else if (metodo == 4)
                Oscilador.MuevaseRK3(t, dt, D, a, xe, m, gamma, A, omega);
            else
                Oscilador.MuevaseRK4(t, dt, D, a, xe, m, gamma, A, omega);
        }

        file.close();

        cout << "\nDatos guardados en 'results/datos/trayectoria.dat'" << endl;

        Oscilador.generarScriptsGnuplot();
    }

    return 0;
}

// --------------- Solicitar los datos -------------------------------
void solicitarDatos(double &x0, double &v0,
                     double &dt, double &tmax,
                     double &D, double &a, double &xe, double &m, int &metodo,
                     double &gamma, double &A, double &omega,
                     int &opcion_acople, double &x2_0, double &v2_0, double &kappa) {
                     
    cout << "Ingrese las condiciones iniciales del sistema base" << endl;
    cout << "Posicion inicial x0: "; cin >> x0;
    cout << "Velocidad inicial v0: "; cin >> v0;

    cout << "Ingrese el paso y el tiempo maximo" << endl;
    cout << "Paso dt: "; cin >> dt;
    cout << "Tiempo de simulacion: "; cin >> tmax;

    while (dt < 0 || tmax < 0) {
        cout << "Dato no valido. Ingrese tiempos positivos: ";
        cout << "Paso dt: "; cin >> dt;
        cout << "Tiempo de simulacion: "; cin >> tmax;
    }

    cout << "Ingrese los parametros del oscilador" << endl;
    cout << "Energia de disociacion D: "; cin >> D;
    cout << "Constante de rigidez a: "; cin >> a;
    cout << "Posicion de equilibrio xe: "; cin >> xe;
    cout << "Masa reducida m: "; cin >> m;

    cout << "Ingrese los parametros de amortiguacion y forzamiento:" << endl;
    cout << "Coeficiente de amortiguamiento gamma: "; cin >> gamma;
    cout << "Amplitud de la fuerza forzante A: "; cin >> A;
    cout << "Frecuencia de la fuerza forzante omega: "; cin >> omega;

    while (gamma < 0){
        cout << "Dato no valido. Ingrese un valor positivo para gamma: ";
        cin >> gamma;
    }


    cout << "\n¿Desea simular DOS osciladores acoplados? (1 para Si, 0 para No): ";
    cin >> opcion_acople;
    
    while (opcion_acople != 1 && opcion_acople != 0) {
        cout << "Opcion no valida. Ingrese 1 para Si, 0 para No: ";
        cin >> opcion_acople;
    }

    if (opcion_acople == 1) {
        cout << "\n--- PARAMETROS DEL SEGUNDO OSCILADOR ---\n";
        cout << "Nota: Se utilizará RK4 para la simulación acoplada.\n";
        cout << "Posicion inicial x0_2: "; cin >> x2_0;
        cout << "Velocidad inicial v0_2: "; cin >> v2_0;
        cout << "Constante de acoplamiento difusivo kappa: "; cin >> kappa;
    } else {
        
        cout << "\nSeleccione el metodo de integracion:" << endl;
        cout << "1. Euler" << endl;
        cout << "2. Euler-Cromer" << endl;
        cout << "3. Runge-Kutta orden 2 (punto medio)" << endl;
        cout << "4. Runge-Kutta orden 3" << endl;
        cout << "5. Runge-Kutta orden 4" << endl;
        cout << "Opcion: ";
        cin >> metodo;

        while (metodo < 1 || metodo > 5) {
            cout << "Opcion no valida. Ingrese un valor entre 1 y 5: ";
            cin >> metodo;
        }
    }
}