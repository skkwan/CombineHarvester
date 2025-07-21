import os 

masses = [
    # [700, 1], 
    # [500, 300],
    [600, 300]
    ]

inFile="/eos/cms/store/group/phys_susy/skkwan/condorHistogramming/2025-07-17-22h05m-2018-three-signals-fixed-yield/out_combined_channels.root"
cardDir="/afs/cern.ch/work/s/skkwan/public/zhmet/CMSSW_14_0_21/src/luna-zhmet/cards/counting-experiment"

os.system("rm *.txt")

for [m1, m2] in masses:
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