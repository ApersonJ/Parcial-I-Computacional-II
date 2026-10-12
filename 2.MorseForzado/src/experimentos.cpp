
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
 
#include "osc_morse_forzado.h"
 
using namespace std;
 
// Parametros base (regimen acotado: no disocia). w0 = a*sqrt(2D/m) ~ 3.16,
// Omega = 2.0: ambas frecuencias son del mismo orden, dt debe ser << 2*pi/3.16.
const double kD = 5.0;
const double kA = 1.0;
const double kXe = 0.0;
const double kM = 1.0;
const double kGamma = 0.2;
const double kAmplitud = 2.0;
const double kOmega = 2.0;
 
const double kX0 = 0.5;
const double kV0 = 0.0;
const double kDt = 0.01;
const double kTmax = 40.0;
const double kDtRef = 0.0005;  // RK4 muy fino, usado como "solucion exacta"
 
const string kNombres[5] = {"Euler", "Euler-Cromer", "RK2", "RK3", "RK4"};
const string kArchivos[5] = {"euler", "euler_cromer", "rk2", "rk3", "rk4"};
const int kEvaluaciones[5] = {1, 1, 2, 3, 4};  // llamadas a la fuerza por paso
 
const string kDatos = "results/datos/experimentos/";
const string kFiguras = "results/figuras/experimentos/";
const string kScripts = "scripts/experimentos/";
 
// Avanza un paso con el metodo indicado (1..5). Los Muevase* reciben
// referencias no constantes, por eso se copian los parametros.
void avanzar(Osc_Morse_Forzado &osc, int metodo, double t, double dt) {
    double D = kD, a = kA, xe = kXe, m = kM;
    double gamma = kGamma, A = kAmplitud, Omega = kOmega;
 
    if (metodo == 1) osc.MuevaseEuler(t, dt, D, a, xe, m, gamma, A, Omega);
    else if (metodo == 2) osc.MuevaseEulerCromer(t, dt, D, a, xe, m, gamma, A, Omega);
    else if (metodo == 3) osc.MuevaseRK2(t, dt, D, a, xe, m, gamma, A, Omega);
    else if (metodo == 4) osc.MuevaseRK3(t, dt, D, a, xe, m, gamma, A, Omega);
    else osc.MuevaseRK4(t, dt, D, a, xe, m, gamma, A, Omega);
}
 
// Devuelve x en los pasos 0, cada, 2*cada, ... El tiempo se calcula como
// i*dt (no con t += dt) para no acumular error de redondeo en cos(Omega*t).
vector<double> trayectoria(int metodo, double dt, double tmax, long cada) {
    Osc_Morse_Forzado osc;
    osc.Inicie(kX0, kV0, kD, kA, kXe);
 
    long n = lround(tmax / dt);
    vector<double> xs;
    for (long i = 0; i <= n; ++i) {
        if (i % cada == 0) xs.push_back(osc.Getx());
        if (i < n) avanzar(osc, metodo, i * dt, dt);
    }
    return xs;
}
 
// Integra hasta tmax y devuelve solo x(tmax).
double xFinal(int metodo, double dt, double tmax) {
    Osc_Morse_Forzado osc;
    osc.Inicie(kX0, kV0, kD, kA, kXe);
 
    long n = lround(tmax / dt);
    for (long i = 0; i < n; ++i) avanzar(osc, metodo, i * dt, dt);
    return osc.Getx();
}
 
// x(tmax) del RK4 de referencia (se calcula una sola vez).
double xFinalReferencia() {
    static const double xref = xFinal(5, kDtRef, kTmax);
    return xref;
}
 
// Guarda t, x, v, P, E de una simulacion completa.
void simular(int metodo, double dt, double tmax, const string &ruta) {
    Osc_Morse_Forzado osc;
    osc.Inicie(kX0, kV0, kD, kA, kXe);
    double D = kD, a = kA, xe = kXe, m = kM;
 
    ofstream file(ruta);
    file << fixed << setprecision(6);
    file << setw(12) << "t" << setw(15) << "x" << setw(15) << "v"
         << setw(15) << "P" << setw(15) << "E" << "\n";
 
    long n = lround(tmax / dt);
    for (long i = 0; i <= n; ++i) {
        double t = i * dt;
        osc.calcularPotencial(osc.Getx(), D, a, xe);
        file << setw(12) << t << setw(15) << osc.Getx() << setw(15) << osc.Getv()
             << setw(15) << osc.GetP() << setw(15) << osc.GetEnergia(m) << "\n";
        if (i < n) avanzar(osc, metodo, t, dt);
    }
    file.close();
}
 
void ejecutarGnuplot(const string &ruta_script) {
    string comando = "gnuplot " + ruta_script;
    system(comando.c_str());
}
 
// ---------------- 2a. Convergencia al refinar dt ----------------
void experimentoConvergencia() {
    cout << "Etapa 2a: convergencia (error en x(tmax) respecto al RK4 de referencia)" << endl;
 
    const vector<double> pasos = {0.02, 0.01, 0.005, 0.0025, 0.00125};
    const double xref = xFinalReferencia();
    vector<vector<double>> err(pasos.size(), vector<double>(5));
 
    ofstream tabla(kDatos + "convergencia.dat");
    tabla << scientific << setprecision(6);
    tabla << "# dt  Euler  Euler-Cromer  RK2  RK3  RK4\n";
    for (size_t i = 0; i < pasos.size(); ++i) {
        tabla << pasos[i];
        for (int met = 1; met <= 5; ++met) {
            err[i][met - 1] = fabs(xFinal(met, pasos[i], kTmax) - xref);
            tabla << "  " << err[i][met - 1];
        }
        tabla << "\n";
    }
    tabla.close();
 
    // Orden observado con los dos pasos mas finos: p = log2(e(dt)/e(dt/2)).
    size_t k = pasos.size() - 1;
    cout << "  Orden observado p = log2(e(dt)/e(dt/2)):" << endl;
    for (int met = 0; met < 5; ++met) {
        double p = log2(err[k - 1][met] / err[k][met]);
        cout << "    " << setw(13) << left << kNombres[met] << right
             << " p = " << fixed << setprecision(2) << p << endl;
    }
 
    ofstream script(kScripts + "plot_convergencia.plt");
    script << "set terminal pngcairo size 900,600 enhanced font 'Verdana,10'\n";
    script << "set output '" << kFiguras << "convergencia.png'\n";
    script << "set title 'Convergencia: error en x(t_{max}) vs paso dt'\n";
    script << "set xlabel 'Paso dt'\nset ylabel '|x(t_{max}) - x_{ref}|'\n";
    script << "set logscale xy\nset grid\nset key left top\n";
    script << "plot '" << kDatos << "convergencia.dat' using 1:2 with linespoints title 'Euler', \\\n";
    script << "     '' using 1:3 with linespoints title 'Euler-Cromer', \\\n";
    script << "     '' using 1:4 with linespoints title 'RK2', \\\n";
    script << "     '' using 1:5 with linespoints title 'RK3', \\\n";
    script << "     '' using 1:6 with linespoints title 'RK4' linecolor rgb 'black'\n";
    script.close();
    ejecutarGnuplot(kScripts + "plot_convergencia.plt");
}
 
// ---------------- 2b. Costo vs precision ----------------
void experimentoCosto() {
    cout << "\nEtapa 2b: costo vs precision (dt = " << kDt << ")" << endl;
 
    const double xref = xFinalReferencia();
    ofstream tabla(kDatos + "costo.dat");
    tabla << "# metodo  evaluaciones_por_paso  tiempo_ms(dt=kDtRef)  error_final(dt=kDt)\n";
 
    cout << "  " << setw(13) << left << "Metodo" << right << setw(14) << "Evaluac/paso"
         << setw(16) << "Tiempo [ms]" << setw(16) << "Error x(tmax)" << endl;
    for (int met = 1; met <= 5; ++met) {
        auto t0 = chrono::steady_clock::now();
        xFinal(met, kDtRef, kTmax);
        auto t1 = chrono::steady_clock::now();
        double ms = chrono::duration<double, milli>(t1 - t0).count();
 
        double e = fabs(xFinal(met, kDt, kTmax) - xref);
 
        cout << "  " << setw(13) << left << kNombres[met - 1] << right
             << setw(14) << kEvaluaciones[met - 1]
             << setw(16) << fixed << setprecision(2) << ms
             << setw(16) << scientific << setprecision(2) << e << endl;
        tabla << kArchivos[met - 1] << "  " << kEvaluaciones[met - 1] << "  " << ms << "  " << e << "\n";
    }
    tabla.close();
}
 
// ---------------- 2c. Comparacion de trayectorias ----------------
void experimentoMetodos() {
    cout << "\nEtapa 2c: comparacion de trayectorias (dt = " << kDt << ")" << endl;
 
    for (int met = 1; met <= 5; ++met)
        simular(met, kDt, kTmax, kDatos + "metodo_" + kArchivos[met - 1] + ".dat");
 
    // Error |x_metodo(t) - x_ref(t)|: la referencia se muestrea cada kDt.
    long cada = lround(kDt / kDtRef);
    vector<double> ref = trayectoria(5, kDtRef, kTmax, cada);
    vector<vector<double>> xs(5);
    for (int met = 1; met <= 5; ++met) xs[met - 1] = trayectoria(met, kDt, kTmax, 1);
 
    ofstream err(kDatos + "error_vs_t.dat");
    err << scientific << setprecision(6);
    err << "# t  Euler  Euler-Cromer  RK2  RK3  RK4  (|x - x_ref|, piso 1e-16)\n";
    for (size_t i = 0; i < ref.size(); ++i) {
        err << i * kDt;
        for (int met = 0; met < 5; ++met) err << "  " << fmax(fabs(xs[met][i] - ref[i]), 1e-16);
        err << "\n";
    }
    err.close();
 
    ofstream sx(kScripts + "plot_metodos_x.plt");
    sx << "set terminal pngcairo size 900,600 enhanced font 'Verdana,10'\n";
    sx << "set output '" << kFiguras << "comparacion_metodos_x.png'\n";
    sx << "set title 'Morse forzado: x(t) segun el metodo (dt = " << kDt << ")'\n";
    sx << "set xlabel 'Tiempo (t)'\nset ylabel 'Posicion (x)'\nset grid\n";
    sx << "plot ";
    for (int met = 0; met < 5; ++met) {
        sx << "'" << kDatos << "metodo_" << kArchivos[met] << ".dat' using 1:2 with lines title '"
           << kNombres[met] << "'" << (met == 4 ? " linecolor rgb 'black'" : "");
        if (met < 4) sx << ", \\\n     ";
    }
    sx << "\n";
    sx.close();
    ejecutarGnuplot(kScripts + "plot_metodos_x.plt");
 
    ofstream se(kScripts + "plot_error_vs_t.plt");
    se << "set terminal pngcairo size 900,600 enhanced font 'Verdana,10'\n";
    se << "set output '" << kFiguras << "error_vs_tiempo.png'\n";
    se << "set title 'Error respecto a la referencia RK4 (dt = " << kDtRef << ")'\n";
    se << "set xlabel 'Tiempo (t)'\nset ylabel '|x - x_{ref}|'\nset logscale y\nset grid\n";
    se << "plot '" << kDatos << "error_vs_t.dat' using 1:2 with lines title 'Euler', \\\n";
    se << "     '' using 1:3 with lines title 'Euler-Cromer', \\\n";
    se << "     '' using 1:4 with lines title 'RK2', \\\n";
    se << "     '' using 1:5 with lines title 'RK3', \\\n";
    se << "     '' using 1:6 with lines title 'RK4' linecolor rgb 'black'\n";
    se.close();
    ejecutarGnuplot(kScripts + "plot_error_vs_t.plt");
}
 
// ---------------- 2d. Transitorio y regimen estacionario ----------------
void experimentoRegimen() {
    cout << "\nEtapa 2d: transitorio y regimen estacionario (RK4, tmax = 100)" << endl;
 
    simular(5, 0.005, 100.0, kDatos + "regimen_rk4.dat");
 
    ofstream s(kScripts + "plot_regimen.plt");
    s << "set terminal pngcairo size 1500,500 enhanced font 'Verdana,10'\n";
    s << "set output '" << kFiguras << "regimen_transitorio.png'\n";
    s << "set multiplot layout 1,3 title 'Morse forzado y amortiguado (RK4)'\n";
    s << "set grid\n";
    s << "set title 'x(t)'\nset xlabel 'Tiempo (t)'\nset ylabel 'Posicion (x)'\n";
    s << "plot '" << kDatos << "regimen_rk4.dat' using 1:2 with lines notitle linecolor rgb 'blue'\n";
    s << "set title 'Energia mecanica E(t)'\nset ylabel 'E = T + P'\n";
    s << "plot '" << kDatos << "regimen_rk4.dat' using 1:5 with lines notitle linecolor rgb 'dark-orange'\n";
    s << "set title 'Espacio de fase'\nset xlabel 'Posicion (x)'\nset ylabel 'Velocidad (v)'\n";
    s << "plot '" << kDatos << "regimen_rk4.dat' using 2:3 with lines notitle linecolor rgb 'purple'\n";
    s << "unset multiplot\n";
    s.close();
    ejecutarGnuplot(kScripts + "plot_regimen.plt");
}

int main() {
    cout << "Etapa 2: comparacion de metodos numericos (Morse forzado)" << endl;
    cout << "Parametros base: D=" << kD << ", a=" << kA << ", xe=" << kXe << ", m=" << kM
         << ", gamma=" << kGamma << ", A=" << kAmplitud << ", Omega=" << kOmega
         << ", x0=" << kX0 << ", v0=" << kV0 << ", dt=" << kDt << ", tmax=" << kTmax
         << "\n" << endl;
 
    experimentoConvergencia();
    experimentoCosto();
    experimentoMetodos();
    experimentoRegimen();
 
    cout << "\nListo. Datos en '" << kDatos << "', graficas en '" << kFiguras << "'." << endl;
    return 0;
}