#include "Ruido.h"

#include <cmath>

void Generador_Ruido::Inicie(unsigned long long semilla_nueva) {
    semilla = semilla_nueva;
    motor.seed(semilla_nueva);
    normal.reset();  // descarta el valor guardado por la distribucion
}

double Generador_Ruido::calcularIncrementoWiener(double dt) {
    return std::sqrt(dt) * normal(motor);
}