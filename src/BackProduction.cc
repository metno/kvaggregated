#include "BackProduction.h"
#include "WorkLoop.h"
#include "proxy/KvDataReceiver.h"
#include <kvcpp/KvApp.h>
#include <milog/milog.h>
#include <boost/lexical_cast.hpp>
#include <stdexcept>

using namespace std;

BackProduction::BackProduction(kvservice::proxy::CallbackCollection & callbacks,
		const WorkLoop & mainLoop, const boost::posix_time::ptime & from,
		const boost::posix_time::ptime & to, const std::vector<int> & stations) :
	callbacks_(callbacks), mainLoop_(mainLoop), from_(from), to_(to), stations_(stations)
{
}

namespace {
boost::posix_time::ptime parseTime(const std::string & s)
{
	try
	{
		return boost::posix_time::time_from_string(s);
	}
	catch ( std::exception & e)
	{
		throw new std::logic_error("Invalid time format: " + s);
	}
}
}

BackProduction::BackProduction(kvservice::proxy::CallbackCollection & callbacks,
		const WorkLoop & mainLoop, const std::string & timeSpec,
		const std::vector<int> & stations) :
	callbacks_(callbacks), mainLoop_(mainLoop), stations_(stations)
{
	const string::size_type sep = timeSpec.find_first_of(',');
	if (sep == string::npos)
	{
		from_ = parseTime(timeSpec);
		//if (from_.undef())
		//	throw std::logic_error("Invalid specification: " + timeSpec);
		to_ = from_;
	}
	else
	{
		string from = timeSpec.substr(0, sep);

		from_ = parseTime(from);
		//if (from_.undef())
		//	throw std::logic_error("Invalid from specification: " + from);

		const string::size_type nextWord = sep + 1;
		if (nextWord == timeSpec.size())
			throw std::logic_error("Invalid specification: " + timeSpec);

		const std::string to = timeSpec.substr(nextWord);
		try
		{
			unsigned duration = boost::lexical_cast<unsigned>(to);
			to_ = from_ + boost::posix_time::hours(duration);
		}
		catch (boost::bad_lexical_cast &)
		{
			to_ = boost::posix_time::time_from_string(to);
		}

		//if (to_.undef())
		//	throw std::logic_error("Invalid to specification: " + to);
	}
}

BackProduction::~BackProduction()
{
}

void BackProduction::operator ()()
{
	boost::posix_time::ptime f(from_);

	while (!mainLoop_.stopping() && f <= to_)
	{
		try
		{
			processData(f);
			LOGINFO("Done processing data for time " << f);
			f += boost::posix_time::hours(1);
		}
		catch ( std::exception & e )
		{
			LOGERROR("Error when processing data: " << e.what());
		}
	}
}

void BackProduction::processData(const boost::posix_time::ptime & time)
{
	milog::LogContext context("BackProduction::processData");
	LOGINFO("Starting processing of data for time " << time);

	kvservice::WhichDataHelper wdh(CKvalObs::CService::All);

	boost::posix_time::ptime to = time + boost::posix_time::seconds((60*60)-1);

	if ( stations_.empty() )
		wdh.addStation(0, time, to);
	else
		for ( std::vector<int>::const_iterator it = stations_.begin(); it != stations_.end(); ++ it )
			wdh.addStation(* it, time, to);

	kvservice::KvDataList dataList;
	kvservice::proxy::internal::KvDataReceiver dr(dataList);
	bool result = kvservice::KvApp::kvApp->getKvData(dr, wdh);
	if (!result)
	{
		const char * err_msg = "Unable to retrieve data from kvalobs.";
		LOGERROR(err_msg);
		return;
	}
	if ( dataList.empty() )
	{
		LOGWARN("No data from kvalobs");
		return;
	}

	LOGDEBUG("Got data. Processing...");
	callbacks_.send(dataList);

	LOGDEBUG("Done");
}

