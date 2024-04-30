import os 

os.system('combine -M GoodnessOfFit asymmCards/combined_mutau_2018_60_20.root --algo saturated -m 60 -n .goodnessOfFit_data --fixedSignalStrength 1')

# Takes a long time! ~30 minutes even, for 500 toys
os.system('combine -M GoodnessOfFit asymmCards/combined_mutau_2018_60_20.root --algo saturated -m 60 -n .goodnessOfFit_toys -t 50 --fixedSignalStrength 1')

# os.system('combineTool.py -M CollectGoodnessOfFit --input higgsCombine.goodnessOfFit_data.GoodnessOfFit.mH60.root higgsCombine.goodnessOfFit_toys.GoodnessOfFit.mH60.123456.root -m 60.0 -o gof.json')

# os.system('plotGof.py gof.json --statistic saturated --mass 60.0 -o m_60_20_gof')

# print('Copy outputs to /eos/user/s/skkwan/www/limits/')