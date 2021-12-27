#include <iostream>
#include <fstream>

using namespace std;
const string ARCHIVO_RANKING = "ranking.txt",
			 RESULTADO_EMPATE = "e",
			 RESULTADO_GANA = "g",
			 RESULTADO_PIERDE = "p";
const int COLUMNAS_DEFECTO = 10,
		  FILAS_DEFECTO = 10, JUGADOR = 1, CPU = 2;

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
void escribir_archivo_configuracion(string nick, int columnas, int filas)
{
	ofstream archivo;
	archivo.open(nombre_archivo_configuracion(nick), fstream::out);
	archivo << columnas;
	archivo << endl;
	archivo << filas;
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

void solicitar_nick_y_crear_archivos()
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
		escribir_archivo_configuracion(nick, COLUMNAS_DEFECTO, FILAS_DEFECTO);
	}
}

int main()
{
	solicitar_nick_y_crear_archivos();
}