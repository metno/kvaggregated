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

#include "AggregatorRunner.h"
#include "proxy/CallbackCollection.h"
#include <kvcpp/kvevents.h>
#include <kvcpp/KvApp.h>
#include <milog/milog.h>
#include <boost/scoped_ptr.hpp>

AggregatorRunner::AggregatorRunner(const std::vector<int> & stations, kvservice::DataAccess & dataAccess,
		kvservice::proxy::CallbackCollection & callbacks) :
	incomingHandler(dataAccess, callbacks)
{
    assert( kvservice::KvApp::kvApp );

    LOGINFO( "Subscribing to data from source" );

    kvservice::KvDataSubscribeInfoHelper sih;
    for ( std::vector<int>::const_iterator it = stations.begin(); it != stations.end(); ++ it ){
			sih.addStationId( * it );
		}
		
		kvservice::KvApp::kvApp->subscribeData( sih, queue );
}

AggregatorRunner::~AggregatorRunner()
{
}

void AggregatorRunner::run()
{
	milog::LogContext context("AgregatorRunner::run (main loop)");
	LOGDEBUG( "Running" );
	try
	{
		while (not stopping())
			processData();
	}
	catch (std::exception & e)
	{
		LOGFATAL( e.what() );
		stop();
	}
	catch (...)
	{
		LOGFATAL( "Unknown exception!" );
		stop();
	}
}

void AggregatorRunner::onStop()
{
	LOGDEBUG( "Stopping AgregatorRunner thread" );
	incomingHandler.stopThreads();
}

void AggregatorRunner::processData()
{
	boost::scoped_ptr<dnmi::thread::CommandBase> base(queue.get(1));

	if ( ! base )
		return;
	kvservice::DataEvent *data =
			dynamic_cast<kvservice::DataEvent *> (base.get());

	if (!data)
	{
		LOGERROR( "Could not understand data received from kvalobs" );
		return;
	}
	
	data->dispatchEvent(incomingHandler);
}
