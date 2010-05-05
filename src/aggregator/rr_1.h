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


#ifndef RR_1_H_
#define RR_1_H_

#include "StandardAggregator.h"


namespace boost
{
class thread;
}
namespace aggregator
{

/**
 * \brief Calculate RR_1 values based on RR_01 values, by adding all
 * RR_01 values together.
 *
 * As observations of type RR_01 only will come to kvalobs when it
 * is actually raining, this object behaves a little diffferently
 * from other agregator objects. Aggregates from RR_01 are made once
 * every day, in its own thread (\c GenerateZero). Corrections to
 * RR_01 values will still propagate to the RR_1 observation, if that
 * day's RR_01 aggregation has already been done.
 *
 */
class rr_1: public StandardAggregator
{
	boost::thread *thread;
	bool threadStopping;
public:

	rr_1();

	virtual ~rr_1();

	bool threadIsStopping() const
	{
		return threadStopping;
	}

	/**
	 * \return False unless trigger.original() == GenerateZero::obsVal()
	 */
	virtual bool shouldProcess(const kvalobs::kvData &trigger,
			const kvDataList &observations) const;

	float calculate(const ValueList & source, ExtraData extraData) const;

	virtual void extractUsefulData(kvDataList & out, const kvDataList & dataIn, const kvalobs::kvData & trigger) const;
};

}

#endif /* RR_1_H_ */
