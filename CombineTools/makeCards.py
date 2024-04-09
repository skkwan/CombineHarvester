import os 


command="haabbtt mutau 2018 4 lowMassSR mediumMassSR highMassSR highMassCR"

os.system(command)

os.system('echo "* autoMCStats 0.0" >> haabbtt_mutau_1_2018_45.txt')
os.system('echo "* autoMCStats 0.0" >> haabbtt_mutau_1_2018_40.txt')

os.system('combine -M AsymptoticLimits haabbtt_mutau_1_2018_40.txt -t -1 -m 40')
os.system('combine -M AsymptoticLimits haabbtt_mutau_1_2018_45.txt -t -1 -m 45')
