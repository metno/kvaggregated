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

#include "AbstractAggregator.h"
#include <kvalobs/kvDataOperations.h>

namespace aggregator
{

AbstractAggregator::AbstractAggregator(int readParam, int writeParam) :
		write_param(writeParam)
{
	read_param.push_back(readParam);
}

AbstractAggregator::~AbstractAggregator()
{
}

namespace
{
float round(float f)
{
	if ( f < 0 )
		f -= 0.05;
	else
		f += 0.05;
	f *= 10;
	f = int(f);
	return f / 10.0;
}
}
AbstractAggregator::kvDataPtr AbstractAggregator::getDataObject(const kvalobs::kvData &trigger,
		const boost::posix_time::ptime &obsTime, float original, float corrected, const std::list<kvalobs::kvData> & sourceData)
{
	int typeID = trigger.typeID();
	if (typeID > 0)
		typeID *= -1;

	kvalobs::kvDataFactory f(trigger.stationID(), obsTime, typeID, trigger.sensor(),
			trigger.level());

	kvDataPtr ret;
	if ( original == invalidParam )
		ret = kvDataPtr(new kvalobs::kvData(f.getMissing(writeParam())));
	else
	{
		original = round(original);
		ret = kvDataPtr(new kvalobs::kvData(f.getData(original, writeParam())));
	}

	if (corrected == invalidParam)
		kvalobs::reject(* ret);
	else
	{
		corrected = round(corrected);
		if (fabs(original - corrected) > 0.05)
			kvalobs::correct(* ret, corrected);
	}

	int fagg = calculateAggregateFlag(sourceData);

	kvalobs::kvControlInfo ci = ret->controlinfo();
	ci.set(0, fagg);
	ret->controlinfo(ci);

	kvalobs::kvUseInfo ui = ret->useinfo();
	ui.setUseFlags(ci);
	ret->useinfo(ui);

	return ret;
}

int AbstractAggregator::calculateAggregateFlag(const kvDataList & sourceData) const
{
	return internal::calculateAggregateFlag_(sourceData);
}

namespace internal
{
namespace
{
int getFmis(const std::vector<kvalobs::kvControlInfo> & ci)
{
	int fmis = 0;
	for ( std::vector<kvalobs::kvControlInfo>::const_iterator it = ci.begin(); it != ci.end(); ++ it )
		switch ( it->MissingFlag() )
		{
		case 0:
			break;
		case 1:
			if ( fmis == 2 )
				return 3;
			fmis = 1;
			break;
		case 2:
			if ( fmis == 1 )
				return 3;
			fmis = 2;
			break;
		case 3:
			return 3;
		case 4:
			if ( fmis == 0 )
				fmis = 4;
			break;
		default:
			break; // ignore invalid values
		}
	return fmis;
}
}

int calculateAggregateFlag_(const AbstractAggregator::kvDataList & sourceData)
{
	std::vector<kvalobs::kvControlInfo> ci;
	for (AbstractAggregator::kvDataList::const_iterator it = sourceData.begin(); it	!= sourceData.end(); ++it)
		ci.push_back(it->controlinfo());

	const int fmis = getFmis(ci);

	int fagg = 0;

	if ( fmis == 2 )
		fagg = 0xB;
	else
	{
		std::set<int> ui2;
		std::set<int> ui3;
		for (AbstractAggregator::kvDataList::const_iterator it = sourceData.begin(); it	!= sourceData.end(); ++it)
		{
			const kvalobs::kvUseInfo & ui = it->useinfo();
			ui2.insert(ui.flag(2));
			ui3.insert(ui.flag(3));
		}

		if ( fmis == 1 and ui3.count(4) )
			fagg = 7;
		else if ( fmis == 4 and ui3.count(3) )
			fagg = 6;
		else if ( (fmis == 1 or fmis == 4) and ui3.count(6) )
			fagg = 9;

		else if ( fmis == 1 and ui3.count(2) )
			fagg = 5;
		else if ( fmis == 4 and ui3.count(1) )
			fagg = 4;
		else if ( (fmis == 1 or fmis == 4) and ui3.count(5) )
			fagg = 8;

		else if ( fmis == 1 or fmis == 3 or fmis == 4 )
			fagg = 0;

		else if ( ui2.count(9) )
			fagg = 0;
		else if ( ui2.count(2) )
			fagg = 3;
		else if ( ui2.count(1) )
			fagg = 2;

		else
			fagg = 1;
	}
	return fagg;
}
}

}
