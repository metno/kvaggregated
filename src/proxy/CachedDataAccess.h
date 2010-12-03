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

#ifndef CACHEDDATAACCESS_H_
#define CACHEDDATAACCESS_H_

#include "DataAccess.h"
#include "ProxyDatabaseConnection.h"
#include <boost/thread/mutex.hpp>



namespace kvservice
{

class CachedDataAccess: public kvservice::DataAccess
{
public:
	explicit CachedDataAccess(const std::string & proxyDatabaseName);
	virtual ~CachedDataAccess();

    virtual void getData( KvDataList &data, int station,
                  const miutil::miTime &from, const miutil::miTime &to,
                  int paramid, int type, int sensor, int lvl ) const;

    virtual CKvalObs::CDataSource::Result_var sendData( const KvDataList & data );

    void clear();

    void deleteOldData(const miutil::miTime & olderThanThis);

private:
    mutable ProxyDatabaseConnection connection_;

    typedef boost::mutex Mutex;

    mutable Mutex proxy_mutex;

};

}

#endif /* CACHEDDATAACCESS_H_ */
