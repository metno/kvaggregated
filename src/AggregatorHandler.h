/*
 Kvalobs - Free Quality Control Software for Meteorological Observations

 $Id: AggregatorHandler.h,v 1.1.2.3 2007/09/27 09:02:15 paule Exp $

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
#ifndef __agregator__AgregatorHandler_h__
#define __agregator__AgregatorHandler_h__

#include "aggregator/AbstractAggregator.h"
#include "proxy/Callback.h"
#include "proxy/DataAccess.h"
#include "metrics.h"
#include <map>


namespace aggregator
{
class GenerateZero;

class AggregatorHandler: public kvservice::proxy::Callback
{
	static AggregatorHandler *agHandler;
	friend class aggregator::GenerateZero;
protected: 
	Metrics *metrics;
public:
	
	AggregatorHandler(kvservice::proxy::CallbackCollection & callbacks, kvservice::DataAccess &dataAccess);
	virtual ~AggregatorHandler();

	virtual void newData(kvservice::KvDataList &data, Metrics &metrics);

	void process(kvservice::KvDataList & out, const kvalobs::kvData & data);

	void save(const kvservice::KvDataList & dataList);

	void setParameterFilter(const std::vector<int> & allowedParameters)
	{
		allowedParameters_ = allowedParameters;
	}

	void setStationFilter(const std::vector<int> & allowedStations)
	{
		allowedStations_ = allowedStations;
	}

	void setTypeFilter(const std::vector<int> & allowedTypes)
	{
		allowedTypes_ = allowedTypes;
	}


	void addHandler(AbstractAggregator *handler);

private:

	void getRelevantObsList(
			AbstractAggregator::ParameterSortedDataList & out,
			const AbstractAggregator & user,
			const kvalobs::kvData & data,
			const AbstractAggregator::TimeSpan & obsTimes) const;

private:
	typedef std::multimap<int, AbstractAggregator *> HandlerMap;

	kvservice::DataAccess & dataAccess_;

	HandlerMap handlers;

	std::vector<int> allowedParameters_;
	std::vector<int> allowedStations_;
	std::vector<int> allowedTypes_;
};
}

#endif // __agregator__AgregatorHandler_h__
