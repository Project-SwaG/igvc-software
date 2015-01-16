#include "GPSWaypointSource.h"

GPSWaypointSource::GPSWaypointSource(std::string file)
{
    _moduleName = "Waypoint Source";
    openFile(file);
}

void GPSWaypointSource::openFile(string file)
{
    _isOpen = false;
    try {
        GPSFileReader::read(file, _data);
        _isOpen = true;
        newFileLoaded();
    } catch (GPSFileNotFoundException){ }
}

GPSData GPSWaypointSource::getNext()
{
    if(!_data.empty())
    {
        GPSData temp = _data.front();
        _data.erase(_data.begin());
        return temp;
    }
    else
    {
        return GPSData();
    }
}
