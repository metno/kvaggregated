/*
  Kvalobs - Free Quality Control Software for Meteorological Observations 

  $Id: ra2rr_12.h,v 1.1.2.5 2007/09/27 09:02:16 paule Exp $                                                       

  Copyright (C) 2007 met.no

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
#ifndef __agregator_ra2rr_12_h__
#define __agregator_ra2rr_12_h__

#include "AbstractAggregator.h"
#include "rr.h"

namespace aggregator
{
/**
 * \brief Calculate RR_12, based on RA values.
 *
 * The calculation will tolerate calculation yielding a small amount
 * of negative rain, reporting it as no rain. This is to allow for
 * evaporation or lack of precision in the measuring instruments.
 */
class ra2rr_12 : public AbstractAggregator
{
public:

    /**
     * \brief Constructor.
     */
    ra2rr_12( );

    /**
     * \brief Determine if enough data has been received in order to
     * create an agregate.
     *
     * We will agregate an observation if the observation is valid for
     * 6 o'clock (am or pm) and we have available data for twelve
     * hours ago.
     *
     * \param trigger The piece of data which triggered the call to
     * this object.
     *
     * \param observations The list returned by \a getRelevantObsList.
     *
     * \return True if we should proceed with calculating an agregate,
     * False otherwise.
     */
    virtual bool shouldProcess( const kvalobs::kvData &trigger,
                                const kvDataList &observations );

protected:
    virtual int timeOffset() const { return -12; }

	virtual float calculate(const std::vector<float> & source, const kvalobs::kvData & trigger) const;

	virtual void extractUsefulData(kvDataList & out, const kvDataList & dataIn, const kvalobs::kvData & trigger) const;
};


/**
 * \brief Distinguishes \a ra2rr_12 from \a ra2rr_12_forward
 */
typedef ra2rr_12 ra2rr_12_backward;

}

#endif // __agregator_ra2rr_12_h__
