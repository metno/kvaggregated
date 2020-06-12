/*
  Kvalobs - Free Quality Control Software for Meteorological Observations

  $Id: paramID.h,v 1.1.2.5 2007/09/27 09:02:16 paule Exp $

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

#include "metrics.h"
#include "LogAppender.h"
#include <milog/milog.h>
#include <miutil/timeconvert.h>
#include <sstream>

using namespace std::chrono;

namespace {
thread_local std::shared_ptr<Metrics> metrics;
miutil::LogAppender logAppender;
}

std::shared_ptr<Metrics>
getMetrics()
{
  if (!metrics) {
    metrics = std::shared_ptr<Metrics>(new Metrics());
  }
  return metrics;
}

void
setMetrics(std::shared_ptr<Metrics> m)
{
  metrics = m;
}


//Set the logfile to use for metrics.
//The log file must be rotate by some external ways. ex logrotate.
bool setMetricsLogfile( const std::string &logfile, const std::string &dir ) {
  if(  logfile.empty() )
    return false;
  
  logAppender=miutil::LogAppender(logfile, dir); 

  if( logAppender.isOk() ) {
    LOGINFO("Writing metrics to: '" << logAppender.logFile() << "'.");
  }

  return logAppender.isOk();
}

//Log the metrics to the logfile.
void logMetrics(std::shared_ptr<Metrics> metrics, const boost::posix_time::ptime &obstime, int stationid, int typeId) {
  if( logAppender.isOk() ) {
    std::ostringstream o;
    std::string status=metrics->sendtToKvalobs()>0?"true":"false";

    //This check for empty messages sendt to kvalobs.
    //This is seen sometimes.
    if(metrics->sendtToKvalobs()<0) {
      status ="empty";
    } 
    
    o << " (" << stationid << "/" << typeId << "/" << boost::posix_time::to_kvalobs_string(obstime) <<")"
      << " sendtToKv: " << status  
      << " cachedb: " << metrics->cacheDb.acc().count() << " ms kvdb: " 
      << metrics->kvDb.acc().count() << " ms duration: "
    << metrics->timeToCompletion().count() << " ms";
    logAppender.log( o.str());
  }
}



using namespace std::chrono;

Metric::Metric()
  : timer_(high_resolution_clock::now())
  , acc_(0)
{}

void
Metric::start()
{
  timer_ = high_resolution_clock::now();
}

Metric::MilliDuration
Metric::stop(bool accumulate)
{
  auto d = duration_cast<milliseconds>(high_resolution_clock::now() - timer_);
  if (accumulate) {
    acc_ += d;
  }
  return d;
}

Metric::MilliDuration
Metrics::timeToCompletion() const
{
  return duration_cast<milliseconds>(high_resolution_clock::now() - startTime_);
}

Metrics::Metrics()
  : sendtToKvalobs_(0), startTime_(high_resolution_clock::now())
{}
