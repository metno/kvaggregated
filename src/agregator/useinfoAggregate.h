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


#ifndef USEINFOAGGREGATE_H_
#define USEINFOAGGREGATE_H_

#include <kvalobs/kvDataFlag.h>
#include <vector>
#include <string>
#include <iosfwd>

namespace agregator
{

kvalobs::kvUseInfo aggregateUseFlag(const std::vector<kvalobs::kvUseInfo> & source);



class BaseUseInfo;
typedef std::vector<BaseUseInfo> UseList;

BaseUseInfo aggregate(const UseList & useflags);


class BaseUseInfo
{
public:
	BaseUseInfo();
	BaseUseInfo(const char *  useinfobase);
	BaseUseInfo(const std::string & useinfobase);

	std::string str() const;

	bool modifiedOriginal() const;
	bool rejected() const;
	bool unchecked() const;
	bool suspicious() const;
	bool verySuspicious() const;
	bool modified() const;
	bool automaticallyModified() const;

	char & operator [] ( unsigned idx );
	char operator [] ( unsigned idx ) const;

	static const unsigned baseUiSize = 5;

private:
	friend bool operator == (const BaseUseInfo & a, const BaseUseInfo & b);

	char ui[baseUiSize];
};

bool operator == (const BaseUseInfo & a, const BaseUseInfo & b);
inline bool operator != (const BaseUseInfo & a, const BaseUseInfo & b) { return not (a == b); }
std::ostream & operator << (std::ostream & s, const BaseUseInfo & ui);


}

#endif /* USEINFOAGGREGATE_H_ */
