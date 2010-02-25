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

#ifndef READWRITELOCK_H_
#define READWRITELOCK_H_

#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

class ScopedReadLock;
class ScopedWriteLock;

/**
 * An unfair read/write lock. Writers will be subject to starvation when using this
 */
class RWMutex : boost::noncopyable
{
public:
	RWMutex() : count_(0), writer_(false)
	{}

private:
	friend class ScopedReadLock;
	friend class ScopedWriteLock;

	boost::mutex mutex;
	boost::condition cond_;
	unsigned count_;
	bool writer_;
};

class ScopedReadLock : boost::noncopyable
{
	RWMutex & m_;
public:
	explicit ScopedReadLock(RWMutex & mutex) : m_(mutex)
	{
		boost::mutex::scoped_lock lock(m_.mutex);
		while ( m_.writer_ )
			m_.cond_.wait(lock);
		++ m_.count_;
	}
	~ScopedReadLock()
	{
		boost::mutex::scoped_lock lock(m_.mutex);
		if ( -- m_.count_ == 0 )
			m_.cond_.notify_one();
	}
};

class ScopedWriteLock : boost::noncopyable
{
	RWMutex & m_;
public:
	explicit ScopedWriteLock(RWMutex & mutex) : m_(mutex)
	{
		boost::mutex::scoped_lock lock(m_.mutex);
		while ( m_.count_ or m_.writer_)
			m_.cond_.wait(lock);
		m_.writer_ = true;
	}
	~ScopedWriteLock()
	{
		boost::mutex::scoped_lock lock(m_.mutex);
		m_.writer_ = false;
		m_.cond_.notify_one();
	}
};


#endif /* READWRITELOCK_H_ */
