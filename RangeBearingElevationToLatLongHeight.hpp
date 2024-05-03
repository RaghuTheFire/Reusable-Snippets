
#include <cmath>

// Function to convert range, bearing, and elevation to latitude, longitude, and height
void RangeBearingElevationToLatLongHeight(double range, double bearing, double elevation,double refLat, double refLon, double refHeight,double& lat, double& lon, double& height) 
{
    const double earthRadius = 6378137.0; // Earth's radius in meters

    double sinBearing = sin(bearing * M_PI / 180.0);
    double cosBearing = cos(bearing * M_PI / 180.0);

    double sinElevation = sin(elevation * M_PI / 180.0);
    double cosElevation = cos(elevation * M_PI / 180.0);

    double refLatRad = refLat * M_PI / 180.0;
    double refLonRad = refLon * M_PI / 180.0;

    double latRad = asin((sin(refLatRad) * cosElevation * cos(range / earthRadius)) +
                         (cos(refLatRad) * sinElevation * cos(range / earthRadius) * cosBearing));

    double lonRad = refLonRad + atan2((sinElevation * sin(range / earthRadius) * cosBearing),
                                      (cos(range / earthRadius) - sin(refLatRad) * sin(latRad)));

    lat = latRad * 180.0 / M_PI;
    lon = lonRad * 180.0 / M_PI;
    height = refHeight + range * sinElevation;
}
