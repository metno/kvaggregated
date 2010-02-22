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

#ifndef WORKLOOP_H_
#define WORKLOOP_H_

#include <boost/noncopyable.hpp>

namespace boost
{
class thread;
}

class WorkLoop : boost::noncopyable
{
public:
	WorkLoop();
	virtual ~WorkLoop();

    /**
     * call run() in this thread
     */
    void start();

    /**
     * call run() in a new thread thread
     */
    void start_thread();

    /**
     * Signal run to stop. If a thread is running, join with it.
     */
    void stop();

    /**
     * Are we about to stop?
     */
    bool stopping() const
    {
      return shutdown_;
    }

protected:

    /**
     * Override to supply functionality. Implementer is supposed to check for
     * stopping() every now and then, and return from function if it has been
     * called.
     */
    virtual void run() =0;

    /**
     * Called by stop before trying to stop threads
     */
    virtual void onStop() {}

private:
    bool shutdown_;
    boost::thread * thread;
};

#endif /* WORKLOOP_H_ */
