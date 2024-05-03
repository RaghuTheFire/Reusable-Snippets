#include <cmath>

// Function to convert Cartesian coordinates to Polar coordinates
void cartesianToPolar(double x, double y, double& r, double& theta) 
{
    r = sqrt(x * x + y * y);  // Calculate the radius
    theta = atan2(y, x);      // Calculate the angle in radians
}
