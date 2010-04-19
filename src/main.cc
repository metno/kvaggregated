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
#include <kvcpp/corba/CorbaKvApp.h>
#include "AggregatorRunner.h"
#include "AgregatorHandler.h"
#include "BackProduction.h"
#include "proxy/KvalobsProxy.h"
#include "configuration/AgregatorConfiguration.h"
#include <kvalobs/kvStation.h>
#include <milog/milog.h>
#include <milog/FLogStream.h>
#include <puTools/miClock.h>
#include <set>
#include <fileutil/pidfileutil.h>
#include <kvalobs/kvPath.h>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
//#include <boost/filesystem.hpp>
#include <boost/filesystem/operations.hpp>   // includes path.hpp
#include <boost/filesystem/convenience.hpp>

#include "agregator/minmax.h"
#include "agregator/rr.h"
#include "agregator/rr_1.h"
#include "agregator/rr_12.h"
#include "agregator/rr_24.h"
#include "agregator/ra2rr_12.h"
#include "agregator/ra2rr_12_forward.h"
#include "agregator/ta_24.h"
#include "agregator/uu_24.h"
#include "agregator/nn_24.h"
#include <memory>

using namespace std;
using namespace aggregator;
using namespace miutil;
using namespace milog;
using namespace dnmi::db;

typedef kvservice::corba::CorbaKvApp KvApp;

namespace
{
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

std::auto_ptr<FLogStream> createLog(const std::string & logFileName, milog::LogLevel level, int maxSize)
{
	std::auto_ptr<FLogStream> ret(new FLogStream(9, maxSize));

	boost::filesystem::path logDir = kvPath("logdir");
	boost::filesystem::path logFile = logDir/logFileName;

	if ( not exists(logDir) )
		create_directories(logDir);
	else
		if ( not is_directory(logDir) )
			throw std::runtime_error("Log directory is a file! " + logDir.string());

	ret->open(logFile.string());
	ret->loglevel(level);
	LogManager::instance()->addStream(ret.get());
	return ret;
}

miutil::conf::ConfSection * getConfSection()
{
	string myconf = "kvAgregated.conf";
	miutil::conf::ConfSection * confSec = KvApp::readConf(myconf);
	if (!confSec)
	{
		myconf = kvPath("sysconfdir") + "/kvalobs.conf";
		confSec = KvApp::readConf(myconf);
	}
	if (!confSec)
		throw std::runtime_error("Cant open conf file: " + myconf);
	return confSec;
}

void runThreadWithBackProduction(BackProduction & back, AggregatorRunner & runner)
{
	boost::thread t(back);
	runDaemon(runner);
	t.join();
}

void runAgregator(const AgregatorConfiguration & conf,
		kvservice::proxy::KvalobsProxy & proxy, kvservice::proxy::CallbackCollection & callbacks)
{
	AggregatorRunner runner(conf.stations(), proxy, callbacks);
	if ( conf.backProduction() )
	{
		BackProduction back(proxy, callbacks, runner, conf.backProductionSpec());
		if (!conf.daemonMode())
			back(); // run outside a thread
		else
			runThreadWithBackProduction(back, runner);
	}
	else
	{
		// even if no backproduction was set, we want to generate data for 3
		// hours back in time
		miutil::miTime to(miutil::miDate::today(), miutil::miClock(	miutil::miClock::oclock().hour(), 0, 0));
		miutil::miTime from(to);
		from.addHour(-3);

		BackProduction back(proxy, callbacks, runner, from, to);
		runThreadWithBackProduction(back, runner);
	}
}

}

int main(int argc, char **argv)
{
	AgregatorConfiguration conf;
	AgregatorConfiguration::ParseResult result = conf.parse(argc, argv);

	if (result != AgregatorConfiguration::No_Action)
		return result;


	try
	{
		// Logging
		milog::Logger::logger().logLevel( milog::INFO );
		std::auto_ptr<FLogStream> fine = createLog("kvAgregated.log", INFO, 1024 * 1024);
		std::auto_ptr<FLogStream> error = createLog("kvAgregated.warn.log", INFO, 100 * 1024);

		try
		{
			// PidFile
			dnmi::file::PidFileHelper pidFile;
			if (conf.runInDaemonMode())
				setupPidFile(pidFile);

			// KvApp
			boost::scoped_ptr<miutil::conf::ConfSection> confSec(getConfSection());
			KvApp app(argc, argv, confSec.get());

			// Proxy database
			kvservice::proxy::CallbackCollection callbacks;
			LOGINFO("Using proxy database <" << conf.proxyDatabaseName() << ">");
			kvservice::proxy::KvalobsProxy proxy(conf.proxyDatabaseName(), callbacks, conf.repopulateDatabase());

			AgregatorHandler handler(callbacks, proxy);
			handler.setParameterFilter(conf.parameters());
			handler.setStationFilter(conf.stations());
			handler.setTypeFilter(conf.types());

			// Standard times
			set<miClock> six;
			six.insert(miClock(6, 0, 0));
			six.insert(miClock(18, 0, 0));

			// Add handlers
			MinMax tan12 = min(TAN, TAN_12, 12, six);
			handler.addHandler(&tan12);
			MinMax tax12 = max(TAX, TAX_12, 12, six);
			handler.addHandler(&tax12);
			MinMax tgn12 = min(TGN, TGN_12, 12, six);
			handler.addHandler(&tgn12);
			boost::scoped_ptr<rr_1> rr1(conf.runInDaemonMode() ? new rr_1 : (rr_1 *) 0);
			if ( rr1 )
				handler.addHandler(rr1.get());
			rr_12 rr12 = rr_12();
			handler.addHandler(&rr12);
			rr_24 rr24 = rr_24();
			handler.addHandler(&rr24);
			ra2rr_12_backward ra2rr_b;
			handler.addHandler(&ra2rr_b);
			ra2rr_12_forward ra2rr_f;
			handler.addHandler(&ra2rr_f);
			ta_24 ta24;
			handler.addHandler(& ta24);
			uu_24 uu24;
			handler.addHandler(& uu24);
			nn_24 nn24;
			handler.addHandler(& nn24);

			try
			{
				runAgregator(conf, proxy, callbacks);
			}
			catch (std::exception & e)
			{
				LOGFATAL(e.what());
				return 1;
			}
		}
		catch ( std::exception & e )
		{
			LOGFATAL(e.what());
			return 1;
		}
	}
	catch ( std::exception & e )
	{
		std::cout << e.what() << std::endl;
		return 1;
	}
}
