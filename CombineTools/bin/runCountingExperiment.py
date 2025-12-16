import os 
from masses import masslist

# print(masslist)
# masslist = [
#     [650, 500]
#     ]

# Always change the following two
nSignalRegions = 3
descriptor = "met3bins"
foldername = "met3bins-asymptoticLimits"

inFile="/eos/cms/store/group/phys_susy/skkwan/condorHistogramming/2025-12-10-22h37m-2018-dataMC-decPresentation-SR123-all-points/out_datacard_combined_channels.root"
cardDir=f"/eos/cms/store/group/phys_susy/skkwan/zhmet-cards/counting-experiment/presentation_2025_12/{foldername}"

os.system("rm *.txt")
os.system(f"mkdir -p {cardDir}")

for [m1, m2] in masslist:
    os.system(f"countingExperiment-ZHMET {inFile} 2018 {m1} {m2}")
    os.system(f"mv zhmet*.txt {cardDir}/")
    print(f">>> Doing per-bin significance for mass points {m1} and {m2}...")
    
    for iRegion in range(1, nSignalRegions+1):
        os.system(f"combine -M AsymptoticLimits {cardDir}/zhmet_m_ll_{iRegion}_2018_TChiZH_{m1}_{m2}.txt -t -1 --expectSignal=1")

    if nSignalRegions > 1:
        print(f">>> More than one signal region: Doing combined significance for mass points {m1} and {m2}...")
        combinedcardname = f"{descriptor}_{m1}_{m2}"
        ## EXAMPLE COMMAND TO BUILD: combineCards.py SR1={cardDir}/zhmet_ll_1_2018_TChiZH_{m1}_{m2}.txt SR2={cardDir}/zhmet_ll_2_2018_TChiZH_{m1}_{m2}.txt > {cardDir}/{combinedcardname}.txt")
        stringForCombination = ""
        for iRegion in range(1, nSignalRegions + 1):
            stringForCombination += f" SR{iRegion}={cardDir}/zhmet_m_ll_{iRegion}_2018_TChiZH_{m1}_{m2}.txt"
        stringForCombination += " " 
        print(stringForCombination)
        os.system(f"combineCards.py {stringForCombination} > {cardDir}/{combinedcardname}.txt")
        os.system(f"mv *.input.root {cardDir}")
        # os.system(f"combine -M Significance {cardDir}/{combinedcardname}.txt -t -1 --expectSignal=1")
        os.system(f"combine -M AsymptoticLimits {cardDir}/{combinedcardname}.txt -t -1")

    # Always move the locally produced .root to a unique name in the cardDir
    # os.system(f"mv higgsCombineTest.Significance.mH120.root {cardDir}/higgsCombineTest.Significance.TChiZH_{m1}_{m2}.root")
    os.system(f"mv higgsCombineTest.AsymptoticLimits.mH120.root {cardDir}/higgsCombineTest.AsymptoticLimits.TChiZH_{m1}_{m2}.root")

print(f"Check {cardDir} for files with significance")
