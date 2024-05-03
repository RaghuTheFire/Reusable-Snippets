#include <cmath>

// Function to convert polar coordinates to Cartesian coordinates
void polarToCartesian(double radius, double angle, double& x, double& y) 
{
    x = radius * cos(angle);
    y = radius * sin(angle);
}
