sizes = ['256', '512', '1024', '2048']
procs = ['1', '2', '4', '8', '16']

for proc in procs:
    for size in sizes:
        with open(f'slurm_script/{size}_{proc}.slurm', 'w') as f:
            f.write(f'#!/bin/bash\n')
            f.write(f'\n')
            f.write(f'#SBATCH --job-name mmult\n')
            f.write(f'#SBATCH --output {size}/plain_{proc}_%j.out\n')
            f.write(f'#SBATCH --error {size}/plain_{proc}_%j.err\n')
            f.write(f'#SBATCH --mail-user leonardo.gemin@gmail.com\n')
            f.write(f'#SBATCH --mail-type ALL\n')
            f.write(f'#SBATCH --time 10:00\n')
            f.write(f'#SBATCH --ntasks {proc}\n')
            f.write(f'#SBATCH --partition allgroups\n')
            f.write(f'#SBATCH --mem 200M\n')
            f.write(f'\n')
            f.write(f'\n')
            f.write(f'mpicc -o mmult -O3 mmult.c\n')
            f.write(f'\n')
            f.write(f'for ((i = 0; i < 10; i++)) do\n')
            f.write(f'    mpirun -np {proc} ./mmult {size}\n')
            f.write(f'done\n')


for size in sizes:
    with open(f'slurm_script/{size}.sh', 'w') as f:
        for proc in procs:
            f.write(f'sbatch {size}_{proc}.slurm\n')