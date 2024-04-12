import os 

os.system("ulimit -s unlimited")

command="haabbtt mutau 2018 3 lowMassSR mediumMassSR highMassSR"

os.system(command)


os.system("mv haabbtt*.txt cards/")

masspoints = [15, 20, 25, 30, 35, 40, 45, 50, 55, 60]
# masspoints = [60]
for masspoint in masspoints:
    os.system('echo "* autoMCStats 0.0" >> cards/haabbtt_mutau_1_2018_{}.txt'.format(masspoint))
    os.system('echo "* autoMCStats 0.0" >> cards/haabbtt_mutau_2_2018_{}.txt'.format(masspoint))
    os.system('echo "* autoMCStats 0.0" >> cards/haabbtt_mutau_3_2018_{}.txt'.format(masspoint))

    os.system('combineCards.py cards/haabbtt_mutau_1_2018_{}.txt cards/haabbtt_mutau_2_2018_{}.txt cards/haabbtt_mutau_3_2018_{}.txt > cards/combined_mutau_2018_{}.txt'.format(masspoint, masspoint, masspoint, masspoint))
    
    os.system("mv *.root cards/")
    os.system('combine -M AsymptoticLimits cards/combined_mutau_2018_{}.txt -t -1 -m {} | tee cards/limits_{}'.format(masspoint, masspoint, masspoint))

os.system("mv higgsCombineTest.AsymptoticLimits*.root cards/")
os.system('hadd -f -j -k cards/higgsCombine_aa_bbtt.root cards/higgsCombineTest.AsymptoticLimits.mH*.root')

print(">>> makeCards.py: Next step: copy cards/higgsCombine_aa_bbtt.root to LUNA limits/ folder: /afs/cern.ch/work/s/skkwan/public/hToA1A2/CMSSW_13_2_6_patch2/src/lunaFramework/limits/")