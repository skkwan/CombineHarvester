import os


# https://cms-analysis.github.io/CombineHarvester/post-fit-shapes-ws.html

masspoints = [[80, 20]]

channel = "mutau"

for masspair in masspoints:
    m1 = masspair[0]
    m2 = masspair[1]

    os.system(f'combine -M FitDiagnostics asymmCards/combined_{channel}_2018_{m1}_{m2}.root -m {m1} --saveShapes --saveWithUncertainties')

    # creates fitDiagnosticsTest.root 
    os.system(f'cp asymmCards/combined_{channel}_2018_{m1}_{m2}.txt .')
    os.system(f'cp asymmCards/combined_{channel}_2018_{m1}_{m2}.root .')
    os.system(f'PostFitShapesFromWorkspace --datacard combined_{channel}_2018_{m1}_{m2}.txt --workspace combined_{channel}_2018_{m1}_{m2}.root --fitresult fitDiagnosticsTest.root:fit_s --postfit --output postfitShapes_a1a2_{m1}_{m2}.root')
