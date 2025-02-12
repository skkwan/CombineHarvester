# Usage:
#   python3 combineCardsForOneYear.py
#
# Prerequisites: This script depends on all the necessary input files being present in the asymmCards/ directory, 
#                i.e. you need to have run makeAsymmCards.py for each of the three tautau channels to combine.
# 

import os

doBlindedLimits = True

os.system("ulimit -s unlimited")

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
    "2b2t": [ [[20, 15]]
                # [[20, 15], [30, 15]],
                # [[30, 20], [40, 20]],
                # [[40, 30], [50, 30], [60, 30]],
                # [[50, 40], [60, 40], [70, 40], [80, 40]],
                # [[60, 50], [70, 50]],
    ]
}

for massList in allMasses[signalType]:
    for masspoint in massList:
        print(f"Combining .txt cards within the year...")
        # Note the multi-line command, we need to cd into asymmCards/ to run combineCards.py, otherwise the .txt file will try to look for a double-nested folder asymmCards/asymmCards/ which does not exist
        os.system(f'cd asymmCards/; combineCards.py hToA1A2_mutau_allcats_{year}_{masspoint[0]}_{masspoint[1]}.txt  hToA1A2_etau_allcats_{year}_{masspoint[0]}_{masspoint[1]}.txt hToA1A2_emu_allcats_{year}_{masspoint[0]}_{masspoint[1]}.txt > hToA1A2_allchannels_allcats_{year}_{masspoint[0]}_{masspoint[1]}.txt; cd ../')

        # Convert text to workspace
        print(f"Converting .txt to workspace....")
        os.system(f'text2workspace.py asymmCards/hToA1A2_allchannels_allcats_{year}_{masspoint[0]}_{masspoint[1]}.txt -m {masspoint[0]}')

    for masspoint in massList:
        print(f"Creating higgsCombineTest.AsymptoticLimits.*.root....")
        if (doBlindedLimits):  # -t -1 option
            combineCommand = f'combine -M AsymptoticLimits asymmCards/hToA1A2_allchannels_allcats_{year}_{masspoint[0]}_{masspoint[1]}.root --rMin=-10 -t -1 -m {masspoint[0]} | tee asymmCards/limits_allchannels_{masspoint[0]}_{masspoint[1]}'
        else: 
            combineCommand = f'combine -M AsymptoticLimits asymmCards/hToA1A2_allchannels_allcats_{year}_{masspoint[0]}_{masspoint[1]}.root -m {masspoint[0]} | tee asymmCards/limits_allchannels_{masspoint[0]}_{masspoint[1]}'
        # this makes a file called higgsCombineTest.AsymptoticLimits.mH60.root
        os.system(combineCommand)

    os.system("rm asymmCards/higgsCombineTest.AsymptoticLimits*.root")
    os.system("mv higgsCombineTest.AsymptoticLimits*.root asymmCards/")

    # Each entry in massList assumes the same m_1. Get this number. 
    thisM1Point = massList[0][1]
    # Next we need to hadd the corresponding mass points but in ascending order. 
    filesToHadd = ""

    print(f"Hadding all files into one .root...")
    for masspoint in massList:
        filesToHadd += f"asymmCards/higgsCombineTest.AsymptoticLimits.mH{masspoint[0]}.root "
    print(filesToHadd)
    os.system(f'hadd -f -j -k asymmCards/higgsCombine_a1a2_{signalType}_{year}_allchannels_m1_{thisM1Point}.root {filesToHadd}')

    user=os.environ['USER']
    firstletteruser=user[0]

    suggested_command = f"cp asymmCards/higgsCombine_a1a2_{signalType}_{year}_allchannels*.root /afs/cern.ch/work/{firstletteruser}/{user}/public/hToA1A2/CMSSW_13_2_6_patch2/src/lunaFramework/limits/"
    print(f"Next step: Make limit plots. Copy the output files to the LUNA framework plotting directory subfolder: e.g. {suggested_command}")
