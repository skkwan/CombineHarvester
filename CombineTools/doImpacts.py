import os

# -t -1 

# https://github.com/hftsoi/haabbtt_combine/blob/main/compute_impacts.sh

# os.system('text2workspace.py "asymmCards/combined_mutau_2018_60_20.txt" -m 60')

os.system('combineTool.py -M Impacts -d asymmCards/combined_mutau_2018_60_20.root -m 60 --doInitialFit --robustFit 1 --expectSignal=0 --rMin=-40 --rMax=40')

os.system('combineTool.py -M Impacts -d asymmCards/combined_mutau_2018_60_20.root -m 60 --robustFit 1 --doFits --parallel=150 --expectSignal=0 --rMin=-40 --rMax=40')

os.system('combineTool.py -M Impacts -d asymmCards/combined_mutau_2018_60_20.root -m 60 -o impacts.json')

os.system('plotImpacts.py -i impacts.json -o impacts_60_20')

os.system('cp impacts_60_20.pdf /eos/user/s/skkwan/www/limits/')