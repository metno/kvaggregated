/*
  Kvalobs - Free Quality Control Software for Meteorological Observations 

  Copyright (C) 2010 met.no

  Contact information:
  Norwegian Meteorological Institute
  Box 43 Blindern
  0313 OSLO
  NORWAY
  email: kvalobs-dev@met.no

  This file is part of KVALOBS

  KVALOBS is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as 
  published by the Free Software Foundation; either version 2 
  of the License, or (at your option) any later version.
  
  KVALOBS is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.
  
  You should have received a copy of the GNU General Public License along 
  with KVALOBS; if not, write to the Free Software Foundation Inc., 
  51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef RA2RR_12_FORWARD_H_
#define RA2RR_12_FORWARD_H_

#include "ra2rr_12.h"

namespace aggregator
{
/**
 * \brief Calculates RR_12 for the time after observation.
 *
 * If a RA observation arrives more than 12 hours too late, or it is
 * corrected more than 12 hours after its validity, recalculations
 * must be made forwards as well as backwards in time.
 *
 * This class handles agregation forward in time (compared to the
 * incoming observation).
 */
class ra2rr_12_forward : public ra2rr_12
{
public:
  ra2rr_12_forward( );

  virtual const TimeSpan getTimeSpan( const kvalobs::kvData &data ) const;

  virtual int timeOffset() const { return 12; }
};

}

#endif /* RA2RR_12_FORWARD_H_ */
