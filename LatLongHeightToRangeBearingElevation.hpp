#include <cmath>

struct LatLonHeight 
{
    double latitude;
    double longitude;
    double height;
};

struct RangeBearingElevation 
{
    double range;
    double bearing;
    double elevation;
};

const double EARTH_RADIUS = 6371.0; // in km

RangeBearingElevation latLonHeightToRangeBearingElevation(const LatLonHeight& source, const LatLonHeight& target) 
{
    double lat1 = source.latitude * M_PI / 180.0;
    double lon1 = source.longitude * M_PI / 180.0;
    double lat2 = target.latitude * M_PI / 180.0;
    double lon2 = target.longitude * M_PI / 180.0;

    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;

    double a = sin(dLat / 2.0) * sin(dLat / 2.0) +
               cos(lat1) * cos(lat2) * sin(dLon / 2.0) * sin(dLon / 2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    double range = EARTH_RADIUS * c; // in km

    double bearing = atan2(sin(lon2 - lon1) * cos(lat2),
                           cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(lon2 - lon1));
    bearing = fmod((bearing * 180.0 / M_PI + 360.0), 360.0); // in degrees

    double elevation = target.height - source.height;

    RangeBearingElevation result = {range, bearing, elevation};
    return result;
}
