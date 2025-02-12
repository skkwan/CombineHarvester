# Usage:
#   First make sure the right channels, year, and signalType are defined at the top of the file
#   Also note the doBlindedLimits flag
#   python3 makeAsymmCards.py 
#
#

import os
import re 

doBlindedLimits = True
channels = ["mutau"] # or ["mutau", "etau", "emu"] for multiple
year = "2018"
# either "4b2t" (cascade) or "2b2t" (non-cascade). Must match the name of the signal histograms in the datacard
signalType = "4b2t" 

# assuming lightest scalar is 15 GeV
allMasses = {
    "4b2t": [   
                # assuming lightest scalar is 15 GeV, 
                [[30, 15] , [40, 15], [50, 15], [60, 15], [70, 15], [80, 15], [90, 15], [100, 15], [110, 15]],
                # next, assuming lightest scalar is 20 GeV
                [[40, 20], [50, 20], [60, 20], [70, 20], [80, 20], [90, 20], [100, 20]], 
                # lastly, 30 GeV
                [[60, 30], [70, 30], [80, 30], [90, 30]],
            ],
    "2b2t": [
                [[20, 15], [30, 15]],
                [[30, 20], [40, 20]],
                [[40, 30], [50, 30], [60, 30]],
                [[50, 40], [60, 40], [70, 40], [80, 40]],
                [[60, 50], [70, 50]],
    ]
}

# Need this to avoid running out of system resources
os.system("ulimit -s unlimited")

for channel in channels:
    for massList in allMasses[signalType]:
        for masspoint in massList:
            command=f"hToA1A2 {channel} {signalType} {year} {masspoint[0]} {masspoint[1]} 3 lowMassSR mediumMassSR highMassSR"
            os.system(command)

        os.system("mv hToA1A2*.txt asymmCards/")
        os.system("mv hAsymm*.root asymmCards/")

        for masspoint in massList:
            ch1_filename   = f"asymmCards/hToA1A2_{channel}_1_{year}_{masspoint[0]}_{masspoint[1]}.txt"
            ch2_filename   = f"asymmCards/hToA1A2_{channel}_2_{year}_{masspoint[0]}_{masspoint[1]}.txt"
            ch3_filename   = f"asymmCards/hToA1A2_{channel}_3_{year}_{masspoint[0]}_{masspoint[1]}.txt"
            allch_filename = f"asymmCards/hToA1A2_{channel}_allcats_{year}_{masspoint[0]}_{masspoint[1]}.txt"
            os.system(f'echo "* autoMCStats 0.0" >> {ch1_filename}')
            os.system(f'echo "* autoMCStats 0.0" >> {ch2_filename}')
            os.system(f'echo "* autoMCStats 0.0" >> {ch3_filename}')

            os.system(f'combineCards.py {ch1_filename} {ch2_filename} {ch3_filename} > {allch_filename}')

            # Convert text file to workspace
            os.system(f'text2workspace.py "{allch_filename}" -m {masspoint[0]}')

#         for masspoint in massList:
#             allch_workspace_rootfile = f"asymmCards/hToA1A2_{channel}_allcats_{year}_{masspoint[0]}_{masspoint[1]}.root"
#             logfile = f"asymmCards/limits_allcats_{year}_{channel}_{masspoint[0]}_{masspoint[1]}"
#             if (doBlindedLimits):  # -t -1 option
#                 combineCommand = f'combine -M AsymptoticLimits {allch_workspace_rootfile} --rMin=-10 -t -1 -m {masspoint[0]} | tee {logfile}'
#             else: 
#                 combineCommand = f'combine -M AsymptoticLimits {allch_workspace_rootfile} -m {masspoint[0]} | tee {logfile}'
#             os.system(combineCommand)

#             # this produces a file called higgsCombineTest.AsymptoticLimits.mH30.root, higgsCombineTest.AsymptoticLimits.mH40.root in the top-level directory for (30, 15)
#             oldName = f'higgsCombineTest.AsymptoticLimits.mH{masspoint[0]}.root'
#             # rename this file to something more descriptive that will not get overwritten, and move it to asymmCards/
#             newName = f"higgsCombineTest.AsymptoticLimits.{year}_{channel}.m2_{masspoint[0]}.m1_{masspoint[1]}.root"
#             os.system(f"mv {oldName} asymmCards/{newName}")

#         # Remember we are itearting over massList, which is e.g. (40, 15), (50, 15), etc. all with the same m_1 = 15 GeV. Get this m_1, can just get the 0th entry, 1st element
#         thisM1Point = massList[0][1]
#         # Next we need to hadd the corresponding mass points but in ascending order. 
#         filesToHadd = ""
#         for masspoint in massList:
#             filesToHadd += f"asymmCards/higgsCombineTest.AsymptoticLimits.{year}_{channel}.m2_{masspoint[0]}.m1_{masspoint[1]}.root"
#             filesToHadd += " "
#         print(filesToHadd)
#         # add the signalType to the file name, to distinguish the cascade and non-cascade ones
#         os.system(f'hadd -f -j -k asymmCards/higgsCombine_a1a2_{signalType}_{year}_{channel}_m1_{thisM1Point}.root {filesToHadd}')

# # Get user name and first letter
# user=os.environ["USER"]
# r = re.compile("^(.)")
# m = r.match(user)
# firstletter = m.group()

# suggested_command = f"cp asymmCards/higgsCombine_a1a2_{signalType}_{year}_*.root /afs/cern.ch/work/{firstletter}/{user}/public/hToA1A2/CMSSW_13_2_6_patch2/src/lunaFramework/limits/"
# print(f"Next step: Make limit plots. Copy the output files to the LUNA framework plotting directory subfolder: e.g. {suggested_command}")
