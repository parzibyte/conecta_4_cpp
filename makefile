GPP=g++
MAIN=main.cpp
OUTPUT_FILE=conecta4
INPUT_FILE=entrada
.DEFAULT_GOAL=run

run:
	# Compilando 
	${GPP} ${MAIN} -o ${OUTPUT_FILE}.exe
	# Ejecutando
	./${OUTPUT_FILE} < ${INPUT_FILE}
