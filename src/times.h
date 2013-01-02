/*
  Kvalobs - Free Quality Control Software for Meteorological Observations 

  $Id: times.h,v 1.1.2.3 2007/09/27 09:02:16 paule Exp $                                                       

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
#ifndef __agregator__times_h__
#define __agregator__times_h__

namespace aggregator {
  static const boost::posix_time::time_duration hours[24] = {
    boost::posix_time::hours( 0),  boost::posix_time::hours(12),
    boost::posix_time::hours( 1),  boost::posix_time::hours(13),
    boost::posix_time::hours( 2),  boost::posix_time::hours(14),
    boost::posix_time::hours( 3),  boost::posix_time::hours(15),
    boost::posix_time::hours( 4),  boost::posix_time::hours(16),
    boost::posix_time::hours( 5),  boost::posix_time::hours(17),
    boost::posix_time::hours( 6),  boost::posix_time::hours(18),
    boost::posix_time::hours( 7),  boost::posix_time::hours(19),
    boost::posix_time::hours( 8),  boost::posix_time::hours(20),
    boost::posix_time::hours( 9),  boost::posix_time::hours(21),
    boost::posix_time::hours(10),  boost::posix_time::hours(22),
    boost::posix_time::hours(11),  boost::posix_time::hours(23)
  };
  const std::set<boost::posix_time::time_duration> allHours( hours, &hours[24] );
  const std::set<boost::posix_time::time_duration> sixAmSixPm( &hours[12], &hours[14] );
  const std::set<boost::posix_time::time_duration> sixAm( &hours[12], &hours[13] );

  const std::set<boost::posix_time::time_duration> elevenPm( &hours[23], &hours[24] );
  const std::set<boost::posix_time::time_duration> midnight( &hours[0], &hours[1] );
}

#endif // __agregator__times_h__
