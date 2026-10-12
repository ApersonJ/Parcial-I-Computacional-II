#ifndef OSC_MATHIEU_DUFFING_H_
#define OSC_MATHIEU_DUFFING_H_

// ---------------------- Clase Osc_Mathieu_Duffing ----------------------
// Aqui solo vive Mathieu-Duffing, sin ruido. La parte estocastica se implementa aparte.
class Osc_Mathieu_Duffing {
 private:
    double x, v;

 public:
    void Inicie(double x0, double v0);

    // Parte determinista (sin ruido).
    double calcularAceleracion(double t, double x, double v,
                                double delta, double omega0, double h,
                                double Omega, double beta) const;

    void MuevaseRK4(double t, double dt,
                    double delta, double omega0, double h,
                    double Omega, double beta);

    void MuevaseEulerMaruyama(double t, double dt, double dW,
                              double delta, double omega0, double h,
                              double Omega, double beta, double sigma);

    void MuevaseHeun(double t, double dt, double dW,
                     double delta, double omega0, double h,
                     double Omega, double beta, double sigma);

    double Getx() const { return x; }
    double Getv() const { return v; }
};

#endif  