#include "collision.h"
#include "vector.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

//---------------------------------------------------------------
//					Instanciar todas las part�culas
//---------------------------------------------------------------
vector<particula> particula::Inicie(double L, double h, int n){
	
	//radio que permita que las particulas quepan en la caja
	double Rad = L/(n);
	
	//semilla para los numeros aleatorios
	random_device rd;
    mt19937 gen(rd());
    
    uniform_real_distribution<double> limit_x(Rad, L-Rad);
    uniform_real_distribution<double> limit_y(Rad, h-Rad);
    uniform_real_distribution<double> limit_v(-1, 1);
	
	//arreglo de particulas
	vector<particula> sistema(n);
	
	//instanciar cada particula con valores aleatorios
	for(int i=0; i<n; i++){
		
		sistema[i].v.Cargue( limit_v(gen), limit_v(gen) );
		//generar part�culas con un radio que quepa en la caja
		sistema[i].R = Rad;
		sistema[i].m = 1; //masa normalizada
		
		//asegurarse de que no se solapen al inicial el codigo
		bool solapamiento;
        do {
            solapamiento = false;
            
            // generar posici�n candidata
            double x_cand = limit_x(gen);
            double y_cand = limit_y(gen);
            sistema[i].r.Cargue(x_cand, y_cand);

            // comprobar distancia con las part�culas ya aceptadas
            for (int j = 0; j < i; j++) {
                vector2D diferencia = sistema[i].r - sistema[j].r;
                double distancia = Norma(diferencia);

                // si la distancia es menor a la suma de los radios, hay solapamiento
                if (distancia < sistema[i].R + sistema[j].R) {
                    solapamiento = true;
                    break; // salir del bucle for y generar una nueva coordenada
                }
            }
        } while (solapamiento);
		
	}
	
	return sistema;
}


//---------------------------------------------------------------
//			Verificar colisiones entre las particulas
//---------------------------------------------------------------
void particula::coli_particula(double dt, int n, int i, vector<particula> &sistema){
	
	//verificar la colision entre la particula i y todas las demas
	for(int j=i+1; j<n; j++){
		
		//distancia entre la particula i y j
		vector2D diferencia = sistema[i].r - sistema[j].r;
        double d = Norma(diferencia);
		
		//vector normal
		vector2D N = diferencia/d;
		
		//velocidad relativa y su componente normal
		vector2D v_rel = sistema[i].v-sistema[j].v;
		double v_rel_n = v_rel * N;
		
		//actualizar el choque en caso de colision
		if((d <= sistema[i].R + sistema[j].R) && (v_rel_n < 0)){
			
			double impulso = -2*v_rel_n/(1/sistema[i].m + 1/sistema[j].m);
			
			sistema[i].new_velocidad(impulso, N);
			sistema[j].new_velocidad(-impulso, N);
			
		}
	}
}


//---------------------------------------------------------------
//			Verificar colisiones con la pared
//---------------------------------------------------------------
double particula::coli_pared(double pared_x, double L, double h, double v_p){

    double impulso = 0.0;

    // Pared derecha
    if ((L - r.Getx()) <= R && v.Getx() > 0) {
        v.Cargue(-v.Getx(), v.Gety());
    }

    // Pared izquierda m�vil
    if ((r.Getx() - pared_x) <= R && v.Getx() < v_p) {
        impulso = 2.0*m*(v_p - v.Getx());
        v.Cargue(2*v_p - v.Getx(), v.Gety());
    }

    // Pared superior
    if ((h - r.Gety()) <= R && v.Gety() > 0) {
        v.Cargue(v.Getx(), -v.Gety());
    }

    // Pared inferior
    if (r.Gety() <= R && v.Gety() < 0) {
        v.Cargue(v.Getx(), -v.Gety());
    }

    return impulso;
}


//---------------------------------------------------------------
//		Generar y ejecutar el script de Gnuplot (animacion)
//---------------------------------------------------------------
void particula::generarScriptsGnuplot(double L, double h){

	ofstream gp("../scripts/animacion.gp");

	gp << "# Animacion generada automaticamente\n";
	gp << "reset\n";
	gp << "L = " << L << "\n";
	gp << "h = " << h << "\n";
	gp << "archivo = '../results/frames.dat'\n\n";

	// numero de frames = numero de bloques del archivo
	gp << "stats archivo nooutput\n";
	gp << "N = STATS_blocks\n\n";

	// salida: gif animado (el tamano respeta la proporcion de la caja)
	gp << "set terminal gif animate delay 5 loop 0 size 900," << "900*h/L+80" << " \n";
	gp << "set output '../results/animations/animacion.gif'\n\n";

	// ejes y estilo
	gp << "set size ratio h/L\n";
	gp << "set xrange [-0.05*L:1.05*L]\n";
	gp << "set yrange [-0.05*h:1.10*h]\n";
	gp << "set xlabel 'x (m)'\n";
	gp << "set ylabel 'y (m)'\n";
	gp << "unset key\n";
	gp << "set style fill solid 0.6 border lc rgb 'black'\n\n";

	// paredes fijas (caja completa)
	gp << "set object 1 rectangle from 0,0 to L,h fs empty border lc rgb 'black' lw 2 front\n\n";

	// bucle de animacion
	gp << "do for [i=0:N-1] {\n";
	gp << "    plot archivo index i using 2:3:4 with circles lc rgb '#1f77b4' notitle, \\\n";
	// pared movil: linea vertical en x = Pared_x (columna 5)
	gp << "         archivo index i every ::0::0 using 5:(0):(0):(h) with vectors nohead lw 3 lc rgb 'red' notitle, \\\n";
	// tiempo del frame, leido de la columna 1 (sin usar stats)
	gp << "         archivo index i every ::0::0 using (0.5*L):(1.03*h):(sprintf('t = %.2f s', $1)) with labels center notitle\n";
	gp << "}\n";
	gp << "set output\n";

	gp.close();

	// ejecutar gnuplot
	int estado = system("gnuplot ../scripts/animacion.gp");
	if(estado != 0){
		cerr << "Error al ejecutar gnuplot. Revise que este instalado y en el PATH." << endl;
	}
}


//---------------------------------------------------------------
//   Auxiliar: script de Gnuplot que anima una magnitud vs t
//   y guarda ademas una imagen con la evolucion completa
//---------------------------------------------------------------
static void scriptMagnitud(const string &nombre, int col, const string &etiqueta,
                           const string &titulo, const string &color){

	string ruta_serie = "../results/serie_" + nombre + ".dat";
	string ruta_gp    = "../scripts/" + nombre + ".gp";

	//-----------------------------------------------------------
	// Extraer de frames.dat una fila (t, valor) por frame
	//-----------------------------------------------------------
	{
		ifstream in("../results/frames.dat");
		ofstream serie(ruta_serie);
		serie << fixed << setprecision(6);

		string linea;
		bool primera_del_bloque = true;

		while(getline(in, linea)){

			// linea en blanco: empieza un nuevo frame
			if(linea.find_first_not_of(" \t\r") == string::npos){
				primera_del_bloque = true;
				continue;
			}
			// comentario / encabezado
			if(linea[0] == '#') continue;

			// solo la primera fila de cada frame (E, T y P son globales)
			if(!primera_del_bloque) continue;
			primera_del_bloque = false;

			istringstream ss(linea);
			vector<double> campos;
			double val;
			while(ss >> val) campos.push_back(val);

			if((int)campos.size() >= col){
				serie << campos[0] << "\t" << campos[col-1] << "\n";
			}
		}
	}

	//-----------------------------------------------------------
	// Script de Gnuplot
	//-----------------------------------------------------------
	ofstream gp(ruta_gp);

	gp << "reset\n";
	gp << "serie = '" << ruta_serie << "'\n\n";

	// N = numero de frames; rangos globales (antes de fijar xrange/yrange)
	gp << "stats serie using 1:2 nooutput\n";
	gp << "N    = STATS_records\n";
	gp << "tmax = STATS_max_x\n";
	gp << "ymin = STATS_min_y\n";
	gp << "ymax = STATS_max_y\n";
	gp << "pad  = (ymax > ymin) ? 0.05*(ymax-ymin) : 1.0\n\n";

	gp << "set title '" << titulo << "'\n";
	gp << "set xlabel 't (s)'\n";
	gp << "set ylabel '" << etiqueta << "'\n";
	gp << "set xrange [0:tmax]\n";
	gp << "set yrange [ymin-pad:ymax+pad]\n";
	gp << "set grid\n";
	gp << "unset key\n\n";

	// --- GIF: la curva crece hasta el frame i, con un punto en el instante actual
	gp << "set terminal gif animate delay 5 loop 0 size 800,500\n";
	gp << "set output '../results/animations/" << nombre << ".gif'\n";
	gp << "do for [i=0:N-1] {\n";
	gp << "    plot serie every ::0::i using 1:2 with lines lw 2 lc rgb '" << color << "' notitle, \\\n";
	gp << "         serie every ::i::i using 1:2 with points pt 7 ps 1.5 lc rgb 'black' notitle\n";
	gp << "}\n";
	gp << "set output\n\n";

	// --- PNG: evolucion completa (trayectoria final de la magnitud)
	gp << "set terminal pngcairo size 1000,600\n";
	gp << "set output '../results/graphics/" << nombre << ".png'\n";
	gp << "plot serie using 1:2 with lines lw 2 lc rgb '" << color << "' notitle, \\\n";
	gp << "     serie every ::0::0 using 1:2 with points pt 7 ps 1.5 lc rgb 'green' notitle, \\\n";
	gp << "     serie every ::N-1::N-1 using 1:2 with points pt 7 ps 1.5 lc rgb 'red' notitle\n";
	gp << "set output\n";

	gp.close();

	string comando = "gnuplot " + ruta_gp;
	if(system(comando.c_str()) != 0){
		cerr << "Gnuplot termino con error en " << ruta_gp << endl;
	}
}

//---------------------------------------------------------------
//				Scripts de energia, temperatura y presion
//---------------------------------------------------------------
void particula::generarScriptEner(){
	scriptMagnitud("energia", 6, "Energia", "Energia vs tiempo", "#d62728");
}

void particula::generarScriptTemp(){
	scriptMagnitud("temperatura", 7, "Temperatura", "Temperatura vs tiempo", "#2ca02c");
}

void particula::generarScriptPresi(){
	scriptMagnitud("presion", 8, "Presion", "Presion vs tiempo", "#1f77b4");
}


