#ifndef ISING2D
#define ISING2D

#include <vector>
#include <random>
#include <string>
#include <ostream>

class Ising2D {
 private:
 	int L;																//lado de la red
 	int N;																//número de spines
 	std::vector< std::vector<int> > spines;								//matriz de spines (todos 1)
 	double J = 1, h = 0;												//cte de acoplamiento y campo externo
 	double E, T, M;														//energía, temperatura y magnetizacion
 	double beta;														//con constante de boltzman normalizada
 	
 	//semilla para numeros aleatorios
 	std::mt19937 gen{std::random_device{}()};

 public:
	void Inicie(int invertidos, int Long, double Temp);
	void MonteCarlo();
	void Muestreo();
	
	void GenerarScriptTermalizacion();
	void GenerarScriptMuestreo();
	
	void GuardarRed(const std::string& archivo);
	void GenerarScriptRed();
	
	void GuardarFrame(std::ostream& f);
	void GenerarScriptAnimacion(int nFrames, int paso);
	
	void GenerarScriptInvertidos();
	
	double GetE(){return E;}
	double GetM(){return M;}
	double GetJ(){return J;}
	double GetN(){return N;}
	
};

#endif
