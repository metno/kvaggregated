#include "BackProduction.h"
#include "WorkLoop.h"
#include "proxy/KvDataReceiver.h"
#include "proxy/KvalobsProxy.h"
#include <kvcpp/KvApp.h>
#include <milog/milog.h>
#include <boost/lexical_cast.hpp>
#include <stdexcept>

using namespace std;

BackProduction::BackProduction(kvservice::proxy::KvalobsProxy & proxy,
		kvservice::proxy::CallbackCollection & callbacks,
		const WorkLoop & mainLoop, const miutil::miTime & from,
		const miutil::miTime & to) :
	proxy_(proxy), callbacks_(callbacks), mainLoop_(mainLoop), from_(from), to_(to)
{
}

BackProduction::BackProduction(kvservice::proxy::KvalobsProxy & proxy,
		kvservice::proxy::CallbackCollection & callbacks,
		const WorkLoop & mainLoop, const std::string & timeSpec) :
	proxy_(proxy), callbacks_(callbacks), mainLoop_(mainLoop)
{
	const string::size_type sep = timeSpec.find_first_of(',');
	if (sep == string::npos)
	{
		from_.setTime(timeSpec);
		if (from_.undef())
			throw std::logic_error("Invalid specification: " + timeSpec);
		to_ = from_;
	}
	else
	{
		string from = timeSpec.substr(0, sep);

		from_.setTime(from);
		if (from_.undef())
			throw std::logic_error("Invalid from specification: " + from);

		const string::size_type nextWord = sep + 1;
		if (nextWord == timeSpec.size())
			throw std::logic_error("Invalid specification: " + timeSpec);

		const std::string to = timeSpec.substr(nextWord);
		try
		{
			unsigned duration = boost::lexical_cast<unsigned>(to);
			to_ = from_;
			to_.addHour(duration);
		}
		catch (boost::bad_lexical_cast &)
		{
			to_.setTime(to);
		}

		if (to_.undef())
			throw std::logic_error("Invalid to specification: " + to);
	}
}

BackProduction::~BackProduction()
{
}

void BackProduction::operator ()()
{
	miutil::miTime f(from_);

	while (!mainLoop_.stopping() && f <= to_)
	{
		processData(f);
		LOGINFO("Done processing data for time " << f);
		f.addHour(1);
	}
}

void BackProduction::processData(const miutil::miTime & time)
{
	milog::LogContext context("BackProduction::processData");
	LOGINFO("Starting processing of data for time " << time);

	kvservice::WhichDataHelper wdh(CKvalObs::CService::All);

	miutil::miTime to(time);
	to.addSec((60*60)-1);

	wdh.addStation(0, time, to);

	kvservice::KvDataList dataList;
	kvservice::proxy::internal::KvDataReceiver dr(dataList);
	bool result = kvservice::KvApp::kvApp->getKvData(dr, wdh);
	if (!result)
	{
		const char * err_msg = "Unable to retrieve data from kvalobs.";
		LOGERROR(err_msg);
		return;
	}

	LOGDEBUG("Got data. Processing...");
	callbacks_.send(dataList);

	LOGDEBUG("Done");
}

