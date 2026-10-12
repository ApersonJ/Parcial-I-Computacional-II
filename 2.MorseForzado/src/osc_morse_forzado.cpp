#include "osc_morse_forzado.h"

#include <cmath>
#include <fstream>
#include <iostream>

using namespace std;

void Osc_Morse_Forzado::Inicie(double x0, double v0,
                        double D, double a, double xe) {
    x = x0;
    v = v0;
    P = D * (1 - exp(-a * (x0 - xe))) * (1 - exp(-a * (x0 - xe)));
}

double Osc_Morse_Forzado::calcularAceleracion(double t, double x, double v,
                                                double D, double a, double xe, double m,
                                                double gamma, double A, double omega) const {
    double force_morse, force_damping, force_forced;
    force_morse = -2.0 * D * a * (exp(-a * (x - xe)) - exp(-2.0 * a * (x - xe)));;
    force_damping = -gamma * v;
    force_forced = A * cos(omega * t);
    return (force_morse + force_damping + force_forced) / m;
}

void Osc_Morse_Forzado::calcularPotencial(double x,
                                           double D, double a, double xe) {
    P = D * (1 - exp(-a * (x - xe))) * (1 - exp(-a * (x - xe)));
}

void Osc_Morse_Forzado::MuevaseEuler(double t, double dt, double D, double a, double xe, double m,
                                     double gamma, double A, double omega) {
    // Hay que guardar la aceleracion antes para que v se calcule con el primer x y no con el actualizado.
    double a_old = calcularAceleracion(t, x, v, D, a, xe, m, gamma, A, omega);
    x += dt * v;
    v += dt * a_old;
}

void Osc_Morse_Forzado::MuevaseEulerCromer(double t, double dt, double D, double a, double xe, double m,
                                           double gamma, double A, double omega) {
    v += dt * calcularAceleracion(t, x, v, D, a, xe, m, gamma, A, omega);
    x += dt * v;
}

void Osc_Morse_Forzado::MuevaseRK2(double t, double dt, double D, double a, double xe, double m,
                                   double gamma, double A, double omega) {
    double Kx1, Kv1;
    double Kx2, Kv2;

    // Coeficientes k1 en el estado actual.
    Kx1 = v;
    Kv1 = calcularAceleracion(t, x, v, D, a, xe, m, gamma, A, omega);

    // Coeficientes k2 evaluados en el punto medio del paso.
    double x_medio = x + (dt / 2.0) * Kx1;
    double v_medio = v + (dt / 2.0) * Kv1;
    Kx2 = v_medio;
    Kv2 = calcularAceleracion(t + dt / 2.0, x_medio, v_medio, D, a, xe, m, gamma, A, omega);

    // Actualizacion con el metodo del punto medio.
    x += dt * Kx2;
    v += dt * Kv2;
}

void Osc_Morse_Forzado::MuevaseRK3(double t, double dt, double D, double a, double xe, double m,
                                   double gamma, double A, double omega) {
    double Kx1, Kv1;
    double Kx2, Kv2;
    double Kx3, Kv3;

    // Coeficientes k1.
    Kx1 = v;
    Kv1 = calcularAceleracion(t, x, v, D, a, xe, m, gamma, A, omega);

    // Coeficientes k2 
    double x2 = x + (dt / 2.0) * Kx1;
    double v2 = v + (dt / 2.0) * Kv1;
    Kx2 = v2;
    Kv2 = calcularAceleracion(t + dt / 2.0, x2, v2, D, a, xe, m, gamma, A, omega);

    // Coeficientes k3 
    double x3 = x - dt * Kx1 + 2.0 * dt * Kx2;
    double v3 = v - dt * Kv1 + 2.0 * dt * Kv2;
    Kx3 = v3;
    Kv3 = calcularAceleracion(t + dt, x3, v3, D, a, xe, m, gamma, A, omega);

    // Actualizacion de x y v.
    x += (dt / 6.0) * (Kx1 + 4.0 * Kx2 + Kx3);
    v += (dt / 6.0) * (Kv1 + 4.0 * Kv2 + Kv3);
}

void Osc_Morse_Forzado::MuevaseRK4(double t, double dt, double D, double a, double xe, double m,
                                   double gamma, double A, double omega) {
    double Kx1, Kx2, Kx3, Kx4;
    double Kv1, Kv2, Kv3, Kv4;

    // Coeficientes K1.
    Kx1 = v;
    Kv1 = calcularAceleracion(t, x, v, D, a, xe, m, gamma, A, omega);

    // Coeficientes K2.
    double x2 = x + dt / 2 * Kx1;
    double v2 = v + dt / 2 * Kv1;
    Kx2 = v2;
    Kv2 = calcularAceleracion(t + dt / 2.0, x2, v2, D, a, xe, m, gamma, A, omega);

    // Coeficientes K3.
    double x3 = x + dt / 2 * Kx2;
    double v3 = v + dt / 2 * Kv2;
    Kx3 = v3;
    Kv3 = calcularAceleracion(t + dt / 2.0, x3, v3, D, a, xe, m, gamma, A, omega);

    // Coeficientes K4.
    double x4 = x + dt * Kx3;
    double v4 = v + dt * Kv3;
    Kx4 = v4;
    Kv4 = calcularAceleracion(t + dt, x4, v4, D, a, xe, m, gamma, A, omega);

    // Actualizacion de x y v.
    x += (dt / 6.0) * (Kx1 + 2 * Kx2 + 2 * Kx3 + Kx4);
    v += (dt / 6.0) * (Kv1 + 2 * Kv2 + 2 * Kv3 + Kv4);
}

// ------------------- Generar scripts y graficar -----------------------
void Osc_Morse_Forzado::generarScriptsGnuplot() {
    // 1. Script para Posicion vs Tiempo x(t).
    ofstream script_x("scripts/plot_x_t.plt");
    script_x << "set terminal pngcairo size 800,600 enhanced font 'Verdana,10'\n";
    script_x << "set output 'results/figuras/posicion_vs_tiempo.png'\n";
    script_x << "set title 'Oscilador de Morse: Posicion vs Tiempo'\n";
    script_x << "set xlabel 'Tiempo (t)'\n";
    script_x << "set ylabel 'Posicion (x)'\n";
    script_x << "set grid\n";
    script_x << "plot 'results/datos/trayectoria.dat' using 1:2 with lines title 'x(t)' linecolor rgb 'blue'\n";
    script_x.close();

    // 2. Script para Velocidad vs Tiempo v(t).
    ofstream script_v("scripts/plot_v_t.plt");
    script_v << "set terminal pngcairo size 800,600 enhanced font 'Verdana,10'\n";
    script_v << "set output 'results/figuras/velocidad_vs_tiempo.png'\n";
    script_v << "set title 'Oscilador de Morse: Velocidad vs Tiempo'\n";
    script_v << "set xlabel 'Tiempo (t)'\n";
    script_v << "set ylabel 'Velocidad (v)'\n";
    script_v << "set grid\n";
    script_v << "plot 'results/datos/trayectoria.dat' using 1:3 with lines title 'v(t)' linecolor rgb 'red'\n";
    script_v.close();

    // 3. Script para Potencial vs Posicion P(x).
    ofstream script_p("scripts/plot_P_x.plt");
    script_p << "set terminal pngcairo size 800,600 enhanced font 'Verdana,10'\n";
    script_p << "set output 'results/figuras/potencial_vs_posicion.png'\n";
    script_p << "set title 'Oscilador de Morse: Energia Potencial vs Posicion'\n";
    script_p << "set xlabel 'Posicion (x)'\n";
    script_p << "set ylabel 'Energia Potencial (P)'\n";
    script_p << "set grid\n";
    script_p << "plot 'results/datos/trayectoria.dat' using 2:4 with lines title 'P(x)' linecolor rgb 'forest-green'\n";
    script_p.close();

    // 4. Script para Espacio de Fase v(x).
    ofstream script_fase("scripts/plot_fase.plt");
    script_fase << "set terminal pngcairo size 800,600 enhanced font 'Verdana,10'\n";
    script_fase << "set output 'results/figuras/espacio_fase.png'\n";
    script_fase << "set title 'Oscilador de Morse: Espacio de Fase'\n";
    script_fase << "set xlabel 'Posicion (x)'\n";
    script_fase << "set ylabel 'Velocidad (v)'\n";
    script_fase << "set grid\n";
    script_fase << "plot 'results/datos/trayectoria.dat' using 2:3 with lines title 'v(x)' linecolor rgb 'purple'\n";
    script_fase.close();

    // 5. Script para Energia: E(t) y error relativo (E(t)-E0)/E0.
    // El oscilador de Morse es conservativo, asi que E(t) permite diagnosticar la precision de los metodos numericos.
    ofstream script_e("scripts/plot_energia.plt");
    script_e << "set terminal pngcairo size 800,900 enhanced font 'Verdana,10'\n";
    script_e << "set output 'results/figuras/energia.png'\n";
    script_e << "set multiplot layout 2,1 title 'Oscilador de Morse: Diagnostico de energia'\n";
    script_e << "set grid\n";
    script_e << "set xlabel 'Tiempo (t)'\n";
    script_e << "set ylabel 'Energia total E(t)'\n";
    script_e << "unset title\n";
    script_e << "plot 'results/datos/trayectoria.dat' using 1:5 with lines title 'E(t)' linecolor rgb 'dark-orange'\n";
    script_e << "set ylabel 'Error relativo (E(t)-E0)/E0'\n";
    script_e << "plot 'results/datos/trayectoria.dat' using 1:6 with lines title '(E-E0)/E0' linecolor rgb 'black'\n";
    script_e << "unset multiplot\n";
    script_e.close();

    // Ejecutar los scripts 
    cout << "\nGenerando graficas con Gnuplot..." << endl;
    system("gnuplot scripts/plot_x_t.plt");
    system("gnuplot scripts/plot_v_t.plt");
    system("gnuplot scripts/plot_P_x.plt");
    system("gnuplot scripts/plot_fase.plt");
    system("gnuplot scripts/plot_energia.plt");

    cout << "Graficas guardadas correctamente en la carpeta 'results/figuras/'" << endl;
}
