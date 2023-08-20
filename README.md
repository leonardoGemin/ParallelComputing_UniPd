# Parallel Computing 
MSc project experiences at UNIPD.


## Requirements
Install MPI: `sudo apt install libcr-dev mpich mpich-doc`


## Sequential version
Given two square matrices A and B of same size, C is compute as follows:

```
for (i = 0; i < SIZE; i++)
    for (j = 0; j < SIZE; j++)
        for (k = 0; k < SIZE; k++)
            C[i][j] += A[i][k] * B[k][j]
```

* Compile with `gcc -o sequential -O3 sequential.c`
* Run with `./sequential <SIZE> [unlock verbose mode]`


## Parallel version
Given two square matrices `A` and `B` of same size, `C` is compute exploiting parallelization.

There are three variants of optimization:
1. Just the parallelization of the algorithm, called "mono"
2. "mono" + transposition of matrix `B` in order to read both `A` and `B` matrices row-wise order during the inner loop. This leads to less cache misses.
3. "mono" + changing of the order of the loops: from `i-j-k` to `k-i-j`. In this way `A[i][k]` is constant in the inner loop, while `B` and `C` are scanned in row-wise order.

* Compile with `mpicc -o parallel -O3 parallel.c`
* Run with `mpirun -np <NUM-OF-PROCESSORS> ./parallel <SIZE> <VARIANT: 0 | 1 | 2> [unlock verbose mode]`


## How to run on CAPRI server
Prepare a slurm file called `test.slurm` (just for example) like 
```
#!/bin/bash

#SBATCH --job-name mmult
#SBATCH --output 2048_monokij_8.out
#SBATCH --error 2048_monokij_8.err
#SBATCH --mail-user leonardo.gemin@studenti.unipd.it
#SBATCH --mail-type ALL
#SBATCH --time 10:00
#SBATCH --ntasks 8
#SBATCH --partition allgroups
#SBATCH --mem 200M


mpicc -o parallel -O3 parallel.c

for ((i = 0; i < 10; i++)) do
    mpirun -np 8 ./parallel 2048 2
done
```
and submit the job through `sbatch test.slurm`.


## Project report
Click [here](https://github.com/leonardoGemin) to see the report of the project.


## Authors
* [Massimiliano Viola](https://github.com/massimilianoviola)
* [Leonardo Gemin](https://github.com/leonardoGemin)