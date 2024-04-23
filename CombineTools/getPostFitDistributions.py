import os


# https://cms-analysis.github.io/CombineHarvester/post-fit-shapes-ws.html

os.system('combine -M FitDiagnostics asymmCards/combined_mutau_2018_100_15.root -m 100 --saveShapes --saveWithUncertainties')

# creates fitDiagnosticsTest.root 

os.system(f'PostFitShapesFromWorkspace --workspace asymmCards/combined_mutau_2018_100_15.root --fitresult fitDiagnosticsTest.root:fit_s --postfit --output asymmCards/postfitShapes_a1a2_100_15.root')