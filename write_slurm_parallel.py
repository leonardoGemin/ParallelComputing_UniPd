import os



path = 'slurm_script'
if not os.path.exists(path):
    os.makedirs(path)



types = ['mono', 'monotransp', 'monokij']
sizes = ['256', '512', '1024', '2048']
procs = ['1', '2', '4', '8', '16']
userMail = "leonardo.gemin@studenti.unipd.it"



i = 0

# i = {0, 1, 2}
#      |  |  +->  mono + k-i-j  -->  (0) + change the order of the three loops from i-j-k to k-i-j.
#      |  +---->  monotransp    -->  (0) + transpose the matrix B to access both A and B row-wise (less cache misses).
#      +------->  mono          -->  Use monodimensional arrays instead of matrices.



for type in types:
    for size in sizes:
        for proc in procs:
            with open(f'{path}/{size}_{type}_{proc}.slurm', 'w') as f:
                f.write(f'#!/bin/bash\n')
                f.write(f'\n')
                f.write(f'#SBATCH --job-name mmult\n')
                f.write(f'#SBATCH --output {size}_{type}_{proc}.out\n')
                f.write(f'#SBATCH --error {size}_{type}_{proc}.err\n')
                f.write(f'#SBATCH --mail-user {userMail}\n')
                f.write(f'#SBATCH --mail-type ALL\n')
                f.write(f'#SBATCH --time 10:00\n')
                f.write(f'#SBATCH --ntasks {proc}\n')
                f.write(f'#SBATCH --partition allgroups\n')
                f.write(f'#SBATCH --mem 200M\n')
                f.write(f'\n')
                f.write(f'\n')
                f.write(f'mpicc -o parallel -O3 parallel.c\n')
                f.write(f'\n')
                f.write(f'for ((i = 0; i < 10; i++)) do\n')
                f.write(f'    mpirun -np {proc} ./parallel {size} {i}\n')
                f.write(f'done\n')
    i += 1