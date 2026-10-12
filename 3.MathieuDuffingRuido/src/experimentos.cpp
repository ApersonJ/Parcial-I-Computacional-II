
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

const double kPi = 3.14159265358979323846;

const double kDelta = 0.1;
const double kOmega0 = 1.0;
const double kOmegaMod = 2.0;  // Omega = 2*omega0 (resonancia parametrica principal)
const double kBeta = 1.0;
const double kHBajo = 0.1;     // por debajo del umbral
const double kHAlto = 0.5;     // por encima del umbral

const string kDatos = "results/datos/experimentos/";
const string kFiguras = "results/figuras/experimentos/";
const string kScripts = "scripts/experimentos/";

// Avanza un paso con el esquema indicado: 1 = Euler-Maruyama, 2 = Heun.
void avanzar(Osc_Mathieu_Duffing &osc, int metodo, double t, double dt, double dW,
             double delta, double omega0, double h, double Omega, double beta, double sigma) {
    if (metodo == 1)
        osc.MuevaseEulerMaruyama(t, dt, dW, delta, omega0, h, Omega, beta, sigma);
    else
        osc.MuevaseHeun(t, dt, dW, delta, omega0, h, Omega, beta, sigma);
}

void ejecutarGnuplot(const string &ruta_script) {
    int estado = system(("gnuplot " + ruta_script).c_str());
    if (estado != 0) cerr << "Aviso: gnuplot fallo con " << ruta_script << endl;
}

// ---------------- 3a. Limite lineal: Var[x] estacionaria exacta ----------------
// Con h = 0 y beta = 0 el modelo es lineal y Var[x] = sigma^2 / (2*delta*omega0^2).
double varianzaFinalLineal(int metodo, double dt, double tmax, int N, unsigned long long semilla) {
    const double delta = 0.5, omega0 = 1.0, sigma = 0.5;
    Generador_Ruido ruido;
    ruido.Inicie(semilla);

    double suma = 0.0, suma2 = 0.0;
    long n = lround(tmax / dt);
    for (int r = 0; r < N; ++r) {
        Osc_Mathieu_Duffing osc;
        osc.Inicie(0.0, 0.0);
        for (long i = 0; i < n; ++i) {
            double dW = ruido.calcularIncrementoWiener(dt);
            avanzar(osc, metodo, i * dt, dt, dW, delta, omega0, 0.0, kOmegaMod, 0.0, sigma);
        }
        suma += osc.Getx();
        suma2 += osc.Getx() * osc.Getx();
    }
    double media = suma / N;
    return (suma2 - N * media * media) / (N - 1);
}

void experimentoValidacionLineal() {
    cout << "Experimento 3a: limite lineal (solucion analitica) y sesgo en dt" << endl;

    const double delta = 0.5, omega0 = 1.0, sigma = 0.5;
    const double teoria = sigma * sigma / (2.0 * delta * omega0 * omega0);
    const int N = 20000;
    const vector<double> pasos = {0.2, 0.1, 0.05, 0.025};

    ofstream archivo(kDatos + "validacion_lineal.dat");
    archivo << fixed << setprecision(6);
    archivo << "# dt  Var[x] Euler-Maruyama  Var[x] Heun  teoria   (N = " << N << ")\n";

    cout << "  Var[x] teorica = " << teoria << " (error estadistico ~ "
         << fixed << setprecision(1) << 100.0 * sqrt(2.0 / (N - 1)) << " %)" << endl;
    cout << "  " << setw(8) << "dt" << setw(18) << "Euler-Maruyama" << setw(12) << "Heun" << endl;
    for (size_t i = 0; i < pasos.size(); ++i) {
        double v_em = varianzaFinalLineal(1, pasos[i], 30.0, N, 1000 + i);
        double v_he = varianzaFinalLineal(2, pasos[i], 30.0, N, 2000 + i);
        archivo << pasos[i] << "  " << v_em << "  " << v_he << "  " << teoria << "\n";
        cout << "  " << setw(8) << setprecision(3) << pasos[i] << setw(18) << setprecision(4) << v_em
             << setw(12) << v_he << endl;
    }
    archivo.close();

    ofstream script(kScripts + "plot_validacion_lineal.plt");
    script << "set terminal pngcairo size 900,600 enhanced font 'Verdana,10'\n";
    script << "set output '" << kFiguras << "validacion_lineal.png'\n";
    script << "set title 'Limite lineal: Var[x] estacionaria vs paso dt'\n";
    script << "set xlabel 'Paso dt'\nset ylabel 'Var[x]'\nset grid\nset key left top\n";
    script << "plot '" << kDatos << "validacion_lineal.dat' using 1:2 with linespoints title 'Euler-Maruyama', \\\n";
    script << "     '' using 1:3 with linespoints title 'Heun', \\\n";
    script << "     " << teoria << " with lines lc rgb 'black' dt 2 title 'teoria'\n";
    script.close();
    ejecutarGnuplot(kScripts + "plot_validacion_lineal.plt");
}

void experimentoConvergenciaFuerte() {
    cout << "\nExperimento 3b: convergencia fuerte (mismo ruido, error en x(T))" << endl;

    const double T = 5.0, sigma = 0.2;
    const long nf = 8192;               // pasos finos (potencia de 2: k siempre divide a nf)
    const double dtf = T / nf;
    const vector<long> ks = {4, 8, 16, 32, 64};
    const int M = 300;                  // caminos brownianos

    Generador_Ruido ruido;
    ruido.Inicie(2024);

    vector<double> err_em(ks.size(), 0.0), err_he(ks.size(), 0.0);
    vector<double> dWf(nf);

    for (int p = 0; p < M; ++p) {
        for (long i = 0; i < nf; ++i) dWf[i] = ruido.calcularIncrementoWiener(dtf);

        // Referencia: Heun con el paso fino.
        Osc_Mathieu_Duffing ref;
        ref.Inicie(0.1, 0.0);
        for (long i = 0; i < nf; ++i)
            avanzar(ref, 2, i * dtf, dtf, dWf[i], kDelta, kOmega0, kHAlto, kOmegaMod, kBeta, sigma);

        for (size_t j = 0; j < ks.size(); ++j) {
            double dt = ks[j] * dtf;
            Osc_Mathieu_Duffing em, he;
            em.Inicie(0.1, 0.0);
            he.Inicie(0.1, 0.0);
            for (long i = 0; i < nf / ks[j]; ++i) {
                double dW = 0.0;  // incremento grueso = suma de ks[j] incrementos finos
                for (long q = 0; q < ks[j]; ++q) dW += dWf[i * ks[j] + q];
                avanzar(em, 1, i * dt, dt, dW, kDelta, kOmega0, kHAlto, kOmegaMod, kBeta, sigma);
                avanzar(he, 2, i * dt, dt, dW, kDelta, kOmega0, kHAlto, kOmegaMod, kBeta, sigma);
            }
            err_em[j] += fabs(em.Getx() - ref.Getx()) / M;
            err_he[j] += fabs(he.Getx() - ref.Getx()) / M;
        }
    }

    ofstream archivo(kDatos + "convergencia_fuerte.dat");
    archivo << scientific << setprecision(6);
    archivo << "# dt  E|x - x_ref| Euler-Maruyama  E|x - x_ref| Heun   (M = " << M << ", T = " << T << ")\n";
    for (size_t j = 0; j < ks.size(); ++j)
        archivo << ks[j] * dtf << "  " << err_em[j] << "  " << err_he[j] << "\n";
    archivo.close();

    // ks[0] es el paso mas fino: p = log2(e(2dt)/e(dt)) con los dos pasos mas finos.
    cout << "  Orden fuerte observado p = log2(e(2dt)/e(dt)) con los dos pasos mas finos:" << endl;
    cout << "    Euler-Maruyama p = " << fixed << setprecision(2) << log2(err_em[1] / err_em[0]) << endl;
    cout << "    Heun           p = " << log2(err_he[1] / err_he[0]) << endl;
    cout << "  Error a dt = " << scientific << setprecision(2) << ks[0] * dtf << ": EM = " << err_em[0]
         << ", Heun = " << err_he[0] << endl;

    ofstream script(kScripts + "plot_convergencia_fuerte.plt");
    script << "set terminal pngcairo size 900,600 enhanced font 'Verdana,10'\n";
    script << "set output '" << kFiguras << "convergencia_fuerte.png'\n";
    script << "set title 'Convergencia fuerte (mismo ruido): error en x(T)'\n";
    script << "set xlabel 'Paso dt'\nset ylabel 'E|x(T) - x_{ref}(T)|'\nset logscale xy\nset grid\nset key left top\n";
    script << "plot '" << kDatos << "convergencia_fuerte.dat' using 1:2 with linespoints title 'Euler-Maruyama', \\\n";
    script << "     '' using 1:3 with linespoints title 'Heun'\n";
    script.close();
    ejecutarGnuplot(kScripts + "plot_convergencia_fuerte.plt");
}

// ----------------  Dependencia con sigma ----------------
// Promedio de x^2 sobre el tramo estacionario (t > t_transitorio) y sobre el ensamble.
// Como <x> = 0 por simetria, coincide con Var[x].
double x2Tardio(double h, double sigma, int N, unsigned long long semilla,
                double dt, double tmax, double t_transitorio) {
    Generador_Ruido ruido;
    ruido.Inicie(semilla);

    long n = lround(tmax / dt);
    double total = 0.0;
    for (int r = 0; r < N; ++r) {
        Osc_Mathieu_Duffing osc;
        osc.Inicie(0.1, 0.0);
        double suma = 0.0;
        long cuenta = 0;
        for (long i = 0; i < n; ++i) {
            double t = i * dt;
            if (t > t_transitorio) {
                suma += osc.Getx() * osc.Getx();
                ++cuenta;
            }
            double dW = ruido.calcularIncrementoWiener(dt);
            avanzar(osc, 2, t, dt, dW, kDelta, kOmega0, h, kOmegaMod, kBeta, sigma);
        }
        total += suma / cuenta;
    }
    return total / N;
}

void experimentoBarridoSigma() {
    cout << "\nExperimento 3c: dependencia con la intensidad del ruido sigma (Heun)" << endl;

    const vector<double> sigmas = {0.0, 0.02, 0.05, 0.1, 0.15, 0.2, 0.3};
    const int N = 300;
    const double dt = 0.01, tmax = 200.0, t_transitorio = 100.0;

    ofstream archivo(kDatos + "barrido_sigma.dat");
    archivo << fixed << setprecision(6);
    archivo << "# sigma  <x^2>(h=" << kHBajo << ")  <x^2>(h=" << kHAlto << ")   (N = " << N << ")\n";
    cout << "  " << setw(8) << "sigma" << setw(16) << "h bajo (0.1)" << setw(16) << "h alto (0.5)" << endl;
    for (size_t i = 0; i < sigmas.size(); ++i) {
        double bajo = x2Tardio(kHBajo, sigmas[i], N, 10 + i, dt, tmax, t_transitorio);
        double alto = x2Tardio(kHAlto, sigmas[i], N, 20 + i, dt, tmax, t_transitorio);
        archivo << sigmas[i] << "  " << bajo << "  " << alto << "\n";
        cout << "  " << setw(8) << setprecision(2) << sigmas[i] << setw(16) << setprecision(4) << bajo
             << setw(16) << alto << endl;
    }
    archivo.close();

    // Estabilidad estadistica: mismo caso con distintas semillas.
    cout << "  Estabilidad con la semilla (h = " << kHAlto << ", sigma = 0.1, 5 semillas):" << endl;
    double s1 = 0.0, s2 = 0.0;
    for (int k = 1; k <= 5; ++k) {
        double valor = x2Tardio(kHAlto, 0.1, N, 100 + k, dt, tmax, t_transitorio);
        s1 += valor;
        s2 += valor * valor;
        cout << "    semilla " << 100 + k << ": " << setprecision(4) << valor << endl;
    }
    double media = s1 / 5.0;
    cout << "    media = " << media << ", desv. estandar = " << sqrt((s2 - 5 * media * media) / 4.0) << endl;

    ofstream script(kScripts + "plot_barrido_sigma.plt");
    script << "set terminal pngcairo size 900,600 enhanced font 'Verdana,10'\n";
    script << "set output '" << kFiguras << "barrido_sigma.png'\n";
    script << "set title 'Respuesta cuadratica media vs intensidad del ruido'\n";
    script << "set xlabel '{/Symbol s}'\nset ylabel '<x^2>'\nset grid\nset key left top\n";
    script << "plot '" << kDatos << "barrido_sigma.dat' using 1:2 with linespoints title 'h = " << kHBajo
           << " (bajo el umbral)', \\\n";
    script << "     '' using 1:3 with linespoints title 'h = " << kHAlto << " (sobre el umbral)'\n";
    script.close();
    ejecutarGnuplot(kScripts + "plot_barrido_sigma.plt");
}

// ----------------  Ruido vs determinismo: mapa estroboscopico ----------------
// Se muestrea (x, v) una vez por periodo del forzamiento, T = 2*pi/Omega.
void experimentoEstroboscopico() {
    cout << "\nExperimento 3d: mapa estroboscopico, determinista vs ruido (h = " << kHAlto << ")" << endl;

    const int pasos_por_periodo = 200, periodos_transitorio = 200, periodos_muestreo = 1500;
    const double T = 2.0 * kPi / kOmegaMod;
    const double dt = T / pasos_por_periodo;
    const vector<double> sigmas = {0.0, 0.05, 0.2};
    const vector<string> nombres = {"s0", "s005", "s02"};

    Generador_Ruido ruido;
    ruido.Inicie(7);

    for (size_t s = 0; s < sigmas.size(); ++s) {
        ofstream archivo(kDatos + "estroboscopico_" + nombres[s] + ".dat");
        archivo << fixed << setprecision(6);
        archivo << "# x(nT)  v(nT)   sigma = " << sigmas[s] << "\n";

        Osc_Mathieu_Duffing osc;
        osc.Inicie(0.1, 0.0);
        long paso = 0;
        for (int p = 0; p < periodos_transitorio + periodos_muestreo; ++p) {
            for (int i = 0; i < pasos_por_periodo; ++i) {
                double dW = ruido.calcularIncrementoWiener(dt);
                avanzar(osc, 2, paso * dt, dt, dW, kDelta, kOmega0, kHAlto, kOmegaMod, kBeta, sigmas[s]);
                ++paso;
            }
            if (p >= periodos_transitorio) archivo << osc.Getx() << "  " << osc.Getv() << "\n";
        }
        archivo.close();
    }

    ofstream script(kScripts + "plot_estroboscopico.plt");
    script << "set terminal pngcairo size 800,800 enhanced font 'Verdana,10'\n";
    script << "set output '" << kFiguras << "estroboscopico.png'\n";
    script << "set title 'Mapa estroboscopico (x, v) en t = nT, h = " << kHAlto << "'\n";
    script << "set xlabel 'x(nT)'\nset ylabel 'v(nT)'\nset grid\n";
    script << "plot '" << kDatos << "estroboscopico_s02.dat' using 1:2 with points pt 7 ps 0.3 lc rgb 'red' title '{/Symbol s} = 0.2', \\\n";
    script << "     '" << kDatos << "estroboscopico_s005.dat' using 1:2 with points pt 7 ps 0.3 lc rgb 'blue' title '{/Symbol s} = 0.05', \\\n";
    script << "     '" << kDatos << "estroboscopico_s0.dat' using 1:2 with points pt 7 ps 1.5 lc rgb 'black' title '{/Symbol s} = 0 (determinista)'\n";
    script.close();
    ejecutarGnuplot(kScripts + "plot_estroboscopico.plt");
}

// ----------------  Autocorrelacion y espectro de potencia ----------------
void espectroYAutocorrelacion(double h, double sigma, const string &etiqueta) {
    const int K = 20;                  // realizaciones promediadas
    const double dt = 0.01, ds = 0.1;  // paso de integracion y de muestreo
    const long stride = lround(ds / dt);
    const int M = 4096;                // muestras por realizacion
    const int lag_max = 600;           // 600*ds = 60 unidades de tiempo
    const double omega_max = 4.0;
    const double t_transitorio = 100.0;

    Generador_Ruido ruido;
    ruido.Inicie(31);

    vector<double> corr(lag_max + 1, 0.0);
    int k_max = (int)(omega_max * M * ds / (2.0 * kPi));
    vector<double> psd(k_max + 1, 0.0);

    for (int r = 0; r < K; ++r) {
        Osc_Mathieu_Duffing osc;
        osc.Inicie(0.1, 0.0);
        long n_trans = lround(t_transitorio / dt);
        long paso = 0;
        for (; paso < n_trans; ++paso) {
            double dW = ruido.calcularIncrementoWiener(dt);
            avanzar(osc, 2, paso * dt, dt, dW, kDelta, kOmega0, h, kOmegaMod, kBeta, sigma);
        }
        vector<double> serie(M);
        for (int m = 0; m < M; ++m) {
            serie[m] = osc.Getx();
            for (long q = 0; q < stride; ++q, ++paso) {
                double dW = ruido.calcularIncrementoWiener(dt);
                avanzar(osc, 2, paso * dt, dt, dW, kDelta, kOmega0, h, kOmegaMod, kBeta, sigma);
            }
        }
        double media = 0.0;
        for (int m = 0; m < M; ++m) media += serie[m] / M;
        for (int m = 0; m < M; ++m) serie[m] -= media;

        for (int lag = 0; lag <= lag_max; ++lag) {
            double c = 0.0;
            for (int m = 0; m + lag < M; ++m) c += serie[m] * serie[m + lag];
            corr[lag] += c / M / K;
        }
        // Periodograma por DFT directa.
        for (int k = 0; k <= k_max; ++k) {
            double re = 0.0, im = 0.0;
            double w = 2.0 * kPi * k / M;
            for (int m = 0; m < M; ++m) {
                re += serie[m] * cos(w * m);
                im -= serie[m] * sin(w * m);
            }
            psd[k] += (re * re + im * im) * ds / (2.0 * kPi * M) / K;
        }
    }

    ofstream fc(kDatos + "autocorrelacion_" + etiqueta + ".dat");
    fc << fixed << setprecision(6);
    fc << "# tau  C(tau)/C(0)   h = " << h << ", sigma = " << sigma << "\n";
    for (int lag = 0; lag <= lag_max; ++lag) fc << lag * ds << "  " << corr[lag] / corr[0] << "\n";
    fc.close();

    ofstream fs(kDatos + "espectro_" + etiqueta + ".dat");
    fs << fixed << setprecision(6);
    fs << "# omega  S(omega)   h = " << h << ", sigma = " << sigma << "\n";
    double pico = 0.0, omega_pico = 0.0;
    for (int k = 1; k <= k_max; ++k) {
        double omega = 2.0 * kPi * k / (M * ds);
        fs << omega << "  " << psd[k] << "\n";
        if (psd[k] > pico) { pico = psd[k]; omega_pico = omega; }
    }
    fs.close();
    cout << "  h = " << h << ", sigma = " << sigma << ": pico del espectro en omega = "
         << setprecision(3) << omega_pico << endl;
}

void experimentoEspectro() {
    cout << "\nExperimento 3e: autocorrelacion y espectro de potencia (promedio de 20 realizaciones)" << endl;
    espectroYAutocorrelacion(kHBajo, 0.1, "h_bajo");
    espectroYAutocorrelacion(kHAlto, 0.05, "h_alto");

    ofstream sc(kScripts + "plot_autocorrelacion.plt");
    sc << "set terminal pngcairo size 900,600 enhanced font 'Verdana,10'\n";
    sc << "set output '" << kFiguras << "autocorrelacion.png'\n";
    sc << "set title 'Autocorrelacion normalizada de x(t)'\n";
    sc << "set xlabel '{/Symbol t}'\nset ylabel 'C({/Symbol t}) / C(0)'\nset grid\n";
    sc << "plot '" << kDatos << "autocorrelacion_h_bajo.dat' using 1:2 with lines title 'h = " << kHBajo
       << ", {/Symbol s} = 0.1', \\\n";
    sc << "     '" << kDatos << "autocorrelacion_h_alto.dat' using 1:2 with lines title 'h = " << kHAlto
       << ", {/Symbol s} = 0.05'\n";
    sc.close();
    ejecutarGnuplot(kScripts + "plot_autocorrelacion.plt");

    ofstream ss(kScripts + "plot_espectro.plt");
    ss << "set terminal pngcairo size 900,600 enhanced font 'Verdana,10'\n";
    ss << "set output '" << kFiguras << "espectro.png'\n";
    ss << "set title 'Espectro de potencia de x(t)'\n";
    ss << "set xlabel '{/Symbol w}'\nset ylabel 'S({/Symbol w})'\nset logscale y\nset grid\n";
    ss << "plot '" << kDatos << "espectro_h_bajo.dat' using 1:2 with lines title 'h = " << kHBajo
       << ", {/Symbol s} = 0.1', \\\n";
    ss << "     '" << kDatos << "espectro_h_alto.dat' using 1:2 with lines title 'h = " << kHAlto
       << ", {/Symbol s} = 0.05'\n";
    ss.close();
    ejecutarGnuplot(kScripts + "plot_espectro.plt");
}

int main() {
    int estado = system(("mkdir -p " + kDatos + " " + kFiguras + " " + kScripts).c_str());
    if (estado != 0) {
        cerr << "No se pudieron crear las carpetas de salida." << endl;
        return 1;
    }

    cout << "Experimentos del Punto 3: Mathieu-Duffing con ruido" << endl;
    cout << "Parametros base: delta=" << kDelta << ", omega0=" << kOmega0 << ", Omega=" << kOmegaMod
         << ", beta=" << kBeta << "\n" << endl;

    experimentoValidacionLineal();
    experimentoConvergenciaFuerte();
    experimentoBarridoSigma();
    experimentoEstroboscopico();
    experimentoEspectro();

    cout << "\nListo. Datos en '" << kDatos << "', graficas en '" << kFiguras << "'." << endl;
    return 0;
}