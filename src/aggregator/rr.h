/*
 Kvalobs - Free Quality Control Software for Meteorological Observations

 $Id: rr.h,v 1.1.2.6 2007/09/27 09:02:16 paule Exp $

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
#ifndef __agregator_rr_h__
#define __agregator_rr_h__

#include "AbstractAggregator.h"
#include "paramID.h"
#include <kvalobs/kvData.h>
#include <puTools/miClock.h>
#include <set>


namespace aggregator
{

/**
 * \brief A framework for calculating sum of rainfall over a period
 * of time.
 */
class rr: public AbstractAggregator
{
public:
	/**
	 * \brief Constructor. Set up the basic parameters of the
	 * AbstractAggregator object.
	 *
	 * \param readParam The paramID that incoming data should
	 * have. All data which comes to this object has this paramID.
	 *
	 * \param writeParam The paramID of generated data.
	 *
	 * \param interestingHours How many hours of data back in time are
	 * we interested in?
	 *
	 * \param generateWhen The times of day when we will generate
	 * data. Agregates will only be generated for these times.
	 */
	rr(int readParam, int writeParam, int interestingHours, const std::set<
			miutil::miClock> &generateWhen);

protected:
	virtual void extractUsefulData(kvDataList & out, const kvDataList & dataIn, const kvalobs::kvData & trigger) const;

	virtual float calculate(const ValueList & source, ExtraData extraData) const;

};

}

#endif // __agregator_rr_h__
