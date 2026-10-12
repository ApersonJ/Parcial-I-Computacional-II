#pragma once

#include <cmath>
#include <iostream>

// ============================================================
//  Clase vector2D
// ============================================================

class vector2D {

private:
    double x, y;

public:

    // 1. Carga y visualización
    void Cargue(double x0, double y0);
    void Show() const;

    // 2. Getters
    double Getx() const { return x; }
    double Gety() const { return y; }

    // 3. Suma y resta
    vector2D operator+(const vector2D &v2) const;
    vector2D operator-(const vector2D &v2) const;

    // 4. Signo menos unario
    vector2D operator-() const;

    // 5. Suma y resta acumuladas
    void operator+=(const vector2D &v2);
    void operator-=(const vector2D &v2);

    // 6. Producto punto y producto cruz
    double operator*(const vector2D &v2) const;
    double operator^(const vector2D &v2) const;

    // 7. Multiplicación y división por escalar
    vector2D operator*(double a) const;
    vector2D operator/(double a) const;

    // 8. Funciones amigas
    friend double Norma2(const vector2D &v1);
    friend double Norma(const vector2D &v1);
};

// Operador global
vector2D operator*(double a, const vector2D &v1);
