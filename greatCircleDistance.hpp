#include <cmath>

double greatCircleDistance(double lat1, double lon1, double lat2, double lon2) 
{
    const double earthRadius = 6371.0; // Radius of the Earth in kilometers
    
    // Convert latitude and longitude to radians
    lat1 = lat1 * M_PI / 180.0;
    lon1 = lon1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;
    lon2 = lon2 * M_PI / 180.0;
    
    // Calculate the differences
    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;
    
    // Apply the Haversine formula
    double a = pow(sin(dLat / 2.0), 2.0) + cos(lat1) * cos(lat2) * pow(sin(dLon / 2.0), 2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    
    return earthRadius * c;
}
