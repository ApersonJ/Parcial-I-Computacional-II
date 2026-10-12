#include "MathieuDuffing.h"

#include <cmath>

using namespace std;

void Osc_Mathieu_Duffing::Inicie(double x0, double v0) {
    x = x0;
    v = v0;
}

double Osc_Mathieu_Duffing::calcularAceleracion(double t, double x, double v,
                                                 double delta, double omega0, double h,
                                                 double Omega, double beta) const {
    double fuerza_amortiguamiento = -delta * v;
    double fuerza_parametrica = -(omega0 * omega0 + h * cos(Omega * t)) * x;
    double fuerza_cubica = -beta * x * x * x;
    return fuerza_amortiguamiento + fuerza_parametrica + fuerza_cubica;
}

// Sin ruido (sigma = 0): sirve de control para comparar con los esquemas
// estocasticos.
void Osc_Mathieu_Duffing::MuevaseRK4(double t, double dt,
                                     double delta, double omega0, double h,
                                     double Omega, double beta) {
    double Kx1, Kx2, Kx3, Kx4;
    double Kv1, Kv2, Kv3, Kv4;

    Kx1 = v;
    Kv1 = calcularAceleracion(t, x, v, delta, omega0, h, Omega, beta);

    double x2 = x + dt / 2 * Kx1;
    double v2 = v + dt / 2 * Kv1;
    Kx2 = v2;
    Kv2 = calcularAceleracion(t + dt / 2.0, x2, v2, delta, omega0, h, Omega, beta);

    double x3 = x + dt / 2 * Kx2;
    double v3 = v + dt / 2 * Kv2;
    Kx3 = v3;
    Kv3 = calcularAceleracion(t + dt / 2.0, x3, v3, delta, omega0, h, Omega, beta);

    double x4 = x + dt * Kx3;
    double v4 = v + dt * Kv3;
    Kx4 = v4;
    Kv4 = calcularAceleracion(t + dt, x4, v4, delta, omega0, h, Omega, beta);

    x += (dt / 6.0) * (Kx1 + 2 * Kx2 + 2 * Kx3 + Kx4);
    v += (dt / 6.0) * (Kv1 + 2 * Kv2 + 2 * Kv3 + Kv4);
}

// Euler-Maruyama: v_{n+1} = v_n + a_n dt + sigma dW.
void Osc_Mathieu_Duffing::MuevaseEulerMaruyama(double t, double dt, double dW,
                                               double delta, double omega0, double h,
                                               double Omega, double beta, double sigma) {
    double a_actual = calcularAceleracion(t, x, v, delta, omega0, h, Omega, beta);
    x += dt * v;  // usa v_n: v todavia no se ha actualizado
    v += a_actual * dt + sigma * dW;
}

// Heun estocastico (predictor-corrector) con el MISMO dW en ambas etapas.
// Con ruido aditivo la interpretacion de Ito y la de Stratonovich coinciden,
// asi que el esquema es valido para este modelo.
void Osc_Mathieu_Duffing::MuevaseHeun(double t, double dt, double dW,
                                      double delta, double omega0, double h,
                                      double Omega, double beta, double sigma) {
    double a1 = calcularAceleracion(t, x, v, delta, omega0, h, Omega, beta);

    // Predictor (un paso de Euler-Maruyama).
    double x_pred = x + dt * v;
    double v_pred = v + a1 * dt + sigma * dW;

    // Corrector: promedio de las pendientes en t_n y t_{n+1}.
    double a2 = calcularAceleracion(t + dt, x_pred, v_pred, delta, omega0, h, Omega, beta);

    x += 0.5 * dt * (v + v_pred);
    v += 0.5 * dt * (a1 + a2) + sigma * dW;
}