/*! \file GPS.hpp
 *  \date Created: Nov 5, 2012
 *  \author Matthew Barulic
 */

#ifndef GPS_HPP_
#define GPS_HPP_

#include <time.h>
#include "common/events/Event.hpp"
#include <common/datastructures/GPSData.hpp>

namespace IGVC {
namespace Sensors {



/*!
 * \brief Interface for GPS devices.
 * \headerfile GPS.hpp <hardware/sensors/gps/GPS.hpp>
 */
class GPS
{
public:
    virtual ~GPS() { }

    /*!
     * \brief Returns the most recent state acquired from the GPS device.
	 */
	virtual GPSData GetState() = 0;

    /*!
     * \brief Returns the GPSState with the given timestamp.
	 */
	virtual GPSData GetStateAtTime(timeval time) = 0;

    /*!
     * \brief Return true if there is at least one state in the buffer.
	 */
	virtual bool StateIsAvailable() = 0;

    /*!
     * \brief Return true if the device is connected and communicating.
     */
    virtual bool isOpen() = 0;

    Event<GPSData> onNewData;
    Event<void*> onDeviceFailure;
    Event<void*> onDataExpiration;
};

} //Sensors
} //IGVC


#endif /* GPS_HPP_ */
