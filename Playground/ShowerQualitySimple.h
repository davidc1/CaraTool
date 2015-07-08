/**
 * \file ShowerQualitySimple.h
 *
 * \ingroup ShowerReco3D
 * 
 * \brief Class def header for a class ShowerQualitySimple
 *
 * @author david caratelli
 */

/** \addtogroup ShowerReco3D

    @{*/

#ifndef LARLITE_SHOWERQUALITYSIMPLE_H
#define LARLITE_SHOWERQUALITYSIMPLE_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class ShowerQualitySimple
     User custom analysis class made by SHELL_USER_NAME
   */
  class ShowerQualitySimple : public ana_base{
  
  public:

    /// Default constructor
    ShowerQualitySimple();

    /// Default destructor
    virtual ~ShowerQualitySimple(){}

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);

    virtual bool finalize();

    /// Set producer name
    void setProducerName(std::string name) { _producer_name = name; }

    /// Set minimum energy to use
    void setMinE(double e) { _minE = e; }

  protected:

    std::string _producer_name;

    int _evt;

    double _minE;

    TTree*  _mc_tree;
    int     _pdg_mc;
    double  _e_det_mc;
    double  _x_det_mc;
    double  _y_det_mc;
    double  _z_det_mc;
    double  _px_det_mc;
    double  _py_det_mc;
    double  _pz_det_mc;
    double  _e_sim_mc;
    double  _x_sim_mc;
    double  _y_sim_mc;
    double  _z_sim_mc;
    double  _px_sim_mc;
    double  _py_sim_mc;
    double  _pz_sim_mc;
    double  _containment;


    TTree*  _reco_tree;
    double  _dedx;
    double  _e_reco;
    double  _x_reco;
    double  _y_reco;
    double  _z_reco;
    double  _px_reco;
    double  _py_reco;
    double  _pz_reco;
    int     _pdg_best;
    int     _best_mcs;
    double  _e_det_best;
    double  _x_det_best;
    double  _y_det_best;
    double  _z_det_best;
    double  _px_det_best;
    double  _py_det_best;
    double  _pz_det_best;
    double  _e_sim_best;
    double  _x_sim_best;
    double  _y_sim_best;
    double  _z_sim_best;
    double  _px_sim_best;
    double  _py_sim_best;
    double  _pz_sim_best;
    double  _dir;
    double  _dist;
    
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
