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
#include <proxy/KvalobsProxy.h>
#include <kvalobs/kvDataOperations.h>
#include <milog/milog.h>
#include <algorithm>
#include <cmath>

namespace aggregator
{

po::po(const kvservice::DataAccess & dataAccess) :
		AbstractAggregator(PR, PO),
		dataAccess_(dataAccess)
{
	addAdditionalReadParam(TA);
	addAdditionalReadParam(PO);
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
	const boost::posix_time::ptime & obstime;
	has_obstime(const boost::posix_time::ptime & t) : obstime(t) {}
	bool operator() (const kvalobs::kvData & d) const
	{
		return d.obstime() == obstime;
	}
};

// returns NULL if not found
const kvalobs::kvData * getData(int parameter, const boost::posix_time::ptime & obstime, const po::ParameterSortedDataList & observations)
{
	po::ParameterSortedDataList::const_iterator collection = observations.find(parameter);
	if ( collection == observations.end() )
		return 0;

	const po::kvDataList & data = collection->second;
	po::kvDataList::const_iterator ret = std::find_if(data.begin(), data.end(), has_obstime(obstime));
	if ( ret == data.end() )
		return 0;
	return &* ret;
}
}

po::kvDataPtr po::process(const kvalobs::kvData & data,
		const ParameterSortedDataList & observations)
{
	kvservice::KvDataList dataList;
	dataAccess_.getData(dataList, data.stationID(), data.obstime(), data.obstime(), PO, - std::abs(data.typeID()), data.sensor(), data.level());
	AbstractAggregator::ParameterSortedDataList alreadyAggregated;
	for ( kvservice::KvDataList::const_iterator it = dataList.begin(); it != dataList.end(); ++ it )
		alreadyAggregated[it->paramID()].push_back(* it);

	return process(data, observations, alreadyAggregated);
}

po::kvDataPtr po::process(const kvalobs::kvData & data, const ParameterSortedDataList & observations, const ParameterSortedDataList & previouslyAggregatedData)
{
	po::kvDataPtr ret = process_(data, observations, 1);

	if ( ! ret )
		return ret;


	const kvalobs::kvData * aggregatedPo = getData(PO, data.obstime(), previouslyAggregatedData);

	if ( not aggregatedPo ) // if we have _not_ sent this data to kvalobs before
	{
		const kvalobs::kvData * po = getData(PO, data.obstime(), observations);
		if ( po ) // and the observation contained its own PO observation
		{
			// check if the aggregated and reported values are very similar.
			const float WRITE_TRESHOLD = 0.305;
			if ( std::fabs(po->corrected() - ret->corrected()) < WRITE_TRESHOLD and
					std::fabs(po->original() - ret->original()) < WRITE_TRESHOLD )
			{
				// If they are, we will not send any data to kvalobs.
				LOGINFO("No essential difference between reported and calculated PO. Will not report aggregated value:\n"
						"From station: " << po->corrected() << ". Calculated: " << ret->corrected());
				return po::kvDataPtr();
			}
		}
	}
	return ret;
}

po::kvDataPtr po::processMethod2(const kvalobs::kvData & data, const ParameterSortedDataList & observations)
{
	return process_(data, observations, 2);
}

po::kvDataPtr po::process_(const kvalobs::kvData & data, const ParameterSortedDataList & observations, int method)
{
	milog::LogContext context("PO aggregation");

	try
	{
		const kvalobs::kvData * pr = getData(PR, data.obstime(), observations);
		const kvalobs::kvData * ta = getData(TA, data.obstime(), observations);

		if ( pr and ta )
		{
			kvalobs::kvDataFactory factory(data);
			float um = getStationMetadata("VS", factory.getMissing(UM_VS));
			float tm = getStationMetadata("VS", factory.getMissing(TM_VS));
			float hp = getStationMetadata("hp", data);

			float original = invalidParam;
			float corrected = invalidParam;

			if ( not kvalobs::original_missing(* pr) and not kvalobs::original_missing(* ta) )
			{
				original =
						method == 2 ?
								computePoWithInversionCorrection(pr->original(), ta->original(), um, tm ,hp) :
								computePo(pr->original(), ta->original(), um, tm ,hp);
			}

			if ( kvalobs::valid(* pr) and kvalobs::valid(* ta) )
			{
				corrected = original;
				if ( pr->original() != pr->corrected() or ta->original() != ta->corrected() )
				{
					corrected =
							method == 2 ?
									computePoWithInversionCorrection(pr->corrected(), ta->corrected(), um, tm ,hp) :
									computePo(pr->corrected(), ta->corrected(), um, tm ,hp);
				}
			}

			std::list<kvalobs::kvData> sourceData;
			sourceData.push_back(* pr);
			sourceData.push_back(* ta);

			AbstractAggregator::kvDataPtr ret = getDataObject(data, data.obstime(), original, corrected, sourceData);
			return ret;
		}
		else
			LOGDEBUG("Missing complete data for PO generation");
	}
	catch ( std::exception & e )
	{
		LOGERROR(e.what());
	}
	return po::kvDataPtr();
}

const po::TimeSpan po::getTimeSpan(const kvalobs::kvData &data) const
{
	boost::posix_time::ptime start = data.obstime() - boost::posix_time::seconds(1);
	return TimeSpan(start, data.obstime());
}

float po::computePo(float pr, float ta, float um, float tm, float hp) const
{
	const double e = 2.718281828;
	double cu = um * ((2.5e-05 * hp) + 0.10701) * 0.0611213 * std::pow(e, (17.5043 * tm) / (241.2 + tm));
	double y = (cu + (0.00325 * hp) + 273.2 + ta) * 29.29;
	return pr / std::pow(e, double(hp) / y);
}

float po::computePoWithInversionCorrection(float pr, float ta, float um, float tm, float hp) const
{
	if ( ta < 1.5 )
	{
		const double e = 2.718281828;
		double cu = um * ((2.5e-05 * hp) + 0.10701) * 0.0611213 * std::pow(e, (17.5043 * tm) / (241.2 + tm));
		double y = (cu + (0.00325 * hp) + 273.2 + (ta *0.315) + 1) * 29.29;
		return pr / std::pow(e, double(hp) / y);
	}
	else
		return computePo(pr, ta, um, tm, hp);
}


float po::getStationMetadata(const std::string & metadataName, const kvalobs::kvData & validFor) const
{
	return dataAccess_.getStationMetadata(metadataName, validFor);
}

}
