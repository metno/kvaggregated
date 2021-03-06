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

#ifndef AGREGATORRUNNER_H_
#define AGREGATORRUNNER_H_

#include "WorkLoop.h"
#include "proxy/IncomingHandler.h"
#include <dnmithread/CommandQue.h>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace kvservice
{
class DataAccess;
namespace proxy
{
class CallbackCollection;
}
}

class AggregatorRunner : public WorkLoop
{
public:
	AggregatorRunner(const std::vector<int> & stations, kvservice::DataAccess & dataAccess,
			kvservice::proxy::CallbackCollection & callbacks);
	~AggregatorRunner();

    dnmi::thread::CommandQue & getCommandQueue() { return queue; }
    const dnmi::thread::CommandQue & getCommandQueue() const { return queue; }

protected:

    virtual void run();
    virtual void onStop();

    void processData();

    dnmi::thread::CommandQue queue;
    kvservice::proxy::internal::IncomingHandler incomingHandler;
};

#endif /* AGREGATORRUNNER_H_ */
