/*! \file HemisphereA100GPS.h
 *  \date Created: Nov 5, 2012
 *  \author Matthew Barulic
 */

#ifndef HEMISPHEREA100GPS_H_
#define HEMISPHEREA100GPS_H_

#include "GPS.hpp"

#include "hardware/serial/SerialPort.h"

#include <list>
#include <QObject>

/*!
 * \brief For connecting to the Hemisphere A100 GPS device
 * \headerfile HemisphereA100GPS.h <hardware/sensors/gps/HemisphereA100GPS.h>
 */
class NMEACompatibleGPS: public GPS
{
    Q_OBJECT
public:
    NMEACompatibleGPS(std::string devicePath, uint baudRate);
    GPSData GetState();
    GPSData GetStateAtTime(timeval);
	bool StateIsAvailable();
    bool isWorking();
    ~NMEACompatibleGPS();

private slots:
    void onNewSerialLine(std::string line);

private:

    SerialPort serialPort; // Serial port for GPS communication

	boost::mutex queueLocker; // mutex for thread-safing the buffer

	size_t maxBufferLength; // maximum number of states to be stored in the buffer

	std::list<GPSData> stateQueue; // buffer of the latest maxBufferLength states. This is a std::list instead of a std::queue because of the need to iterate to retrieve a state at a given timestamp
};

#endif /* HEMISPHEREA100GPS_H_ */
