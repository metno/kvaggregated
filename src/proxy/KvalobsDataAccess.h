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

#ifndef KVALOBSDATAACCESS_H_
#define KVALOBSDATAACCESS_H_

#include "metrics.h"
#include "DataAccess.h"

namespace kvservice
{

/**
 * Access to data directly from kvalobs.
 *
 * This class implements some methods in addition to the DataAccess interface.
 * The idea is that objects of this class will be used by anyone who needs to
 * access kvalobs in any way.
 */
class KvalobsDataAccess: public kvservice::DataAccess
{
public:
	KvalobsDataAccess();
	virtual ~KvalobsDataAccess();

    virtual void getData(Metrics &m, KvDataList &data, int station,
                  const boost::posix_time::ptime &from, const boost::posix_time::ptime &to,
                  int paramid, int type, int sensor, int lvl ) const;

    /// station==0 means all stations
    void getAllData(Metrics &m, KvDataList & data, const boost::posix_time::ptime &from, const boost::posix_time::ptime &to, int station = 0) const;

    virtual CKvalObs::CDataSource::Result_var sendData(Metrics &m, const KvDataList & data );

	/**
	 * Get station metadata from kvalobs. This i a service function to
	 * subclasses. Will search the kvalobs database for metadata with the
	 * given name, which it applicable to the given kvData object. Data is
	 * fetched from the station_metadata table.
	 *
	 * \throws std::runtime_error if unable to find metadata, or if there is
	 * an error when contacting kvalobs.
	 *
	 * \param metadataname Name of the metadata to fetch
	 * \param validFor The object this metadata will be applied to.
	 *
	 * \return The value of the given metadata
	 */
	virtual float getStationMetadata(Metrics &m, const std::string & metadataName, const kvalobs::kvData & validFor) const;

};

}

#endif /* KVALOBSDATAACCESS_H_ */
