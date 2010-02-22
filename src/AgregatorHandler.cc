/*
 Kvalobs - Free Quality Control Software for Meteorological Observations

 $Id: AgregatorHandler.cc,v 1.1.2.5 2007/09/27 09:02:15 paule Exp $

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
#include "AgregatorHandler.h"
#include "KvDataFunctors.h"
#include "paramID.h"
#include <kvalobs/kvDataOperations.h>
#include <milog/milog.h>

using namespace kvservice;
using namespace kvservice::proxy;
using namespace milog;

namespace agregator
{
AgregatorHandler *AgregatorHandler::agHandler = 0;

AgregatorHandler::AgregatorHandler(KvalobsProxy & proxy) :
	Callback(proxy.getCallbackCollection()), proxy_(proxy)
{
	if (!agHandler)
		agHandler = this;
	else
		throw std::logic_error(
				"There can only be one instance of the AgregatorHandler at the same time");
}

AgregatorHandler::~AgregatorHandler()
{
	if (agHandler == this)
		agHandler = 0;
}

void AgregatorHandler::addHandler(AbstractAgregator * handler)
{
	LOGINFO("Adding handler: " << handler->readParam() << " -> "
			<< handler->writeParam());
	proxy_.addInteresting(handler->readParam());
	proxy_.addInteresting(handler->writeParam());
	handlers.insert(Handler(handler->readParam(), handler));
}

#ifdef AGREGATOR_DEBUG
struct largeStationNo
{
	bool operator() ( const kvalobs::kvData & data ) const
	{
		return data.stationID() > 99;
	}
};
#endif // AGREGATOR_DEBUG

void AgregatorHandler::newData(KvDataList &data)
{
#ifdef AGREGATOR_DEBUG
	data.remove_if( largeStationNo() );
#endif // AGREGATOR_DEBUG
	for (IKvDataList d = data.begin(); d != data.end(); d++)
	{
		if (d->paramID() == RA)
		{
			LOGDEBUG("Found RA parameter in station " << d->stationID()
					<< ", type " << d->typeID()
					<< ". Ignoring all RR_1 observations from this station.");
			StationHasParamid shp(RR_1, &*d);
			data.remove_if(shp);
			break;
		}
	}

	kvservice::KvDataList toSave;
	for (CIKvDataList dl = data.begin(); dl != data.end(); ++dl)
		process(toSave, *dl);
	save(toSave);
}

void AgregatorHandler::process(kvservice::KvDataList & out, const kvalobs::kvData & data)
{
	const int paramID = data.paramID();

	if ( ! allowedParameters_.empty() &&
		find(allowedParameters_.begin(), allowedParameters_.end(), paramID) == allowedParameters_.end())
		return;

	HandlerMap::const_iterator it = handlers.lower_bound(paramID);
	const HandlerMap::const_iterator end = handlers.upper_bound(paramID);
	while (it != end)
	{
		try
		{
			LOGDEBUG("Processing:\n" << decodeutility::kvdataformatter::createString(data));

			//it->second->process( data );
			AbstractAgregator * agregator = it->second;
			if (agregator->isInterestedIn(data))
			{
				std::auto_ptr<kvalobs::kvData> d =
						agregator->process(data, getRelevantObsList(data, agregator->getTimeSpan(data)));

				if ( d.get() )
				{
					//save(*d);
					out.push_back(* d);
					LOGDEBUG("Processing done - not saved yet");
					process(out, * d);
				}
			}
			++it;
		} catch (exception & e)
		{
			LOGFATAL(typeid( e ).name() << ":\n\t" << e.what());
			throw ;
		}
		catch ( ... )
		{
			LOGFATAL( "Unknown exception" );
			throw;
		}
	}
}

void AgregatorHandler::save( const kvservice::KvDataList & dl )
{
	CKvalObs::CDataSource::Result_var res = proxy_.sendData( dl );

	if ( res->res != CKvalObs::CDataSource::OK )
	{
		std::ostringstream ss;
		ss << "Error when submitting data: " << res->message << '\n'
		<< decodeutility::kvdataformatter::createString( dl );
		const std::string msg = ss.str();
		throw std::runtime_error( msg );
	}
}

namespace
{
	struct assertObsTimeMatches
	{
		const AbstractAgregator::TimeSpan & obsTimes_;
		assertObsTimeMatches(const AbstractAgregator::TimeSpan & obsTimes ) : obsTimes_(obsTimes)
		{}
		void operator () (const kvalobs::kvData & d)
		{
			if ( d.obstime() <= obsTimes_.first || d.obstime() > obsTimes_.second )
			{
				std::ostringstream errMsg;
				errMsg << d.obstime()<<" is not in range ("<<obsTimes_.first<<", "<<obsTimes_.second<<"]";
				throw std::logic_error(errMsg.str());
			}
		}
	};
}

list<kvalobs::kvData>
AgregatorHandler::getRelevantObsList( const kvalobs::kvData & data,
		const AbstractAgregator::TimeSpan & obsTimes ) const
{
	list<kvalobs::kvData> ret;

	proxy_.getData( ret, data.stationID(), obsTimes.first, obsTimes.second,
			data.paramID(), data.typeID(), data.sensor(), data.level() );

	for_each(ret.begin(), ret.end(), assertObsTimeMatches(obsTimes));

	list<kvalobs::kvData>::iterator find = std::find_if(ret.begin(), ret.end(), std::bind1st(kvalobs::compare::same_kvData(), data));
	if ( find == ret.end() )
		ret.push_back(data);
	else
		* find = data;

	return ret;
}

}
