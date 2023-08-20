import os



path = 'slurm_script'
if not os.path.exists(path):
    os.makedirs(path)



sizes = ['256', '512', '1024', '2048']

for size in sizes:
    with open(f'{path}/{size}_seq.slurm', 'w') as f:
        f.write(f'#!/bin/bash\n')
        f.write(f'\n')
        f.write(f'#SBATCH --job-name mmult\n')
        f.write(f'#SBATCH --output {size}/plain_seq_%j.out\n')
        f.write(f'#SBATCH --error {size}/plain_seq_%j.err\n')
        f.write(f'#SBATCH --mail-user massimiliano.viola@studenti.unipd.it\n')
        f.write(f'#SBATCH --mail-type ALL\n')
        f.write(f'#SBATCH --time 10:00\n')
        f.write(f'#SBATCH --ntasks 1\n')
        f.write(f'#SBATCH --partition allgroups\n')
        f.write(f'#SBATCH --mem 200M\n')
        f.write(f'\n')
        f.write(f'\n')
        f.write(f'gcc -o sequential -O3 sequential.c\n')
        f.write(f'\n')
        f.write(f'for ((i = 0; i < 10; i++)) do\n')
        f.write(f'    ./sequential {size}\n')
        f.write(f'done\n')