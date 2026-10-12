#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <cstdlib>

#include "osc_morse_forzado.h"
#include "../include/acoplados.h"

using namespace std;

// Función auxiliar
double calcularAcelAcoplada(const Osc_Morse_Forzado& osc_calc, double t, double x, double v, double x_vecino,
                            double D, double a, double xe, double m, double kGamma, double A, double omega, double kappa) {
    double a_aislada = osc_calc.calcularAceleracion(t, x, v, D, a, xe, m, kGamma, A, omega);
    double a_acople = kappa * (x_vecino - x) / m;
    return a_aislada + a_acople;
}

// Integrador RK4 simultáneo
void avanzarAcopladosRK4(Osc_Morse_Forzado &osc1, Osc_Morse_Forzado &osc2, double t, double dt,
                         double D, double a, double xe, double m, double kGamma, double A, double omega, double kappa) {
    double x1 = osc1.Getx(), v1 = osc1.Getv();
    double x2 = osc2.Getx(), v2 = osc2.Getv();

    double Kx1_1 = v1;
    double Kv1_1 = calcularAcelAcoplada(osc1, t, x1, v1, x2, D, a, xe, m, kGamma, A, omega, kappa);
    double Kx2_1 = v2;
    double Kv2_1 = calcularAcelAcoplada(osc2, t, x2, v2, x1, D, a, xe, m, kGamma, A, omega, kappa);

    double t_med = t + 0.5 * dt;
    double x1_2 = x1 + 0.5 * dt * Kx1_1, v1_2 = v1 + 0.5 * dt * Kv1_1;
    double x2_2 = x2 + 0.5 * dt * Kx2_1, v2_2 = v2 + 0.5 * dt * Kv2_1;

    double Kx1_2 = v1_2;
    double Kv1_2 = calcularAcelAcoplada(osc1, t_med, x1_2, v1_2, x2_2, D, a, xe, m, kGamma, A, omega, kappa);
    double Kx2_2 = v2_2;
    double Kv2_2 = calcularAcelAcoplada(osc2, t_med, x2_2, v2_2, x1_2, D, a, xe, m, kGamma, A, omega, kappa);

    double x1_3 = x1 + 0.5 * dt * Kx1_2, v1_3 = v1 + 0.5 * dt * Kv1_2;
    double x2_3 = x2 + 0.5 * dt * Kx2_2, v2_3 = v2 + 0.5 * dt * Kv2_2;

    double Kx1_3 = v1_3;
    double Kv1_3 = calcularAcelAcoplada(osc1, t_med, x1_3, v1_3, x2_3, D, a, xe, m, kGamma, A, omega, kappa);
    double Kx2_3 = v2_3;
    double Kv2_3 = calcularAcelAcoplada(osc2, t_med, x2_3, v2_3, x1_3, D, a, xe, m, kGamma, A, omega, kappa);

    double t_fin = t + dt;
    double x1_4 = x1 + dt * Kx1_3, v1_4 = v1 + dt * Kv1_3;
    double x2_4 = x2 + dt * Kx2_3, v2_4 = v2 + dt * Kv2_3;

    double Kx1_4 = v1_4;
    double Kv1_4 = calcularAcelAcoplada(osc1, t_fin, x1_4, v1_4, x2_4, D, a, xe, m, kGamma, A, omega, kappa);
    double Kx2_4 = v2_4;
    double Kv2_4 = calcularAcelAcoplada(osc2, t_fin, x2_4, v2_4, x1_4, D, a, xe, m, kGamma, A, omega, kappa);

    double x1_nuevo = x1 + (dt / 6.0) * (Kx1_1 + 2*Kx1_2 + 2*Kx1_3 + Kx1_4);
    double v1_nuevo = v1 + (dt / 6.0) * (Kv1_1 + 2*Kv1_2 + 2*Kv1_3 + Kv1_4);
    double x2_nuevo = x2 + (dt / 6.0) * (Kx2_1 + 2*Kx2_2 + 2*Kx2_3 + Kx2_4);
    double v2_nuevo = v2 + (dt / 6.0) * (Kv2_1 + 2*Kv2_2 + 2*Kv2_3 + Kv2_4);

    osc1.Inicie(x1_nuevo, v1_nuevo, D, a, xe);
    osc2.Inicie(x2_nuevo, v2_nuevo, D, a, xe);
}

void generarScriptGnuplotAcoplados(double kappa) {
    string dir_figuras = "results/figuras/acoplados/";
    string script_file = "scripts/plot_acoplados.plt";
    system(("mkdir -p " + dir_figuras).c_str());

    ofstream script(script_file);
    script << "set terminal pngcairo size 1000,800 enhanced font 'Verdana,10'\n";
    script << "set grid\n";

    // ---- IMAGEN 1: Sincronización (x1 vs x2 y Error e(t)) ----
    script << "set output '" << dir_figuras << "sincronizacion_k_" << kappa << ".png'\n";
    script << "set multiplot layout 2,1 title 'Sincronizacion ({/Symbol k} = " << kappa << ")'\n";
    script << "set xlabel 'Tiempo (t)'\nset ylabel 'Posiciones'\n";
    script << "plot 'results/datos/acoplados.dat' using 1:2 w l lw 1.5 lc rgb 'blue' t 'x_1', \\\n";
    script << "     '' using 1:5 w l lw 1.5 lc rgb 'red' dt 2 t 'x_2'\n";
    
    script << "set title 'Error e(t) = |x_1 - x_2|'\nset ylabel 'Error'\n";
    script << "set logscale y\nset format y '10^{%L}'\n";
    script << "plot 'results/datos/acoplados.dat' using 1:8 w l lc rgb 'black' t 'e(t)'\n";
    script << "unset multiplot\n";
    script << "unset logscale y\nset format y '%g'\n"; 

    // ---- IMAGEN 2: Dinámica temporal múltiple (x, v, P vs t) ----
    script << "set output '" << dir_figuras << "dinamica_k_" << kappa << ".png'\n";
    script << "set multiplot layout 3,1 title 'Evolucion Temporal ({/Symbol k} = " << kappa << ")'\n";
    
    script << "unset title\n"; 
    
    script << "set xlabel 'Tiempo (t)'\n";
    script << "set ylabel 'Posicion x'\n";
    script << "plot 'results/datos/acoplados.dat' using 1:2 w l lc rgb 'blue' t 'x_1', '' using 1:5 w l lc rgb 'red' dt 2 t 'x_2'\n";
    script << "set ylabel 'Velocidad v'\n";
    script << "plot 'results/datos/acoplados.dat' using 1:3 w l lc rgb 'blue' t 'v_1', '' using 1:6 w l lc rgb 'red' dt 2 t 'v_2'\n";
    script << "set ylabel 'Potencial P'\n";
    script << "plot 'results/datos/acoplados.dat' using 1:4 w l lc rgb 'blue' t 'P_1', '' using 1:7 w l lc rgb 'red' dt 2 t 'P_2'\n";
    script << "unset multiplot\n";

    // ---- IMAGEN 3: Espacio de Fase (v vs x) ----
    script << "set terminal pngcairo size 800,800 enhanced font 'Verdana,10'\n"; 
    script << "set output '" << dir_figuras << "fase_k_" << kappa << ".png'\n";
    script << "set title 'Espacio de Fase ({/Symbol k} = " << kappa << ")'\n";
    script << "set xlabel 'Posicion (x)'\nset ylabel 'Velocidad (v)'\n";
    script << "plot 'results/datos/acoplados.dat' using 2:3 w l lc rgb 'blue' t 'Oscilador 1', \\\n";
    script << "     '' using 5:6 w l lc rgb 'red' dt 2 t 'Oscilador 2'\n";
    
    script.close();

    cout << "Generando graficas con Gnuplot..." << endl;
    system(("gnuplot " + script_file).c_str());
    cout << "3 graficas guardadas exitosamente en: " << dir_figuras << "\n" << endl;
}

void ejecutarAcoplados(double x1_0, double v1_0, double x2_0, double v2_0,
                       double D, double a, double xe, double m,
                       double kGamma, double A, double omega, double kappa) {
    
    Osc_Morse_Forzado osc1, osc2;
    osc1.Inicie(x1_0, v1_0, D, a, xe);
    osc2.Inicie(x2_0, v2_0, D, a, xe);

    double dt = 0.01;
    double Tmax = 80.0; // Mantenemos 80 segundos para que se vea claro el caos
    string datos_file = "results/datos/acoplados.dat";

    ofstream file(datos_file);
    file << fixed << setprecision(6);
    file << "# 1:t    2:x1    3:v1    4:P1    5:x2    6:v2    7:P2    8:error\n";

    long n = lround(Tmax / dt);
    cout << "\nSimulando dinamica acoplada (Esto puede tomar unos segundos)...\n";

    for (long i = 0; i <= n; ++i) {
        double t = i * dt;
        
        // Calcular los potenciales en la posición actual antes de guardarlos
        osc1.calcularPotencial(osc1.Getx(), D, a, xe);
        osc2.calcularPotencial(osc2.Getx(), D, a, xe);
        
        double error_sync = fabs(osc1.Getx() - osc2.Getx());

        // Guardar las 8 columnas requeridas para todas las gráficas
        file << setw(10) << t 
             << setw(15) << osc1.Getx() 
             << setw(15) << osc1.Getv()
             << setw(15) << osc1.GetP()
             << setw(15) << osc2.Getx() 
             << setw(15) << osc2.Getv()
             << setw(15) << osc2.GetP()
             << setw(15) << scientific << error_sync << "\n";
        
        if (i < n) avanzarAcopladosRK4(osc1, osc2, t, dt, D, a, xe, m, kGamma, A, omega, kappa);
    }
    file.close();
    
    generarScriptGnuplotAcoplados(kappa);
}