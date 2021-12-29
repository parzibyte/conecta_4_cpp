#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdlib.h> // rand y RAND_MAX
#include <unistd.h> // getpid

using namespace std;

struct JugadorParaRanking
{
	string nombre;
	double puntuacion, movimientos;
};

void imprimir(vector<JugadorParaRanking> jugadores)
{
	int i;
	for (i = 0; i < jugadores.size(); i++)
	{
		JugadorParaRanking jugadorActual = jugadores[i];
		cout << jugadorActual.nombre << " con puntuacion " << jugadorActual.puntuacion << " y " << jugadorActual.movimientos << " movimientos"
			 << "\n";
	}
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

int main()
{
	vector<JugadorParaRanking> jugadores;
	jugadores.push_back(JugadorParaRanking{"Luis", 10, 5});
	jugadores.push_back(JugadorParaRanking{"Abraham", 15, 4});
	jugadores.push_back(JugadorParaRanking{"Marijo", 50, 60});
	jugadores.push_back(JugadorParaRanking{"Paco", 10, 4});
	jugadores.push_back(JugadorParaRanking{"Charly", 10, 8});
	jugadores.push_back(JugadorParaRanking{"Fer", 20, 3});
	cout << "Antes"
		 << "\n";
	imprimir(jugadores);
	jugadores = ordenar(jugadores);
	cout << "Despues"
		 << "\n";
	imprimir(jugadores);
	return 0;
}