#ifndef NOISEPERSEGMENT_CXX
#define NOISEPERSEGMENT_CXX

#include "NoisePerSegment.h"
#include <TStopwatch.h>
namespace larlite {

  void NoisePerSegment::SetMap(unsigned int lar_ch,
		      short crate,
		      short slot,
		      short femch)
  {
    if(_ch_to_crate.size() <= lar_ch) {
      _ch_to_crate.resize(lar_ch+1,-1);
      _ch_to_slot.resize(lar_ch+1,-1);
      _ch_to_femch.resize(lar_ch+1,-1);
    }
    _ch_to_crate[lar_ch] = crate;
    _ch_to_slot[lar_ch]  = slot;
    _ch_to_femch[lar_ch] = femch;
  }

  bool NoisePerSegment::initialize() {

    _evtN = 0;

    // number of blocks?
    _nBlocks = int(_nsamples/20.);

    int counter_tree = 0;

    _t_ch = new TTree("ch_tree","");
    _t_ch->Branch("larch",&_larch,"larch/i");
    _t_ch->Branch("crate",&_crate,"crate/i");
    _t_ch->Branch("slot",&_slot,"slot/i");
    _t_ch->Branch("femch",&_femch,"femch/i");
    _t_ch->Branch("rms",&_rms,"rms/F");
    _t_ch->Branch("evt",&_evtN,"evt/i");
    _t_ch->Branch("mean",&_mean,"mean/F");
    _t_ch->Branch("block",&_block,"block/i");
    _t_ch->SetDirectory(0);

    _t_ev = new TTree("ev_tree","");
    _t_ev->Branch("larch",&_larch,"larch/i");
    _t_ev->Branch("crate",&_crate,"crate/i");
    _t_ev->Branch("slot",&_slot,"slot/i");
    _t_ev->Branch("femch",&_femch,"femch/i");
    _t_ev->Branch("nblocks",&_nBlocks,"nblocks/i");
    _t_ev->Branch("highn",&_highn,"highn/i");
    _t_ev->Branch("lown",&_lown,"lown/i");
    _t_ev->Branch("fracOK",&_fracOK,"fracOK/d");
    _t_ev->SetDirectory(0);

    return true;
  }
  
  bool NoisePerSegment::analyze(storage_manager* storage) {

    auto wfs = storage->get_data<event_rawdigit>("daq");
    size_t nwfs = wfs->size();

    TStopwatch fWatch;
    fWatch.Start();
    
    std::map<int,int> chmap;

    // Prepare vectors
    _ch_to_index.resize(nwfs);;


    // Make a map from channel index to channel number
    for (size_t i=0; i < wfs->size(); i++){
      auto const& wf = (*wfs).at(i);
      int chnum = wf.Channel();
      chmap[chnum] = i;

      _larch = chnum;
      _crate = _ch_to_crate.at(chnum);
      _slot  = _ch_to_slot.at(chnum);
      _femch = _ch_to_femch.at(chnum);
      
      auto const& adcs = wf.ADCs();

      _highn = 0;
      _lown  = 0;

      // loop over the number of blocks
      for(int b=0; b < _nBlocks; b++){

	// Block number
	_block = b;

	// Mean
	for(size_t j = 0; j < _nBlocks; ++j)
	  _mean += adcs[j+b*_nBlocks];
	_mean /= ((float)adcs.size());
	// RMS
	for(size_t k = 0; k < _nBlocks; ++k)
	_rms += (adcs[k+b*_nBlocks]-_mean)*(adcs[k+b*_nBlocks]-_mean);
	_rms = sqrt( _rms / ((float)adcs.size()));

	_t_ch->Fill();

	if (_rms < _rmsLow)
	  _lown += 1;
	if (_rms > _rmsHigh)
	  _highn += 1;
	
      }// loop over blocks

      _fracOK = (_nblocks-(_highn+_lown))/((float)(_nblocks));
      
      _t_ev->Fill();

    }// loop over waveforms
    
    _evtN += 1;
    
    return true;
  }

  bool NoisePerSegment::finalize() {
    
    if(_fout){
      _fout->cd();
      std::cout << "writing ch tree" << std::endl;
      _t_ch->Write();
      std::cout << "writing ev tree" << std::endl;
      _t_ev->Write();
    }
    return true;
  }

}
#endif
