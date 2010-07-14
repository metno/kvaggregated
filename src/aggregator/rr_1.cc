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

#include "rr_1.h"
#include "times.h"
#include "paramID.h"
#include "GenerateZero.h"
#include <kvalobs/kvDataOperations.h>
#include <milog/milog.h>
#include <boost/thread/thread.hpp>

using namespace miutil;
using namespace kvalobs;


namespace aggregator
{
rr_1::rr_1(bool startThread) :
	StandardAggregator(RR_01, RR_1, 1, allHours), thread(0), threadStopping(false)
{
	if ( startThread )
	{
		GenerateZero g0(*this);
		thread = new boost::thread(g0);
	}
}

rr_1::~rr_1()
{
	threadStopping = true;
	if ( thread )
	{
		LOGDEBUG("Stopping RR_1 autogeneration");
		thread->join();
		delete thread;
	}
}

bool rr_1::shouldProcess(const kvData &trigger, const kvDataList &observations) const
{
	return true;
//	// Will only generate when receiving values from GenerateZero thread
//	bool generate = (trigger.original() == GenerateZero::obsVal());
//	if (not generate)
//	{
//		const miTime & obstime = trigger.obstime();
//		generate = obstime.date() != miDate::today() or (obstime.clock()
//				<= miClock(6, 0, 0) and miClock::oclock()
//				> GenerateZero::genClock);
//	}
//	return generate;
}

float rr_1::calculate(const ValueList & source, ExtraData ) const
{
	float sum = 0;
	for ( ValueList::const_iterator it = source.begin(); it != source.end(); ++ it )
		if ( * it > 0 )
			sum += * it;
	return sum;
}

void rr_1::extractUsefulData(kvDataList & out, const kvDataList & dataIn, const kvalobs::kvData & trigger) const
{
	out = dataIn;
}

}