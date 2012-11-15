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

#include "ra2rr_12_forward.h"

namespace aggregator
{

ra2rr_12_forward::ra2rr_12_forward()
{
	name += "-Forward";
}

const ra2rr_12_forward::TimeSpan ra2rr_12_forward::getTimeSpan(const kvalobs::kvData &data) const
{
	TimeSpan ts = ra2rr_12::getTimeSpan(data);
	ts.first += boost::posix_time::hours(timeOffset() + 1);
	ts.second += boost::posix_time::hours(timeOffset());
	return ts;
}

}
