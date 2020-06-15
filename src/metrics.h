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
#ifndef __agregator_Metrics_h__
#define __agregator_Metrics_h__

#include <memory>
#include <chrono>
#include <boost/date_time/posix_time/ptime.hpp>

class Metric {
  public:
    typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimeType; 
    typedef std::chrono::duration<int,std::milli> MilliDuration;

    Metric();

    void start();
    //Return the duration
    MilliDuration stop(bool accmulate=true);
    MilliDuration acc() const { return acc_;}
        
  private:
    TimeType timer_;
    MilliDuration acc_;
};



class Metrics {
  public:
    Metrics();

    //Measure the total performance of the sqlite3 access. 
    //One observation 
    Metric cacheDb;

    //Measure the total performance for comunication with kvalobs.
    //One observation.
    Metric kvDb;


    //For the observation. Did it trigger an generation of a 
    //aggregated value to be sendt to kvalobs.
    void sendtToKvalobs(bool isSendt) { sendtToKvalobs_=isSendt;}
    bool sendtToKvalobs()const{ return sendtToKvalobs_;}

    //The total duration used from the start of computation to the
    //compution is finished. This include the metrics for cacheDb and
    //kvDb. The computation is triggered by the receipt of an observation.
    Metric::MilliDuration timeToCompletion() const;
  private: 
    bool sendtToKvalobs_;
    Metric::TimeType startTime_;
};


//Manage thread local storage for metrics.
std::shared_ptr<Metrics> getMetrics();
void setMetrics( std::shared_ptr<Metrics> m);

//Set the logfile to use for metrics.
//The log file must be rotate by some external ways. ex logrotate.
bool setMetricsLogfile( const std::string &logfile, const std::string &dir);

//Log the metrics to the logfile.
void logMetrics(std::shared_ptr<Metrics> metrics, const boost::posix_time::ptime &obstime, int stationid, int typeId);


#endif