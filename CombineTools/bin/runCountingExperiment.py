import os 
from masses import masslist

print(masslist)
# masses = [
#     [700, 1], 
#     [500, 300],
#     [600, 300]
#     ]

inFile="/eos/cms/store/group/phys_susy/skkwan/condorHistogramming/2025-07-18-00h00m-2018-all-signals-dataMC/out_combined_channels.root"
cardDir="/eos/cms/store/group/phys_susy/skkwan/zhmet-cards/counting-experiment"

os.system("rm *.txt")

for [m1, m2] in masslist:
    os.system(f"countingExperiment-ZHMET {inFile} 2018 {m1} {m2}")
    os.system(f"mv zhmet*.txt {cardDir}")
    print(f">>> Doing per-bin significance for mass points {m1} and {m2}...")
    os.system(f"combine -M Significance {cardDir}/zhmet_ll_1_2018_TChiZH_{m1}_{m2}.txt -t -1 --expectSignal=1")
    os.system(f"combine -M Significance {cardDir}/zhmet_ll_2_2018_TChiZH_{m1}_{m2}.txt -t -1 --expectSignal=1")
    # os.system(f"combine -M Significance {cardDir}/zhmet_ll_3_2018_TChiZH_{m1}_{m2}.txt -t -1 --expectSignal=1")
    # os.system(f"combine -M Significance {cardDir}/zhmet_ll_4_2018_TChiZH_{m1}_{m2}.txt -t -1 --expectSignal=1")

    print(f">>> Doing combined significance for mass points {m1} and {m2}...")
    combinedcardname = f"twoSRs_{m1}_{m2}"
    os.system(f"combineCards.py SR1={cardDir}/zhmet_ll_1_2018_TChiZH_{m1}_{m2}.txt SR2={cardDir}/zhmet_ll_2_2018_TChiZH_{m1}_{m2}.txt > {cardDir}/{combinedcardname}.txt")
    os.system(f"mv *.input.root {cardDir}")
    # SR3=zhmet_ll_3_2018_TChiZH_{m1}_{m2}.txt SR4=zhmet_ll_4_2018_TChiZH_{m1}_{m2}.txt 
    os.system(f"combine -M Significance {cardDir}/{combinedcardname}.txt -t -1 --expectSignal=1")
    # Move this one to a unique name in the cardDir
    os.system(f"mv higgsCombineTest.Significance.mH120.root {cardDir}/higgsCombineTest.Significance.TChiZH_{m1}_{m2}.root")

print(f"Check {cardDir} for files with significance")
