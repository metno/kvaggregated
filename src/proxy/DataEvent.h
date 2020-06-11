/*
 Kvalobs - Free Quality Control Software for Meteorological Observations

 $Id: IncomingHandler.h,v 1.1.2.4 2007/09/27 09:02:16 paule Exp $

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
#ifndef __kvservice__proxy__DataEvent_h__
#define __kvservice__proxy__DataEvent_h__

#include <chrono>
#include <memory>
#include <kvcpp/kvevents.h>
#include "metrics.h"

namespace kvservice {
namespace proxy {

/**
 * DataEvent is just a wrapper class of KvObsDataListPtr
 * so we can add metrics to the processing of data.
 */

class DataEvent
{
public:
  DataEvent() = delete;
  DataEvent(KvObsDataListPtr theData);

  KvObsDataList* operator->() { return theData_.get(); }
  KvObsDataList& operator&() { return *theData_.get(); }
  
  IKvObsDataList begin(){ return theData_->begin();}
  IKvObsDataList end(){ return theData_->end();}

  CIKvObsDataList begin()const{ return theData_->begin();}
  CIKvObsDataList end()const{ return theData_->end();}

  KvObsDataList::reverse_iterator rbegin()const{ return theData_->rbegin();}
  KvObsDataList::reverse_iterator rend()const{ return theData_->rend();}
    
  KvObsDataList::reference front() { return theData_->front();}
  KvObsDataList::const_reference front()const { return theData_->front();}

  bool empty()const { return theData_->empty();}
  Metrics metrics;
  
private:
  KvObsDataListPtr theData_;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};


typedef std::shared_ptr<DataEvent> DataEventPtr;

}
}

#endif
