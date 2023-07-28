EXECS=mmult
MPICC?=mpicc
MPIRUN?=mpirun

all: ${EXECS}
	${MPIRUN} -np 3 ./${EXECS}

${EXECS}: ${EXECS}.c
	${MPICC} -o ${EXECS} ${EXECS}.c

clean:
	rm -f ${EXECS}
