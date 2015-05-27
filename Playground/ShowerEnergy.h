/**
 * \file ShowerEnergy.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class ShowerEnergy
 *
 * @author david
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_SHOWERENERGY_H
#define LARLITE_SHOWERENERGY_H

#include "Analysis/ana_base.h"
#include "BasicTool/GeoAlgo/GeoAlgo.h"
#include "CalorimetryAlg.h"
#include "DataFormat/mcshower.h"
#include "DataFormat/simch.h"
// LArUtil includes
#include "LArUtil/Geometry.h"
#include "LArUtil/GeometryUtilities.h"
#include "LArUtil/LArProperties.h"
#include "LArUtil/DetectorProperties.h"
#include <cmath>

namespace larlite {
  /**
     \class ShowerEnergy
     User custom analysis class made by david
   */
  class ShowerEnergy : public ana_base{
  
  public:

    /// Default constructor
    ShowerEnergy();

    /// Default destructor
    virtual ~ShowerEnergy(){};

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);

    virtual bool finalize();

    void clearShrTree();
    void clearIDETree();

    void setShrProducer(std::string s) { _ShowerProducer = s; }

    protected:

    std::string _ShowerProducer;

    // shr tree
    TTree* _shr_tree;
    // shr tree variables
    double _E;
    double _Edep;
    double _EideU, _EideV, _EideY; // energy seen by each plane for shower
    double _EboxU, _EboxV, _EboxY; // energy seen by each plane after box model correction
    double _EbrkU, _EbrkV, _EbrkY; // energy seen by each plane after birks model correction
    double _QideU, _QideV, _QideY; // electrons seen by each plane for shower (lifetime-corrected)
    double _pitchU, _pitchV, _pitchY; /// pitch for various planes [ cm ]
    double _X, _Y, _Z;
    double _px, _py, _pz;
    
    // IDE tree
    TTree* _ide_tree;
    int _pl;
    double _x, _y, _z; /// coordinates of IDE deposition point
    double _e; // energy deposited by this IDE
    double _eboxU, _eboxV, _eboxY; // energy seen by each plane after box model correction
    double _ebrkU, _ebrkV, _ebrkY; // energy seen by each plane after birks model correction
    double _q; // number of electrons for this IDE
    double _qLife; // number of electrons corrected for lifetime corr.

    // GeoAlgo
    geoalgo::GeoAlgo gA;

    // CalorimetryAlgo
    ::calo::CalorimetryAlg _calo;

    // LArProperties
    ::larutil::LArProperties* LArProp;

    // Double max
    double _max = std::numeric_limits<double>::max();

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
