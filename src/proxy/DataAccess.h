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


#ifndef DATAACCESS_H_
#define DATAACCESS_H_

#include <kvcpp/kvservicetypes.h>
#include <kvskel/datasource.hh>
#include <boost/noncopyable.hpp>
#include <stdexcept>

namespace kvservice
{
/**
 * Virtual base class for access to kvalobs data.
 */
class DataAccess : boost::noncopyable
{
public:
	virtual ~DataAccess() {}

    /**
     * Get kvalobs data. The source is either the database, or the proxy database
     */
    virtual void getData( KvDataList &data, int station,
                  const boost::posix_time::ptime &from, const boost::posix_time::ptime &to,
                  int paramid, int type, int sensor, int lvl ) const = 0;

    /**
     * Send data to kvalobs. Data will also be stored in proxy database
     */
    virtual CKvalObs::CDataSource::Result_var sendData( const KvDataList &data ) =0;

    virtual float getStationMetadata(const std::string & metadataName, const kvalobs::kvData & validFor) const
    {
    	throw std::runtime_error("Unable to get station meta data");
    }

    /**
     * Tell subclass that we care about this particualer parameter. Subclasses
     * are free to do as they like with parameters which have not been
     * registered like this.
     *
     * @param param A parameter we are interested in.
     */
    virtual void addInteresting(int param) {}

    /**
     * Tell subclass to cache data for faster retrieval, if the subclass has
     * such capabilities.
     *
     * @param data The data to be cached
     */
    virtual void cacheData(const KvDataList &data) {}
};

}

#endif /* DATAACCESS_H_ */
