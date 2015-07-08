#ifndef LARLITE_HITENERGY_CXX
#define LARLITE_HITENERGY_CXX

#include "HitEnergy.h"

static const double kCalAmpConstants_UB[3] =  {9.200e-4, 1.062e-3, 1.084e-03};

static const double kCalAreaConstants_UB[3] = {5.0142e-3, 5.1605e-3, 5.4354e-3 };

namespace larlite {

  HitEnergy::HitEnergy()
    : _hit_tree(nullptr)
  {
    _name = "HitEnergy";
    _fout = 0;
    _shr  = true;
    LArProp = (::larutil::LArProperties*)(::larutil::LArProperties::GetME());
  }

  bool HitEnergy::initialize() {

    // Prepare Shower Tree
    if (_hit_tree)
      delete _hit_tree;
    _hit_tree = new TTree("_hit_tree","shr_tree");
    _hit_tree->Branch("_start",&_start,"start/D");
    _hit_tree->Branch("_end",&_end,"end/D");
    _hit_tree->Branch("_peak",&_peak,"peak/D");
    _hit_tree->Branch("_pl",&_pl,"pl/I");
    _hit_tree->Branch("_ch",&_ch,"ch/I");
    _hit_tree->Branch("_amp",&_amp,"amp/D");
    _hit_tree->Branch("_int",&_int,"int/D");
    _hit_tree->Branch("_q",&_q,"q/D");
    _hit_tree->Branch("_qlife",&_qlife,"qlife/D");
    _hit_tree->Branch("_e",&_e,"e/D");
    _hit_tree->Branch("_ebox",&_ebox,"ebox/D");
    _hit_tree->Branch("_ebrk",&_ebrk,"ebrk/D");
    _hit_tree->Branch("_dx",&_dx,"dx/D"); 
    _hit_tree->Branch("_qint",&_qint,"qint/D"); 
    _hit_tree->Branch("_qamp",&_qamp,"qamp/D"); 
    _hit_tree->Branch("_dist",&_dist,"dist/D"); 
    _hit_tree->Branch("_eAmpBox",&_eAmpBox,"eAmpBox/D"); 
    _hit_tree->Branch("_eAmpBrk",&_eAmpBrk,"eAmpBrk/D"); 
    _hit_tree->Branch("_eIntBox",&_eIntBox,"eIntBox/D"); 
    _hit_tree->Branch("_eIntBrk",&_eIntBrk,"eIntBrk/D"); 


    return true;
  }
  
  bool HitEnergy::analyze(storage_manager* storage) {


    // declare momentum components
    double px, py, pz;
    px = py = pz = 1;
    double x,y,z;
    x = y = z = 1;
    if (_shr){
      // get Showers
      auto evt_shower = storage->get_data<event_mcshower>("mcreco");
      // get mcshower's info
      auto const& shr = evt_shower->at(0);
      auto const& start = shr.DetProfile();
      px   = start.Px();
      py   = start.Py();
      pz   = start.Pz();
      x    = start.X();
      y    = start.Y();
      z    = start.Z();
    }
    else{
      // get Tracks
      auto evt_track  = storage->get_data<event_mctrack>("mcreco");
      auto const& trk = evt_track->at(0);
      auto const& start = trk[0];
      px   = start.Px();
      py   = start.Py();
      pz   = start.Pz();
      x    = start.X();
      y    = start.Y();
      z    = start.Z();
    }

    // get directions from momentum info
    double theta = acos(py/sqrt(px*px+py*py+pz*pz));
    double phi   = atan(pz/px);

    // get per-plane pitch
    std::vector<double> pitch(3,0.);
    pitch[0] = larutil::GeometryUtilities::GetME()->PitchInView(0,phi,theta);
    pitch[1] = larutil::GeometryUtilities::GetME()->PitchInView(1,phi,theta);
    pitch[2] = larutil::GeometryUtilities::GetME()->PitchInView(2,phi,theta);

    // get Hits
    auto evt_hits = storage->get_data<event_hit>("gaushit");

    if (!evt_hits){
      std::cout << "This hit Type does not exist! Exiting..." << std::endl;
      return false;
    }

    // get simchannels
    auto evt_simch = storage->get_data<event_simch>("largeant");

    if (!evt_simch){
      std::cout << "No simch! This is a problem..." << std::endl;
      return false;
    }

    // make a Channel -> simch map
    fillSimchMap(evt_simch);

    // loop over hits
    for (size_t h=0; h < evt_hits->size(); h++){
      // get hit
      auto const& hit = evt_hits->at(h);
      // get hit info
      _ch = hit.Channel();
      _pl = ::larutil::Geometry::GetME()->ChannelToPlane(hit.Channel());
      
      _start = hit.StartTick()+3200;
      _end   = hit.EndTick()+3200;
      _peak  = hit.PeakTime()+3200;
      _amp   = hit.PeakAmplitude();
      _int   = hit.Integral();
      _qint  = _int/kCalAreaConstants_UB[_pl];
      _qint *= _calo.LifetimeCorrection(hit.StartTick());
      _qamp  = _int/kCalAmpConstants_UB[_pl];
      _qamp *= _calo.LifetimeCorrection(hit.StartTick());
      _eAmpBox  = pitch[_pl]*LArProp->ModBoxCorrection(_qamp/pitch[_pl]);
      _eAmpBrk  = pitch[_pl]*LArProp->BirksCorrection(_qamp/pitch[_pl]);
      _eIntBox  = pitch[_pl]*LArProp->ModBoxCorrection(_qint/pitch[_pl]);
      _eIntBrk  = pitch[_pl]*LArProp->BirksCorrection(_qint/pitch[_pl]);
      // find associated simchannel
      if (_simchMap.find(_ch) != _simchMap.end()){
	larlite::simch simch = evt_simch->at(_simchMap[_ch]);
	// get vector of IDEs between time-ticks associated
	// with hit
	double s = _peak-2*(_peak-_start);
	double e = _peak+2*(_end-_peak);
	std::vector<larlite::ide> ideVec = simch.TrackIDsAndEnergies(s,e);
	_e = 0;
	_q = 0;
	for (auto const& ide : ideVec){
	  _e += ide.energy;
	  _q += ide.numElectrons;
	  _dist = sqrt((ide.x-x)*(ide.x-x)+(ide.y-y)*(ide.y-y)+(ide.z-z)*(ide.z-z));
	}// for all IDEs
      }// if simchannel exists
      _qlife = _q*_calo.LifetimeCorrection(hit.StartTick());
      
      _ebox  = pitch[_pl]*LArProp->ModBoxCorrection(_qlife/pitch[_pl]);
      _ebrk  = pitch[_pl]*LArProp->BirksCorrection(_qlife/pitch[_pl]);
      _dx    = pitch[_pl];

      _hit_tree->Fill();

    }// for all hits
	

    return true;
  }

  bool HitEnergy::finalize() {

    // Write trees & histos
    _hit_tree->Write();
  
    return true;
  }


  void HitEnergy::fillSimchMap(const larlite::event_simch* evt_simch)
  {

    _simchMap.clear();

    for (size_t s=0; s < evt_simch->size(); s++){
      auto const& simch = evt_simch->at(s);
      _simchMap[simch.Channel()] = s;
    }

    return;
  }

}
#endif
