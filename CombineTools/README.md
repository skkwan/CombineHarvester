# haabbtt Combine

## Setup (do only once)

You need two folders: `HiggsAnalysis/CombinedLimit` which is the Combine tool itself (a command-line interface to many different statistical techniques, available inside RooFits/Roostats), and `CombineHarvester`, which is a framework for the production and analysis of datacards for use with the CMS Combine tool.

1. Get the Combine tool. Instructions are taken from [HiggsAnalysis/CombinedLimit](http://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/latest/#installation-instructions) documentation: the latest instructions as of April 8, 2024 was this on el9:

```bash
cmssw-el7
Singularity> cmsrel CMSSW_11_3_4
Singularity> cd CMSSW_11_3_4/src
Singularity> cmsenv
Singularity> git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit
Singularity> cd HiggsAnalysis/CombinedLimit
Singularity> cd $CMSSW_BASE/src/HiggsAnalysis/CombinedLimit
Singularity> git fetch origin
Singularity> git checkout v9.2.0
Singularity> scramv1 b clean; scramv1 b # always make a clean build
```

2. Get the CombineHarvester framework. Instructions are taken from [CombineHarvester](http://cms-analysis.github.io/CombineHarvester/)

```bash
Singularity> git clone https://github.com/cms-analysis/CombineHarvester.git CombineHarvester
Singularity> cd CombineHarvester/
Singularity> git checkout v2.0.0
```

Add my own fork as a remote for development:
```bash
Singularity> git remote -v
origin  https://github.com/cms-analysis/CombineHarvester.git (fetch)
origin  https://github.com/cms-analysis/CombineHarvester.git (push)
Singularity> git remote rename origin upstream
Singularity> git remote add origin git@github.com:skkwan/CombineHarvester-haabbtt.git
```

3. (Not applicable to anyone except past me) I ran these steps because I had an older branch from the `lxplus7` days with changes that I wanted to keep, instead of starting from scratch.
I made `v2.0.0` from the upstream its own branch:
```bash
git checkout v2.0.0
git switch -c upstream-v2.0.0
```
And rebased my older development branch on top of `v2.0.0`.
```bash
git checkout devel-thesisEndorsement
git rebase upstream-v2.0.0
```

## Development 
1. Copy the main code `haabbtt.cpp` to `CombineHarvester/CombineTools/bin/haabbtt-full.cpp` and make sure `BuildFile.xml` is modified
   (these files I should have committed to the correct folders in my fork). 

   I called the original example `haabbtt-full.cpp` (compiling to the `haabbtt-full` executable) and my work in progress version 
   `haabbtt.cpp` (compiling to the `haabbtt` executable).

2. Compile after every change, in the `bin/` directory (if we are only changing `haabbtt.cpp` or `haabbtt-full.cpp`):
   `scram b -j 8`

## To run the example
1. Make sure your histogram `.root` files are copied to
   `/afs/cern.ch/work/s/skkwan/public/combineArea/CMSSW_11_3_4/src/auxiliaries/shapes/`.
   The file should have the categories as the top-level folder. The histograms for the variable to fit, should be named like `data_obs`, not `data_obs_m_sv`.

2. For the example from HF with all channels, years, with a root file containing directories 1 2 3 4 are SR1 SR2 SR3 CR finale m(tt) from 1bjet events after DNN cuts; 5 6 7 are SR1 SR2 CR from 2bjets.
   ```bash
   # Go to the CMSSW_10_2_13/src/CombineHarvester/CombineTools working directory:
   /afs/cern.ch/work/s/skkwan/public/combineArea/CMSSW_11_3_4/src/CombineHarvester/CombineTools/src/AABBTT_allyears
   cmsenv
   haabbtt-full mt 2018 7 1 2 3 4 5 6 7
   ```

   For my version, 2018 mu-tau cut-based categories, and using only one mass point 45 GeV:
   ```bash
   cd /afs/cern.ch/work/s/skkwan/public/combineArea/CMSSW_11_3_4/src/CombineHarvester/CombineTools/src/AABBTT_allyears
   cmsenv
   haabbtt mutau 2018 4 lowMassSR mediumMassSR highMassSR highMassCR
   ```

   This makes a bunch of text files.

2. Add statistical uncertainties for the backgrounds: 
   ```bash
   echo "* autoMCStats 0.0" >> haabbtt_mutau_1_2018_45.txt
   echo "* autoMCStats 0.0" >> haabbtt_mutau_1_2018_40.txt
   ```


3. Compute the expected limit (-t -1 means expected, blinded): 
   ```bash
   combine -M AsymptoticLimits haabbtt_mutau_1_2018_40.txt -t -1 -m 40
   combine -M AsymptoticLimits haabbtt_mutau_1_2018_45.txt -t -1 -m 45
   ```

   This gives something like
   ```bash
    -- AsymptoticLimits ( CLs ) --
   Observed Limit: r < 0.0022
   Expected  2.5%: r < 0.0009
   Expected 16.0%: r < 0.0013
   Expected 50.0%: r < 0.0024
   Expected 84.0%: r < 0.0034
   Expected 97.5%: r < 0.0046
   ```

4. If you want to combine several categories, make a combined txt file:
   `combineCards.py haabbtt_mutau_1_2018_45.txt haabbtt_mutau_2_2018_45.txt haabbtt_mutau_3_2018_45.txt > combined_mutau_2018_45.txt` 

   `combine -M AsymptoticLimits combined_mutau_2018_45.txt -t -1 -m 45`

   This gives something like
   ```bash
   -- AsymptoticLimits ( CLs ) --
   Observed Limit: r < 0.0021
   Expected  2.5%: r < 0.0009
   Expected 16.0%: r < 0.0013
   Expected 50.0%: r < 0.0024
   Expected 84.0%: r < 0.0034
   Expected 97.5%: r < 0.0041
   ```

## Troubleshooting

Using [FitDiagnostics](http://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/part3/nonstandard/):

```bash
# Inside the cards/ directory
cd ${CMSSW_BASE}/src/CombineHarvester/CombineTools/cards/
combine -M FitDiagnostics haabbtt_mutau_3_2018_60.txt -m 60

# Up one directory
cd ${CMSSW_BASE}/src/CombineHarvester/CombineTools
combine -M FitDiagnostics cards/combined_mutau_2018_60.txt -t -1 -m 60 
```

Using `diffNuisances.py`:
```bash
python diffNuisances.py fitDiagnosticsTest.root
```

## Datacard validation
TODO:

## Multi-dimensional fit for debugging
```bash
python3 doMultiDimFit.py
```

## Pulls and impacts
```bash
python3 doImpacts.py
```

## Post-fit distributions

```bash
python3 getPostFitDistributions.py
```
makes `postfitShapes_a1a2_100_15.root`. Need to put this into data/MC.

## For thesis endorsement
1. `python3 makeAsymmCards.py` to make limits for limit plots for each set of mass points.
2. `python3 doImpacts.py` for one mass point to get impacts.
3. (Can be done in parallel with the previous step) `python3 getPostFitDistributions.py` for one mass point, to run FitDiagnostics to save shapes and get a RooFitResult, and run PostFitShapesFromWorkspace on the results to get pre-fit and post-fit distributions.
   ```bash
   root -l postfitShapes_a1a2_60_20.root
   root [1] .ls
   TFile**         postfitShapes_a1a2_60_20.root
   TFile*         postfitShapes_a1a2_60_20.root
   KEY: TDirectoryFile   ch1_prefit;1    ch1_prefit
   KEY: TDirectoryFile   ch2_prefit;1    ch2_prefit
   KEY: TDirectoryFile   ch3_prefit;1    ch3_prefit
   KEY: TDirectoryFile   ch1_postfit;1   ch1_postfit
   KEY: TDirectoryFile   ch2_postfit;1   ch2_postfit
   KEY: TDirectoryFile   ch3_postfit;1   ch3_postfit
   ```
4. TODO: Plot the post-fit distributions.