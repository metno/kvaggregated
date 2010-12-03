#ifndef BACKPRODUCTION_H_
#define BACKPRODUCTION_H_

#include "proxy/CallbackCollection.h"
#include <puTools/miTime.h>
#include <string>

class WorkLoop;

/**
 * Creates new agregates, based on new data.
 * 
 * @note This is _not_ part of normal production. Rather, the creation of a 
 * BackProduction object is the result of having run kvAgregated using the -b
 * option. 
 */
class BackProduction
{
public:
	BackProduction(kvservice::proxy::CallbackCollection & callbacks,
			const WorkLoop & mainLoop,
			const miutil::miTime & from, 
			const miutil::miTime & to);
	
	/**
	 * @param timeSpec A string specification of what times to use. The 
	 *                 format is 
	 *                     2008-04-08T06:00:00,2008-04-08T10:00:00
	 *                 or
	 *                     2008-04-08T06:00:00,4 (where the last part is number of hours)
	 * 
	 * @throw std::logic_error if specification is invalid
	 */
	BackProduction(kvservice::proxy::CallbackCollection & callbacks,
			const WorkLoop & mainLoop, const std::string & timeSpec);
	
	~BackProduction();

	void operator () ();
	
private:

	void processData( const miutil::miTime & time);

	kvservice::proxy::CallbackCollection & callbacks_;
	const WorkLoop & mainLoop_;
	miutil::miTime from_;
	miutil::miTime to_;
};


#endif /*BACKPRODUCTION_H_*/
