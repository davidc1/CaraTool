/**
 * \file NoisePerSegment.h
 *
 * \ingroup SimpleWFAna
 * 
 * \brief Class def header for a class NoisePerSegment
 *
 * @author davidc1
 */

/** \addtogroup SimpleWFAna

    @{*/

#ifndef NOISEPERSEGMENT_H
#define NOISEPERSEGMENT_H

#include "Analysis/ana_base.h"
#include <map>
#include <iostream>
#include <vector>
#include <set>
#include <cmath>
#include "DataFormat/rawdigit.h"

namespace larlite {
  /**
     \class NoisePerSegment
     User custom analysis class made by kterao
   */
  class NoisePerSegment : public ana_base{
  
  public:

    /// Default constructor
    NoisePerSegment(){ _name="NoisePerSegment"; _fout=0; _nsamples=9550; _minLength=4; _verbose=false; _rmsLow = 0.5; _rmsHigh = 10; };

    /// Default destructor
    virtual ~NoisePerSegment(){};

    /** IMPLEMENT in NoisePerSegment.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in NoisePerSegment.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in NoisePerSegment.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    void setNSamples(size_t n) { _nsamples = n; }

    void SetMap(unsigned int lar_ch,
		short crate,
		short slot,
		short femch);

    void setNoisePerSegmentMinLength(size_t l) {_minLength = l; }
    
    void setVerbose(bool on) { _verbose = on; }
    
    void setRMSLow(double n) { _rmsLow = n; }
    
    void setRMSHigh(double n) { _rmsHigh = n; }

  protected:

    bool _verbose;

    // nimumum number of ticks to create NoisePerSegment region
    size_t _minLength;

    size_t _nsamples;    

    int   _block;
    float _rms, _mean;

    // channel location information
    unsigned int _larch, _crate, _slot, _femch;
    
    // cuts to define low & high noise
    double _rmsLow, _rmsHigh;

    // noisy block amount
    int _highn, _lown, _nblocks, _nBlocks;
    double _fracOK;

    std::vector<int> _ch_to_index;
    std::vector<short> _ch_to_crate;
    std::vector<short> _ch_to_slot;
    std::vector<short> _ch_to_femch;

    std::vector<TTree*> _tree_v;
    TTree* _t_ch;
    TTree* _t_ev;
    int _evtN;

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
