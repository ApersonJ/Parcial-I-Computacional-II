#ifndef OSC_MORSE_FORZADO_H_
#define OSC_MORSE_FORZADO_H_

// ---------------------- Clase Osc_Morse_Forzado ----------------------
// Representa un oscilador de Morse Forzado y Amortiguado.
class Osc_Morse_Forzado {
 private:
    double x, v;
    double P;

 public:
    void Inicie(double x0, double v0,
                double D, double a, double xe);

    double calcularAceleracion(double t, double x, double v,
                                double D, double a, double xe, double m,
                                double gamma, double A, double omega) const;

    void calcularPotencial(double x,
                            double D, double a, double xe);

    
    void MuevaseEuler(double t, double dt, double D, double a, double xe, double m,
                      double gamma, double A, double omega);

    void MuevaseEulerCromer(double t, double dt, double D, double a, double xe, double m,
                            double gamma, double A, double omega);

    void MuevaseRK2(double t, double dt, double D, double a, double xe, double m,
                     double gamma, double A, double omega);
                     
    void MuevaseRK3(double t, double dt, double D, double a, double xe, double m,
                     double gamma, double A, double omega);

    void MuevaseRK4(double t, double dt, double D, double a, double xe, double m,
                     double gamma, double A, double omega);

    // Genera y ejecuta los scripts de Gnuplot para la simulacion principal.
    void generarScriptsGnuplot();

    double Getx() const { return x; }
    double Getv() const { return v; }
    double GetP() const { return P; }

    // Energia mecanica total E, ya no se espera que sea constante, 
    // ya que el oscilador es forzado y amortiguado.
    double GetEnergia(double m) const { return 0.5 * m * v * v + P; }
};

#endif  // OSC_MORSE_FORZADO_H_
