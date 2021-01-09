#ifndef MVECTOR
#define MVECTOR

#include "math.h"
#include <stdio.h>

class mvector {
    public:
        double x, y, z;

        mvector(double _x, double _y, double _z);

        mvector operator +(const mvector& a);

        mvector operator -(const mvector& a);

        mvector operator *(const double& d);

        mvector operator /(const double& d);

        mvector operator *(const mvector& a);

        mvector operator ,(const mvector& a);

        double abs();

        void normalize();

        void out();
};

#endif