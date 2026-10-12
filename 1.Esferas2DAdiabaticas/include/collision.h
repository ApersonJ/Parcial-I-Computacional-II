#ifndef PARTICULA
#define PARTICULA

#include <vector>
#include "vector.h"

class particula {
 private:
 	vector2D r, v;
 	double R, m;

 public:
 	//instanciar las n particulas con valores aleatorios
    static std::vector<particula> Inicie(double L, double h, int n);
    
    //movimiento de cada particula
    void mueva(double dt){
    	r += v*dt;
	}
	
	//actualizacion de la velocidad en caso de colision
	void new_velocidad(double impulso, vector2D N){
		v += impulso/m*N;
	}
	
	//realizar las colisiones entre particulas
	void coli_particula(double dt, int n, int i, std::vector<particula> &sistema);
	
	//realizar las colisiones entre particulas y las paredes de la caja
    double coli_pared(double pared_x, double L, double h, double v_p);
    

    // Genera y ejecuta los scripts de Gnuplot para la animacion principal.
    void generarScriptsGnuplot(double L, double h);
    
    // Genera y ejecuta los scripts de Gnuplot para las animaciones de termodinamica
    void generarScriptEner();	//temperatura
    void generarScriptTemp();	//energia
    void generarScriptPresi();	//presion
    
    //funciones getters
	vector2D Getr(){return r;}
	vector2D Getv(){return v;}
	double GetR(){return R;}
	double Getm(){return m;}
	
};

#endif
