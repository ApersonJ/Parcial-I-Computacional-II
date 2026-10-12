#include "vector.h"

#include <cmath>
#include <iostream>

// ============================================================
//  Implementaciones de vector2D
// ============================================================

void vector2D::Cargue(double x0, double y0) {
    x = x0;
    y = y0;
}

void vector2D::Show() const {
    std::cout << "(" << x << ", " << y << ")";
}

vector2D vector2D::operator+(const vector2D &v2) const {
    vector2D resultado;
    resultado.Cargue(x + v2.x, y + v2.y);
    return resultado;
}

vector2D vector2D::operator-(const vector2D &v2) const {
    vector2D resultado;
    resultado.Cargue(x - v2.x, y - v2.y);
    return resultado;
}

vector2D vector2D::operator-() const {
    vector2D resultado;
    resultado.Cargue(-x, -y);
    return resultado;
}

void vector2D::operator+=(const vector2D &v2) {
    x += v2.x;
    y += v2.y;
}

void vector2D::operator-=(const vector2D &v2) {
    x -= v2.x;
    y -= v2.y;
}

double vector2D::operator*(const vector2D &v2) const {
    return x * v2.x + y * v2.y;
}

double vector2D::operator^(const vector2D &v2) const {
    return x * v2.y - y * v2.x;
}

vector2D vector2D::operator*(double a) const {
    vector2D resultado;
    resultado.Cargue(a * x, a * y);
    return resultado;
}

vector2D vector2D::operator/(double a) const {
    vector2D resultado;

    if (a != 0.0) {
        resultado.Cargue(x / a, y / a);
    }
    else {
        std::cout << "Error: no se puede dividir un vector entre cero.\n";
        resultado.Cargue(0.0, 0.0);
    }

    return resultado;
}

double Norma2(const vector2D &v1) {
    return v1.x * v1.x + v1.y * v1.y;
}

double Norma(const vector2D &v1) {
    return std::sqrt(Norma2(v1));
}

vector2D operator*(double a, const vector2D &v1) {
    vector2D resultado;
    resultado.Cargue(a * v1.Getx(), a * v1.Gety());
    return resultado;
}
