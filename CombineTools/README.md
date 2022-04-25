# haabbtt Combine

## Setup

1. 

```
export SCRAM_ARCH=slc6_amd64_gcc530
scram project CMSSW CMSSW_10_2_13
cd CMSSW_10_2_13/src
cmsenv
git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit
# IMPORTANT: Checkout the recommended tag on the link above
git clone https://github.com/cms-analysis/CombineHarvester.git CombineHarvester
scram b
```

2. In `CombineHarvester/CombineTools/src`, mkdir `auxiliaries/shapes` and store final root files there.

3. Copy the main code `haabbtt.cpp` to `CombineHarvester/CombineTools/bin/` and make sure `BuildFile.xml` is modified
   (these files I should have committed to the correct folders in my fork)

4. Compile after every change, in the `bin/` directory (if we are only changing `haabbtt.cpp`):
   `scram b -j 8`

## To run the example
1. For the example from HF with all channels, years, 

   ```
   # Go to the working directory:
    cd AABBTT_allyears
   haabbtt mt 2018 7 1 2 3 4 5 6 7
   ```

   In the root file, directories 1 2 3 4 are SR1 SR2 SR3 CR finale m(tt) from 1bjet events after DNN cuts; 5 6 7 are SR1 SR2 CR from 2bjets.

2. Add statistical uncertainties for the backgrounds: 
   `echo "* autoMCStats 0.0" >> haabbtt_em_1_2016_40.txt`

3. Compute the expected limit (-t -1 means expected, blinded): 
   `combine -M AsymptoticLimits haabbtt_em_1_2016_40.txt -t -1 -m 40`

4. If you want to combine several categories, make a combined txt file:
   `combineCards.py haabbtt_em_1_2016_40.txt haabbtt_em_2_2016_40.txt > combined_em_2016_40.txt` 