import os

# -t -1 

# https://github.com/hftsoi/haabbtt_combine/blob/main/compute_impacts.sh

# os.system('text2workspace.py "asymmCards/combined_mutau_2018_60_20.txt" -m 60')

# Unblinded impacts 

# for value in [0, 1]:
#     os.system(f'combineTool.py -M Impacts -d asymmCards/combined_mutau_2018_60_20.root -m 60 --doInitialFit --robustFit 1 --expectSignal {value} --rMin -40 --rMax 40')

#     os.system(f'combineTool.py -M Impacts -d asymmCards/combined_mutau_2018_60_20.root -m 60 --robustFit 1 --doFits --parallel 150 --expectSignal {value} --rMin -40 --rMax 40')

#     os.system(f'combineTool.py -M Impacts -d asymmCards/combined_mutau_2018_60_20.root -m 60 -o impacts_expectSignal{value}.json')

#     os.system(f'plotImpacts.py -i impacts_expectSignal{value}.json -o impacts_60_20_expectSignal{value}')

#     os.system(f'cp impacts_60_20_expectSignal{value}.pdf /eos/user/s/skkwan/www/limits/')


# Blinded impacts
for value in [0, 1]:
    os.system(f'combineTool.py -M Impacts -d asymmCards/combined_mutau_2018_60_20.root -m 60 --doInitialFit --robustFit 1 -t -1 --expectSignal {value} --rMin -40 --rMax 40')

    os.system(f'combineTool.py -M Impacts -d asymmCards/combined_mutau_2018_60_20.root -m 60 --robustFit 1 --doFits --parallel 150 -t -1 --expectSignal {value} --rMin -40 --rMax 40')

    os.system(f'combineTool.py -M Impacts -d asymmCards/combined_mutau_2018_60_20.root -m 60 -o impacts_blinded_expectSignal{value}.json')

    os.system(f'plotImpacts.py -i impacts_blinded_expectSignal{value}.json -o impacts_blinded_60_20_expectSignal{value}')

    os.system(f'cp impacts_blinded_60_20_expectSignal{value}.pdf /eos/user/s/skkwan/www/limits/')