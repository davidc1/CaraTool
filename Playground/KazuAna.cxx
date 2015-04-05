#ifndef LARLITE_KAZUANA_CXX
#define LARLITE_KAZUANA_CXX

#include "KazuAna.h"

void KazuAna::AnaFile(std::string fname)
  
{
  
  auto ch=new TChain("ptree","");
  
  float amp;
  float rms_sub;
  float rms_ref;
  float rms_diff;
  float mean;
  unsigned int crate;
  unsigned int slot;
  unsigned int femch;
  ch->SetBranchAddress("amp",&amp);
  ch->SetBranchAddress("rms_diff",&rms_diff);
  ch->SetBranchAddress("rms_sub",&rms_sub);
  ch->SetBranchAddress("rms_ref",&rms_ref);
  ch->SetBranchAddress("sub_crate",&crate);
  ch->SetBranchAddress("sub_slot",&slot);
  ch->SetBranchAddress("sub_femch",&femch);
  ch->AddFile(fname.c_str());

  if(ch->GetEntries()<1) return;
  
  std::vector<std::vector<std::vector<std::vector<float> > > > data_map;
  data_map.resize(9);
  
  for(size_t i=0; i<ch->GetEntries(); ++i) {
  ch->GetEntry(i);
  
  if(data_map.at(crate).size() <= slot) data_map.at(crate).resize(slot+1,
    std::vector<std::vector<float> >(64,
    std::vector<float>()
    )
    );
  

  
  data_map.at(crate).at(slot).at(femch).push_back(rms_diff-rms_sub-rms_ref);
  
}
  


  // Average

  for(auto const& crate_data : data_map) {
  for(auto const& slot_data : crate_data) {
  
  
  for(size_t ch=0; ch<slot_data.size(); ++ch) {


  
  if(!slot_data.at(ch).size()) continue;
  


  float something=0;

  for(auto const& v : slot_data.at(ch)) 



    something += v;



  something /= (float)(slot_data.at(ch).size());



  std::cout<<something<<std::endl;

}

}

    

}






  
  }

#endif

