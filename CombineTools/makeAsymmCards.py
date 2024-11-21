import os 

doBlindedLimits = True

os.system("ulimit -s unlimited")

channel = "mutau"
year = "2018"
# either 4b2t (cascade) or 2b2t (non-cascade). Must match the name of the signal histograms in the datacard
signalType = "2b2t" 

# assuming lightest scalar is 15 GeV
allMasses = {
    "4b2t": [   
                # assuming lightest scalar is 15 GeV, 
                [[30, 15], [40, 15], [50, 15], [60, 15], [70, 15], [80, 15], [90, 15], [100, 15]],
                # next, assuming lightest scalar is 20 GeV
                [[40, 20], [50, 20], [60, 20], [70, 20], [80, 20], [90, 20], [100, 20]], 
                #   # lastly, 30 GeV
                [[60, 30], [70, 30], [80, 30], [90, 30]]
            ],
    "2b2t": [
                [[20, 15], [30, 15]],
                [[30, 20], [40, 20]],
                [[40, 30], [50, 30], [60, 30]],
                [[50, 40], [60, 40], [70, 40], [80, 40]],
                [[60, 50], [70, 50]],
    ]
}


for massList in allMasses[signalType]:
    for masspoint in massList:
        command=f"hToA1A2 {channel} {signalType} {year} {masspoint[0]} {masspoint[1]} 3 lowMassSR mediumMassSR highMassSR"
        os.system(command)

    os.system("mv hToA1A2*.txt asymmCards/")
    os.system("mv hAsymm*.root asymmCards/")

    for masspoint in massList:
        os.system(f'echo "* autoMCStats 0.0" >> asymmCards/hToA1A2_{channel}_1_{year}_{masspoint[0]}_{masspoint[1]}.txt')
        os.system(f'echo "* autoMCStats 0.0" >> asymmCards/hToA1A2_{channel}_2_{year}_{masspoint[0]}_{masspoint[1]}.txt')
        os.system(f'echo "* autoMCStats 0.0" >> asymmCards/hToA1A2_{channel}_3_{year}_{masspoint[0]}_{masspoint[1]}.txt')

        os.system(f'combineCards.py asymmCards/hToA1A2_{channel}_1_{year}_{masspoint[0]}_{masspoint[1]}.txt asymmCards/hToA1A2_{channel}_2_{year}_{masspoint[0]}_{masspoint[1]}.txt asymmCards/hToA1A2_{channel}_3_{year}_{masspoint[0]}_{masspoint[1]}.txt > asymmCards/combined_{channel}_{year}_{masspoint[0]}_{masspoint[1]}.txt')

        # Convert text to workspace
        os.system(f'text2workspace.py "asymmCards/combined_{channel}_{year}_{masspoint[0]}_{masspoint[1]}.txt" -m {masspoint[0]}')


    for masspoint in massList:
        if (doBlindedLimits):  # -t -1 option
            combineCommand = f'combine -M AsymptoticLimits asymmCards/combined_{channel}_{year}_{masspoint[0]}_{masspoint[1]}.root --rMin=-10 --run blind -t -1 -m {masspoint[0]} | tee asymmCards/limits_{masspoint[0]}_{masspoint[1]}'
        else: 
            combineCommand = f'combine -M AsymptoticLimits asymmCards/combined_{channel}_{year}_{masspoint[0]}_{masspoint[1]}.root -m {masspoint[0]} | tee asymmCards/limits_{masspoint[0]}_{masspoint[1]}'
        # this makes a file called higgsCombineTest.AsymptoticLimits.mH60.root
        os.system(combineCommand)

    os.system("rm asymmCards/higgsCombineTest.AsymptoticLimits*.root")
    os.system("mv higgsCombineTest.AsymptoticLimits*.root asymmCards/")

    # Each entry in massList assumes the same m_1. Get this number. 
    thisM1Point = massList[0][1]
    # Next we need to hadd the corresponding mass points but in ascending order. 
    filesToHadd = ""
    for masspoint in massList:
        filesToHadd += f"asymmCards/higgsCombineTest.AsymptoticLimits.mH{masspoint[0]}.root "
    print(filesToHadd)
    os.system(f'hadd -f -j -k asymmCards/higgsCombine_a1a2_{year}_{channel}_m1_{thisM1Point}.root {filesToHadd}')

    print("Next step: Make limit plots")
