#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "vector.h"
#include "collision.h"

using namespace std;

void gen_frames(int n, double dt, double L, double h, double v_p, vector<particula> &sistema);

int main(){
	int n;
	double L, h;
	double dt = 0.1;
	double v_p;
	
	
	cout << "Ingrese el numero de particulas: "; cin >> n;
	cout << "Ingrese el ancho de la caja (m): "; cin >> L;
	cout << "Ingrese el alto de la caja (m): "; cin >> h;
	cout << "Ingerese la velocidad de la pared (m/s): "; cin >> v_p;
	
	vector<particula> sistema = particula::Inicie(L, h, n);
	vector<particula> sistema_t = sistema;
	
	gen_frames(n, dt, L, h, v_p, sistema);

	sistema[0].generarScriptsGnuplot(L, h);
	
	sistema[0].generarScriptEner();
	sistema[0].generarScriptTemp();
	sistema[0].generarScriptPresi();
	
	return 0;
}

//---------------------------------------------------------------
//				Guardar los frames de la animacion
//---------------------------------------------------------------
void gen_frames(int n, double dt, double L, double h, double v_p, vector<particula> &sistema){
	
	int pasos_guardado = 10; 
	int contador = 0;
	
	//coordenada x de la pared movil
	double pared_x = 0;
	
	//calcular el factor de empaquetamiento FEA para condicion de finalizacion
	
	double Rad = L/(2*n);						//radio de las particulas
	double A_p = n*M_PI*Rad*Rad;				//area ocupada por las particulas
	double A = L*h; 							//area inicial de la caja
	double FEA_max = M_PI / (2.0*sqrt(3.0)); 	//factor de empaquetamiento maximo
	FEA_max -= 0.2; 							// margen de error para no llevarlo al limite exacto
	
	
	
	//calcular cantidades termodinamicas
	
	double E = 0;
	double T;
	double k_b = 1; //cte de boltzman normalizada
	//calcular la energía total y temperatura iniciales del sistema
	for (int i = 0; i < n; i++){
		E += 0.5*sistema[i].Getm()*Norma2(sistema[i].Getv());
	}
	T = 1/(n*k_b)*E;
	double P = 0; //en el primer instante ninguna particula choca con la pared
	
	//condiciones para el calculo de la presion
	double impulso_acumulado = 0.0;
	double dt_med = 1.0;
	double tiempo_med = 0.0;
	
	
	ofstream frames("../results/frames.dat");
	
	frames << fixed << setprecision(6);
    frames << "#"
		 << setw(15) << "t"
         << setw(15) << "x"
         << setw(15) << "y"
         << setw(15) << "R"
         << setw(15) << "Pared_x"
         << setw(15) << "E"
         << setw(15) << "T"
         << setw(15) << "P" << endl;
	
	for (double t = 0; A > (A_p/FEA_max); t += dt) {

    // ------------------------------------------------
    // Mover la pared
    // ------------------------------------------------
    pared_x += v_p * dt;

    A = (L - pared_x) * h;


    // ------------------------------------------------
    // Mover las partículas
    // ------------------------------------------------
    for (int i = 0; i < n; i++) {
        sistema[i].mueva(dt);
    }


    // ------------------------------------------------
    // Detectar colisiones
    // ------------------------------------------------
    for (int i = 0; i < n; i++) {

        impulso_acumulado += sistema[i].coli_pared(pared_x, L, h, v_p);
        sistema[i].coli_particula(dt, n, i, sistema);
    
	}


    // ------------------------------------------------
    // Calcular energía actual
    // ------------------------------------------------
    E = 0.0;

    for (int i = 0; i < n; i++) {

        E += 0.5 *
             sistema[i].Getm() *
             Norma2(sistema[i].Getv());
    }


    // ------------------------------------------------
    // Calcular temperatura
    // ------------------------------------------------
    T = E / (n * k_b);


    // ------------------------------------------------
    // Acumular tiempo para presión
    // ------------------------------------------------
    tiempo_med += dt;


    if (tiempo_med >= dt_med) {

        P = impulso_acumulado /
            (h * tiempo_med);

        impulso_acumulado = 0.0;
        tiempo_med = 0.0;
    }


    // ------------------------------------------------
    // Guardar frame
    // ------------------------------------------------
    if (contador % pasos_guardado == 0) {

        for (int i = 0; i < n; i++) {

            frames << t << "\t"
                   << sistema[i].Getr().Getx() << "\t"
                   << sistema[i].Getr().Gety() << "\t"
                   << sistema[i].GetR() << "\t"
                   << pared_x << "\t"
                   << E << "\t"
                   << T << "\t"
                   << P << "\n";
        }

        frames << "\n\n";
    }

    contador++;
}
	
	frames.close();
}





