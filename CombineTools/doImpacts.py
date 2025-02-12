import os

channels=["mutau", "etau", "emu"]
year="2018"
signalType="2b2t"


# https://github.com/hftsoi/haabbtt_combine/blob/main/compute_impacts.sh

# os.system('text2workspace.py "asymmCards/combined_mutau_{year}_60_20.txt" -m 60')

outputDir=f"/eos/user/s/skkwan/www/limits_{year}_{channel}/"

allMasses = {
    "4b2t": [   
            #     # assuming lightest scalar is 15 GeV, 
            #     [[30, 15], [40, 15], [50, 15], [60, 15], [70, 15], [80, 15], [90, 15], [100, 15]],
            #     # next, assuming lightest scalar is 20 GeV
            #     [[40, 20], [50, 20], [60, 20], [70, 20], [80, 20], [90, 20], [100, 20]], 
            #     #   # lastly, 30 GeV
            #     [[60, 30], [70, 30], [80, 30], [90, 30]]
             ],
    "2b2t": [
                [[40, 30]]
                # [[20, 15], [30, 15]],
                # [[30, 20], [40, 20]],
                #[[40, 30], # [50, 30], [60, 30]],
                # [[50, 40], [60, 40], [70, 40], [80, 40]],
                # [[60, 50], [70, 50]],
    ]
}

# # Unblinded impacts 

# for value in [0, 1]:
#     os.system(f'combineTool.py -M Impacts -d asymmCards/combined_{channel}_{year}_60_20.root -m 60 --doInitialFit --robustFit 1 --expectSignal {value} --rMin -40 --rMax 40')

#     os.system(f'combineTool.py -M Impacts -d asymmCards/combined_{channel}_{year}_60_20.root -m 60 --robustFit 1 --doFits --parallel 150 --expectSignal {value} --rMin -40 --rMax 40')

#     os.system(f'combineTool.py -M Impacts -d asymmCards/combined_{channel}_{year}_60_20.root -m 60 -o impacts_expectSignal{value}.json')

#     os.system(f'plotImpacts.py -i impacts_expectSignal{value}.json -o impacts_60_20_expectSignal{value}')

#     os.system(f'cp impacts_60_20_expectSignal{value}.pdf /eos/user/s/skkwan/www/limits/')


# Blinded impacts
for channel in channels:
    for massList in allMasses[signalType]:
        for masspoint in massList:
            for value in [0, 1]:
                os.system(f'combineTool.py -M Impacts -d asymmCards/combined_{channel}_{year}_{masspoint[0]}_{masspoint[1]}.root -m {masspoint[0]} --doInitialFit --robustFit 1 -t -1 --expectSignal {value} --rMin -40 --rMax 40')

                os.system(f'combineTool.py -M Impacts -d asymmCards/combined_{channel}_{year}_{masspoint[0]}_{masspoint[1]}.root -m {masspoint[0]} --robustFit 1 --doFits --parallel 150 -t -1 --expectSignal {value} --rMin -40 --rMax 40')

                os.system(f'combineTool.py -M Impacts -d asymmCards/combined_{channel}_{year}_{masspoint[0]}_{masspoint[1]}.root -m {masspoint[0]} -o impacts_blinded_expectSignal{value}.json')

                os.system(f'plotImpacts.py -i impacts_blinded_expectSignal{value}.json -o impacts_blinded_{masspoint[0]}_{masspoint[1]}_expectSignal{value}')

                os.system(f'cp impacts_blinded_{masspoint[0]}_{masspoint[1]}_expectSignal{value}.pdf {outputDir}')
