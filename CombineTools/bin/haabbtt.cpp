#include <string>
#include <map>
#include <set>
#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>
#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/Observation.h"
#include "CombineHarvester/CombineTools/interface/Process.h"
#include "CombineHarvester/CombineTools/interface/Utilities.h"
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "CombineHarvester/CombineTools/interface/BinByBin.h"

using namespace std;

// Add shape unc only when template norms are positive
// and when there are no negative bins in both Up and Down shapes
// e.g. diboson has some negative genweights, fake estimation involves data/MC subtraction...
void addshapes(ch::CombineHarvester* cb, TFile* input_file, vector<pair<int,string>> categories, vector<string> proc_names, string syst_name, float init_value) {
  // Loop over categories
  for (auto categories_itn = categories.begin(); categories_itn != categories.end(); ++categories_itn) {
    string category_name = categories_itn->second;// ch::Categories is of the type vector<pair<int, string>>
    TDirectory* dir = (TDirectory*) input_file->Get(category_name.c_str());
    if (dir == nullptr) {
      cout << "Warning: category " << category_name << " missing in file!" << endl;
      throw;
    }
    // Loop over the given processes
    for (auto proc_names_itn = proc_names.begin(); proc_names_itn != proc_names.end(); ++proc_names_itn) {
      TH1F* shapeBase = (TH1F*) dir->Get((*proc_names_itn).c_str());
      TH1F* shapeUp = (TH1F*) dir->Get((*proc_names_itn + "_" + syst_name + "Up").c_str());
      TH1F* shapeDown = (TH1F*) dir->Get((*proc_names_itn + "_" + syst_name + "Down").c_str());
      // Check if each of the given processes has at least the shape templates in file
      // They could be invalid for use but must be there if passed in argument
      if (shapeBase == nullptr) {
	cout << "Warning: the nominal process shape " << *proc_names_itn << " in category " << category_name << " does not exist!" << endl;
	throw;
      }
      if (shapeUp == nullptr) {
	cout << "Warning: the Up shape " << *proc_names_itn << "_" << syst_name << "Up" << " in category " << category_name << " does not exist!" << endl;
	throw;
      }
      if (shapeDown == nullptr) {
	cout << "Warning: the Down shape " << *proc_names_itn << "_" << syst_name << "Down" << " in category " << category_name << " does not exist!" << endl;
	throw;
      }
      // Check if the template shapes have positive norms
      Float_t shapeBase_norm = 0.0;
      Float_t shapeUp_norm = 0.0;
      Float_t shapeDown_norm = 0.0;
      shapeBase_norm = shapeBase->Integral();
      shapeUp_norm = shapeUp->Integral();
      shapeDown_norm = shapeDown->Integral();
      bool HasPositiveNorms = shapeBase_norm > 0.0 and shapeUp_norm > 0.0 and shapeDown_norm > 0.0;
            
      /* // Negative bins are fine except the shape looks crazy
            // Check if the Up and Down shapes have any negative bin
	 bool shapeUp_negativebins = false;
      bool shapeDown_negativebins = false;
      for (int i = 1; i <= shapeUp->GetNbinsX(); ++i) {
      if (shapeUp->GetBinContent(i) < 0.0) shapeUp_negativebins = true;
    }
      for (int i = 1; i <= shapeDown->GetNbinsX(); ++i) {
      if (shapeDown->GetBinContent(i) < 0.0) shapeDown_negativebins = true;
    }
      bool HasNegativeBins = shapeUp_negativebins or shapeDown_negativebins;
            */
            
	if (HasPositiveNorms) {
	  cb->cp().bin({category_name}).process({*proc_names_itn}).AddSyst(*cb, syst_name, "shape", ch::syst::SystMap<>::init(init_value));
	}
	else {
	  cout << "Skipping shape with non-positive norms: " << syst_name << " for process " << *proc_names_itn << " in category " << category_name << endl;
	}
    }
  }
}

int main(int argc, char** argv) {
    
  std::string channel = *(argv + 1);
  std::string year = *(argv + 2);
  std::string ncats = *(argv + 3);

    
  // Take category names from inputs
  cout << "Categories for channel and year: " << channel << " " << year << ":" << endl;
  std::vector<std::pair<int, std::string>> cats = {};
  for (int i = 1; i <= stoi(ncats); i++) {
    cats.push_back({i, *(argv + 3 + i)});
    cout << "Categories are: " << cats[i-1].first << ": " << cats[i-1].second << endl;
  }
    
  // Where the datacard is stored
  string aux_shapes = string(getenv("CMSSW_BASE")) + "/src/auxiliaries/shapes/";
    
  ch::CombineHarvester cb;
  // Uncomment this next line to see a *lot* of debug information
  cb.SetVerbosity(3);
    
  // List of categories (also called 'bins' in Combine)
  // ch::Categories cats = {
  //    {1, "1"},
  // };
     
  // List of mass points for the signal, the masses are added at the end of the signal string names
  //vector<string> masses = {"12","15","20","25","30","35","40","45","50","55","60"};
  vector<string> masses = {"40", "45"};
    
  // Observed data (name must be "data_obs" in datacards)
  cb.AddObservations({"*"}, {"haabbtt"}, {year}, {channel}, cats);
    
  // List of backgrounds in the datacards
  vector<string> bkg_procs = {"ttbar","ZJ","ST","VV","ggh_htt","ggh_hww","qqh_htt","qqh_hww","Zh_htt","Zh_hww","Wh_htt","Wh_hww","tth"};
  if (channel=="emu") bkg_procs.push_back("WJ");
    
  vector<string> bkg_procs_noEMB_nofake = bkg_procs;

  bkg_procs.push_back("embedded");
  bkg_procs.push_back("fake");
    
  cb.AddProcesses({"*"}, {"haabbtt"}, {year}, {channel}, bkg_procs, cats, false);
    
  // List of signals in the datacards. The mass will be concatenated at the end by the code
  vector<string> sig_procs = {"gghbbtt","vbfbbtt"};
  cb.AddProcesses(masses, {"haabbtt"}, {year}, {channel}, sig_procs, cats, true);
    
  // vector<string> sig_ggh = {"gghbbtt_12","gghbbtt_15","gghbbtt_20","gghbbtt_25","gghbbtt_30","gghbbtt_35","gghbbtt_40","gghbbtt_45","gghbbtt_50","gghbbtt_55","gghbbtt_60"};
  // vector<string> sig_vbf = {"vbfbbtt_12","vbfbbtt_15","vbfbbtt_20","vbfbbtt_25","vbfbbtt_30","vbfbbtt_35","vbfbbtt_40","vbfbbtt_45","vbfbbtt_50","vbfbbtt_55","vbfbbtt_60"};
  // if (channel=="emu" /*or channel=="etau" or channel=="mutau"*/){
  //    sig_ggh.push_back("gghbbtt12");
  //    sig_vbf.push_back("vbfbbtt12");
  // }
  vector<string> sig_ggh = {"gghbbtt"};
  vector<string> sig_vbf = {"vbfbbtt"};
    
  using ch::syst::SystMap;
  using ch::syst::era;
  using ch::syst::bin_id;
  using ch::syst::process;
  using ch::JoinStr;
    
  // =========================== Normalization uncertainties ===========================
  // The AddSyst method supports {$BIN, $PROCESS, $MASS, $ERA, $CHANNEL, $ANALYSIS}
  cb.cp().process({"ggh_htt","qqh_htt","Zh_htt","Wh_htt"}).AddSyst(cb, "CMS_htt_BR", "lnN", SystMap<>::init(1.018));
  cb.cp().process({"ggh_hww","qqh_hww","Zh_hww","Wh_hww"}).AddSyst(cb, "CMS_hww_BR", "lnN", SystMap<>::init(1.015));
  cb.cp().process(JoinStr({{"ggh_htt","ggh_hww"},sig_ggh})).AddSyst(cb, "pdf_Higgs_gg", "lnN", SystMap<>::init(1.032));
  cb.cp().process(JoinStr({{"qqh_htt","qqh_hww"},sig_vbf})).AddSyst(cb, "pdf_Higgs_qq", "lnN", SystMap<>::init(1.021));
  cb.cp().process({"ZJ"}).AddSyst(cb, "CMS_normalization_Z_13TeV", "lnN", SystMap<>::init(1.02));
  cb.cp().process({"ttbar"}).AddSyst(cb, "CMS_ttbar_xs", "lnN", SystMap<>::init(1.042));
  cb.cp().process({"VV"}).AddSyst(cb, "CMS_VV_xs", "lnN", SystMap<>::init(1.05));
  cb.cp().process({"ST"}).AddSyst(cb, "CMS_ST_xs", "lnN", SystMap<>::init(1.05));
  cb.cp().process({"embedded"}).AddSyst(cb, "CMS_normalization_embedded_13TeV", "lnN", SystMap<>::init(1.04));
  // Luminosity uncertainties, no embedded
  if (year=="2016"){
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf})).AddSyst(cb, "lumi_13TeV_2016", "lnN", SystMap<>::init(1.01));
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf})).AddSyst(cb, "lumi_13TeV_correlated16-18", "lnN", SystMap<>::init(1.006));
  }
  if (year=="2017"){
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf})).AddSyst(cb, "lumi_13TeV_2017", "lnN", SystMap<>::init(1.02));
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf})).AddSyst(cb, "lumi_13TeV_correlated16-18", "lnN", SystMap<>::init(1.009));
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf})).AddSyst(cb, "lumi_13TeV_correlated17-18", "lnN", SystMap<>::init(1.006));
  }
  if (year=="2018"){
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf})).AddSyst(cb, "lumi_13TeV_2018", "lnN", SystMap<>::init(1.015));
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf})).AddSyst(cb, "lumi_13TeV_correlated16-18", "lnN", SystMap<>::init(1.02));
    cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf})).AddSyst(cb, "lumi_13TeV_correlated17-18", "lnN", SystMap<>::init(1.002));
  }
    
  // if (channel=="emu"){
  //   cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf})).AddSyst(cb, "CMS_eleID_13TeV", "lnN", SystMap<>::init(1.02));
  //   cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf})).AddSyst(cb, "CMS_muID_13TeV", "lnN", SystMap<>::init(1.02));
  //   // eleID 50% correlated with MC
  //   cb.cp().process({"embedded"}).AddSyst(cb, "CMS_eleID_13TeV", "lnN", SystMap<>::init(1.01));// 2% * 50%
  //   cb.cp().process({"embedded"}).AddSyst(cb, "CMS_EMB_eleID_13TeV", "lnN", SystMap<>::init(1.01732));// 2% * sqrt(1-50%^2)
  //   // muID 50% correlated with MC
  //   cb.cp().process({"embedded"}).AddSyst(cb, "CMS_muID_13TeV", "lnN", SystMap<>::init(1.01));
  //   cb.cp().process({"embedded"}).AddSyst(cb, "CMS_EMB_muID_13TeV", "lnN", SystMap<>::init(1.01732));
        
  //   cb.cp().process({"fake"}).AddSyst(cb, "CMS_normalization_qcd_13TeV", "lnN", SystMap<>::init(1.20));
  // }
    
  // if (channel=="etau"){
  //   cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf})).AddSyst(cb, "CMS_eleID_13TeV", "lnN", SystMap<>::init(1.02));
  //   // eleID 50% correlated with MC
  //   cb.cp().process({"embedded"}).AddSyst(cb, "CMS_eleID_13TeV", "lnN", SystMap<>::init(1.01));
  //   cb.cp().process({"embedded"}).AddSyst(cb, "CMS_EMB_eleID_13TeV", "lnN", SystMap<>::init(1.01732));
        
  //   cb.cp().process({"fake"}).AddSyst(cb, "CMS_normalization_fake_13TeV", "lnN", SystMap<>::init(1.20));
  // }
    
  // if (channel=="mutau"){
  //   cb.cp().process(JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf})).AddSyst(cb, "CMS_muID_13TeV", "lnN", SystMap<>::init(1.02));
  //   // muID 50% correlated with MC
  //   cb.cp().process({"embedded"}).AddSyst(cb, "CMS_muID_13TeV", "lnN", SystMap<>::init(1.01));
  //   cb.cp().process({"embedded"}).AddSyst(cb, "CMS_EMB_muID_13TeV", "lnN", SystMap<>::init(1.01732));
        
  //   cb.cp().process({"fake"}).AddSyst(cb, "CMS_normalization_fake_13TeV", "lnN", SystMap<>::init(1.20));
  // }
    
  // // =========================== Shape uncertainties ===========================
  // // The AddSyst method supports {$BIN, $PROCESS, $MASS, $ERA, $CHANNEL, $ANALYSIS}
    
  // TFile* file;
  // file = new TFile((aux_shapes+"final_"+channel+"_"+year+".root").c_str());// To be used for the addshapes function
    
  // // btagging efficiency, no embedded
  // //addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_btagsf_heavy_"+year, 1.00);
  // //addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_btagsf_light_"+year, 1.00);
  // addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_btagsf_hf", 1.00);
  // addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_btagsf_lf", 1.00);
  // addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_btagsf_hfstats1_"+year, 1.00);
  // addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_btagsf_hfstats2_"+year, 1.00);
  // addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_btagsf_lfstats1_"+year, 1.00);
  // addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_btagsf_lfstats2_"+year, 1.00);
  // addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_btagsf_cferr1", 1.00);
  // addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_btagsf_cferr2", 1.00);
    
  // // Trigger efficiency
  // if (channel=="etau" or channel=="mutau"){
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_trgeff_single_"+channel+"_"+year, 1.00);
  //   // 50% correlated with MC
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_trgeff_single_"+channel+"_"+year, 0.50);// 1.00 * 50%
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_trgeff_single_"+channel+"_"+year, 0.866);// 1.00 * sqrt(1-50%^2)
  //   if (channel=="mutau" or (channel=="etau" && year!="2016")){
  //     addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_trgeff_cross_"+channel+"_"+year, 1.00);
  //     addshapes(&cb, file, cats, {"embedded"}, "CMS_trgeff_cross_"+channel+"_"+year, 0.50);
  //     addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_trgeff_cross_"+channel+"_"+year, 0.866);
  //   }
  // }
  // if (channel=="emu"){
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_trgeff_Mu8E23_"+channel+"_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_trgeff_Mu23E12_"+channel+"_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_trgeff_both_"+channel+"_"+year, 1.00);
  //   // 50% correlated with MC
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_trgeff_Mu8E23_"+channel+"_"+year, 0.50);
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_trgeff_Mu23E12_"+channel+"_"+year, 0.50);
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_trgeff_both_"+channel+"_"+year, 0.50);
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_trgeff_Mu8E23_"+channel+"_"+year, 0.866);
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_trgeff_Mu23E12_"+channel+"_"+year, 0.866);
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_trgeff_both_"+channel+"_"+year, 0.866);
  // }
    
  // // tau related corrections, no fake bkg
  // if (channel=="etau" or channel=="mutau"){
  //   // tau ID efficiency (VSjet)
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_pt20to25_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_pt25to30_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_pt30to35_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_pt35to40_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_pt40to500_"+year, 1.00);
  //   //addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_pt500to1000_"+year, 1.00);
  //   //addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_ptgt1000_"+year, 1.00);
        
  //   // 50% correlated with MC
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_tauideff_pt20to25_"+year, 0.50);
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_tauideff_pt25to30_"+year, 0.50);
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_tauideff_pt30to35_"+year, 0.50);
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_tauideff_pt35to40_"+year, 0.50);
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_tauideff_pt40to500_"+year, 0.50);
  //   //addshapes(&cb, file, cats, {"embedded"}, "CMS_tauideff_pt500to1000_"+year, 0.50);
  //   //addshapes(&cb, file, cats, {"embedded"}, "CMS_tauideff_ptgt1000_"+year, 0.50);
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauideff_pt20to25_"+year, 0.866);
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauideff_pt25to30_"+year, 0.866);
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauideff_pt30to35_"+year, 0.866);
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauideff_pt35to40_"+year, 0.866);
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauideff_pt40to500_"+year, 0.866);
  //   //addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauideff_pt500to1000_"+year, 0.866);
  //   //addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauideff_ptgt1000_"+year, 0.866);
        
  //   // tau ID efficiency (VSe), no anti-lepton in embedded
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_VSe_bar_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_VSe_end_"+year, 1.00);
        
  //   // tau ID efficiency (VSmu), no anti-lepton in embedded
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_VSmu_eta0to0p4_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_VSmu_eta0p4to0p8_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_VSmu_eta0p8to1p2_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_VSmu_eta1p2to1p7_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauideff_VSmu_eta1p7to2p3_"+year, 1.00);
        
  //   // tau ES
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_TES_dm0_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_TES_dm1_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_TES_dm10_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_TES_dm11_"+year, 1.00);
  //   // 50% correlated with MC
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_TES_dm0_"+year, 0.50);
  //       addshapes(&cb, file, cats, {"embedded"}, "CMS_TES_dm1_"+year, 0.50);
  //       addshapes(&cb, file, cats, {"embedded"}, "CMS_TES_dm10_"+year, 0.50);
  //       addshapes(&cb, file, cats, {"embedded"}, "CMS_TES_dm11_"+year, 0.50);
  //       addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_TES_dm0_"+year, 0.866);
  //       addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_TES_dm1_"+year, 0.866);
  //       addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_TES_dm10_"+year, 0.866);
  //       addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_TES_dm11_"+year, 0.866);
        
  //       // tau ES (ele fake), no embedded
  //       addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_eleTES_dm0_"+year, 1.00);
  //       addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_eleTES_dm1_"+year, 1.00);
        
  //       // tau ES (mu fake), no embedded
  //       addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_muTES_dm0_"+year, 1.00);
  //       addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_muTES_dm1_"+year, 1.00);
  //   }
    
  //   // Tau ID efficiency with different WP than used in measurement, for e+tau only
  //   if (channel=="etau"){
  //       addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,sig_ggh,sig_vbf}), "CMS_tauidWP_et_"+year, 1.00);
  //       addshapes(&cb, file, cats, {"embedded"}, "CMS_tauidWP_et_"+year, 0.50);
  //       addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tauidWP_et_"+year, 0.866);
  //   }
    
  //   // Leptons ES (MC and embedded are fully uncorrelated)
  //   if (channel=="etau" or channel=="emu"){
  //       // ele ES
  //       addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_eleES_bar_"+year, 1.00);
  //       addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_eleES_end_"+year, 1.00);
  //       addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_eleES_bar_"+year, 1.00);
  //       addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_eleES_end_"+year, 1.00);
  //   }
  //   if (channel=="mutau" or channel=="emu"){
  //       // mu ES
  //       addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_muES_eta0to1p2_"+year, 1.00);
  //       addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_muES_eta1p2to2p1_"+year, 1.00);
  //       addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_muES_eta2p1to2p4_"+year, 1.00);
  //       addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_muES_eta0to1p2_"+year, 1.00);
  //       addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_muES_eta1p2to2p1_"+year, 1.00);
  //       addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_muES_eta2p1to2p4_"+year, 1.00);
  //   }
    
  //   // JES, no embedded
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_JetAbsolute", 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_JetAbsolute_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_JetBBEC1", 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_JetBBEC1_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_JetEC2", 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_JetEC2_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_JetFlavorQCD", 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_JetHF", 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_JetHF_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_JetRelativeBal", 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_JetRelativeSample_"+year, 1.00);
  //   addshapes(&cb, file, cats, JoinStr({bkg_procs_noEMB_nofake,{"fake"},sig_ggh,sig_vbf}), "CMS_JER", 1.00);
    
  //   // recoil correction, for Z+jets, W+jets, ggh and qqh (no W+jets in e+tau and mu+tau)
  //   // UES uncertainties, for MC without recoil correction
  //   if (channel=="etau" or channel=="mutau"){
  //     addshapes(&cb, file, cats, JoinStr({{"ZJ","ggh_htt","ggh_hww","qqh_htt","qqh_hww","fake"},sig_ggh,sig_vbf}), "CMS_0j_resolution_"+year, 1.00);
  //     addshapes(&cb, file, cats, JoinStr({{"ZJ","ggh_htt","ggh_hww","qqh_htt","qqh_hww","fake"},sig_ggh,sig_vbf}), "CMS_0j_response_"+year, 1.00);
  //     addshapes(&cb, file, cats, JoinStr({{"ZJ","ggh_htt","ggh_hww","qqh_htt","qqh_hww","fake"},sig_ggh,sig_vbf}), "CMS_1j_resolution_"+year, 1.00);
  //     addshapes(&cb, file, cats, JoinStr({{"ZJ","ggh_htt","ggh_hww","qqh_htt","qqh_hww","fake"},sig_ggh,sig_vbf}), "CMS_1j_response_"+year, 1.00);
  //     addshapes(&cb, file, cats, JoinStr({{"ZJ","ggh_htt","ggh_hww","qqh_htt","qqh_hww","fake"},sig_ggh,sig_vbf}), "CMS_gt1j_resolution_"+year, 1.00);
  //     addshapes(&cb, file, cats, JoinStr({{"ZJ","ggh_htt","ggh_hww","qqh_htt","qqh_hww","fake"},sig_ggh,sig_vbf}), "CMS_gt1j_response_"+year, 1.00);
  //     addshapes(&cb, file, cats, {"ttbar","ST","VV","Zh_htt","Zh_hww","Wh_htt","Wh_hww","tth","fake"}, "CMS_UES_"+year, 1.00);
  //   }
  //   if (channel=="emu"){
  //     addshapes(&cb, file, cats, JoinStr({{"ZJ","WJ","ggh_htt","ggh_hww","qqh_htt","qqh_hww","fake"},sig_ggh,sig_vbf}), "CMS_0j_resolution_"+year, 1.00);
  //     addshapes(&cb, file, cats, JoinStr({{"ZJ","WJ","ggh_htt","ggh_hww","qqh_htt","qqh_hww","fake"},sig_ggh,sig_vbf}), "CMS_0j_response_"+year, 1.00);
  //     addshapes(&cb, file, cats, JoinStr({{"ZJ","WJ","ggh_htt","ggh_hww","qqh_htt","qqh_hww","fake"},sig_ggh,sig_vbf}), "CMS_1j_resolution_"+year, 1.00);
  //     addshapes(&cb, file, cats, JoinStr({{"ZJ","WJ","ggh_htt","ggh_hww","qqh_htt","qqh_hww","fake"},sig_ggh,sig_vbf}), "CMS_1j_response_"+year, 1.00);
  //     addshapes(&cb, file, cats, JoinStr({{"ZJ","WJ","ggh_htt","ggh_hww","qqh_htt","qqh_hww","fake"},sig_ggh,sig_vbf}), "CMS_gt1j_resolution_"+year, 1.00);
  //     addshapes(&cb, file, cats, JoinStr({{"ZJ","WJ","ggh_htt","ggh_hww","qqh_htt","qqh_hww","fake"},sig_ggh,sig_vbf}), "CMS_gt1j_response_"+year, 1.00);
  //     addshapes(&cb, file, cats, {"ttbar","ST","VV","Zh_htt","Zh_hww","Wh_htt","Wh_hww","tth","fake"}, "CMS_UES_"+year, 1.00);
  //   }
    
  //   // Z pt reweighting
  //   addshapes(&cb, file, cats, {"ZJ","fake"}, "CMS_Zpt_"+year, 1.00);
    
  //   // top pt reweighting (no need to add if data/MC agreement is fairly good with nominal top pt sf applied)
  //   //addshapes(&cb, file, cats, {"ttbar","fake"}, "CMS_toppt_"+year, 1.00);
    
  //   // tau tracking efficiency in embedded (on real tauh, no effect on fake bkg)
  //   if (channel=="etau" or channel=="mutau"){
  //     addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tautrack_dm0dm10_"+year, 1.00);
  //     addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tautrack_dm1_"+year, 1.00);
  //     addshapes(&cb, file, cats, {"embedded"}, "CMS_EMB_tautrack_dm11_"+year, 1.00);
  //   }
    
  //   // Non-DY MC contamination to embedded (selected from taus, no effect on fake bkg)
  //   addshapes(&cb, file, cats, {"embedded"}, "CMS_nonDY_"+year, 1.00);
    
  //   // Reducible background estimation
  //   if (channel=="etau" or channel=="mutau"){
  //     // fake factor for cross triggered events
  //     addshapes(&cb, file, cats, {"fake"}, "CMS_crosstrg_fakefactor_"+year, 1.00);
  //     // fake rate measurement
  //     addshapes(&cb, file, cats, {"fake"}, "CMS_jetFR_pt0to25_"+year, 1.00);
  //     addshapes(&cb, file, cats, {"fake"}, "CMS_jetFR_pt25to30_"+year, 1.00);
  //     addshapes(&cb, file, cats, {"fake"}, "CMS_jetFR_pt30to35_"+year, 1.00);
  //     addshapes(&cb, file, cats, {"fake"}, "CMS_jetFR_pt35to40_"+year, 1.00);
  //     addshapes(&cb, file, cats, {"fake"}, "CMS_jetFR_pt40to50_"+year, 1.00);
  //     addshapes(&cb, file, cats, {"fake"}, "CMS_jetFR_pt50to60_"+year, 1.00);
  //     addshapes(&cb, file, cats, {"fake"}, "CMS_jetFR_ptgt60_"+year, 1.00);
  //   }
  //   if (channel=="emu"){
  //     // SS correction and closure
  //     addshapes(&cb, file, cats, {"fake"}, "CMS_SScorrection_"+year, 1.00);
  //     addshapes(&cb, file, cats, {"fake"}, "CMS_SSclosure_"+year, 1.00);
  //     addshapes(&cb, file, cats, {"fake"}, "CMS_SSboth2D_"+year, 1.00);
  //     addshapes(&cb, file, cats, {"fake"}, "CMS_osss_"+year, 1.00);
  //   }
 
   
    
    // Name of the input datacard
    // The ExtractShapes method supports {$BIN, $PROCESS, $MASS, $SYSTEMATIC}
    cb.cp().backgrounds().ExtractShapes(
					aux_shapes + "test_"+channel+"_"+year+".root",
                                        "$BIN/$PROCESS",
                                        "$BIN/$PROCESS_$SYSTEMATIC");
    cb.cp().signals().ExtractShapes(
				    aux_shapes + "test_"+channel+"_"+year+".root",
                                    "$BIN/$PROCESS$MASS",
                                    "$BIN/$PROCESS$MASS_$SYSTEMATIC");
    
    ch::SetStandardBinNames(cb);
    
    set<string> bins = cb.bin_set();
    
    TFile output(("haabbtt"+year+"_"+channel+".input.root").c_str(), "RECREATE");
    
    for (auto b : bins) {
      for (auto m : masses) {
	      cout << ">> Writing datacard for bin: " << b << " and mass: " << m  << "\n";
	      cb.cp().bin({b}).mass({m, "*"}).WriteDatacard(b + "_" + m + ".txt", output);
      }
    }
    
}
