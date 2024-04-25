import os


# https://cms-analysis.github.io/CombineHarvester/post-fit-shapes-ws.html

os.system('combine -M FitDiagnostics asymmCards/combined_mutau_2018_60_20.root -m 600 --saveShapes --saveWithUncertainties')

# creates fitDiagnosticsTest.root 
os.system(f'cp asymmCards/combined_mutau_2018_60_20.txt .')
os.system(f'cp asymmCards/combined_mutau_2018_60_20.root .')
os.system(f'PostFitShapesFromWorkspace --datacard combined_mutau_2018_60_20.txt --workspace combined_mutau_2018_60_20.root --fitresult fitDiagnosticsTest.root:fit_s --postfit --output postfitShapes_a1a2_60_20.root')