/**
 * \file KazuAna.h
 *
 * \ingroup Playground
 * 
 * \brief Class def header for a class KazuAna
 *
 * @author david
 */

/** \addtogroup Playground

    @{*/
#ifndef LARLITE_KAZUANA_H
#define LARLITE_KAZUANA_H

#include <iostream>
#include <vector>
#include <TChain.h>
/**
   \class KazuAna
   User defined class KazuAna ... these comments are used to generate
   doxygen documentation!
 */
class KazuAna{

public:

  /// Default constructor
  KazuAna(){};

  /// Default destructor
  virtual ~KazuAna(){};

  void AnaFile(std::string fname);

};

#endif
/** @} */ // end of doxygen group 

