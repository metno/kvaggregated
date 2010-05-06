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

#include "po.h"
#include <paramID.h>
#include <proxy/KvalobsDataAccess.h>
#include <kvalobs/kvDataOperations.h>
#include <algorithm>
#include <cmath>

namespace aggregator
{

po::po() :
		AbstractAggregator(PR, PO)
{
	addAdditionalReadParam(TA);
}

po::~po()
{
}

namespace
{
struct same_obstime_as
{
	const kvalobs::kvData & data;
	same_obstime_as(const kvalobs::kvData & d) : data(d) {}
	bool operator() (const kvalobs::kvData & d) const
	{
		return d.obstime() == data.obstime();
	}
};
struct has_obstime
{
	const miutil::miTime & obstime;
	has_obstime(const miutil::miTime & t) : obstime(t) {}
	bool operator() (const kvalobs::kvData & d) const
	{
		return d.obstime() == obstime;
	}
};

// returns NULL if not found
const kvalobs::kvData * getData(int parameter, const miutil::miTime & obstime, const po::ParameterSortedDataList & observations)
{
	po::ParameterSortedDataList::const_iterator collection = observations.find(parameter);
	if ( collection == observations.end() )
		return 0;
	po::kvDataList::const_iterator data = std::find_if(collection->second.begin(), collection->second.end(), has_obstime(obstime));
	if ( data == collection->second.end() )
		return 0;
	return &* data;
}
}

po::kvDataPtr po::process(const kvalobs::kvData & data, const ParameterSortedDataList & observations)
{
	const kvalobs::kvData * pr = getData(PR, data.obstime(), observations);
	const kvalobs::kvData * ta = getData(TA, data.obstime(), observations);
	if ( ! pr or ! ta )
		return po::kvDataPtr();

	kvalobs::kvDataFactory factory(data);
	float um = getStationMetadata("VS", factory.getMissing(UM_VS));
	float tm = getStationMetadata("VS", factory.getMissing(TM_VS));
	float hp = getStationMetadata("hp", data);

	kvDataPtr ret;
	if ( kvalobs::original_missing(* pr) or kvalobs::original_missing(* ta) )
		ret = kvDataPtr(new kvalobs::kvData(factory.getMissing(PO)));
	else
	{
		float poOriginal = computePo(pr->original(), ta->original(), um, tm ,hp);
		ret = kvDataPtr(new kvalobs::kvData(factory.getData(poOriginal, PO)));
	}

	if ( not kvalobs::valid(* pr) or not kvalobs::valid(* ta) )
		kvalobs::reject(* ret);
	else if ( pr->original() != pr->corrected() or ta->original() != ta->corrected() )
	{
		float poCorrected = computePo(pr->corrected(), ta->corrected(), um, tm ,hp);
		kvalobs::correct(* ret, poCorrected);
	}

	return ret;
}

const po::TimeSpan po::getTimeSpan(const kvalobs::kvData &data) const
{
	miutil::miTime start = data.obstime();
	start.addSec(-1);
	return TimeSpan(start, data.obstime());
}

float po::computePo(float pr, float ta, float um, float tm, float hp) const
{
	double cu = um * ((2.5e-05 * hp) + 0.10701) * 0.0611213 * std::pow((17.5043 * tm), (241.2 + tm));
	double y = (cu + ((0.00325 * hp) + 273.2 + ta)) * 29.29;

	std::cout << "cu = " << cu << "\ny = " << y << std::endl;

	return pr / std::pow(double(hp), y);
}

float po::getStationMetadata(const std::string & metadataName, const kvalobs::kvData & validFor) const
{
	kvservice::KvalobsDataAccess dataAccess;
	return dataAccess.getStationMetadata(metadataName, validFor);
}


}
