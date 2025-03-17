/* Kvalobs - Free Quality Control Software for Meteorological Observations

 $Id: main.cc,v 1.4.2.9 2007/09/27 09:02:15 paule Exp $

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
#include <kvcpp/KvApp.h>
#include <kvalobs/kvPath.h>
#include <decodeutility/kvalobsdataserializer.h>
#include <decodeutility/kvalobsdata.h>
#include <kvsubscribe/DataSubscriber.h>
#include <miutil/getprogname.h>
#include "AggregatorRunner.h"
#include "AggregatorHandler.h"
#include "BackProduction.h"
#include "proxy/KvalobsProxy.h"
#include "configuration/AggregatorConfiguration.h"
#include <kvalobs/kvStation.h>
#include <milog/milog.h>
#include <milog/FLogStream.h>
#include <set>
#include <fileutil/pidfileutil.h>
#include <kvalobs/kvPath.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
//#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>   // includes path.hpp
#include <boost/filesystem/convenience.hpp>
//#include <decodeutility.h>
#include "aggregator/minmax.h"
#include "aggregator/rr.h"
#include "aggregator/rr_1.h"
#include "aggregator/rr_12.h"
#include "aggregator/rr_24.h"
#include "aggregator/ra2rr_12.h"
#include "aggregator/ra2rr_12_forward.h"
#include "aggregator/ta_24.h"
#include "aggregator/uu_24.h"
#include "aggregator/nn_24.h"
#include "aggregator/po.h"
#include "aggregator/ot_24.h"
#include <memory>
#include "metrics.h"

using namespace std;
using namespace aggregator;
//using namespace miutil;
using namespace milog;
using namespace dnmi::db;

using kvservice::KvApp;


namespace
{
void myDebug(const std::string &message, const kvalobs::serialize::KvalobsData &d ){
	LOGDEBUG("DataSubscribe incomming : [\n" << message << "]");
}

void setupPidFile(dnmi::file::PidFileHelper & pidFile)
{
	//PID-file
	std::string pidFileName = dnmi::file::createPidFileName(kvPath("rundir"), "kvAgregated");

	bool pidfileError;
	if (dnmi::file::isRunningPidFile(pidFileName, pidfileError))
	{
		if (pidfileError)
		{
			std::ostringstream msg;
			msg << "An error occured while reading the pidfile " <<
					pidFileName	<< ". Remove the file if it exist and "
					"<agregate> is not running. If it is running and there are "
					"problems. Kill <agregate> and restart it.";
			throw std::runtime_error(msg.str());
		}
		else
		{
			std::ostringstream msg;
			msg << "Is <agregate> allready running? If not remove the pidfile: " << pidFileName;
			throw std::runtime_error(msg.str());;
		}
	}
	pidFile.createPidFile(pidFileName);
}

void runDaemon(AggregatorRunner & runner)
{
	runner.start_thread();

	LOGINFO("Starting main loop");
	KvApp::kvApp->run();
}

void createLog(const std::string & logFileName, milog::LogLevel level, int maxSize)
{
	std::unique_ptr<FLogStream> ret(new FLogStream(9, maxSize));

	boost::filesystem::path logDir = kvPath("logdir");
	boost::filesystem::path logFile = logDir/logFileName;

	if ( not exists(logDir) )
		create_directories(logDir);
	else
		if ( not is_directory(logDir) )
			throw std::runtime_error("Log directory is a file! " + logDir.string());

	ret->open(logFile.string());
	ret->loglevel(level);
	LogManager::instance()->addStream(ret.release());
}

void runThreadWithBackProduction(BackProduction & back, AggregatorRunner & runner)
{
	boost::thread t(back);
	runDaemon(runner);
	t.join();
}

void runAgregator(const AggregatorConfiguration & conf,
		kvservice::DataAccess & dataAccess, kvservice::proxy::CallbackCollection & callbacks)
{
	if ( conf.backProduction() )
	{
		BackProduction back(callbacks, nullptr, conf.backProductionSpec(), conf.stations());
		back(); // run outside a thread
		return;
	}

	// even if no backproduction was set, we want to generate data for 3
	// hours back in time
	boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();
	boost::posix_time::ptime to(now.date(), boost::posix_time::hours(now.time_of_day().hours()));
	boost::posix_time::ptime from = to - boost::posix_time::hours(3);

	AggregatorRunner runner(conf.stations(), dataAccess, callbacks);
	BackProduction back(callbacks, &runner, from, to);
	runThreadWithBackProduction(back, runner);
	// runDaemon(runner);
}
}

int main(int argc, char **argv)
{
	kvalobs::serialize::KvalobsDataSerializer::defaultProducer="kvAgregated";
	AggregatorConfiguration conf;
	AggregatorConfiguration::ParseResult result = conf.parse(argc, argv);

	kvalobs::subscribe::DataSubscriber::setDebugWriter(myDebug);

	if (result != AggregatorConfiguration::No_Action)
		return result;

	try {
		// Logging
		milog::Logger::logger().logLevel( milog::INFO );
		//milog::Logger::logger().logLevel( milog::DEBUG );
		if ( not conf.logToStdOut() ) 
		{
			// createLog("kvAgregated.log", DEBUG, 1024 * 1024);
			// createLog("kvAgregated.warn.log", DEBUG, 100 * 1024);
			createLog("kvAgregated.log", ERROR, 1024 * 1024);
			createLog("kvAgregated.warn.log", WARN, 100 * 1024);
		}

		// PidFile
		dnmi::file::PidFileHelper pidFile;
		if ( !conf.backProduction() ) {
			setupPidFile(pidFile);
			setMetricsLogfile("kvAgregated_metrics.log", kvalobs::kvPath(kvalobs::logdir));
		}

		// KvApp
		LOGINFO("Programname used in configuration lookup: '" << miutil::getProgramName()<<"'");
		std::unique_ptr<kvservice::KvApp> app(kvservice::KvApp::create(miutil::getProgramName(), argc, argv));

		// Proxy database
		kvservice::proxy::CallbackCollection callbacks;

		// std::unique_ptr<kvservice::DataAccess> dataAccess(new kvservice::KvalobsDataAccess);

		std::unique_ptr<kvservice::DataAccess> dataAccess;
		if ( conf.proxyDatabaseName() != "" )
		{
			if ( conf.backProduction() )
			{
				LOGFATAL("Cannot use proxy database in backproduction mode");
				return 1;
			}
			LOGINFO("Using proxy database <" << conf.proxyDatabaseName() << ">");
			dataAccess.reset(
					new kvservice::proxy::KvalobsProxy(conf.proxyDatabaseName(), conf.repopulateDatabase())
			);
		}
		else
		{
			LOGINFO("Using no proxy database");
			dataAccess.reset(new kvservice::KvalobsDataAccess);
		}

		AggregatorHandler handler(callbacks, * dataAccess);
		handler.setParameterFilter(conf.parameters());
		handler.setStationFilter(conf.stations());
		handler.setTypeFilter(conf.types());

		// Standard times
		set<boost::posix_time::time_duration> six;
		six.insert(boost::posix_time::hours(6));
		six.insert(boost::posix_time::hours(18));

		// Add handlers
		MinMax tan12 = min(TAN, TAN_12, 12, six);
		handler.addHandler(&tan12);
		MinMax tax12 = max(TAX, TAX_12, 12, six);
		handler.addHandler(&tax12);
		MinMax tgn12 = min(TGN, TGN_12, 12, six);
		handler.addHandler(&tgn12);
		rr_1 rr1;
		handler.addHandler(&rr1);
		rr_12 rr12;
		handler.addHandler(&rr12);
		rr_24 rr24;
		handler.addHandler(&rr24);

		ra2rr_12_backward ra2rr_b;
		handler.addHandler(&ra2rr_b);
		ra2rr_12_forward ra2rr_f;
		handler.addHandler(&ra2rr_f);

		ta_24 ta24(& * dataAccess);
		handler.addHandler(& ta24);
		uu_24 uu24;
		handler.addHandler(& uu24);

		nn_24 nn24;
		handler.addHandler(& nn24);

		po p(* dataAccess);
		handler.addHandler(& p);

		ot_24 ot24;
		handler.addHandler(& ot24);

		runAgregator(conf, * dataAccess, callbacks);
	}
	catch ( std::exception & e ) 
	{
		LOGFATAL(e.what());
		return 1;
	}
}
