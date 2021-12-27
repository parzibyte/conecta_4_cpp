#include <iostream>
#include <fstream>

using namespace std;
const string ARCHIVO_RANKING = "ranking.txt",
			 RESULTADO_EMPATE = "e",
			 RESULTADO_GANA = "g",
			 RESULTADO_PIERDE = "p";
const int COLUMNAS_DEFECTO = 10,
		  FILAS_DEFECTO = 10, JUGADOR = 1, CPU = 2;

struct ConfiguracionTablero
{
	int columnas, filas;
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

int main()
{
	string nick = solicitar_nick_y_crear_archivos();
	auto configuracion = obtener_configuracion_tablero(nick);
	cout << "Filas: " << configuracion.filas << " Columnas :" << configuracion.columnas;
	cambiar_configuracion(nick, ConfiguracionTablero{12, 34});
	configuracion = obtener_configuracion_tablero(nick);
	cout << "Después de cambiar\nFilas: " << configuracion.filas << " Columnas :" << configuracion.columnas;
}