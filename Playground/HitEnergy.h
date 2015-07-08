/**
 * \file HitEnergy.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class HitEnergy
 *
 * @author david
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_HITENERGY_H
#define LARLITE_HITENERGY_H

#include "Analysis/ana_base.h"
#include "BasicTool/GeoAlgo/GeoAlgo.h"
#include "CalorimetryAlg.h"
#include "DataFormat/hit.h"
#include "DataFormat/simch.h"
#include "DataFormat/mcshower.h"
#include "DataFormat/mctrack.h"
// LArUtil includes
#include "LArUtil/Geometry.h"
#include "LArUtil/GeometryUtilities.h"
#include "LArUtil/LArProperties.h"
#include "LArUtil/DetectorProperties.h"
#include <cmath>

namespace larlite {
  /**
     \class HitEnergy
     User custom analysis class made by david
   */
  class HitEnergy : public ana_base{
  
  public:

    /// Default constructor
    HitEnergy();

    /// Default destructor
    virtual ~HitEnergy(){};

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);

    virtual bool finalize();

    /// set wether to use showers or tracks
    void useShowers(bool on) { _shr = on; }

    protected:

    // use showers or tracks?
    bool _shr;

    // channel -> simch position in evt_simch map
    void fillSimchMap(const larlite::event_simch* evt_simch);
    std::map<unsigned int, size_t> _simchMap;

    // CalorimetryAlgo
    ::calo::CalorimetryAlg _calo;

    // LArProperties
    ::larutil::LArProperties* LArProp;

    // Double max
    double _max = std::numeric_limits<double>::max();

    // Tree to be filled
    TTree* _hit_tree;
    int _ch;
    int _pl;
    double _start, _end, _peak;
    double _amp, _int;
    double _e, _ebox, _ebrk;
    double _q, _qlife;
    double _dx;
    double _dist; // distance from start point
    double _qint; // charge in hit from ADC integral (lifetime corr.)
    double _qamp; // charge in hit from ADC amplitude (lifetime corr.)
    double _eAmpBox; // E in hit from Amp w/ Box Model
    double _eAmpBrk; // E in hit from Amp w/ Birks Model
    double _eIntBox; // E in hit from Int w/ Box Model
    double _eIntBrk; // E in hit from Int w/ Birks Model

  };
}
#endif

//**************************************************************************
// 
// For Analysis framework documentation, read Manual.pdf here:
//
// http://microboone-docdb.fnal.gov:8080/cgi-bin/ShowDocument?docid=3183
//
//**************************************************************************

/** @} */ // end of doxygen group 
