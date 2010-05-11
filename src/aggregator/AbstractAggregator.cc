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

#include "AbstractAggregator.h"
#include <kvalobs/kvDataOperations.h>

namespace aggregator
{

AbstractAggregator::AbstractAggregator(int readParam, int writeParam) :
		write_param(writeParam)
{
	read_param.push_back(readParam);
}

AbstractAggregator::~AbstractAggregator()
{
}

namespace
{
float round(float f)
{
	if ( f < 0 )
		f -= 0.05;
	else
		f += 0.05;
	f *= 10;
	f = int(f);
	return f / 10.0;
}
}
AbstractAggregator::kvDataPtr AbstractAggregator::getDataObject(const kvalobs::kvData &trigger,
		const miutil::miTime &obsTime, float original, float corrected, const kvalobs::kvUseInfo & ui)
{
	int typeID = trigger.typeID();
	if (typeID > 0)
		typeID *= -1;

	kvalobs::kvDataFactory f(trigger.stationID(), obsTime, typeID, trigger.sensor(),
			trigger.level());

	kvDataPtr ret;
	if ( original == invalidParam )
		ret = kvDataPtr(new kvalobs::kvData(f.getMissing(writeParam())));
	else
	{
		original = round(original);
		ret = kvDataPtr(new kvalobs::kvData(f.getData(original, writeParam())));
	}

	if (corrected == invalidParam)
		kvalobs::reject(* ret);
	else
	{
		corrected = round(corrected);
		if (original != corrected)
			kvalobs::correct(* ret, corrected);
	}

	ret->useinfo(ui);

	return ret;
}

}
