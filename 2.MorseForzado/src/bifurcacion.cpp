#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "osc_morse_forzado.h"

using namespace std;

// Parámetros fijos para la bifurcación
const double D = 5.0;
const double a = 1.0; 
const double xe = 0.0;
const double m = 1.0;
const double KGamma = 0.5;
const double omega = 2.6;

// Parámetros numéricos
const double x0 = 0.5;
const double v0 = 0.0;
const int kPasosPorPeriodo = 1000;
const int kPeriodosTransitorios = 200; // Periodos a ignorar para estabilizar la órbita
const int kPeriodosMuestreo = 100;     // Puntos de Poincaré por cada amplitud
const double kUmbralDisociacion = 15.0; // Si x supera esto, el enlace se rompió

// Rutas
const string kDatosBifurcacion = "results/datos/bifurcacion.dat";
const string kScriptBifurcacion = "scripts/plot_bifurcacion.plt";

void ejecutarBifurcacion() {
    ofstream archivo(kDatosBifurcacion);
    archivo << fixed << setprecision(6);
    archivo << "# A    x_poincare    v_poincare\n";

    double T = 2.0 * M_PI / omega;
    double dt = T / kPasosPorPeriodo;

    cout << "Iniciando barrido de amplitud A..." << endl;

    // Barrido de A desde 1.5 hasta 4.0
    for (double A = 1.5; A <= 4.0; A += 0.02) {
        Osc_Morse_Forzado osc;
        osc.Inicie(x0, v0, D, a, xe);
        
        double t = 0.0;
        bool disociado = false;

        // 1. Eliminar el transitorio (no guardamos datos)
        for (int p = 0; p < kPeriodosTransitorios; ++p) {
            for (int i = 0; i < kPasosPorPeriodo; ++i) {
                // RK4 
                osc.MuevaseRK4(t, dt, D, a, xe, m, KGamma, A, omega);
                t += dt;
                
                if (osc.Getx() > kUmbralDisociacion) {
                    disociado = true;
                    break;
                }
            }
            if (disociado) break;
        }

        if (disociado) {
            cout << "Disociación detectada en A = " << A << ". Omitiendo..." << endl;
            continue; // Pasamos al siguiente A
        }

        // 2. Muestreo de Poincaré (guardamos 1 punto por periodo)
        for (int p = 0; p < kPeriodosMuestreo; ++p) {
            for (int i = 0; i < kPasosPorPeriodo; ++i) {
                osc.MuevaseRK4(t, dt, D, a, xe, m, KGamma, A, omega);
                t += dt;
                
                if (osc.Getx() > kUmbralDisociacion) {
                    disociado = true;
                    break;
                }
            }
            if (disociado) break;

            // Al finalizar el periodo exacto, guardamos el estado (Corte de Poincaré)
            archivo << A << "  " << osc.Getx() << "  " << osc.Getv() << "\n";
        }
    }
    archivo.close();
    cout << "Barrido finalizado. Datos guardados en " << kDatosBifurcacion << endl;
}

void generarScriptBifurcacion() {
    ofstream script(kScriptBifurcacion);
    script << "set terminal pngcairo size 1000,700 enhanced font 'Verdana,10'\n";
    script << "set output 'results/figuras/diagrama_bifurcacion.png'\n";
    script << "set title 'Diagrama de Bifurcación: Oscilador de Morse Forzado'\n";
    script << "set xlabel 'Amplitud de forzamiento (A)'\n";
    script << "set ylabel 'Posición x en t = nT (Poincaré)'\n";
    script << "set grid\n";
    // Usamos puntos muy pequeños (pt 7, ps 0.1) para que el caos se vea como una nube
    script << "plot '" << kDatosBifurcacion << "' using 1:2 with points pt 7 ps 0.2 lc rgb 'black' notitle\n";
    script.close();
    
    system(("gnuplot " + kScriptBifurcacion).c_str());
}


// ----------------  Cálculo del exponente de Lyapunov ----------------
const double kPeriodosEval = 300;
const double kD0 = 1e-8;

const string kDatos = "results/datos/lyapunov.dat";
const string kScript = "scripts/plot_lyapunov.plt";

void calcularLyapunov() {

    cout << "Calculando exponente de Lyapunov..." << endl;

    ofstream archivo(kDatos);
    archivo << fixed << setprecision(6);
    archivo << "# A    Lyapunov_Max\n";

    double T = 2.0 * M_PI / omega;
    double dt = T / kPasosPorPeriodo;

    cout << "Calculando exponente de Lyapunov vs Amplitud (A)..." << endl;

    for (double A = 1.5; A <= 4.0; A += 0.02) {
        Osc_Morse_Forzado osc_ref;
        osc_ref.Inicie(x0, v0, D, a, xe);
        
        double t = 0.0;
        bool disociado = false;

        // 1. Transitorio (solo evoluciona la trayectoria de referencia)
        for (int p = 0; p < kPeriodosTransitorios; ++p) {
            for (int i = 0; i < kPasosPorPeriodo; ++i) {
                osc_ref.MuevaseRK4(t, dt, D, a, xe, m, KGamma, A, omega);
                t += dt;
                if (osc_ref.Getx() > kUmbralDisociacion) { disociado = true; break; }
            }
            if (disociado) break;
        }

        if (disociado) {
            cout << "Disociación en A = " << A << endl;
            continue;
        }

        // 2. Crear trayectoria perturbada separada por d0 en posición
        Osc_Morse_Forzado osc_pert;
        osc_pert.Inicie(osc_ref.Getx() + kD0, osc_ref.Getv(), D, a, xe);

        double suma_lyap = 0.0;

        // 3. Evolución y renormalización periódica
        for (int p = 0; p < kPeriodosEval; ++p) {
            for (int i = 0; i < kPasosPorPeriodo; ++i) {
                osc_ref.MuevaseRK4(t, dt, D, a, xe, m, KGamma, A, omega);
                osc_pert.MuevaseRK4(t, dt, D, a, xe, m, KGamma, A, omega);
                t += dt;
                if (osc_ref.Getx() > kUmbralDisociacion) { disociado = true; break; }
            }
            if (disociado) break;

            // Distancia en el espacio de fase d1 = || S_pert - S_ref ||
            double dx = osc_pert.Getx() - osc_ref.Getx();
            double dv = osc_pert.Getv() - osc_ref.Getv();
            double d1 = sqrt(dx * dx + dv * dv);

            if (d1 > 0.0) {
                // Acumular la divergencia logarítmica
                suma_lyap += log(d1 / kD0);
                
                // Renormalizar S_pert acercándolo de nuevo a distancia d0
                double factor = kD0 / d1;
                double x_nuevo = osc_ref.Getx() + dx * factor;
                double v_nuevo = osc_ref.Getv() + dv * factor;
                osc_pert.Inicie(x_nuevo, v_nuevo, D, a, xe);
            }
        }

        if (!disociado) {
            // El exponente es el promedio de las tasas en el tiempo total de evaluación
            double lambda = suma_lyap / (kPeriodosEval * T);
            archivo << A << "  " << lambda << "\n";
        }
    }
    archivo.close();
    cout << "Datos guardados en " << kDatos << endl;
}

void generarScriptLyapunov() {
    ofstream script(kScript);
    script << "set terminal pngcairo size 1000,500 enhanced font 'Verdana,10'\n";
    script << "set output 'results/figuras/lyapunov.png'\n";
    script << "set title 'Mayor Exponente de Lyapunov vs Amplitud de Forzamiento'\n";
    script << "set xlabel 'Amplitud (A)'\n";
    script << "set ylabel '{/Symbol l}_{max}'\n";
    script << "set grid\n";
    // Línea en y=0 para diferenciar visualmente caos (>0) de estabilidad (<0)
    script << "set xzeroaxis linetype 1 linecolor rgb 'black' linewidth 2\n";
    script << "plot '" << kDatos << "' using 1:2 with linespoints pt 7 ps 0.5 lc rgb 'red' title 'Exponente {/Symbol l}'\n";
    script.close();
    
    system(("gnuplot " + kScript).c_str());
}

int main() {
    ejecutarBifurcacion();
    generarScriptBifurcacion();
    calcularLyapunov();
    generarScriptLyapunov();
    return 0;
}