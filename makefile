GPP=g++
MAIN=main.cpp
OUTPUT_FILE=conecta4
.DEFAULT_GOAL=run

run:
	# Compilando 
	${GPP} ${MAIN} -o ${OUTPUT_FILE}.exe
	# Ejecutando
	./${OUTPUT_FILE}
