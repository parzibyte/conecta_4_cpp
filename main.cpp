#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdlib.h> // rand y RAND_MAX
#include <unistd.h> // getpid
using namespace std;
const string ARCHIVO_RANKING = "ranking.txt",
			 RESULTADO_EMPATE = "e",
			 RESULTADO_GANA = "g",
			 RESULTADO_PIERDE = "p";
const int COLUMNAS_DEFECTO = 10,
		  FILAS_DEFECTO = 10,
		  MAXIMO_NUMERO_COLUMNAS = 10,
		  MAXIMO_NUMERO_FILAS = 10,
		  FICHAS_JUNTAS_NECESARIAS_PARA_GANAR = 4,
		  MAXIMO_JUGADORES_RANKING = 10;
const char ESPACIO_VACIO = ' ',
		   JUGADOR_HUMANO = 'X',
		   JUGADOR_CPU = 'O',
		   DELIMITADOR_RESULTADOS = ',',
		   DELIMITADOR_MOVIMIENTOS = ',',
		   DELIMITADOR_RANKING = ',';

void actualizarJugadorEnRanking(string nick);
struct ConfiguracionTablero
{
	int columnas, filas;
};

struct ConteoConColumna
{
	int conteo, columna;
};

struct EstadisticasDeJugador
{
	double partidas_ganadas, partidas_perdidas, empates, total_movimientos;
	double porcentaje_ganadas, porcentaje_perdidas, porcentaje_empatadas, promedio_movimientos;
};

struct Movimiento
{
	char jugador;
	int columna;
};

struct JugadorParaRanking
{
	string nombre;
	double puntuacion, movimientos;
};

string solicitar_nick()
{
	string nombre;
	cout << "Dame tu nick para comenzar el juego: ";
	cin >> nombre;
	return nombre;
}

string nombre_archivo_configuracion(string nick)
{
	return nick + "_configuracion.txt";
}

string nombre_archivo_resultados(string nick)
{
	return nick + "_resultados.txt";
}
string nombre_archivo_ultima_partida(string nick)
{
	return nick + "_ultimapartida.dat";
}

bool archivo_existe(string nombre)
{
	ifstream archivo(nombre.c_str());
	return archivo.good();
}
void cambiar_configuracion(string nick, ConfiguracionTablero configuracion)
{
	ofstream archivo;
	archivo.open(nombre_archivo_configuracion(nick).c_str(), fstream::out);
	archivo << configuracion.columnas;
	archivo << endl;
	archivo << configuracion.filas;
	archivo.close();
}

void escribir_archivo_resultados(string nick, string resultado, int movimientos)
{
	ofstream archivo;
	archivo.open(nombre_archivo_configuracion(nick).c_str(), fstream::out);
	archivo << resultado;
	archivo << ",";
	archivo << movimientos;
	archivo << endl;
	archivo.close();
}

string solicitar_nick_y_crear_archivos()
{
	string nick = solicitar_nick();
	// Si no existe el de configuración, asumo que no existen los otros 2 archivos
	if (!archivo_existe(nombre_archivo_configuracion(nick)))
	{
		// Creamos los 3
		std::ofstream(nombre_archivo_configuracion(nick).c_str());
		std::ofstream(nombre_archivo_resultados(nick).c_str());
		std::ofstream(nombre_archivo_ultima_partida(nick).c_str());
		// Y escribimos la configuración inicial
		ConfiguracionTablero inicial;
		inicial.columnas = COLUMNAS_DEFECTO;
		inicial.filas = FILAS_DEFECTO;
		cambiar_configuracion(nick, inicial);
	}
	return nick;
}

ConfiguracionTablero obtener_configuracion_tablero(string nick)
{
	ConfiguracionTablero configuracion;
	ifstream archivo(nombre_archivo_configuracion(nick).c_str());
	// Leemos la línea dentro de "linea" y convertimos
	string linea;
	getline(archivo, linea);
	configuracion.columnas = atoi(linea.c_str());
	// Lo mismo pero para la segunda línea, misma que representa las columnas
	getline(archivo, linea);
	configuracion.filas = atoi(linea.c_str());
	return configuracion;
}

vector<vector<char> > inicializarTablero(ConfiguracionTablero configuracion)
{
	vector<vector<char> > tablero;
	int x, y;
	for (y = 0; y < configuracion.filas; y++)
	{
		vector<char> fila;
		tablero.push_back(fila);
		for (x = 0; x < configuracion.columnas; x++)
		{
			tablero.at(y).push_back(ESPACIO_VACIO);
		}
	}
	return tablero;
}

void imprimir_tablero(vector<vector<char> > tablero)
{
	cout << endl;
	int y, x;
	const int filas = tablero.size(),
			  columnas = tablero.at(0).size();
	// Tablero
	for (y = 0; y < filas; y++)
	{
		for (x = 0; x < columnas; x++)
		{
			char actual = tablero[y][x];
			cout << "|" << actual;
			if (x + 1 >= columnas)
			{
				cout << "|";
			}
		}
		cout << endl;
	}

	// Imprimir línea de +--+--+--+
	for (x = 0; x < columnas; ++x)
	{
		cout << "+-";
		if (x + 1 >= columnas)
		{
			cout << "+";
		}
	}
	cout << endl;
	// Y los números
	for (x = 0; x < columnas; ++x)
	{
		cout << "|" << x;
		if (x + 1 >= columnas)
		{
			cout << "|";
		}
	}
	cout << endl;
}
int obtener_primera_fila_vacia(int columna, vector<vector<char> > tablero)
{
	int i;
	for (i = tablero.size() - 1; i >= 0; i--)
	{
		if (tablero[i][columna] == ESPACIO_VACIO)
		{
			return i;
		}
	}
	return -1;
}
int solicitar_columna(vector<vector<char> > tablero)
{
	while (true)
	{
		cout << "Seleccione columna para su tirada: ";
		int columna;
		cin >> columna;
		if (columna >= 0 && columna <= tablero[0].size() - 1 && obtener_primera_fila_vacia(columna, tablero) != -1)
		{
			return columna;
		}
		else if (obtener_primera_fila_vacia(columna, tablero) == -1)
		{
			cout << "No hay filas vacias en esta columna" << endl;
		}
		else
		{
			cout << "Columna inválida" << endl;
		}
	}
}

vector<vector<char> > colocar_pieza(int columna, vector<vector<char> > tablero, char jugador)
{
	tablero[obtener_primera_fila_vacia(columna, tablero)][columna] = jugador;
	return tablero;
}

int contarArribaDerecha(int x, int y, char jugador, vector<vector<char> > tablero)
{
	int xFin = (x + FICHAS_JUNTAS_NECESARIAS_PARA_GANAR < tablero[0].size()) ? x + FICHAS_JUNTAS_NECESARIAS_PARA_GANAR - 1 : tablero[0].size() - 1;
	int yInicio = (y - FICHAS_JUNTAS_NECESARIAS_PARA_GANAR >= 0) ? y - FICHAS_JUNTAS_NECESARIAS_PARA_GANAR + 1 : 0;
	int contador = 0;
	while (x <= xFin && yInicio <= y)
	{
		if (tablero[y][x] == jugador)
		{
			contador++;
		}
		else
		{
			return contador;
		}
		x++;
		y--;
	}
	return contador;
}

int contarArribaIzquierda(int x, int y, char jugador, vector<vector<char> > tablero)
{
	int xFin = (x - FICHAS_JUNTAS_NECESARIAS_PARA_GANAR >= 0) ? x - FICHAS_JUNTAS_NECESARIAS_PARA_GANAR + 1 : 0;
	int yInicio = (y - FICHAS_JUNTAS_NECESARIAS_PARA_GANAR >= 0) ? y - FICHAS_JUNTAS_NECESARIAS_PARA_GANAR + 1 : 0;
	int contador = 0;
	while (x >= xFin && yInicio <= y)
	{
		if (tablero[y][x] == jugador)
		{
			contador++;
		}
		else
		{
			return contador;
		}
		x--;
		y--;
	}
	return contador;
}

int contarArriba(int x, int y, char jugador, vector<vector<char> > tablero)
{
	int yInicio = (y - FICHAS_JUNTAS_NECESARIAS_PARA_GANAR >= 0) ? y - FICHAS_JUNTAS_NECESARIAS_PARA_GANAR + 1 : 0;
	int contador = 0;
	for (; yInicio <= y; yInicio++)
	{
		if (tablero[yInicio][x] == jugador)
		{
			contador++;
		}
		else
		{
			return contador;
		}
	}
	return contador;
}

int contarAbajo(int x, int y, char jugador, vector<vector<char> > tablero)
{
	int yFin = (y + FICHAS_JUNTAS_NECESARIAS_PARA_GANAR <= tablero.size() - 1) ? y + FICHAS_JUNTAS_NECESARIAS_PARA_GANAR - 1 : tablero.size() - 1;
	int contador = 0;
	for (; y <= yFin; y++)
	{
		if (tablero[y][x] == jugador)
		{
			contador++;
		}
		else
		{
			return contador;
		}
	}
	return contador;
}

int contarDerecha(int x, int y, char jugador, vector<vector<char> > tablero)
{
	int xFin = (x + FICHAS_JUNTAS_NECESARIAS_PARA_GANAR < tablero[0].size()) ? x + FICHAS_JUNTAS_NECESARIAS_PARA_GANAR - 1 : tablero[0].size() - 1;
	int contador = 0;
	for (; x <= xFin; x++)
	{
		if (tablero[y][x] == jugador)
		{
			contador++;
		}
		else
		{
			return contador;
		}
	}
	return contador;
}

int contarAbajoDerecha(int x, int y, char jugador, vector<vector<char> > tablero)
{
	int xFin = (x + FICHAS_JUNTAS_NECESARIAS_PARA_GANAR < tablero[0].size()) ? x + FICHAS_JUNTAS_NECESARIAS_PARA_GANAR - 1 : tablero[0].size() - 1;
	int yFin = (y + FICHAS_JUNTAS_NECESARIAS_PARA_GANAR < tablero.size()) ? y + FICHAS_JUNTAS_NECESARIAS_PARA_GANAR - 1 : tablero.size() - 1;
	int contador = 0;
	while (x <= xFin && y <= yFin)
	{
		if (tablero[y][x] == jugador)
		{
			contador++;
		}
		else
		{
			return contador;
		}
		x++;
		y++;
	}
	return contador;
}
int contarIzquierda(int x, int y, char jugador, vector<vector<char> > tablero)
{
	int xFin = (x - FICHAS_JUNTAS_NECESARIAS_PARA_GANAR >= 0) ? x - FICHAS_JUNTAS_NECESARIAS_PARA_GANAR + 1 : 0;
	int contador = 0;
	for (; x >= xFin; x--)
	{
		if (tablero[y][x] == jugador)
		{
			contador++;
		}
		else
		{
			return contador;
		}
	}
	return contador;
}
int contarAbajoIzquierda(int x, int y, char jugador, vector<vector<char> > tablero)
{
	int xFin = (x - FICHAS_JUNTAS_NECESARIAS_PARA_GANAR >= 0) ? x - FICHAS_JUNTAS_NECESARIAS_PARA_GANAR + 1 : 0;
	int yFin = (y + FICHAS_JUNTAS_NECESARIAS_PARA_GANAR < tablero.size()) ? y + FICHAS_JUNTAS_NECESARIAS_PARA_GANAR - 1 : tablero.size() - 1;
	int contador = 0;
	while (x >= xFin && y <= yFin)
	{

		if (tablero[y][x] == jugador)
		{
			contador++;
		}
		else
		{
			return contador;
		}
		x--;
		y++;
	}
	return contador;
}

bool jugador_gana(char jugador, vector<vector<char> > tablero)
{
	/*
 * Solo necesitamos
 * Arriba
 * Derecha
 * Arriba derecha
 * Abajo derecha
 * Las demás direcciones son útiles para cuando se elige a la columna ganadora
 *
 * */
	int y;
	for (y = 0; y < tablero.size(); y++)
	{
		int x;
		for (x = 0; x < tablero[y].size(); x++)
		{
			if (contarAbajo(x, y, jugador, tablero) >= FICHAS_JUNTAS_NECESARIAS_PARA_GANAR)
			{
				return true;
			}

			if (contarDerecha(x, y, jugador, tablero) >= FICHAS_JUNTAS_NECESARIAS_PARA_GANAR)
			{
				return true;
			}

			if (contarAbajoIzquierda(x, y, jugador, tablero) >= FICHAS_JUNTAS_NECESARIAS_PARA_GANAR)
			{
				return true;
			}
			if (contarAbajoDerecha(x, y, jugador, tablero) >= FICHAS_JUNTAS_NECESARIAS_PARA_GANAR)
			{
				return true;
			}
		}
	}
	return false;
}

int obtener_columna_ganadora(char jugador, vector<vector<char> > tablero)
{
	vector<vector<char> > tablero_original;
	tablero_original = tablero;
	int i;
	for (i = 0; i < tablero[0].size(); i++)
	{
		tablero = tablero_original;
		if (obtener_primera_fila_vacia(i, tablero) != -1)
		{
			tablero = colocar_pieza(i, tablero, jugador);
			if (jugador_gana(jugador, tablero))
			{
				return i;
			}
		}
	}
	return -1;
}
int obtener_primera_fila_llena(int columna, vector<vector<char> > tablero)
{
	int i;
	for (i = 0; i < tablero.size(); ++i)
	{
		if (tablero[i][columna] != ESPACIO_VACIO)
		{
			return i;
		}
	}
	return -1;
}
ConteoConColumna obtener_columna_en_la_que_se_obtiene_mayor_puntaje(char jugador, vector<vector<char> > tableroOriginal)
{
	ConteoConColumna conteoConCoordenada;
	conteoConCoordenada.conteo = 0;
	conteoConCoordenada.columna = -1;
	vector<vector<char> > tablero = tableroOriginal;
	int x, conteo;
	for (x = 0; x < tablero[0].size(); ++x)
	{
		tablero = tableroOriginal;

		if (obtener_primera_fila_vacia(x, tablero) != -1)
		{
			tablero = colocar_pieza(x, tablero, jugador);
			int fila_pieza_recien_colocada = obtener_primera_fila_llena(x, tablero);
			if (fila_pieza_recien_colocada != -1)
			{
				conteo = contarArriba(x, fila_pieza_recien_colocada, jugador, tablero);
				if (conteo > conteoConCoordenada.conteo)
				{
					conteoConCoordenada.conteo = conteo;
					conteoConCoordenada.columna = x;
				}
				conteo = contarAbajo(x, fila_pieza_recien_colocada, jugador, tablero);
				if (conteo > conteoConCoordenada.conteo)
				{
					conteoConCoordenada.conteo = conteo;
					conteoConCoordenada.columna = x;
				}
				conteo = contarDerecha(x, fila_pieza_recien_colocada, jugador, tablero);
				if (conteo > conteoConCoordenada.conteo)
				{
					conteoConCoordenada.conteo = conteo;
					conteoConCoordenada.columna = x;
				}
				conteo = contarIzquierda(x, fila_pieza_recien_colocada, jugador, tablero);
				if (conteo > conteoConCoordenada.conteo)
				{
					conteoConCoordenada.conteo = conteo;
					conteoConCoordenada.columna = x;
				}
				conteo = contarAbajoIzquierda(x, fila_pieza_recien_colocada, jugador, tablero);
				if (conteo > conteoConCoordenada.conteo)
				{
					conteoConCoordenada.conteo = conteo;
					conteoConCoordenada.columna = x;
				}
				conteo = contarArribaDerecha(x, fila_pieza_recien_colocada, jugador, tablero);
				if (conteo > conteoConCoordenada.conteo)
				{
					conteoConCoordenada.conteo = conteo;
					conteoConCoordenada.columna = x;
				}
				conteo = contarAbajoDerecha(x, fila_pieza_recien_colocada, jugador, tablero);
				if (conteo > conteoConCoordenada.conteo)
				{
					conteoConCoordenada.conteo = conteo;
					conteoConCoordenada.columna = x;
				}
				conteo = contarArribaIzquierda(x, fila_pieza_recien_colocada, jugador, tablero);
				if (conteo > conteoConCoordenada.conteo)
				{
					conteoConCoordenada.conteo = conteo;
					conteoConCoordenada.columna = x;
				}
			}
		}
	}
	return conteoConCoordenada;
}
int aleatorio_en_rango(int minimo, int maximo)
{
	// Asegurarnos de invocar a srand una vez
	static bool primeraVez = true;
	if (primeraVez)
	{
		srand(getpid());
		primeraVez = false;
	}
	return minimo + rand() / (RAND_MAX / (maximo - minimo + 1) + 1);
}

int obtener_columna_aleatoria(char jugador, vector<vector<char> > tableroOriginal)
{
	vector<vector<char> > tablero;
	while (1)
	{
		tablero = tableroOriginal;
		int columna = aleatorio_en_rango(0, tablero[0].size() - 1);
		if (obtener_primera_fila_vacia(columna, tablero) != -1)
		{
			return columna;
		}
	}
}
int obtener_columna_central(char jugador, vector<vector<char> > tableroOriginal)
{
	int mitad = (tableroOriginal[0].size() - 1) / 2;
	if (obtener_primera_fila_vacia(mitad, tableroOriginal) != -1)
	{
		return mitad;
	}
	return -1;
}
char obtener_oponente(char jugador)
{
	if (jugador == JUGADOR_HUMANO)
	{
		return JUGADOR_CPU;
	}
	return JUGADOR_HUMANO;
}
int elegir_mejor_columna(char jugador, vector<vector<char> > tablero)
{
	// Voy a comprobar si puedo ganar...
	int posibleColumnaGanadora = obtener_columna_ganadora(jugador, tablero);
	if (posibleColumnaGanadora != -1)
	{
		cout << "*elijo ganar*\n";
		return posibleColumnaGanadora;
	}
	// Si no, voy a comprobar si mi oponente gana con el siguiente movimiento, para evitarlo
	char oponente = obtener_oponente(jugador);
	int posibleColumnaGanadoraDeOponente = obtener_columna_ganadora(oponente, tablero);
	if (posibleColumnaGanadoraDeOponente != -1)
	{
		cout << "*elijo evitar que mi oponente gane*\n";
		return posibleColumnaGanadoraDeOponente;
	}
	// En caso de que nadie pueda ganar en el siguiente movimiento, buscaré en dónde se obtiene el mayor
	// puntaje al colocar la pieza
	ConteoConColumna conteoConColumnaJugador = obtener_columna_en_la_que_se_obtiene_mayor_puntaje(jugador, tablero);
	ConteoConColumna conteoConColumnaOponente = obtener_columna_en_la_que_se_obtiene_mayor_puntaje(oponente, tablero);
	if (conteoConColumnaOponente.conteo > conteoConColumnaJugador.conteo)
	{
		cout << "*elijo quitarle el puntaje a mi oponente*\n";
		return conteoConColumnaOponente.columna;
	}
	else if (conteoConColumnaJugador.conteo > 1)
	{
		cout << "*elijo colocarla en donde obtengo un mayor puntaje*\n";
		return conteoConColumnaJugador.columna;
	}
	// Si no, regresar la central por si está desocupada

	int columnaCentral = obtener_columna_central(jugador, tablero);
	if (columnaCentral != -1)
	{
		cout << "*elijo ponerla en el centro*\n";
		return columnaCentral;
	}
	// Finalmente, devolver la primera disponible de manera aleatoria
	int columna = obtener_columna_aleatoria(jugador, tablero);
	if (columna != -1)
	{
		cout << "*elijo la primera vacía aleatoria*\n";
		return columna;
	}
	cout << "Esto no debería suceder\n";
	return 0;
}

bool esEmpate(vector<vector<char> > tablero)
{
	int i;
	for (i = 0; i < tablero[0].size(); ++i)
	{
		if (obtener_primera_fila_vacia(i, tablero) != -1)
		{
			return false;
		}
	}
	return true;
}
void guardarPartidaTerminada(string nick, string resultado, int movimientos)
{
	ofstream archivo;
	archivo.open(nombre_archivo_resultados(nick).c_str(), ios_base::app);
	archivo << resultado << DELIMITADOR_RESULTADOS << movimientos;
	archivo << endl;
	archivo.close();
	actualizarJugadorEnRanking(nick);
}
void guardar_movimientos_de_partida(string nick, vector<Movimiento> movimientos)
{
	ofstream archivo;
	archivo.open(nombre_archivo_ultima_partida(nick).c_str(), fstream::out);
	int i;
	for (i = 0; i < movimientos.size(); i++)
	{
		Movimiento movimiento = movimientos[i];
		archivo << movimiento.jugador << DELIMITADOR_MOVIMIENTOS << movimiento.columna << "\n";
	}
	archivo.close();
}

vector<Movimiento> obtener_movimientos_de_partida(string nick)
{
	vector<Movimiento> movimientos;
	Movimiento movimiento;
	ifstream archivo(nombre_archivo_ultima_partida(nick).c_str());
	string linea, jugador, columna;
	while (getline(archivo, linea))
	{
		stringstream input_stringstream(linea);
		getline(input_stringstream, jugador, DELIMITADOR_MOVIMIENTOS);
		getline(input_stringstream, columna, DELIMITADOR_MOVIMIENTOS);
		movimiento.jugador = jugador[0];
		movimiento.columna = atoi(columna.c_str());
		movimientos.push_back(movimiento);
	}
	return movimientos;
}
void anunciar_victoria(char jugador, string nick)
{
	if (jugador == JUGADOR_HUMANO)
	{
		cout << nick << " gana la partida. Felicidades"
			 << "\n";
	}
	else
	{
		cout << "El CPU gana la partida. Felicidades"
			 << "\n";
	}
}
void anunciar_empate()
{
	cout << "El juego termina en empate "
		 << "\n";
}

void jugar(string nick)
{
	ConfiguracionTablero configuracion = obtener_configuracion_tablero(nick);
	vector<vector<char> > tablero = inicializarTablero(configuracion);
	int jugadorActual = JUGADOR_HUMANO;
	int columna;
	int conteo_movimientos = 0;
	vector<Movimiento> movimientos;
	Movimiento movimiento;
	while (true)
	{
		imprimir_tablero(tablero);
		if (jugadorActual == JUGADOR_HUMANO)
		{
			cout << nick << ", elige: " << endl;
			columna = solicitar_columna(tablero);
			cout << nick << " elige la columna " << columna << endl;
			conteo_movimientos++;
		}
		else
		{
			cout << "CPU, elige:" << endl;
			columna = elegir_mejor_columna(jugadorActual, tablero);
		}
		tablero = colocar_pieza(columna, tablero, jugadorActual);
		movimiento.columna = columna;
		movimiento.jugador = jugadorActual;
		movimientos.push_back(movimiento);
		if (jugador_gana(jugadorActual, tablero))
		{
			imprimir_tablero(tablero);
			anunciar_victoria(jugadorActual, nick);
			if (jugadorActual == JUGADOR_HUMANO)
			{
				guardarPartidaTerminada(nick, RESULTADO_GANA, conteo_movimientos);
			}
			else
			{
				guardarPartidaTerminada(nick, RESULTADO_PIERDE, conteo_movimientos);
			}
			break;
		}
		else if (esEmpate(tablero))
		{
			imprimir_tablero(tablero);
			anunciar_empate();
			if (jugadorActual == JUGADOR_HUMANO)
			{
				guardarPartidaTerminada(nick, RESULTADO_EMPATE, conteo_movimientos);
			}
			break;
		}
		jugadorActual = obtener_oponente(jugadorActual);
	}
	guardar_movimientos_de_partida(nick, movimientos);
}
void visualizar_configuracion_del_tablero(string nick)
{
	ConfiguracionTablero configuracion = obtener_configuracion_tablero(nick);
	cout << "La configuracion para '"
		 << nick << "' es un tablero de "
		 << configuracion.columnas << " columnas y "
		 << configuracion.filas << " filas" << endl;
}

void solicitar_nueva_configuracion_tablero_y_guardar(string nick)
{
	int filas, columnas;
	while (true)
	{
		cout << "Ingresa las filas: " << endl;
		cin >> filas;
		if (filas > 0 && filas <= MAXIMO_NUMERO_FILAS)
		{
			break;
		}
		else
		{
			cout << "Numero de filas no valido. Intenta de nuevo" << endl;
		}
	}
	while (true)
	{
		cout << "Ingresa las columnas: " << endl;
		cin >> columnas;
		if (columnas > 0 && columnas <= MAXIMO_NUMERO_COLUMNAS)
		{
			break;
		}
		else
		{
			cout << "Numero de columnas no valido. Intenta de nuevo" << endl;
		}
	}
	ConfiguracionTablero nuevaConfiguracion;
	nuevaConfiguracion.columnas = columnas;
	nuevaConfiguracion.filas = filas;
	cambiar_configuracion(nick, nuevaConfiguracion);
	cout << "Se ha guardado la configuracion" << endl;
}

EstadisticasDeJugador obtener_estadisticas(string nick)
{
	EstadisticasDeJugador estadisticas;
	estadisticas.empates = 0;
	estadisticas.partidas_ganadas = 0;
	estadisticas.partidas_perdidas = 0;
	estadisticas.total_movimientos = 0;
	ifstream archivo(nombre_archivo_resultados(nick).c_str());
	string linea, resultado, movimientos;
	while (getline(archivo, linea))
	{
		stringstream input_stringstream(linea);
		getline(input_stringstream, resultado, DELIMITADOR_RESULTADOS);
		getline(input_stringstream, movimientos, DELIMITADOR_RESULTADOS);
		if (resultado == RESULTADO_EMPATE)
		{
			estadisticas.empates++;
		}
		else if (resultado == RESULTADO_GANA)
		{
			estadisticas.partidas_ganadas++;
		}
		else if (resultado == RESULTADO_PIERDE)
		{
			estadisticas.partidas_perdidas++;
		}
		estadisticas.total_movimientos += atoi(movimientos.c_str());
	}
	double total_partidas = estadisticas.partidas_ganadas + estadisticas.partidas_perdidas + estadisticas.empates;
	estadisticas.porcentaje_ganadas = (estadisticas.partidas_ganadas * 100) / total_partidas;
	estadisticas.porcentaje_perdidas = (estadisticas.partidas_perdidas * 100) / total_partidas;
	estadisticas.porcentaje_empatadas = (estadisticas.empates * 100) / total_partidas;
	estadisticas.promedio_movimientos = estadisticas.total_movimientos / total_partidas;
	return estadisticas;
}

void ver_estadisticas(string nick)
{
	EstadisticasDeJugador estadisticas = obtener_estadisticas(nick);
	cout << "Mostrando estadisticas para " << nick << "\n";
	cout << "Porcentaje de partidas ganadas: " << estadisticas.porcentaje_ganadas << " %\n";
	cout << "Porcentaje de partidas perdidas: " << estadisticas.porcentaje_perdidas << " %\n";
	cout << "Porcentaje de partidas empatadas: " << estadisticas.porcentaje_empatadas << " %\n";
	cout << "Promedio de movimientos " << estadisticas.promedio_movimientos << "\n";
}

void repetir_ultima_partida(string nick)
{
	// Consumimos el último salto de línea que seguramente está en el búfer
	getchar();
	cout << "Repitiendo ultima partida guardada para '" << nick << "'"
		 << "\n";
	vector<Movimiento> movimientos = obtener_movimientos_de_partida(nick);
	ConfiguracionTablero configuracion = obtener_configuracion_tablero(nick);
	vector<vector<char> > tablero = inicializarTablero(configuracion);
	int i;
	for (i = 0; i < movimientos.size(); i++)
	{
		char jugador = movimientos[i].jugador;
		int columna = movimientos[i].columna;
		if (jugador == JUGADOR_HUMANO)
		{
			cout << "El jugador elige la columna " << columna << "\n";
		}
		else
		{
			cout << "El CPU elige la columna " << columna << "\n";
		}
		tablero = colocar_pieza(columna, tablero, jugador);
		imprimir_tablero(tablero);
		if (jugador_gana(jugador, tablero))
		{
			anunciar_victoria(jugador, nick);
		}
		else if (esEmpate(tablero))
		{
			anunciar_empate();
		}
		cout << "Presiona Enter para ver el siguiente movimiento...";
		getchar();
	}
	cout << "Se ha terminado la repeticion"
		 << "\n";
}

JugadorParaRanking calcular_puntaje(string nick)
{
	EstadisticasDeJugador estadisticas = obtener_estadisticas(nick);
	JugadorParaRanking jugador;
	jugador.nombre = nick;
	double total_partidas = estadisticas.partidas_ganadas + estadisticas.partidas_perdidas + estadisticas.empates;
	jugador.puntuacion = (estadisticas.partidas_ganadas - estadisticas.partidas_perdidas) / total_partidas;
	jugador.movimientos = estadisticas.promedio_movimientos;
	return jugador;
}

vector<JugadorParaRanking> obtenerJugadoresRanking()
{
	vector<JugadorParaRanking> jugadores;
	JugadorParaRanking jugadorParaRanking;
	ifstream archivo(ARCHIVO_RANKING.c_str());
	string linea, nombre, puntuacion, movimientos;
	while (getline(archivo, linea))
	{
		stringstream input_stringstream(linea);
		getline(input_stringstream, nombre, DELIMITADOR_RANKING);
		getline(input_stringstream, puntuacion, DELIMITADOR_RANKING);
		getline(input_stringstream, movimientos, DELIMITADOR_RANKING);
		jugadorParaRanking.nombre = nombre;
		jugadorParaRanking.puntuacion = atof(puntuacion.c_str());
		jugadorParaRanking.movimientos = atof(movimientos.c_str());
		jugadores.push_back(jugadorParaRanking);
	}
	return jugadores;
}
void guardarJugadoresRanking(vector<JugadorParaRanking> jugadores)
{
	ofstream archivo;
	archivo.open(ARCHIVO_RANKING.c_str(), fstream::out);
	// Solo escribir [MAXIMO_JUGADORES_RANKING] jugadores
	int verdaderoFin = MAXIMO_JUGADORES_RANKING;
	if (jugadores.size() < verdaderoFin)
	{
		verdaderoFin = jugadores.size();
	}
	int i;
	for (i = 0; i < verdaderoFin; i++)
	{
		JugadorParaRanking jugador = jugadores[i];
		archivo << jugador.nombre << DELIMITADOR_RANKING << jugador.puntuacion << DELIMITADOR_RANKING << jugador.movimientos << "\n";
	}
	archivo.close();
}

int indiceDeJugador(vector<JugadorParaRanking> jugadores, string jugadorBuscado)
{
	int i;
	for (i = 0; i < jugadores.size(); i++)
	{
		JugadorParaRanking jugador = jugadores[i];
		if (jugador.nombre == jugadorBuscado)
		{
			return i;
		}
	}
	return -1;
}

vector<JugadorParaRanking> ordenar(vector<JugadorParaRanking> jugadores)
{
	int i, j;
	for (i = 0; i < jugadores.size() - 1; i++)
	{
		for (j = i + 1; j < jugadores.size(); j++)
		{
			JugadorParaRanking jugadorActual = jugadores[i];
			JugadorParaRanking jugadorDerecha = jugadores[j];
			if (jugadorActual.puntuacion < jugadorDerecha.puntuacion)
			{
				jugadores[i] = jugadorDerecha;
				jugadores[j] = jugadorActual;
			}
			else if (jugadorActual.puntuacion == jugadorDerecha.puntuacion)
			{
				if (jugadorActual.movimientos > jugadorDerecha.movimientos)
				{
					jugadores[i] = jugadorDerecha;
					jugadores[j] = jugadorActual;
				}
			}
		}
	}
	return jugadores;
}

void actualizarJugadorEnRanking(string nick)
{
	JugadorParaRanking jugador = calcular_puntaje(nick);
	vector<JugadorParaRanking> jugadores = obtenerJugadoresRanking();
	int posibleIndice = indiceDeJugador(jugadores, jugador.nombre);
	if (posibleIndice == -1)
	{
		jugadores.push_back(jugador);
	}
	else
	{
		jugadores[posibleIndice] = jugador;
	}
	guardarJugadoresRanking(ordenar(jugadores));
}

void mostrarTop10()
{
	vector<JugadorParaRanking> jugadores = obtenerJugadoresRanking();
	cout << "Mostrando top 10\n";
	cout << "--------------------------------------------------------"
		 << "\n";
	int i;
	for (i = 0; i < jugadores.size(); i++)
	{
		JugadorParaRanking jugadorActual = jugadores[i];
		cout << i + 1 << ". " << jugadorActual.nombre << " con puntuacion " << jugadorActual.puntuacion << " y " << jugadorActual.movimientos << " movimientos promedio"
			 << "\n";
	}
	cout << "--------------------------------------------------------"
		 << "\n";
}

int main()
{
	string nick = solicitar_nick_y_crear_archivos();
	int eleccion = 0;
	while (eleccion != 7)
	{
		cout << "1. Visualizar configuracion del tablero" << endl;
		cout << "2. Cambiar configuracion del tablero" << endl;
		cout << "3. Ver estadisticas" << endl;
		cout << "4. Reproducir ultima partida" << endl;
		cout << "5. Mostrar top 10" << endl;
		cout << "6. Jugar partida" << endl;
		cout << "7. Salir" << endl;
		cout << "Elige [1-7]: " << endl;
		cin >> eleccion;
		if (eleccion == 1)
		{
			visualizar_configuracion_del_tablero(nick);
		}
		else if (eleccion == 2)
		{
			solicitar_nueva_configuracion_tablero_y_guardar(nick);
		}
		else if (eleccion == 3)
		{
			ver_estadisticas(nick);
		}
		else if (eleccion == 4)
		{
			repetir_ultima_partida(nick);
		}
		else if (eleccion == 5)
		{
			mostrarTop10();
		}
		else if (eleccion == 6)
		{
			jugar(nick);
		}
	}
}