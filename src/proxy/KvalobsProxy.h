/*
  Kvalobs - Free Quality Control Software for Meteorological Observations 

  $Id: KvalobsProxy.h,v 1.1.2.7 2007/09/27 09:02:16 paule Exp $                                                       

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
#ifndef __kvservice__proxy__KvalobsProxy_h__
#define __kvservice__proxy__KvalobsProxy_h__

#include "DataAccess.h"
#include "CachedDataAccess.h"
#include "KvalobsDataAccess.h"
#include "CallbackCollection.h"
#include <boost/utility.hpp>
#include <set>
#include <puTools/miTime>
#include <dnmithread/CommandQue.h>
#include <decodeutility/kvDataFormatter.h>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>



namespace kvservice
{
  namespace proxy
  {
    class KvalobsProxy : public DataAccess, boost::noncopyable
    {
      public:
        KvalobsProxy( const std::string & proxyDatabaseName, CallbackCollection & callbacks, bool repopulate = false );
        ~KvalobsProxy( );

        /**
         * Get kvalobs data. The source is either the database, or the proxy database
         */
        void getData( KvDataList &data, int station,
                      const miutil::miTime &from, const miutil::miTime &to,
                      int paramid, int type, int sensor, int lvl ) const;

        /**
         * Send data to kvalobs. Data will also be stored in proxy database
         */
        CKvalObs::CDataSource::Result_var sendData( const KvDataList &data );

        void cacheData(const KvDataList &data);

        /**
         * Add parameter to store in cache database
         */
        void addInteresting( int param )
        {
          interestingParameters_.insert( param );
        }

        // Operations on proxy:
        void db_clear();
        void db_cleanup();
        void db_populate( int hours = 24 );
        void db_repopulate( int hours = 24 )
        {
          db_clear();
          db_populate( hours );
        }

        const miutil::miTime & getOldestInProxy() const
        {
          return oldestInProxy;
        }

        void setOldestInProxy( const miutil::miTime & newTime )
        {
          oldestInProxy = newTime;
        }

      private:
        void adaptDataToKvalobs_(KvDataList & out, const kvalobs::kvData & data) const;

        std::set<int> interestingParameters_;
        
        miutil::miTime oldestInProxy;

        CachedDataAccess cache_;
        KvalobsDataAccess kvalobs_;

        typedef boost::recursive_mutex::scoped_lock Lock;
        mutable boost::recursive_mutex kv_mutex;

        class Cleaner;
        Cleaner * cleaner_;
        boost::thread * cleanerThread_;
    };
  }
}

#endif // __kvservice__proxy__KvalobsProxy_h__
