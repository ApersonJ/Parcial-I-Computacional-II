# Primer Parcial - Física Computacional II

Universidad Distrital Francisco José de Caldas · Programa Académico de Física

## Integrantes

- Jesús David Vitola González
- Nicolas López Prieto

## Fenomenologías asignadas

| Punto | Fenomenología | Carpeta |
|---|---|---|
| 1 | Gas de esferas rígidas 2D con paredes móviles (compresión adiabática) | `1.Esferas2DAdiabaticas/` |
| 2 | Oscilador de Morse forzado (y dos osciladores acoplados) | `2.MorseForzado/` |
| 3 | Oscilador de Mathieu-Duffing con ruido | `3.MathieuDuffingRuido/` |
| 4 | Modelo de Ising 2D con Metropolis | `4.Ising2D/` |

## Dependencias

- Linux (o WSL en Windows). El programa usa `unistd.h` y `mkdir -p`.
- `g++` con soporte de C++17.
- `make`.
- `gnuplot` (versión 5 o superior) para generar gráficas y animaciones `.gif`.

En Ubuntu/Debian:

```
sudo apt install g++ make gnuplot
```

## Compilación y ejecución

Todos los comandos se ejecutan desde la carpeta raíz del proyecto (donde está este archivo).

| Comando | Qué hace |
|---|---|
| `make` | Compila todo y crea `bin/parcial` |
| `make run` | Compila (si hace falta) y abre el menú con los cuatro puntos |
| `make clean` | Borra `build/` y los ejecutables (`bin/`) |
| `make exp2` | Experimentos del Punto 2 (comparación de métodos, convergencia, régimen transitorio) |
| `make bif2` | Diagrama de bifurcación y exponente de Lyapunov del Punto 2 |
| `make exp3` | Experimentos del Punto 3 (validación lineal, convergencia fuerte, barrido en sigma, estroboscópico, autocorrelación y espectro) |
| `make experimentos` | Ejecuta `exp2`, `bif2` y `exp3` en secuencia |

### Menú principal

`make run` muestra:

```
1. Gas de Esferas 2D con Paredes Moviles
2. Oscilador de Morse Forzado
3. Oscilador de Mathieu-Duffing con Ruido
4. Modelo de Ising 2D
0. Salir
```

Cada opción pide sus datos por teclado. Al terminar un punto vuelve el menú, y se puede elegir otro punto o repetir el mismo sin salir. Para terminar se escribe `0`.

Cada punto fue escrito con rutas relativas a su propia carpeta. Por eso `main.cpp` entra a la carpeta del punto antes de ejecutarlo y regresa a la raíz al terminar. Los resultados de cada punto quedan siempre en su propia carpeta `results/`.


## Semilla de los números aleatorios

- **Punto 3:** la semilla se ingresa por teclado (por ejemplo `12345`) y queda registrada en `3.MathieuDuffingRuido/results/datos/parametros.txt`. Con la misma semilla se obtienen los mismos resultados. Los experimentos de `make exp3` usan semillas fijas dentro del código.
- **Puntos 1 y 4:** el generador (`std::mt19937`) se inicializa con `std::random_device`, así que la semilla cambia en cada ejecución y los resultados no son idénticos entre corridas.
- **Punto 2:** es determinista, no usa números aleatorios.

## Estructura de carpetas

```
.
├── README.md
├── Makefile
├── main.cpp                  Menú principal
├── include/                  Cabeceras de entrada de cada punto (punto1.h ... punto4.h)
├── 1.Esferas2DAdiabaticas/
├── 2.MorseForzado/
├── 3.MathieuDuffingRuido/
├── 4.Ising2D/
├── informe/                  Informe técnico (.tex y .pdf)
└── presentacion/             Presentaciones (HTML)
```

Cada carpeta de punto sigue la misma organización:

```
N.Punto/
├── include/    Cabeceras (.h) de las clases del punto
├── src/        Código fuente (.cpp)
├── scripts/    Scripts de gnuplot (algunos los genera el propio programa)
├── results/    Datos (.dat), figuras (.png) y animaciones (.gif)
└── docs/       Documentación del punto (.tex, .pdf, imágenes, referencias)
```

Detalle de `results/` por punto:

- **Punto 1:** `frames.dat`, `serie_*.dat` (energía, presión, temperatura), `graphics/` y `animations/`.
- **Punto 2:** `datos/` (trayectoria, bifurcación, Lyapunov, acoplados) y `figuras/`, con subcarpetas `experimentos/` y `acoplados/`.
- **Punto 3:** `datos/` (momentos, trayectorias, histograma, parámetros) y `figuras/`, con subcarpeta `experimentos/`.
- **Punto 4:** `datos/`, `termalizacion/`, `muestreo/`, `red/` y `animations/`.
