#include <iostream>

using namespace std;
const string ARCHIVO_RANKING = "ranking.txt";

string solicitar_nick()
{
	string nombre;
	cout << "Dame tu nick para comenzar el juego: ";
	cin >> nombre;
	return nombre;
}

int main()
{
	cout << solicitar_nick();
}