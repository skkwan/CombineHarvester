import os 

doBlindedLimits = False

os.system("ulimit -s unlimited")

# assuming lightest scalar is 15 GeV
allMasses = [  # assuming lightest scalar is 15 GeV, 
             [[40, 15], [60, 15], [80, 15], [100, 15]],
               # next, assuming lightest scalar is 20 GeV
            # [[40, 20], [60, 20], [80, 20], [100, 20]], 
            # #   # lastly, 30 GeV
            #   [[60, 30], [80, 30]]
]

for massList in allMasses:
    for masspoint in massList:
        command=f"hToA1A2 mutau 2018 {masspoint[0]} {masspoint[1]} 3 lowMassSR mediumMassSR highMassSR"
        os.system(command)

    os.system("mv hToA1A2*.txt asymmCards/")

    for masspoint in massList:
        os.system(f'echo "* autoMCStats 0.0" >> asymmCards/hToA1A2_mutau_1_2018_{masspoint[0]}_{masspoint[1]}.txt')
        os.system(f'echo "* autoMCStats 0.0" >> asymmCards/hToA1A2_mutau_2_2018_{masspoint[0]}_{masspoint[1]}.txt')
        os.system(f'echo "* autoMCStats 0.0" >> asymmCards/hToA1A2_mutau_3_2018_{masspoint[0]}_{masspoint[1]}.txt')

        os.system(f'combineCards.py asymmCards/hToA1A2_mutau_1_2018_{masspoint[0]}_{masspoint[1]}.txt asymmCards/hToA1A2_mutau_2_2018_{masspoint[0]}_{masspoint[1]}.txt asymmCards/hToA1A2_mutau_3_2018_{masspoint[0]}_{masspoint[1]}.txt > asymmCards/combined_mutau_2018_{masspoint[0]}_{masspoint[1]}.txt')

        # Convert text to workspace
        os.system(f'text2workspace.py "asymmCards/combined_mutau_2018_{masspoint[0]}_{masspoint[1]}.txt" -m {masspoint[0]}')

    os.system("mv *.root asymmCards/")

    for masspoint in massList:
        if (doBlindedLimits):  # -t -1 option
            combineCommand = f'combine -M AsymptoticLimits asymmCards/combined_mutau_2018_{masspoint[0]}_{masspoint[1]}.root -t -1 -m {masspoint[0]} | tee asymmCards/limits_{masspoint[0]}_{masspoint[1]}'
        else: 
            combineCommand = f'combine -M AsymptoticLimits asymmCards/combined_mutau_2018_{masspoint[0]}_{masspoint[1]}.root -m {masspoint[0]} | tee asymmCards/limits_{masspoint[0]}_{masspoint[1]}'
        # this makes a file called higgsCombineTest.AsymptoticLimits.mH60.root
        os.system(combineCommand)

    os.system("mv higgsCombineTest.AsymptoticLimits*.root asymmCards/")

    # Each entry in massList assumes the same m_1. Get this number. 
    thisM1Point = massList[0][1]
    # Next we need to hadd the corresponding mass points but in ascending order. 
    filesToHadd = ""
    for masspoint in massList:
        filesToHadd += f"asymmCards/higgsCombineTest.AsymptoticLimits.mH{masspoint[0]}.root "
    print(filesToHadd)
    os.system(f'hadd -f -j -k asymmCards/higgsCombine_a1a2_m1_{thisM1Point}.root {filesToHadd}')


    print(filesToHadd)
    print(thisM1Point)
print(">>> makeCards.py: Next step: copy asymmCards/higgsCombine_a1a2_m1_*.root to LUNA limits/ folder: /afs/cern.ch/work/s/skkwan/public/hToA1A2/CMSSW_13_2_6_patch2/src/lunaFramework/limits/")

