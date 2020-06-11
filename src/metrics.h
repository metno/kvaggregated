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

#include <chrono>

class Metric {
  public:
    Metric();

    void start();
    //Return the duration
    std::chrono::duration<int,std::milli> stop(bool accmulate=true);
    std::chrono::duration<int,std::milli> acc() const { return acc_;}
  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> timer_;
    std::chrono::duration<int,std::milli> acc_;
};



class Metrics {
  public:
    Metrics();

    Metric db;
    Metric kvDb;
};


#endif