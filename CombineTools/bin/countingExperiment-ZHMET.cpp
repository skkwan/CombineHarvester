/*
 * countingExperiment-ZHMET.cpp
 * 
 * Based on CombineHarvester Example2.cpp
 *
 * Author: Stephanie Kwan
 */

#include <string>
#include <iostream>
#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/Observation.h"
#include "CombineHarvester/CombineTools/interface/Process.h"
#include "CombineHarvester/CombineTools/interface/Utilities.h"
#include "CombineHarvester/CombineTools/interface/Systematics.h"

using namespace std;

int main(int argc, char** argv) {

    std::string channel = *(argv + 1);
    std::string year = *(argv + 2);
    // these determine which processes actually go into the signal for each datacard
    std::string mass1 = *(argv + 3);
    std::string mass2 = *(argv + 4);

    //! [part1]
    // Use the CMSSW_BASE environment variable to get the full path to the auxiliaries folder
    // string in_dir = string(getenv("CMSSW_BASE")) + "/src/auxiliaries/datacards/sm/htt_mt/";
    string in_dir = "/eos/cms/store/group/phys_susy/skkwan/condorHistogramming/2025-07-03-21h38m-2018-mm-binByMT2MET/";

    // Create a new CombineHarvester instance
    ch::CombineHarvester cb;
    // Uncomment this next line to see a *lot* of debug information
    // cb.SetVerbosity(3);
    

    // Here we will just define two categories for an 8TeV analysis. Each entry in
    // the vector below specifies a bin name and corresponding bin_id.
    ch::Categories cats = {
        {1, "SR1"},
        {2, "SR2"}
    };
    // ch::Categories is just a typedef of vector<pair<int, string>>
    //! [part1]

    //! [part2]
    // vector<string> masses = ch::MassesFromRange("120-135:5");
    // Or equivalently, specify the mass points explicitly:
    //    vector<string> masses = {"120", "125", "130", "135"};
    // We use mass1 and mass2
    vector<string> masses = {"TChiZH_700_1", "TChiZH_500_300", "TChiZH_600_300"};
    //! [part2]

    
    cb.AddObservations({"*"}, {"zhmet"}, {year}, {channel}, cats);

    vector<string> bkg_procs = {"DYJets", "ttbar", "TTZ", "WJets", "diboson"};
    cb.AddProcesses({"*"}, {"zhmet"}, {year}, {channel}, bkg_procs, cats, false);

    // vector<string> sig_ggh = {"TChiZH_" + mass1 + "_" + mass2};
    vector<string> sig_procs = masses;
    cb.AddProcesses(masses, {"zhmet"}, {year}, {channel}, sig_procs, cats, true);


    // Some of the code for this is in a nested namespace, so
    // we'll make some using declarations first to simplify things a bit.
    using ch::syst::SystMap;
    using ch::syst::era;
    using ch::syst::bin_id;
    using ch::syst::process;

    // Extract from the datacard 
    cb.cp().backgrounds().ExtractShapes(
        in_dir + "out_"+channel+".root",
                            "$BIN/$PROCESS",
                            "$BIN/$PROCESS_$SYSTEMATIC");
    cb.cp().signals().ExtractShapes(
        in_dir + "out_"+channel+".root",
                        "$BIN/$PROCESS",
                        "$BIN/$PROCESS_$SYSTEMATIC");

    // This function modifies every entry to have a standardised bin name of
    // the form: {analysis}_{channel}_{bin_id}_{era}
    // which is commonly used in the htt analyses
    ch::SetStandardBinNames(cb);

    // First we generate a set of bin names:
    set<string> bins = cb.bin_set();

    // Create the output ROOT file that will contain all the shapes
    std::string outputName = "zhmet_" + year + "_" + channel + ".input.root";
    TFile output(outputName.c_str(), "RECREATE");
    
    // Finally we iterate through each bin,mass combination and write a datacard
    for (auto b : bins) {
        for (auto m : masses) {
          cout << ">> Writing datacard for bin: " << b << " and mass: " << m
               << "\n";
          // We need to filter on both the mass and the mass hypothesis,
          // where we must remember to include the "*" mass entry to get
          // all the data and backgrounds.
          cb.cp().bin({b}).mass({m, "*"}).WriteDatacard(
              b + "_" + m + ".txt", output);
        }
    }

}