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

#include "useinfoAggregate.h"
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <cstring>
#include <ostream>

namespace aggregator
{

kvalobs::kvUseInfo aggregateUseFlag(const std::vector<kvalobs::kvUseInfo> & source)
{
	UseList l;
	for ( std::vector<kvalobs::kvUseInfo>::const_iterator it = source.begin(); it != source.end(); ++ it )
		l.push_back(it->flagstring().substr(0, BaseUseInfo::baseUiSize));
	return kvalobs::kvUseInfo(aggregate(l).str() + "00000000000");
}

#define HAS_PROPERTY_F(what) std::mem_fun_ref(& BaseUseInfo::what)
#define FIND(flags, what) std::find_if(flags.begin(), flags.end(), HAS_PROPERTY_F(what))
#define ANY_FLAGS_ARE(flags, what) (FIND(flags, what) != flags.end())

BaseUseInfo aggregate(const UseList & flags)
{
	// don't even consider modifying this code without running/modifying the tests

	bool allOk = true;
	for ( UseList::const_iterator it = flags.begin(); it != flags.end(); ++ it )
		if ( * it != "00000" )
		{
			allOk = false;
			break;
		}
	if ( allOk )
		return "99000";

	if ( ANY_FLAGS_ARE(flags, modifiedOriginal) )
	{
		UseList::const_iterator find = FIND(flags, modified);
		if ( find == flags.end() )
			return "98999";

		if ( not ANY_FLAGS_ARE(flags, suspicious) )
		{

		if ( (*find)[1] == '8' and (*find)[2] == '9' )
		{
			if ( (*find)[3] == '2' )
				return "98029";
			else if ( (*find)[3] == '4' )
				return "98147";
		}

			if ( (*find)[3] == '1' )
				return "99019";
			if ( (*find)[3] == '3' )
				return "99137";
		}
	}
	if ( ANY_FLAGS_ARE(flags, rejected) )
		return "99989";


	UseList checkedFlags = flags;
	UseList::const_iterator sep = std::remove_if(checkedFlags.begin(), checkedFlags.end(), HAS_PROPERTY_F(unchecked));
	checkedFlags.resize(sep - checkedFlags.begin());

	if ( checkedFlags.empty() or not (ANY_FLAGS_ARE(checkedFlags, suspicious)) )
		return "99999";

	UseList suspicious;
	for ( UseList::const_iterator it = checkedFlags.begin(); it != checkedFlags.end(); ++ it )
		if ( it->suspicious() )
			suspicious.push_back(* it);

	if ( not suspicious.empty() )
	{
		BaseUseInfo ret = "99000";
		UseList unmodifiedSuspicios = suspicious;
		UseList::const_iterator sep = std::remove_if(unmodifiedSuspicios.begin(), unmodifiedSuspicios.end(), HAS_PROPERTY_F(modified));
		unmodifiedSuspicios.resize(sep - unmodifiedSuspicios.begin());

		if ( not unmodifiedSuspicios.empty() )
		{
			UseList::const_iterator worstSuspicious = FIND(unmodifiedSuspicios, verySuspicious);
			if ( worstSuspicious != unmodifiedSuspicios.end() )
				ret = * worstSuspicious;
			else
				ret = unmodifiedSuspicios.front();
		}
		else // All suspicious values have been modified
		{
			if ( ANY_FLAGS_ARE(suspicious, automaticallyModified) )
				ret[2] = '1';
			else
				ret[2] = '0';
			BaseUseInfo selected = suspicious.front();
			for ( UseList::const_iterator it = suspicious.begin(); it != suspicious.end(); ++ it )
				if ( (*it)[3] > selected[3] )
					selected = * it;
			ret[3] = selected[3];
			ret[4] = selected[4];
		}
		ret[0] = '9';
		ret[1] = '9';
		return ret;

	}
}



BaseUseInfo::BaseUseInfo()
{
	memset(ui,'9', baseUiSize);
}

BaseUseInfo::BaseUseInfo(const char * useinfobase)
{
	std::copy(useinfobase, useinfobase + baseUiSize, ui);
}

BaseUseInfo::BaseUseInfo(const std::string & useinfobase)
{
	if ( useinfobase.size() != baseUiSize )
		throw std::logic_error("BaseUseInfo contruction string must have size 5");
	std::copy(useinfobase.begin(), useinfobase.end(), ui);
}

std::string BaseUseInfo::str() const
{
	return std::string(ui, baseUiSize);
}

namespace
{
bool eq(const char *a, const char * ui)
{
	return ! std::memcmp(a, ui, BaseUseInfo::baseUiSize);
}
}

bool BaseUseInfo::modifiedOriginal() const
{
	return (ui[1] == '8' or ui[2] == '3') and not rejected();
//	return ui[1] == '8'; // wrong - but most tests pass
	//	return ui[3] != '0' and ui[3] != '9';
}

bool BaseUseInfo::rejected() const
{
	return ui[3] == '8';
	// and ui[2] == '3';
}

bool BaseUseInfo::unchecked() const
{
	return eq("99999", ui);
}

bool BaseUseInfo::suspicious() const
{
	return ui[2] == '1' or verySuspicious();
}

bool BaseUseInfo::verySuspicious() const
{
	return ui[2] == '2';
}

bool BaseUseInfo::modified() const
{
	return ui[3] != '0' and ui[3] != '8' and ui[3] != '9';
}

bool BaseUseInfo::automaticallyModified() const
{
	const char m = ui[3];
	return m == '3' or m == '4' or m == '6';
}

char & BaseUseInfo::operator [] ( unsigned idx )
{
	return ui[idx];
}
char BaseUseInfo::operator [] ( unsigned idx ) const
{
	return ui[idx];
}



bool operator == (const BaseUseInfo & a, const BaseUseInfo & b)
{
	return a.str() == b.str();
}

std::ostream & operator << (std::ostream & s, const BaseUseInfo & ui)
{
	return s << ui.str();
}


}
