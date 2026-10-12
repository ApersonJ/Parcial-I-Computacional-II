# ============================================================
#  Parcial I - Fisica Computacional II
#  Uso:  make          -> compila todo (bin/parcial)
#        make run      -> ejecuta el menu (los 4 puntos)
#        make clean    -> borra objetos y ejecutables
#  Extras: make exp2 bif2 exp3 (ejecutables de experimentos)
# ============================================================
CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall

P1 := 1.Esferas2DAdiabaticas
P2 := 2.MorseForzado
P3 := 3.MathieuDuffingRuido
P4 := 4.Ising2D

INC := -Iinclude -I$(P1)/include -I$(P2)/include -I$(P3)/include -I$(P4)/include

SRC_P1 := $(P1)/src/punto1.cpp $(P1)/src/collision.cpp $(P1)/src/vector.cpp
SRC_P2 := $(P2)/src/punto2.cpp $(P2)/src/acoplados.cpp $(P2)/src/osc_morse_forzado.cpp
SRC_P3 := $(P3)/src/punto3.cpp $(P3)/src/MathieuDuffing.cpp $(P3)/src/Ruido.cpp
SRC_P4 := $(P4)/src/punto4.cpp $(P4)/src/Ising2D.cpp

SRC := main.cpp $(SRC_P1) $(SRC_P2) $(SRC_P3) $(SRC_P4)
OBJ := $(SRC:%.cpp=build/%.o)

.PHONY: all run clean dirs exp2 bif2 exp3 experimentos

all: dirs bin/parcial

bin/parcial: $(OBJ)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $^ -o $@

build/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# Crea las carpetas de salida (ofstream no las crea)
dirs:
	@mkdir -p $(P1)/results/animations $(P1)/results/graphics $(P1)/scripts
	@mkdir -p $(P2)/results/datos/experimentos $(P2)/results/figuras/experimentos $(P2)/scripts/experimentos
	@mkdir -p $(P3)/results/datos $(P3)/results/figuras $(P3)/scripts
	@mkdir -p $(P4)/results/datos $(P4)/results/animations $(P4)/results/muestreo $(P4)/results/red $(P4)/results/termalizacion $(P4)/scripts

run: all
	./bin/parcial

# --- ejecutables aparte (cada uno tiene su propio main) ---
exp2: dirs $(P2)/bin/experimentos
bif2: dirs $(P2)/bin/bifurcacion
exp3: dirs $(P3)/bin/experimentos

$(P2)/bin/experimentos: $(P2)/src/experimentos.cpp $(P2)/src/osc_morse_forzado.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC) $^ -o $@
	cd $(P2) && ./bin/experimentos

$(P2)/bin/bifurcacion: $(P2)/src/bifurcacion.cpp $(P2)/src/osc_morse_forzado.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC) $^ -o $@
	cd $(P2) && ./bin/bifurcacion

$(P3)/bin/experimentos: $(P3)/src/experimentos.cpp $(P3)/src/MathieuDuffing.cpp $(P3)/src/Ruido.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC) $^ -o $@
	cd $(P3) && ./bin/experimentos

experimentos: exp2 bif2 exp3

clean:
	rm -rf build bin $(P2)/bin $(P3)/bin