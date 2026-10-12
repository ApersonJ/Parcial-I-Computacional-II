#ifndef GENERADOR_RUIDO_H_
#define GENERADOR_RUIDO_H_

#include <random>

// ---------------------- Clase Generador_Ruido ----------------------
//   dW = sqrt(dt) * eta,   eta ~ N(0,1)
class Generador_Ruido {
 private:
    std::mt19937_64 motor;
    std::normal_distribution<double> normal;
    unsigned long long semilla;

 public:
    void Inicie(unsigned long long semilla_nueva);

    double calcularIncrementoWiener(double dt);

    unsigned long long GetSemilla() const { return semilla; }
};

#endif 