import os

# os.system('text2workspace.py "asymmCards/combined_mutau_2018_60_20.txt" -m 60')

os.system("combine asymmCards/combined_mutau_2018_60_20.root -M MultiDimFit --saveWorkspace -n htt.postfit -m 60")

os.system("combine higgsCombinehtt.postfit.MultiDimFit.mH60.root -M MultiDimFit -n htt.total --algo grid --snapshotName MultiDimFit --setParameterRanges r=0,4")

# # CMS_JER_2018
# os.system("combine higgsCombinehtt.postfit.MultiDimFit.mH60.root -M MultiDimFit --algo grid --snapshotName MultiDimFit --setParameterRanges r=0,4  --freezeNuisanceGroups others -n htt.freeze_all_except_CMS_tauES_dm1_2018")

# combine higgsCombinehtt.postfit.MultiDimFit.mH120.root -M MultiDimFit -n htt.total --algo grid --snapshotName MultiDimFit --setParameterRanges r=0,4
