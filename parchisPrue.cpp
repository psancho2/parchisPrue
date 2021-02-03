//GUILLLERMO GONZALEZ HERRERA
//PEDRO JIMENEZ VILLAREJO 
//GRUPO D 
#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <windows.h>

using namespace std;

typedef enum { Amarillo = 0, Azul = 1, Rojo = 2, Verde = 3, Ninguno = 4, Gris = 5 } tColor;
const int NUM_CASILLAS = 68;
const int MAX_JUG = 4;
const int NUM_FICHAS = 4;
typedef int tFichas[NUM_FICHAS]; //-1 casa, 0-67 tablero y 101-108 subida a meta y meta
typedef tFichas tJugadores[MAX_JUG];
typedef tColor tCasillas[NUM_CASILLAS];
const string Archivo = "pruebas6.txt";
const bool Debug = true;

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

// PROTOTIPOS DE LOS SUBPROGRAMAS DE VISUALIZACIÓN DEL TABLERO
void mostrar(const tJugadores jugadores, const tCasillas calle1, const tCasillas calle2);
void setColor(tColor color);
void iniciaColores();

char letra(tColor color);
bool esSeguro(int pos);
string colorACadena(tColor color);
int zanataJugador(tColor jugador);
int salidaJugador(tColor jugador);

void iniciar(tJugadores jugadores, tCasillas calle1, tCasillas calle2, tColor& jugadorTurno);//
bool puente(const tCasillas calle1, const tCasillas calle2, int casilla);
int cuantasEn(const tFichas jugador, int casilla); //
int primeraEn(const tFichas jugador, int casilla); //
int segundaEn(const tFichas jugador, int casilla); //
void saleFicha(tJugadores jugadores, tColor jugadorTurno, tCasillas calle1, tCasillas calle2);
void aCasita(tJugadores jugadores, int casilla, tCasillas calle2);
bool todasEnMeta(const tFichas jugador);
bool procesa5(tJugadores jugadores, tColor jugadorTurno, int& premio, bool& pasaTurno, tCasillas calle1, tCasillas calle2);
void abrirPuente(tJugadores jugadores, tColor jugadorTurno, int casillaOrig, int casillaDest, int& premio, int& ultimaFichaMovida, tCasillas calle1, tCasillas calle2);
bool procesa6(tJugadores jugadores, tColor jugadorTurno, int& premio, bool& pasaTurno, int& seises, int& ultimaFichaMovida, int& tirada, tCasillas calle1, tCasillas calle2);
bool jugar(tJugadores jugadores, tColor jugadorTurno, int& premio, bool& fin, int& seises, int& ultimaFichaMovida, int tirada, tCasillas calle1, tCasillas calle2);
bool puedeMover(const tJugadores jugadores, tColor jugadorTurno, int ficha, int& casilla, int tirada, const tCasillas calle1, const tCasillas calle2);
void mover(tJugadores jugadores, tColor jugadorTurno, int ficha, int casilla, int& premio, int& ultimaFichaMovida, tCasillas calle1, tCasillas calle2);
void cargar(tJugadores jugadores, tColor& jugadorTurno, tCasillas calle1, tCasillas calle2);

int main() {
	tJugadores jugadores; //Array bidimensional de los jugadores y la posicion de sus 4 fichas
	tCasillas calle1, calle2; //Array de todas las casillas del tablero
	tColor jugador; //El color del jugador que va a jugar
	bool fin = false, jugadaForzosa = false;
	int premio = 0, tirada = 0, seises = 0, ultimaFichaMovida = -1, ficha = -1;
	iniciaColores();
	iniciar(jugadores, calle1, calle2, jugador);
	mostrar(jugadores, calle1, calle2);
	if (Debug) {
		cargar(jugadores, jugador, calle1, calle2);
	}

	cout << primeraEn(jugadores[jugador], -1) << endl;
	mostrar(jugadores, calle1, calle2);

	while (!fin) {
		bool pasarTurno = true;
		bool jugadaForzosa = false;
		setColor(jugador);
		cout << "Le toca al jugador " << colorACadena(jugador) << endl;

		if (premio > 0) {
			cout << "El jugador " << colorACadena(jugador) << " tiene " << premio << " movimientos" << endl;
			tirada = premio;
			premio = 0;
		}
		else {
			if (Debug) {
				cout << "Dame tirada (0 para salir): ";
				cin >> tirada;
			}
			else {
				tirada = rand() % 6 + 1;
			}

			cout << "Sale un " << tirada << endl;
		}

		if (tirada == 0) fin = true;
		else {
			if (tirada == 5) {
				jugadaForzosa = procesa5(jugadores, jugador, premio, pasarTurno, calle1, calle2);
				
			}
			else if (tirada == 6) {
				jugadaForzosa = procesa6(jugadores, jugador, premio, pasarTurno, seises, ultimaFichaMovida, tirada, calle1, calle2);
			}

			if (!jugadaForzosa) {
				pasarTurno = jugar(jugadores, jugador, premio, fin, seises, ultimaFichaMovida, tirada, calle1, calle2);
			}
			fin = todasEnMeta(jugadores[jugador]);

			if (pasarTurno && !fin) {
				jugador = tColor((int(jugador) + 1) % MAX_JUG);
				seises = 0;
				ultimaFichaMovida = ficha;
			}

			mostrar(jugadores, calle1, calle2);
			system("PAUSE");
		}
	}

	setColor(jugador);
	cout << "EL JUGADOR " << colorACadena(jugador) << " HA GANADO" << endl;
	system("PAUSE");

	return 0;
}
//Funcion que saca una ficha de casa del jugadorTurno
void saleFicha(tJugadores jugadores, tColor jugadorTurno, tCasillas calle1, tCasillas calle2) {
	int s = salidaJugador(jugadorTurno);
	calle2[s] = calle1[s];
	calle1[s] = Ninguno;
	jugadores[jugadorTurno][primeraEn(jugadores[jugadorTurno], -1)] = s;
	calle1[s] = jugadorTurno;
}
//Funcion que envia  a su casa a la ficha que esté en esa casilla de la calle2

void aCasita(tJugadores jugadores, int casilla, tCasillas calle2) {
	tColor comido = calle2[casilla];
	int ficha;
	ficha = segundaEn(jugadores[comido], casilla);
	jugadores[comido][ficha] = -1;
	calle2[casilla] = Ninguno;
}
//Funcion que si hay fichas de jugadorTurno en casa y este jugador no tiene dos fichas en la salida,
// saca una y devuelve true.Si no, no hace nada y devuelve false.
bool procesa5(tJugadores jugadores, tColor jugadorTurno, int& premio, bool& pasaTurno, tCasillas calle1, tCasillas calle2) {
	bool res = false;
	if (cuantasEn(jugadores[jugadorTurno], -1) != 0) {
		int s = salidaJugador(jugadorTurno);
		if (calle2[s] == Ninguno) {
			res = true;
			saleFicha(jugadores, jugadorTurno, calle1, calle2);
			premio = 0;
			pasaTurno = true;
			cout << "Sale de casa una ficha (premio: " << premio << "; pasa turno: " << boolalpha << pasaTurno << ")" << endl;
		}
		else {
			if (calle2[s] != jugadorTurno || calle1[s] != jugadorTurno) {
				res = true;
				aCasita(jugadores, s, calle2);
				saleFicha(jugadores, jugadorTurno, calle1, calle2);
				premio = 20;
				pasaTurno = false;
				cout << "Sale de casa una ficha (premio: " << premio << "; pasa turno: " << boolalpha << pasaTurno << ")" << endl;
			}
			else
			{
				cout << "No puede salir una ficha de casa" << endl;
			}
		}
	}
	system("PAUSE");
	return res;
}
void cargar(tJugadores jugadores, tColor& jugadorTurno, tCasillas calle1, tCasillas calle2) {
	ifstream archivo;
	int jugador, casilla;

	archivo.open(Archivo);
	if (archivo.is_open()) {
		for (int i = 0; i < MAX_JUG; i++)
			for (int f = 0; f < NUM_FICHAS; f++) {
				archivo >> casilla;
				jugadores[i][f] = casilla;
				if ((casilla >= 0) && (casilla < NUM_CASILLAS))
					if (calle1[casilla] == Ninguno)
						calle1[casilla] = tColor(i);
					else
						calle2[casilla] = tColor(i);
			}
		archivo >> jugador;
		jugadorTurno = tColor(jugador);
		archivo.close();
	}
}
bool puente(const tCasillas calle1, const tCasillas calle2, int casilla) {
	bool res = false;
	if (casilla >= 0 && casilla < NUM_FICHAS) {
		if (calle2[casilla] == calle1[casilla] && calle1[casilla] != Ninguno) res = true;
	}
	return res;
}
bool puedeMover(const tJugadores jugadores, tColor jugadorTurno, int ficha, int& casilla, int tirada, const tCasillas calle1, const tCasillas calle2) {
	bool m = true;
	int cont = casilla;

	if (casilla != -1 || casilla != 108) {

		while (m && tirada > 0) {
			tirada--;
			cont++;
			if ((cont - 1) == zanataJugador(jugadorTurno)) {
				cont = 101;
			}
			else if (cont == 68) {
				cont = 0;
			}

			if (cont == 108 && tirada != 0) m = false;
			else if (esSeguro(cont) && puente(calle1, calle2, cont)) m = false;
			else if (puente(calle1, calle2, cont) && tirada == 0) m = false;

		}
	}

	if (m) casilla = cont;

	return m;
}
void mover(tJugadores jugadores, tColor jugadorTurno, int ficha, int casilla, int& premio, int& ultimaFichaMovida, tCasillas calle1, tCasillas calle2) {
	int j = jugadores[jugadorTurno][ficha];

	if (calle1[j] == jugadorTurno) {
		calle1[j] = calle2[j];
		calle2[j] = Ninguno;
	}
	else if (calle2[j] == jugadorTurno) {
		calle2[j] = Ninguno;
	}

	jugadores[jugadorTurno][ficha] = casilla;

	if (casilla == 108) {
		premio += 10;
		cout << "Has llegado a la meta, premio 10" << endl;
	}
	if (casilla < 101) {

		if (calle1[casilla] == Ninguno) {
			calle1[casilla] = jugadorTurno;
		}
		else {
			if (calle1[casilla] == jugadorTurno) {
				calle2[casilla] = calle1[casilla];
				calle1[casilla] = jugadorTurno;
			}
			else {
				if (esSeguro(casilla) == true) {
					calle2[casilla] = calle1[casilla];
					calle1[casilla] = jugadorTurno;
				}
				else {
					calle2[casilla] = calle1[casilla];
					aCasita(jugadores, casilla, calle2);
					calle1[casilla] = jugadorTurno;
					premio += 20;
					cout << "Has comido una ficha del jugador " << colorACadena(calle2[casilla]) << " premio 20" << endl;

				}
			}
		}
	}
}
bool procesa6(tJugadores jugadores, tColor jugadorTurno, int& premio, bool& pasarTurno, int& seises, int& ultimaFichaMovida, int& tirada, tCasillas calle1, tCasillas calle2) {
	bool seis = false, puentes = false;
	int casillaPuente1, casillaPuente2, ficha;

	seises++;
	int j = jugadores[jugadorTurno][ultimaFichaMovida];
	if (cuantasEn(jugadores[jugadorTurno], -1) == 0) {
		tirada = 7;
	}
	if (seises == 3) {
		if (ultimaFichaMovida != -1) {
			if (j <= 100) {
				if (calle2[j] == Ninguno) {
					calle2[j] = calle1[j];
					calle1[j] = Ninguno;
					aCasita(jugadores, j, calle2);
				}
				else if (segundaEn(jugadores[jugadorTurno], j) != ultimaFichaMovida && calle1[j] == jugadorTurno) {
					calle1[j] = calle2[j];
					calle2[j] = Ninguno;
					jugadores[jugadorTurno][ultimaFichaMovida] = -1;
				}
				else {
					if (calle1[j] == jugadorTurno) {
						tColor caux;
						caux = calle1[j];
						calle1[j] = calle2[j];
						calle2[j] = caux;
					}
					aCasita(jugadores, j, calle2);
				}
				cout << "Has sacado 3 seises, la ficha vuelve a casa" << endl;
			}
		}
		seis = true;
		pasarTurno = true;
	}
	else {
		for (int i = 0; i < NUM_CASILLAS; i++) {
			if (puente(calle1, calle2, i) && calle1[i] == jugadorTurno) {
				casillaPuente1 = i;
				casillaPuente2 = i;
				puentes = true;
				if (puedeMover(jugadores, jugadorTurno, segundaEn(jugadores[jugadorTurno], casillaPuente1), casillaPuente1, tirada, calle1, calle2)) {
					ficha = segundaEn(jugadores[jugadorTurno], i);
				}							
			}
		}
		if (puentes) {
			casillaPuente1 = jugadores[jugadorTurno][ficha];
			casillaPuente2 = casillaPuente1;
			if (puedeMover(jugadores, jugadorTurno, ficha, casillaPuente2, tirada, calle1, calle2)) {
				abrirPuente(jugadores, jugadorTurno, casillaPuente1, casillaPuente2, premio, ultimaFichaMovida, calle1, calle2);
			}
			jugadores[jugadorTurno][ficha] = casillaPuente2;
			seis = true;
		}

		pasarTurno = false;
	}
	return seis;
}
bool jugar(tJugadores jugadores, tColor jugadorTurno, int& premio, bool& fin, int& seises, int& ultimaFichaMovida, int tirada, tCasillas calle1, tCasillas calle2) {
	int  casilla, ficha = 1, cont = 0;
	bool res = true;
	for (int i = 0; i < NUM_FICHAS; i++) {
		casilla = jugadores[jugadorTurno][i];
		if (puedeMover(jugadores, jugadorTurno, i, casilla, tirada, calle1, calle2)) {
			cont++;
			ficha = i;
			cout << i << ": Puede ir a la casilla " << casilla << endl;
		}
		else
			cout << i << ": No se puede mover" << endl;	
	}
	system("PAUSE");

	if (cont != 0) {
 
		if (cont == 1) {
			casilla = jugadores[jugadorTurno][ficha];
			if (puedeMover(jugadores, jugadorTurno, ficha, casilla, tirada, calle1, calle2)) {
				mover(jugadores, jugadorTurno, ficha, casilla, premio, ultimaFichaMovida, calle1, calle2);
				res = false;
			}

		}
		else {
			cout << "Que ficha quieres mover: ";
			cin >> ficha;
			ficha--;
			casilla = jugadores[jugadorTurno][ficha];
			while (!puedeMover(jugadores, jugadorTurno, ficha, casilla, tirada, calle1, calle2)) {
				cout << "La ficha elegida no se puede mover, elija otra: ";
				cin >> ficha;
				ficha--;
				casilla = jugadores[jugadorTurno][ficha];
			}
			casilla = jugadores[jugadorTurno][ficha];
			if (puedeMover(jugadores, jugadorTurno, ficha, casilla, tirada, calle1, calle2)) {
				mover(jugadores, jugadorTurno, ficha, casilla, premio, ultimaFichaMovida, calle1, calle2);
				res = false;
			}
		}
	}
	return res;
}
bool todasEnMeta(const tFichas jugador) {
	bool res = false;
	if (cuantasEn(jugador, 108) == 4) {
		res = true;
	}
	return res;
}
void abrirPuente(tJugadores jugadores, tColor jugadorTurno, int casillaOrig, int casillaDest, int& premio, int& ultimaFichaMovida, tCasillas calle1, tCasillas calle2) {
	int f = segundaEn(jugadores[jugadorTurno], casillaOrig);
	mover(jugadores, jugadorTurno, f, casillaDest, premio, ultimaFichaMovida, calle1, calle2);
	ultimaFichaMovida = f;
}
int cuantasEn(const tFichas jugador, int casilla) {
	int res = 0;
	for (int i = 0; i < NUM_FICHAS; i++) {
		if (casilla == jugador[i]) res++;
	}
	return res;
}
int primeraEn(const tFichas jugador, int casilla) {
	int res = -1;
	for (int i = 0; i < NUM_FICHAS; i++) {
		if (casilla == jugador[i]) {
			res = i;
			i = 4;
		}
	}
	return res;
}
int segundaEn(const tFichas jugador, int casilla) {
	int res = -1;
	for (int i = 0; i < NUM_FICHAS; i++) {
		if (casilla == jugador[i]) {
			res = i;
		}
	}
	return res;
}
void iniciar(tJugadores jugadores, tCasillas calle1, tCasillas calle2, tColor& jugadorTurno) {
	srand(time(NULL));

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			jugadores[i][j] = -1;
		}
	}
	for (int i = 0; i < NUM_CASILLAS; i++) {
		calle1[i] = Ninguno;
		calle2[i] = Ninguno;
	}
	int c = (rand() % 3);
	jugadorTurno = tColor(c);
	iniciaColores();
	setColor(Gris);
}
char letra(tColor color) {
	char res;
	if (color == 0) {
		res = 'M';
	}
	else if (color == 1) {
		res = 'A';
	}
	else if (color == 2) {
		res = 'R';
	}
	else if (color == 3) {
		res = 'V';
	}
	return res;
}

string colorACadena(tColor color) {
	string res = "";
	if (color == 0) {
		res = "Amarillo";
	}
	else if (color == 1) {
		res = "Azul";
	}
	else if (color == 2) {
		res = "Rojo";
	}
	else if (color == 3) {
		res = "Verde";
	}
	return res;
}

bool esSeguro(int pos) {
	if (pos == 0 || pos == 5 || pos == 12 || pos == 17 || pos == 22 || pos == 29 || pos == 34 || pos == 39 || pos == 46 || pos == 51 || pos == 56 || pos == 63) {
		return true;
	}
	else {
		return false;
	}
}

int zanataJugador(tColor jugador) {
	int r = -1;
	if (jugador == Amarillo) r = 0;
	else if (jugador == Azul) r = 17;
	else if (jugador == Rojo) r = 34;
	else if (jugador == Verde) r = 51;
	return r;
}
int salidaJugador(tColor jugador) {
	int r = -1;
	if (jugador == Amarillo) r = 5;
	else if (jugador == Azul) r = 22;
	else if (jugador == Rojo) r = 39;
	else if (jugador == Verde) r = 56;
	return r;
}





// IMPLEMENTACIÓN DE LOS SUBPROGRAMAS DE VISUALIZACIÓN DEL TABLERO


void mostrar(const tJugadores jugadores, const tCasillas calle1, const tCasillas calle2) {
	int casilla, ficha;
	tColor jug;

	cout << "\x1b[2J\x1b[H"; // Borra la pantalla y se situa en la esquina superior izquierda
	setColor(Gris);
	cout << endl;

	// Filas con la numeración de las casillas...
	for (int i = 0; i < NUM_CASILLAS; i++)
		cout << i / 10;
	cout << endl;
	for (int i = 0; i < NUM_CASILLAS; i++)
		cout << i % 10;
	cout << endl;

	// Borde superior...
	for (int i = 0; i < NUM_CASILLAS; i++)
		cout << '>';
	cout << endl;

	// Primera fila de posiciones de fichas...
	for (int i = 0; i < NUM_CASILLAS; i++) {
		setColor(calle2[i]);
		if (calle2[i] != Ninguno)
			cout << primeraEn(jugadores[calle2[i]], i) + 1;
		else
			cout << ' ';
		setColor(Gris);
	}
	cout << endl;

	// "Mediana"
	for (int i = 0; i < NUM_CASILLAS; i++)
		if (esSeguro(i))
			cout << 'o';
		else
			cout << '-';
	cout << endl;


	// Segunda fila de posiciones de fichas...
	for (int i = 0; i < NUM_CASILLAS; i++) {
		setColor(calle1[i]);
		if (calle1[i] != Ninguno)
			cout << segundaEn(jugadores[calle1[i]], i) + 1;
		else
			cout << ' ';
		setColor(Gris);
	}
	cout << endl;

	jug = Amarillo;
	// Borde inferior...
	for (int i = 0; i < NUM_CASILLAS; i++)
		if (i == zanataJugador(jug)) {
			setColor(tColor(jug));
			cout << "V";
			setColor(Gris);
		}
		else if (i == salidaJugador(jug)) {
			setColor(tColor(jug));
			cout << "^";
			setColor(Gris);
			jug = tColor(int(jug) + 1);
		}
		else
			cout << '>';
	cout << endl;

	// Metas y casas...
	for (int i = 0; i < NUM_FICHAS; i++) {
		casilla = 0;
		jug = Amarillo;
		setColor(jug);
		while (casilla < NUM_CASILLAS) {
			if (casilla == zanataJugador(jug)) {
				ficha = primeraEn(jugadores[jug], 101 + i);
				if (ficha != -1) {
					cout << ficha + 1;
					if (cuantasEn(jugadores[jug], 101 + i) > 1) {
						ficha = segundaEn(jugadores[jug], 101 + i);
						if (ficha != -1) {
							cout << ficha + 1;
						}
						else
							cout << "V";
					}
					else
						cout << "V";
				}
				else
					cout << "VV";
				casilla++;
			}
			else if (casilla == salidaJugador(jug)) {
				if (jugadores[jug][i] == -1) // En casa
					cout << i + 1;
				else
					cout << "^";
				jug = tColor(int(jug) + 1);;
				setColor(jug);
			}
			else
				cout << ' ';
			casilla++;
		}
		cout << endl;
	}

	// Resto de metas...
	for (int i = 105; i <= 107; i++) {
		casilla = 0;
		jug = Amarillo;
		setColor(jug);
		while (casilla < NUM_CASILLAS) {
			if (casilla == zanataJugador(jug)) {
				ficha = primeraEn(jugadores[jug], i);
				if (ficha != -1) {
					cout << ficha + 1;
					if (cuantasEn(jugadores[jug], i) > 1) {
						ficha = segundaEn(jugadores[jug], i);
						if (ficha != -1) {
							cout << ficha + 1;
						}
						else
							cout << "V";
					}
					else
						cout << "V";
				}
				else
					cout << "VV";
				casilla++;
				jug = tColor(int(jug) + 1);
				setColor(jug);
			}
			else
				cout << ' ';
			casilla++;
		}
		cout << endl;
	}

	casilla = 0;
	jug = Amarillo;
	setColor(jug);
	while (casilla < NUM_CASILLAS) {
		cout << ((jugadores[jug][0] == 108) ? '1' : '.');
		cout << ((jugadores[jug][1] == 108) ? '2' : '.');
		jug = tColor(int(jug) + 1);
		setColor(jug);
		cout << "               ";
		casilla += 17;
	}
	cout << endl;
	casilla = 0;
	jug = Amarillo;
	setColor(jug);
	while (casilla < NUM_CASILLAS) {
		cout << ((jugadores[jug][2] == 108) ? '3' : '.');
		cout << ((jugadores[jug][3] == 108) ? '4' : '.');
		jug = tColor(int(jug) + 1);
		setColor(tColor(jug));
		cout << "               ";
		casilla += 17;
	}
	cout << endl << endl;
	setColor(Gris);
}


void setColor(tColor color) {
	switch (color) {
	case Azul:
		cout << "\x1b[34;107m";
		break;
	case Verde:
		cout << "\x1b[32;107m";
		break;
	case Rojo:
		cout << "\x1b[31;107m";
		break;
	case Amarillo:
		cout << "\x1b[33;107m";
		break;
	case Gris:
	case Ninguno:
		cout << "\x1b[90;107m";
		break;
	}
}



void iniciaColores() {
#ifdef _WIN32
	for (DWORD stream : {STD_OUTPUT_HANDLE, STD_ERROR_HANDLE}) {
		DWORD mode;
		HANDLE handle = GetStdHandle(stream);

		if (GetConsoleMode(handle, &mode)) {
			mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			SetConsoleMode(handle, mode);
		}
	}
#endif
}