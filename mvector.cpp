#include "mvector.h"

mvector::mvector(double _x, double _y, double _z) {
    x = _x;
    y = _y;
    z = _z;
}

mvector mvector::operator +(const mvector& a) {
    mvector result(a.x+this->x, a.y+this->y, a.z+this->z);
    return result;
}

mvector mvector::operator -(const mvector& a) {
    mvector result(this->x-a.x, this->y-a.y, this->z-a.z);
    return result;
}

mvector mvector::operator *(const double& d) {
    mvector result(this->x*d, this->y*d, this->z*d);
    return result;
}

mvector mvector::operator /(const double& d) {
    mvector result(this->x/d, this->y/d, this->z/d);
    return result;
}

mvector mvector::operator *(const mvector& a) {
    double x = this->y*a.z - this->z*a.y;
    double y = this->x*a.z - this->z*a.x;
    double z = this->x*a.y - this->y*a.x;
    mvector result(x, y, z);
    return result;
}

mvector mvector::operator ,(const mvector& a) {
    mvector result(this->x*a.x, this->y*a.y, this->z*a.z);
    return result;
}

double mvector::abs() {
    return sqrt(x*x + y*y + z*z);
}

void mvector::normalize() {
    double a = 1/abs();
    //printf("asd %f\n", a);
    this->x *= a;
    this->y *= a;
    this->z *= a;
}

void mvector::out() {
    printf("x\t%f\ty\t%f\tz\t%f\n", this->x, this->y, this->z);
}
