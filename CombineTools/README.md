# haabbtt Combine

This repository runs on the datacards (`out_mutau.root`, `out_etau.root`, `out_emu.root`) of the [LUNA framework](https://gitlab.cern.ch/skkwan/lunaFramework/-/tree/main?ref_type=heads) and contains all steps involving Combine.

Current status as of January 29, 2025:
- Works on 2018 for all three channels
- Currently producing 2017 and 2016 per-channel limits
- Currently producing year and full Run-2 combinations

## Table of Contents:
- [Setup](#setup-do-only-once)
- [To create limits, impacts, and postfit for a single channel](#to-create-limits-impacts-and-postfit-for-one-channel)
- [To combine three channels into one year](#to-combine-three-channels-into-one-year)
- [Troubleshooting](#troubleshooting)

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

## To create limits, impacts, and postfit for one channel
1. The main `C++` executable is in [bin/hToA1A2.cpp](bin/hToA1A2.cpp). Note that [bin/BuildFile.xml](bin/BuildFile.xml) must include the line `<bin file="hToA1A2.cpp" name="hToA1A2"></bin>` to build the executable. The input file directory pointing to `out_mutau.root`/`out_etau.root`/`out_emu.root` is also specified inside [bin/hToA1A2.cpp](bin/hToA1A2.cpp).

   Only for reference, the old HIG-22-007 h->aa->2b2tau executable lives at [bin/haabbtt-full.cpp](bin/haabbtt-full.cpp).

2. After every change, make sure we re-compile the executable. Make sure `cmssw-el7` is activated:
   ```bash
   [skkwan@lxplus909 bin]$ cmssw-el7 
   Singularity> cmsenv
   ```
   Then compile from the top-level directory:
   ```bash
   Singularity> cd ${CMSSW_BASE}/src
   Singularity> scram b -j 8
   ```
3. Make sure the histogram `out_mutau.root`, `out_etau.root`, etc. files are copied to `${CMSSW_BASE}/src/auxiliaries/shapes/`. With the LUNA framework, this file is the one produced from `runBkgEstimation.sh`. 
   The file should have the categories as the top-level folder. The histograms for the variable to fit, should be named like `data_obs`, not `data_obs_m_sv`. For example, this is what one file should look like:

   ```bash
   [skkwan@lxplus909 shapes]$ root -l out_etau.root
   root [0] 
   Attaching file out_etau.root as _file0...
   (TFile *) 0x55f8dca1eef0
   root [1] .ls
   TFile**         out_etau.root
   TFile*         out_etau.root
   KEY: TDirectoryFile   inclusive;1     inclusive
   KEY: TDirectoryFile   lowMassSR;1     lowMassSR
   KEY: TDirectoryFile   mediumMassSR;1  mediumMassSR
   KEY: TDirectoryFile   highMassSR;1    highMassSR
   KEY: TDirectoryFile   highMassCR;1    highMassCR
   ```

4. With the cut-based categories ("lowMassSR", "mediumMassSR", etc.), and looping over mass points:
   ```bash
   Singularity> cd ${CMSSW_BASE}/src/CombineHarvester/CombineTools
   Singularity> python3 makeAsymmCards.py
   ```
   This wrapper [makeAsymmCards.py](makeAsymmCards.py) script does the following steps:
   - Runs the [bin/hToA1A2.cpp](bin/hToA1A2.cpp) executable on the specified mass points
   - Moves the resulting .txt files to the `asymmCards/` folder 
   - Add statistical uncertainties for the backgrounds like this:
      ```bash
      echo "* autoMCStats 0.0" >> haabbtt_mutau_1_2018_45.txt
      echo "* autoMCStats 0.0" >> haabbtt_mutau_1_2018_40.txt
      ```
   - Runs `combineCards.py` to combine the `.txt` cards from the three signal region channels, into one .txt file.
   - Runs  `text2workspace.py` to convert the `.txt` cards to a `RooWorkSpace`.
   - Runs `combine -M AsymptoticLimits`, either blinded or unblinded. 
      This prints something like
      ```bash
      -- AsymptoticLimits ( CLs ) --
      Observed Limit: r < 0.0022
      Expected  2.5%: r < 0.0009
      Expected 16.0%: r < 0.0013
      Expected 50.0%: r < 0.0024
      Expected 84.0%: r < 0.0034
      Expected 97.5%: r < 0.0046
      ```
   - Moves the resulting `higgsCombineTest.AsymptoticLimits*.root` files to `asymmCards/` again.
   - Runs `hadd` to combine all the mass points into one `RooWorkSpace`. 
5. Next we create limit plots in the LUNA framework.
   ```bash
   cp asymmCards/higgsCombine_a1a2*.root <path to your area>
   # e.g. cp asymmCards/higgsCombine_a1a2*.root /afs/cern.ch/work/s/skkwan/public/hToA1A2/CMSSW_13_2_6_patch2/src/lunaFramework/limits
   # then cd to LUNA, use a regular CMSSW area (not cmssw el7, also you don't need the venv activated), and follow instructions there
   ```
6. Get impacts (only one example mass point necessary):
   ```bash
   python3 doImpacts.py
   ```
7. (Can be done in parallel with the previous step) Get the post-fit distributions for one mass point: run FitDiagnostics to save shapes and get a RooFitResult, and run PostFitShapesFromWorkspace on the results to get pre-fit and post-fit distributions.
   ```bash
   # edit channel variable in this file
   python3 getPostFitDistributions.py
   ```

   The resulting file should look like this:
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
8. Copy the post-fit distributions to your working area:
  ```bash
   cp postfitShapes_a1a2_60_20.root <your working area>
   # e.g. working area is /eos/user/s/skkwan/www/limits
   ```
   And in the LUNA repository in `dataMCPlots/` run the post-fit plots: 
   ```bash
   # cd to LUNA repository dataMCPlots/
   python3 runPostFitPlots.py
   ```
9. Make the goodness of fit data, toys, and plot:
   ```bash
   python3 doGoodnessOfFit.py
   ```

## To combine three channels into one year

1. First you need to have run the previous step for all three tautau channels and all the files should still be in `asymmCards/`.
2. Next, run [combineCardsForOneYear.py](combineCardsForOneYear.py)
   ```bash
   python3 combineCardsForOneYear.py 
   ```
3. To plot the limits, follow the prompts and copy the resulting files to your plotting area (in LUNA).

## Troubleshooting

- In general if Combine seems to not be doing anything, check the print statements for any errors or missing files.
- The code pipes the expected and measured limits to `asymmCards/limit_*` files, so make sure they contain something like
   ```bash
   % cat limits_allchannels_100_15
       <<< Combine >>> 
         <<< v9.2.0 >>>
         >>> Random number generator seed is 123456
         >>> Method used is AsymptoticLimits

         -- AsymptoticLimits ( CLs ) --
         Observed Limit: r < 1.1997
         Expected  2.5%: r < 0.6371
         Expected 16.0%: r < 0.8523
         Expected 50.0%: r < 1.1992
         Expected 84.0%: r < 1.7107
         Expected 97.5%: r < 2.3611

         Done in 1.21 min (cpu), 1.21 min (real)
   ```
   If the file is blank/incomplete, 
   ```bash
   % cat limits_allchannels_100_15
   <<< Combine >>> 
   <<< v9.2.0 >>>
   >>> Random number generator seed is 123456
   % 
   ```
   That means Combine did not succeed.

- Using [FitDiagnostics](http://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/part3/nonstandard/) to troubleshoot:
   ```bash
   # Inside the cards/ directory
   cd ${CMSSW_BASE}/src/CombineHarvester/CombineTools/cards/
   combine -M FitDiagnostics haabbtt_mutau_3_2018_60.txt -m 60

   # Up one directory
   cd ${CMSSW_BASE}/src/CombineHarvester/CombineTools
   combine -M FitDiagnostics cards/combined_mutau_2018_60.txt -t -1 -m 60 
   ```
- Using `diffNuisances.py` to troubleshoot:
   ```bash
   python diffNuisances.py fitDiagnosticsTest.root
   ```
- Multi-dimensional fit for debugging:
   ```bash
   python3 doMultiDimFit.py
   ```
