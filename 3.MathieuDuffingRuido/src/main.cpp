#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "Ruido.h"
#include "MathieuDuffing.h"

using namespace std;

// Rutas de salida
const string kDirDatos = "results/datos/";
const string kDirFiguras = "results/figuras/";
const string kDirScripts = "scripts/";

const int kTrayectoriasGuardadas = 5;    
const double kLimiteDivergencia = 1.0e6; 

// Prototipos de funciones
void solicitarDatos(double &x0, double &v0,
                    double &dt, double &tmax,
                    double &delta, double &omega0, double &h, double &Omega,
                    double &beta, double &sigma,
                    int &N, unsigned long long &semilla, int &metodo);
void guardarDatos(double x0, double v0, double dt, double tmax,
                  double delta, double omega0, double h, double Omega,
                  double beta, double sigma,
                  int N, unsigned long long semilla, int metodo);
void generarScriptsGnuplot(int n_guardar);

// --------------------------- funcion principal ------------------------------
int ejecutarPunto3() {
    double x0, v0;
    double dt, tmax;
    // Parametros del sistema.
    double delta, omega0, h, Omega, beta, sigma;
    int N;                    
    unsigned long long semilla;
    int metodo;

    solicitarDatos(x0, v0, dt, tmax, delta, omega0, h, Omega, beta, sigma,
                   N, semilla, metodo);

    if (metodo == 1)
        cout << "\nMetodo de integracion          = Euler-Maruyama" << endl;
    else if (metodo == 2)
        cout << "\nMetodo de integracion          = Heun estocastico" << endl;
    else
        cout << "\nMetodo de integracion          = RK4 determinista (sigma ignorado)" << endl;

    // Crear las carpetas de salida (ofstream no las crea).
    int estado = system(("mkdir -p " + kDirDatos + " " + kDirFiguras + " " + kDirScripts).c_str());
    if (estado != 0) {
        cerr << "No se pudieron crear las carpetas de salida." << endl;
        return 1;
    }

    guardarDatos(x0, v0, dt, tmax, delta, omega0, h, Omega, beta, sigma,
                 N, semilla, metodo);

    Generador_Ruido ruido;
    ruido.Inicie(semilla);

    long n = lround(tmax / dt);
    int n_guardar = min(N, kTrayectoriasGuardadas);

    vector<double> suma_x(n + 1, 0.0), suma_x2(n + 1, 0.0);
    vector<double> suma_v(n + 1, 0.0), suma_v2(n + 1, 0.0);
    vector<double> x_final(N);
    vector<vector<double> > trayectorias(n_guardar, vector<double>(n + 1));

    cout << "Simulando " << N << " realizaciones..." << endl;

    for (int r = 0; r < N; ++r) {
        Osc_Mathieu_Duffing Oscilador;
        Oscilador.Inicie(x0, v0);

        for (long i = 0; i <= n; ++i) {
            double t = i * dt;  
            double x = Oscilador.Getx();
            double v = Oscilador.Getv();

            if (fabs(x) > kLimiteDivergencia || x != x) {
                cerr << "La trayectoria diverge (realizacion " << r << ", t = " << t
                     << "). Con beta = 0 y h por encima del umbral de resonancia"
                     << " parametrica x crece sin limite: aumente beta o reduzca h." << endl;
                return 1;
            }

            suma_x[i] += x;
            suma_x2[i] += x * x;
            suma_v[i] += v;
            suma_v2[i] += v * v;
            if (r < n_guardar) trayectorias[r][i] = x;

            if (i < n) {
                if (metodo == 1) {
                    double dW = ruido.calcularIncrementoWiener(dt);
                    Oscilador.MuevaseEulerMaruyama(t, dt, dW, delta, omega0, h, Omega, beta, sigma);
                } else if (metodo == 2) {
                    double dW = ruido.calcularIncrementoWiener(dt);
                    Oscilador.MuevaseHeun(t, dt, dW, delta, omega0, h, Omega, beta, sigma);
                } else {
                    Oscilador.MuevaseRK4(t, dt, delta, omega0, h, Omega, beta);
                }
            }
        }
        x_final[r] = Oscilador.Getx();
    }

    // ---- Momentos del ensamble: <x>, Var[x], <v>, Var[v] ----
    ofstream momentos(kDirDatos + "momentos.dat");
    momentos << fixed << setprecision(6);
    momentos << "# 1:t    2:<x>    3:Var[x]    4:<v>    5:Var[v]\n";
    for (long i = 0; i <= n; ++i) {
        double media_x = suma_x[i] / N;
        double media_v = suma_v[i] / N;
        // Varianza muestral (N-1); con una sola realizacion queda en 0.
        double var_x = (N > 1) ? (suma_x2[i] - N * media_x * media_x) / (N - 1) : 0.0;
        double var_v = (N > 1) ? (suma_v2[i] - N * media_v * media_v) / (N - 1) : 0.0;
        momentos << setw(12) << i * dt << setw(15) << media_x << setw(15) << var_x
                 << setw(15) << media_v << setw(15) << var_v << "\n";
    }
    momentos.close();

    // ---- Algunas trayectorias completas ----
    ofstream tray(kDirDatos + "trayectorias.dat");
    tray << fixed << setprecision(6);
    tray << "# t  x de las primeras " << n_guardar << " realizaciones\n";
    for (long i = 0; i <= n; ++i) {
        tray << setw(12) << i * dt;
        for (int r = 0; r < n_guardar; ++r) tray << setw(15) << trayectorias[r][i];
        tray << "\n";
    }
    tray.close();

    // ---- Muestras de x(tmax): sirven para el histograma P(x, tmax) ----
    ofstream final(kDirDatos + "x_final.dat");
    final << fixed << setprecision(6);
    final << "# x(tmax) de cada realizacion\n";
    for (int r = 0; r < N; ++r) final << x_final[r] << "\n";
    final.close();

    cout << "\nDatos guardados en '" << kDirDatos << "'" << endl;
    cout << "Semilla utilizada: " << ruido.GetSemilla() << endl;

    generarScriptsGnuplot(n_guardar);

    return 0;
}

// --------------- Solicitar los datos -------------------------------
void solicitarDatos(double &x0, double &v0,
                    double &dt, double &tmax,
                    double &delta, double &omega0, double &h, double &Omega,
                    double &beta, double &sigma,
                    int &N, unsigned long long &semilla, int &metodo) {
    cout << "Ingrese las condiciones iniciales" << endl;
    cout << "Posicion inicial x0: "; cin >> x0;
    cout << "Velocidad inicial v0: "; cin >> v0;

    cout << "Ingrese el paso y el tiempo maximo" << endl;
    cout << "Paso dt: "; cin >> dt;
    cout << "Tiempo de simulacion: "; cin >> tmax;

    while (dt <= 0 || tmax <= 0) {
        cout << "Dato no valido. Ingrese tiempos positivos: ";
        cout << "Paso dt: "; cin >> dt;
        cout << "Tiempo de simulacion: "; cin >> tmax;
    }

    cout << "Ingrese los parametros del oscilador" << endl;
    cout << "Amortiguamiento delta (delta > 0): "; cin >> delta;
    cout << "Frecuencia natural omega0 (omega0 > 0): "; cin >> omega0;
    cout << "Amplitud de la modulacion parametrica h: "; cin >> h;
    cout << "Frecuencia de la modulacion Omega (Omega > 0): "; cin >> Omega;
    cout << "Coeficiente no lineal beta (beta >= 0): "; cin >> beta;

    while (delta <= 0 || omega0 <= 0 || Omega <= 0 || beta < 0) {
        cout << "Dato no valido: delta, omega0 y Omega deben ser positivos y beta >= 0." << endl;
        cout << "Amortiguamiento delta: "; cin >> delta;
        cout << "Frecuencia natural omega0: "; cin >> omega0;
        cout << "Frecuencia de la modulacion Omega: "; cin >> Omega;
        cout << "Coeficiente no lineal beta: "; cin >> beta;
    }

    cout << "Ingrese los parametros del ruido" << endl;
    cout << "Intensidad del ruido sigma (sigma >= 0): "; cin >> sigma;
    while (sigma < 0) {
        cout << "Dato no valido. Ingrese sigma >= 0: ";
        cin >> sigma;
    }

    cout << "Numero de realizaciones del ensamble N: "; cin >> N;
    while (N < 1) {
        cout << "Dato no valido. Ingrese N >= 1: ";
        cin >> N;
    }

    cout << "Semilla del generador (entero >= 0): "; cin >> semilla;

    cout << "\nSeleccione el metodo de integracion:" << endl;
    cout << "1. Euler-Maruyama" << endl;
    cout << "2. Heun estocastico" << endl;
    cout << "3. RK4 determinista (Sin Ruido)" << endl;
    cout << "Opcion: ";
    cin >> metodo;

    while (metodo < 1 || metodo > 3) {
        cout << "Opcion no valida. Ingrese un valor entre 1 y 3: ";
        cin >> metodo;
    }
}

// --------------- Registrar los parametros (reproducibilidad) --------------
void guardarDatos(double x0, double v0, double dt, double tmax,
                       double delta, double omega0, double h, double Omega,
                       double beta, double sigma,
                       int N, unsigned long long semilla, int metodo) {
    ofstream archivo(kDirDatos + "parametros.txt");
    archivo << "x0 = " << x0 << "\nv0 = " << v0
            << "\ndt = " << dt << "\ntmax = " << tmax
            << "\ndelta = " << delta << "\nomega0 = " << omega0
            << "\nh = " << h << "\nOmega = " << Omega
            << "\nbeta = " << beta << "\nsigma = " << sigma
            << "\nN = " << N << "\nsemilla = " << semilla
            << "\nmetodo = " << metodo << "\n";
    archivo.close();
}

// --------------- Scripts de Gnuplot ---------------------------------------
void generarScriptsGnuplot(int n_guardar) {
    // 1. Realizaciones individuales y media del ensamble.
    ofstream s1(kDirScripts + "plot_trayectorias.plt");
    s1 << "set terminal pngcairo size 900,600 enhanced font 'Verdana,10'\n";
    s1 << "set output '" << kDirFiguras << "trayectorias.png'\n";
    s1 << "set title 'Mathieu-Duffing con ruido: realizaciones y media'\n";
    s1 << "set xlabel 'Tiempo (t)'\nset ylabel 'Posicion (x)'\nset grid\n";
    s1 << "plot for [c=2:" << n_guardar + 1 << "] '" << kDirDatos
       << "trayectorias.dat' using 1:c with lines lw 1 title sprintf('realizacion %d', c-1), \\\n";
    s1 << "     '" << kDirDatos << "momentos.dat' using 1:2 with lines lw 3 lc rgb 'black' title '<x>(t)'\n";
    s1.close();

    // 2. Momentos del ensamble: <x>(t) y Var[x](t).
    ofstream s2(kDirScripts + "plot_momentos.plt");
    s2 << "set terminal pngcairo size 900,800 enhanced font 'Verdana,10'\n";
    s2 << "set output '" << kDirFiguras << "momentos.png'\n";
    s2 << "set multiplot layout 2,1 title 'Momentos del ensamble'\n";
    s2 << "set grid\nset xlabel 'Tiempo (t)'\n";
    s2 << "set ylabel '<x>'\n";
    s2 << "plot '" << kDirDatos << "momentos.dat' using 1:2 with lines lc rgb 'blue' notitle\n";
    s2 << "set ylabel 'Var[x]'\n";
    s2 << "plot '" << kDirDatos << "momentos.dat' using 1:3 with lines lc rgb 'red' notitle\n";
    s2 << "unset multiplot\n";
    s2.close();

    // 3. Histograma normalizado de x(tmax): estimacion de P(x, tmax).
    ofstream s3(kDirScripts + "plot_histograma.plt");
    s3 << "set terminal pngcairo size 900,600 enhanced font 'Verdana,10'\n";
    s3 << "set output '" << kDirFiguras << "histograma.png'\n";
    s3 << "set title 'Distribucion de x al final de la simulacion'\n";
    s3 << "set xlabel 'Posicion (x)'\nset ylabel 'P(x, t_{max})'\nset grid\n";
    s3 << "stats '" << kDirDatos << "x_final.dat' using 1 nooutput\n";
    s3 << "bw = (STATS_max - STATS_min) / 40.0\n";
    s3 << "if (bw == 0) bw = 1.0\n";
    s3 << "bin(x) = bw * floor(x / bw) + bw / 2.0\n";
    s3 << "set boxwidth bw\nset style fill solid 0.5\n";
    s3 << "plot '" << kDirDatos << "x_final.dat' using (bin($1)):(1.0 / (STATS_records * bw)) "
       << "smooth freq with boxes lc rgb 'forest-green' notitle\n";
    s3.close();

    cout << "\nGenerando graficas con Gnuplot..." << endl;
    int e1 = system(("gnuplot " + kDirScripts + "plot_trayectorias.plt").c_str());
    int e2 = system(("gnuplot " + kDirScripts + "plot_momentos.plt").c_str());
    int e3 = system(("gnuplot " + kDirScripts + "plot_histograma.plt").c_str());
    if (e1 != 0 || e2 != 0 || e3 != 0)
        cerr << "Aviso: gnuplot no pudo generar alguna grafica." << endl;
    else
        cout << "Graficas guardadas en '" << kDirFiguras << "'" << endl;
}