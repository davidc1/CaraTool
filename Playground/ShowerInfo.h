/**
 * \file ShowerInfo.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class ShowerInfo
 *
 * @author david
 */

/** \addtogroup Playground

    @{*/

#ifndef LARLITE_SHOWERINFO_H
#define LARLITE_SHOWERINFO_H

#include "Analysis/ana_base.h"
#include "BasicTool/GeoAlgo/GeoAlgo.h"
#include "DataFormat/mcshower.h"
#include "DataFormat/shower.h"
#include "TH1D.h"

namespace larlite {
  /**
     \class ShowerInfo
     User custom analysis class made by david
   */
  class ShowerInfo : public ana_base{
  
  public:

    /// Default constructor
    ShowerInfo(){ _name="ShowerInfo"; _fout=0;};

    /// Default destructor
    virtual ~ShowerInfo(){};

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);

    virtual bool finalize();

    void clearShrTree();
    void clearIPTree();

    void setShrProducer(std::string s) { _ShowerProducer = s; }

    protected:

    std::string _ShowerProducer;

    // shr tree
    TTree* _shrTree;
    // shr tree variables
    double _E;
    double _x, _y, _z;
    double _px, _py, _pz;
    double _UdEdx, _VdEdx, _YdEdx;
    
    // IP tree
    TTree* _ipTree;
    double _rad1, _rad2;
    double _IP;

    // GeoAlgo
    geoalgo::GeoAlgo gA;

    // Histogram
    TH1D* _hBestIP;

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
