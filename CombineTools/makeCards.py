import os 


command="haabbtt mutau 2018 4 lowMassSR mediumMassSR highMassSR highMassCR"

os.system(command)

masspoints = [15, 20, 25, 30, 35, 40, 45, 50, 55, 60]
# masspoints = [15]

for masspoint in masspoints:
    os.system('echo "* autoMCStats 0.0" >> haabbtt_mutau_1_2018_{}.txt'.format(masspoint))
    os.system('echo "* autoMCStats 0.0" >> haabbtt_mutau_2_2018_{}.txt'.format(masspoint))
    os.system('echo "* autoMCStats 0.0" >> haabbtt_mutau_3_2018_{}.txt'.format(masspoint))

    os.system('combineCards.py haabbtt_mutau_1_2018_{}.txt haabbtt_mutau_2_2018_{}.txt haabbtt_mutau_3_2018_{}.txt > combined_mutau_2018_{}.txt'.format(masspoint, masspoint, masspoint, masspoint))

    os.system('combine -M AsymptoticLimits combined_mutau_2018_{}.txt -t -1 -m {} | tee limits_{}'.format(masspoint, masspoint, masspoint))
