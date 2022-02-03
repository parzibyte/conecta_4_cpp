# Tutorial y explicación

En el blog de Parzibyte: https://parzibyte.me/blog/2022/02/03/conecta-4-cpp-ia-ranking-estadisticas-jugadores/

Se trata del conocido juego 'Conecta 4'. Las partidas se jugarán sobre un tablero de a lo sumo 10x10 casillas, en el que tendremos que ir situando piezas alternativamente con el ordenador hasta conectar cuatro de ellas en horizontal, en vertical o en diagonal. Ganará quien primer consiga el objetivo (ordenador o jugador), y se considerará un empate cuando se llene el tablero sin haberlo conseguido ninguno de los dos.

El funcionamiento del juego se apoyará en un fichero ranking.txt para almacenar las puntuaciones de los 10 mejores jugadores, y en tres ficheros para cada jugador, cuyo nombre será el nick del jugador, una barra baja y las siguientes terminaciones:

- configuracion.txt, que almacenará el tamaño del tablero incluyendo el número de casillas (filas y columnas del tablero inicialmente 10 x 10).
- resultados.txt, que contendrá los resultados conseguidos en las partidas.
- ultimapartida.dat, que almacenará los datos necesarios para reproducir la última partida jugada.

## Diseño del juego

El juego comenzará solicitando al usuario su nick

Dame tu nick para comenzar el juego:

A continuación, mostrará por pantalla el siguiente menú:

1. Visualizar configuración del tablero. 
2. Cambiar configuración del tablero.
3. Ver estadísticas.
4. Reproducir última partida.
5. Mostrar top 10.
6. Jugar partida.
0. Salir


Dame opción:


- La opción 0 (Salir) acabará la ejecución del programa.


- La opción 1 (visualizar configuración) mostrará la información referente al tamaño del tablero, leyendo para ello el fichero correspondiente al jugador, de extensión txt y cuyo nombre será nombre la concatenación de su nick con la cadena “_configuración” con su nick. Por ejemplo, para el nick Maria, el fichero será Maria_configuracion.txt


- La opción 2 (cambiar configuración) permitirá modificar el contenido del fichero de configuración correspondiente al jugador, teniendo en cuenta el tamaño máximo del tablero y cambiando su tamaño para las partidas que se jueguen a partir de ese momento.


- La opción 3 (ver estadísticas), se apoyará en la información contenida en el fichero de resultados asociado al nick del jugador, y mostrará el porcentaje de partidas ganadas, perdidas y empatadas; y el número promedio de movimientos del jugador por partida.


- La opción 4 reproducirá los movimientos sobre el tablero de la última partida, de acuerdo a la información almacenada en el fichero de última partida asociado al jugador, que será de tipo binario. Los movimientos se reproducirán uno a uno, pidiendo al usuario que pulse una tecla tras cada movimiento.


- La opción 5 mostrará el ranking de los 10 mejores jugadores. A este fin, se calculará la puntuación de cada jugador como (número de partidas ganadas – números de partidas perdidas) / partidas totales. En caso de empate entre jugadores, se considerará mejor jugador el que tenga el menor número de movimientos por partida.


- La opción 6 iniciará el juego. A este fin, el ordenador mostrará un tablero como el que se muestra debajo (para un caso de ejemplo compuesto de 6 filas y 10 columnas), y solicitará al usuario el número de columna en el que quiere depositar su ficha.



```
| | | | | | | | | | |
| | | | | | | | | | |
| | | | | | | | | | |
| | | | | | | | | | |
| | | | | | | | | | |
| | | | | | | | | | |
| | | | | | | | | | |
| | | | | | | | | | |
| | | | | | | | | | |
| | | | | | | | | | |
+-+-+-+-+-+-+-+-+-+-+
|0|1|2|3|4|5|6|7|8|9|

Seleccione la columna para su tirada:
```



El ordenador introducirá la ficha del jugador en la columna indicada (una X mayúscula) y a continuación introducirá la suya propia (una O mayúscula), diseñando para ello un algoritmo apropiado. El diseño de este algoritmo influirá en la nota del proyecto (1 punto).

A continuación, se muestra un ejemplo de un posible resultado tras pedir depositar la ficha en la columna 3

```
| | | | | | | | | | |
| | | | | | | | | | |
| | | | | | | | | | |
| | | | | | | | | | |
| | | | | | | | | | |
| | | | | | | | | | |
| | | | | | | | | | |
| | | | | | | | | | |
| | | | | | | | | | |
| | | |X| | | | | | |
+-+-+-+-+-+-+-+-+-+-+
|0|1|2|3|4|5|6|7|8|9|

Seleccione la columna para su tirada:
```


El programa deberá continuar solicitando al usuario su tirada hasta que el ordenador o el usuario consigan ganar la partida (o el tablero se llene). En ese momento, se actualizará el fichero terminado en _resultados.txt con la nueva información, para permitir visualizar las estadísticas en la opción 3 en el futuro; y también el fichero terminado en _ultimapartida.dat, para permitir utilizar posteriormente la opción 4 si el usuario desea revisar sus movimientos. Finalmente, se regresará al menú principal.


Se recuerdan algunos aspectos evaluables de importancia:


- Uso apropiado de comentarios (al principio del programa, precediendo cada cabecera de funciones y aclaratorios en el código).
- Seguimiento de guía de estilo, especialmente en cuestiones relativa al sangrado del código.
- Estructuración adecuada, siguiendo una descomposición lógica en funciones que facilite el seguimiento.
