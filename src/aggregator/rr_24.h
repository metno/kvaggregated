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


#ifndef RR_24_H_
#define RR_24_H_

#include "rr.h"

namespace aggregator
{

/**
 * A specialisation of rr, for calculating RR_24, based on RR_12.
 */
class rr_24: public rr
{
public:
	/**
	 * \brief Constructor.
	 */
	rr_24();

	/**
	 * \brief Determine if enough data has been received in order to
	 * create an agregate.
	 *
	 * \param trigger The piece of data which triggered the call to
	 * this object.
	 *
	 * \param observations The list returned by \a getRelevantObsList.
	 *
	 * \return true if exactly two relevant observations has been
	 * found. False otherwise.
	 */
	virtual bool shouldProcess(const kvalobs::kvData &trigger,
			const ParameterSortedDataList &observations) const;

	virtual void extractUsefulData(kvDataList & out, const kvDataList & dataIn, const kvalobs::kvData & trigger) const;
};
}

#endif /* RR_24_H_ */
