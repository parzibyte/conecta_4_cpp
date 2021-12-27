#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
const string ARCHIVO_RANKING = "ranking.txt",
			 RESULTADO_EMPATE = "e",
			 RESULTADO_GANA = "g",
			 RESULTADO_PIERDE = "p";
const int COLUMNAS_DEFECTO = 10,
		  FILAS_DEFECTO = 10,
		  FICHAS_JUNTAS_NECESARIAS_PARA_GANAR = 4;
const char ESPACIO_VACIO = ' ',
		   JUGADOR = 'X',
		   CPU = 'O';

struct ConfiguracionTablero
{
	int columnas, filas;
};

struct ConteoConColumna
{
	int conteo, columna;
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
	ifstream archivo(nombre);
	return archivo.good();
}
void cambiar_configuracion(string nick, ConfiguracionTablero configuracion)
{
	ofstream archivo;
	archivo.open(nombre_archivo_configuracion(nick), fstream::out);
	archivo << configuracion.columnas;
	archivo << endl;
	archivo << configuracion.filas;
	archivo.close();
}

void escribir_archivo_resultados(string nick, string resultado, int movimientos)
{
	ofstream archivo;
	archivo.open(nombre_archivo_configuracion(nick), fstream::out);
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
		std::ofstream(nombre_archivo_configuracion(nick));
		std::ofstream(nombre_archivo_resultados(nick));
		std::ofstream(nombre_archivo_ultima_partida(nick));
		// Y escribimos la configuración inicial
		cambiar_configuracion(nick, ConfiguracionTablero{COLUMNAS_DEFECTO, FILAS_DEFECTO});
	}
	return nick;
}

ConfiguracionTablero obtener_configuracion_tablero(string nick)
{
	ConfiguracionTablero configuracion;
	ifstream archivo(nombre_archivo_configuracion(nick));
	// Leemos la línea dentro de "linea" y convertimos
	string linea;
	getline(archivo, linea);
	configuracion.columnas = stoi(linea);
	// Lo mismo pero para la segunda línea, misma que representa las columnas
	getline(archivo, linea);
	configuracion.filas = stoi(linea);
	return configuracion;
}

vector<vector<char>> inicializarTablero(ConfiguracionTablero configuracion)
{
	vector<vector<char>> tablero;
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

void imprimir_tablero(vector<vector<char>> tablero)
{
	cout << endl; //TODO: quitar antes de entregar
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
int obtener_primera_fila_vacia(int columna, vector<vector<char>> tablero)
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
int solicitar_columna(int columnas, vector<vector<char>> tablero)
{
	while (true)
	{
		cout << "Seleccione columna para su tirada: ";
		int columna;
		cin >> columna;
		if (columna >= 0 && columna <= columnas - 1)
		{
			return columna;
		}
		else if (obtener_primera_fila_vacia(columna, tablero) == -1)
		{
			cout << "No hay filas vacías en esta columna";
		}
		else
		{
			cout << "Columna inválida";
		}
	}
}

vector<vector<char>> colocar_pieza(int columna, vector<vector<char>> tablero, char jugador)
{
	//TODO: tal vez aquí registrar la partida
	tablero[obtener_primera_fila_vacia(columna, tablero)][columna] = jugador;
	return tablero;
}

int contarArriba(int x, int y, char jugador, vector<vector<char>> tablero)
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
			contador = 0;
		}
	}
	return contador;
}

int contarDerecha(int x, int y, char jugador, vector<vector<char>> tablero)
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
			contador = 0;
		}
	}
	return contador;
}

int contarArribaDerecha(int x, int y, char jugador, vector<vector<char>> tablero)
{
	int xFin = (x + FICHAS_JUNTAS_NECESARIAS_PARA_GANAR < tablero[0].size()) ? x + FICHAS_JUNTAS_NECESARIAS_PARA_GANAR - 1 : tablero[0].size() - 1;
	int yInicio = (y - tablero[0].size() >= 0) ? y - FICHAS_JUNTAS_NECESARIAS_PARA_GANAR + 1 : 0;
	int contador = 0;
	while (x <= xFin && yInicio <= y)
	{
		if (tablero[y][x] == jugador)
		{
			contador++;
		}
		else
		{
			contador = 0;
		}
		x++;
		y--;
	}
	return contador;
}

int contarAbajoDerecha(int x, int y, char jugador, vector<vector<char>> tablero)
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
			contador = 0;
		}
		x++;
		y++;
	}
	return contador;
}

bool jugador_gana(char jugador, vector<vector<char>> tablero)
{
	/*
 * Solo necesitamos
 * Arriba
 * Derecha
 * Arriba derecha
 * Abajo derecha
 *
 * */
	int y;
	for (y = 0; y < tablero.size(); y++)
	{
		int x;
		for (x = 0; x < tablero[y].size(); x++)
		{
			if (contarArriba(x, y, jugador, tablero) >= FICHAS_JUNTAS_NECESARIAS_PARA_GANAR)
			{
				return true;
			}

			if (contarDerecha(x, y, jugador, tablero) >= FICHAS_JUNTAS_NECESARIAS_PARA_GANAR)
			{
				return true;
			}

			if (contarArribaDerecha(x, y, jugador, tablero) >= FICHAS_JUNTAS_NECESARIAS_PARA_GANAR)
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

int obtener_columna_ganadora_para_cpu(vector<vector<char>> tablero)
{
	vector<vector<char>> tablero_original;
	tablero_original = tablero;
	int i;
	for (i = 0; i < tablero[0].size(); i++)
	{
		tablero = tablero_original;
		if (obtener_primera_fila_vacia(i, tablero) != -1)
		{
			tablero = colocar_pieza(i, tablero, CPU);
			if (jugador_gana(CPU, tablero))
			{
				return i;
			}
		}
	}
	return -1;
}
int obtener_primera_fila_llena(int columna, vector<vector<char>> tablero)
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
ConteoConColumna obtener_columna_en_la_que_se_obtiene_mayor_puntaje(char jugador, vector<vector<char>> tableroOriginal)
{
	ConteoConColumna conteoConCoordenada{0, -1};
	vector<vector<char>> tablero = tableroOriginal;
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
				int y;
				for (y = fila_pieza_recien_colocada; y < tablero.size(); y++)
				{
					conteo = contarArriba(x, y, jugador, tablero);
					if (conteo > conteoConCoordenada.conteo)
					{
						conteoConCoordenada.conteo = conteo;
						conteoConCoordenada.columna = x;
					}
					conteo = contarArribaDerecha(x, y, jugador, tablero);
					if (conteo > conteoConCoordenada.conteo)
					{
						conteoConCoordenada.conteo = conteo;
						conteoConCoordenada.columna = x;
					}
					conteo = contarDerecha(x, y, jugador, tablero);
					if (conteo > conteoConCoordenada.conteo)
					{
						conteoConCoordenada.conteo = conteo;
						conteoConCoordenada.columna = x;
					}
					conteo = contarAbajoDerecha(x, y, jugador, tablero);
					if (conteo > conteoConCoordenada.conteo)
					{
						conteoConCoordenada.conteo = conteo;
						conteoConCoordenada.columna = x;
					}
				}
			}
		}
	}
	return conteoConCoordenada;
}
int main()
{
	string nick = solicitar_nick_y_crear_archivos();
	auto configuracion = obtener_configuracion_tablero(nick);
	int x = 10, y = 10;
	auto tablero = inicializarTablero(configuracion);
	imprimir_tablero(tablero);
	int columna = solicitar_columna(configuracion.columnas, tablero);
	tablero = colocar_pieza(columna, tablero, CPU);
	tablero = colocar_pieza(2, tablero, CPU);
	tablero = colocar_pieza(5, tablero, CPU);
	imprimir_tablero(tablero);
	ConteoConColumna mejor = obtener_columna_en_la_que_se_obtiene_mayor_puntaje(CPU, tablero);
	cout << endl
		 << endl
		 << "Conteo de " << mejor.conteo << " con columna " << mejor.columna;
	//imprimir_tablero(tablero);
}