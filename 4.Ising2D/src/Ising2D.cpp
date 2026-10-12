#include "Ising2D.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <cstdlib>

using namespace std;

//-------------------------------------------------------------
//			Instanciar el estado inicial del sistema
//-------------------------------------------------------------
void Ising2D::Inicie(int invertidos, int Long, double Temp){
	
	this->L = Long;
	this->N = L*L;
	this->T = Temp;
	this->beta = 1/T;
	spines.assign(L, std::vector<int>(L, 1));
	
    uniform_int_distribution<int> random_L(0, L-1);
	
	//plantar los spines invertidos iniciales
	for(int k=0; k<invertidos; k++){
		//escojer un spin aleatorio de la red
		//volver a escoger si se repite
		int i, j;
		do{
			i = random_L(gen);
			j = random_L(gen);
		}while (spines[i][j] == -1);

		spines[i][j] = -spines[i][j];
	}
	
	M = N - 2*invertidos; //magnetizacion inicial
	E = 0;
	for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {

            int s = spines[i][j];
            
            // vecinos
            int arriba = spines[(i - 1 + L) % L][j];	//% L permite que el elemento final de la red detecte el inicio como vecino
            int abajo  = spines[(i + 1) % L][j];
            int izquierda = spines[i][(j - 1 + L) % L];
            int derecha   = spines[i][(j + 1) % L];

            // interacción
            E += -0.5 * J * s *
                 (arriba + abajo + izquierda + derecha);

            // campo externo
            E += -h * s;				//Energia inicial
        }
    }
}


//-------------------------------------------------------------
//		realizar inversiones de spines segun probabilidad
//-------------------------------------------------------------
void Ising2D::MonteCarlo(){
	
	uniform_int_distribution<int> random_L(0, L-1);
	uniform_real_distribution<double> random_r(0,1);
	
	for(int k=0; k<N; k++){
		//escoger un spin cualquiera de la red
		int i = random_L(gen);
		int j = random_L(gen);
		
		//definir los spines vecinos de ese spin
        int arriba = spines[(i - 1 + L) % L][j];
        int abajo  = spines[(i + 1) % L][j];
        int izquierda = spines[i][(j - 1 + L) % L];
        int derecha   = spines[i][(j + 1) % L];
        
        int spin_sum = arriba + abajo + izquierda + derecha;
        
        //calcular el cambio de energía proponiendo que el spin se invierte
        double deltaE = 2*spines[i][j] * (J*spin_sum + h);
        
        //algoritmo de Metropolis
        //aceptar la inversion del spin dependiendo de como cambio su energía
        
        if(deltaE <= 0){						//la energia disminuye
        	spines[i][j] = -spines[i][j];		//se acepta directamente el cambio
        	
        	E += deltaE;						//actualizar la energía
			M += 2.0 * spines[i][j];			//actualizar la magnetizacion
        	
		}else{									//la energia aumenta
			double P = exp(-beta * deltaE);
			double r = random_r(gen);
			
			if(r < P){							//el cambio se acepta aleatoriamente segun su porbabilidad
				spines[i][j] = -spines[i][j];
        		E += deltaE;						
				M += 2.0 * spines[i][j];
			}
		}   
	}
	
}


//------------------------------------
//				Muestreo
//------------------------------------
void Ising2D::Muestreo(){
	int MCS_m = 10000;		//numero de muestras
	double sum_E = 0;
	double sum_M = 0;
	double sum_E2 = 0;
	double sum_M2 = 0;
	double sum_abs_M = 0;
	
	ofstream file2("../results/datos/muestreo.dat");

	file2 << fixed << setprecision(6);
	file2 << "#"
		 << setw(15) << "MCS"
         << setw(15) << "E"
         << setw(15) << "M" << endl;
	
	for(int i=0; i<MCS_m; i++){
		MonteCarlo();
		
        file2 << i << "\t"
               << E << "\t"
               << M << "\n";
        
        sum_E += E;
        sum_M += M;
        sum_abs_M += fabs(M);
        
        sum_E2 += E*E;
        sum_M2 += M*M;
	
	}
	file2.close();
	
	//promedio de mediciones
    double E_prom = sum_E/MCS_m;
    double M_prom = sum_M/MCS_m;
    double M_prom_abs = sum_abs_M/MCS_m;
    
    double E2_prom = sum_E2/MCS_m;
    double M2_prom = sum_M2/MCS_m;
    
	//------------------------------------
	//		Observables finales
	//------------------------------------	
    
    //energia y magnetizacion
    double E_spin = E_prom/N;
    double M_spin = M_prom_abs/N;
    
    //promedio de calor específico y de suceptivilidad magnetica
    double C_v = (E2_prom - E_prom*E_prom)/(N*T*T);
    double X = (M2_prom - M_prom*M_prom)/(N*T);
    
    cout<<"Energia promedio del sistema: "<<E_prom<<endl;
    cout<<"Energia de cada spin: "<<E_spin<<endl<<endl;
    
    cout<<"Magnetizacion promedio del sistema: "<<M_prom<<endl;
    cout<<"Magnetizacion absoluta de cada spin: "<<M_spin<<endl<<endl;
    
    cout<<"Calor especifico del sistema: "<<C_v<<endl;
    cout<<"Suceptivilidad magnetica del sistema: "<<X<<endl;
    
}


//-------------------------------------------------------------
//     Generar script de gnuplot para los datos de termalización
//-------------------------------------------------------------
void Ising2D::GenerarScriptTermalizacion(){

	ofstream gp("../scripts/termalizacion.gp");

	gp << "set terminal pngcairo size 900,600 enhanced font 'Arial,12'\n";
	gp << "set grid\n";
	gp << "set xlabel 'MCS'\n\n";

	// Grafica de E vs MCS
	gp << "set output '../results/termalizacion/E_vs_MCS.png'\n";
	gp << "set title 'Termalizacion: Energia vs MCS'\n";
	gp << "set ylabel 'E'\n";
	gp << "plot '../results/datos/termalizacion.dat' using 1:2 with lines lw 1.5 lc rgb '#d62728' notitle\n\n";

	// Grafica de M vs MCS
	gp << "set output '../results/termalizacion/M_vs_MCS.png'\n";
	gp << "set title 'Termalizacion: Magnetizacion vs MCS'\n";
	gp << "set ylabel 'M'\n";
	gp << "plot '../results/datos/termalizacion.dat' using 1:3 with lines lw 1.5 lc rgb '#1f77b4' notitle\n\n";

	gp << "set output\n";
	gp.close();
	if(system("gnuplot ../scripts/termalizacion.gp") != 0)
		cerr << "Error al ejecutar gnuplot (termalizacion)" << endl;
}


//-------------------------------------------------------------
//     Generar script de gnuplot para los datos de muestreo
//-------------------------------------------------------------
void Ising2D::GenerarScriptMuestreo(){

	ofstream gp("../scripts/muestreo.gp");

	gp << "set terminal pngcairo size 900,600 enhanced font 'Arial,12'\n";
	gp << "set grid\n";
	gp << "set xlabel 'MCS'\n\n";

	// Grafica de E vs MCS
	gp << "set output '../results/muestreo/E_vs_MCS.png'\n";
	gp << "set title 'Muestreo: Energia vs MCS'\n";
	gp << "set ylabel 'E'\n";
	gp << "plot '../results/datos/muestreo.dat' using 1:2 with lines lw 1.5 lc rgb '#d62728' notitle\n\n";

	// Grafica de M vs MCS
	gp << "set output '../results/muestreo/M_vs_MCS.png'\n";
	gp << "set title 'Muestreo: Magnetizacion vs MCS'\n";
	gp << "set ylabel 'M'\n";
	gp << "plot '../results/datos/muestreo.dat' using 1:3 with lines lw 1.5 lc rgb '#1f77b4' notitle\n\n";

	gp << "set output\n";
	gp.close();
	if(system("gnuplot ../scripts/muestreo.gp") != 0)
		cerr << "Error al ejecutar gnuplot (muestreo)" << endl;
}



//-------------------------------------------------------------
//     Guardar la matriz de spines en un archivo
//-------------------------------------------------------------
void Ising2D::GuardarRed(const string& archivo){

	ofstream f(archivo);

	for(int i=0; i<L; i++){
		for(int j=0; j<L; j++){
			f << spines[i][j] << " ";
		}
		f << "\n";
	}
	f.close();
}


//-------------------------------------------------------------
//     Generar script de gnuplot para graficar la red
//-------------------------------------------------------------
void Ising2D::GenerarScriptRed(){

	ofstream gp("../scripts/red.gp");

	gp << "set terminal pngcairo size 700,700 enhanced font 'Arial,12'\n";
	gp << "set size square\n";
	gp << "set palette defined (-1 '#1f77b4', 1 '#d62728')\n";   // -1 azul, +1 rojo
	gp << "set cbrange [-1:1]\n";
	gp << "set cbtics ('-1' -1, '+1' 1)\n";
	gp << "set xrange [-0.5:" << L-0.5 << "]\n";
	gp << "set yrange [" << L-0.5 << ":-0.5]\n";                 // invertido: fila 0 arriba
	gp << "unset key\n";
	gp << "set xlabel 'j'\n";
	gp << "set ylabel 'i'\n\n";

	// Red inicial
	gp << "set output '../results/red/red_inicial.png'\n";
	gp << "set title 'Red inicial'\n";
	gp << "plot '../results/datos/red_inicial.dat' matrix with image\n\n";

	// Red al terminar la termalizacion
	gp << "set output '../results/red/red_termalizada.png'\n";
	gp << "set title 'Red despues de la termalizacion'\n";
	gp << "plot '../results/datos/red_termalizada.dat' matrix with image\n\n";

	// Red al terminar el muestreo
	gp << "set output '../results/red/red_muestreo.png'\n";
	gp << "set title 'Red despues del muestreo'\n";
	gp << "plot '../results/datos/red_muestreo.dat' matrix with image\n\n";

	gp << "set output\n";
	gp.close();

	if(system("gnuplot ../scripts/red.gp") != 0)
		cerr << "Error al ejecutar gnuplot (red)" << endl;
}



//-------------------------------------------------------------
//     Escribir la red como un bloque (frame) de la animacion
//-------------------------------------------------------------
void Ising2D::GuardarFrame(ostream& f){

	for(int i=0; i<L; i++){
		for(int j=0; j<L; j++){
			f << spines[i][j] << " ";
		}
		f << "\n";
	}
	f << "\n\n";		// dos lineas en blanco separan los bloques (index de gnuplot)
}


//-------------------------------------------------------------
//     Generar script de gnuplot para la animacion
//-------------------------------------------------------------
void Ising2D::GenerarScriptAnimacion(int nFrames, int paso){

	ofstream gp("../scripts/animacion.gp");

	gp << "set terminal gif animate delay 5 loop 0 size 600,600\n";
	gp << "set output '../results/animations/animacion.gif'\n";
	gp << "set size square\n";
	gp << "set palette defined (-1 '#1f77b4', 1 '#d62728')\n";
	gp << "set cbrange [-1:1]\n";
	gp << "set cbtics ('-1' -1, '+1' 1)\n";
	gp << "set xrange [-0.5:" << L-0.5 << "]\n";
	gp << "set yrange [" << L-0.5 << ":-0.5]\n";
	gp << "unset key\n\n";

	gp << "do for [k=0:" << nFrames-1 << "] {\n";
	gp << "    set title sprintf('MCS = %d', k*" << paso << ")\n";
	gp << "    plot '../results/datos/animacion.dat' index k matrix with image\n";
	gp << "}\n";
	gp << "set output\n";
	gp.close();

	if(system("gnuplot ../scripts/animacion.gp") != 0)
		cerr << "Error al ejecutar gnuplot (animacion)" << endl;
}


//-------------------------------------------------------------
//     Generar script de gnuplot para el numero de spines invertidos
//-------------------------------------------------------------
void Ising2D::GenerarScriptInvertidos(){

	ofstream gp("../scripts/invertidos.gp");

	gp << "set terminal pngcairo size 900,600 enhanced font 'Arial,12'\n";
	gp << "set grid\n";
	gp << "set output '../results/termalizacion/Inv_vs_MCS.png'\n";
	gp << "set title 'Termalizacion: spines invertidos vs MCS'\n";
	gp << "set xlabel 'MCS'\n";
	gp << "set ylabel 'Numero de spines con -1'\n";
	gp << "plot '../results/datos/termalizacion.dat' using 1:4 with lines lw 1.5 lc rgb '#2ca02c' notitle\n";
	gp << "set output\n";
	gp.close();

	if(system("gnuplot ../scripts/invertidos.gp") != 0)
		cerr << "Error al ejecutar gnuplot (invertidos)" << endl;
}









