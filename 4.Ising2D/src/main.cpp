#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>

#include "Ising2D.h"

using namespace std;

int ejecutarPunto3(){
	Ising2D sistema;
	
	int invertidos, Long;
	double Temp;
	double k_b=1; //cte de boltzman normalizada
	
	cout << "Ingrese el lado de la red: "; cin >> Long;
	cout << "Ingrese el numero inicial de spines invertidos: "; cin >> invertidos;
	
	double T_c = 2/log(1+sqrt(2))*sistema.GetJ()/k_b;		//temperatura de curie
	cout << "La temperatura de Curie es " << T_c <<"K"<<endl;
	cout << "Ingrese la temperatura(K): "; cin >> Temp;
	
	sistema.Inicie(invertidos, Long, Temp);
	sistema.GuardarRed("../results/datos/red_inicial.dat");
	
	//------------------------------------
	//			Termalizacion
	//------------------------------------
	
	int MCS_t = 5000;		//pasos de montecarlo para la termalizacion
	int Inv = invertidos;	//numero de spines con -1
	
	ofstream file("../results/datos/termalizacion.dat");

	file << fixed << setprecision(6);
	file << "#"
		 << setw(15) << "MCS"
         << setw(15) << "E"
         << setw(15) << "M" 
		 << setw(15) << "#Invertidos" << endl;
		
	ofstream anim("../results/datos/animacion.dat");
	int paso = 10;                                  // un frame cada 10 MCS
	
	sistema.GuardarFrame(anim);                     // frame 0: estado inicial
	
	for(int i=0; i<MCS_t; i++){
		sistema.MonteCarlo();
	
		file << i << "\t" 
			<< sistema.GetE() << "\t" 
			<< sistema.GetM() << "\t"
			<< Inv << "\n";
		
		Inv = (sistema.GetN()-sistema.GetM())/2;
		
		if((i+1) % paso == 0)
			sistema.GuardarFrame(anim);
	}
	file.close();
	anim.close();
	
	sistema.GenerarScriptAnimacion(1 + MCS_t/paso, paso);
	
	sistema.GenerarScriptTermalizacion();
	sistema.GenerarScriptInvertidos();
	sistema.GuardarRed("../results/datos/red_termalizada.dat");
	
	//------------------------------------
	//				Muestreo
	//------------------------------------
	sistema.Muestreo();
	sistema.GenerarScriptMuestreo();
	sistema.GuardarRed("../results/datos/red_muestreo.dat");
	
	sistema.GenerarScriptRed();
		
	return 0;
}







