#include "static_lib.h"

double Derivative1(double x, double dx) {

    return (cos(x + dx) - cos(x)) / dx;

}

double Derivative2(double x, double dx) {

    return (cos(x + dx) - cos(x - dx)) / (2 * dx);
    
}