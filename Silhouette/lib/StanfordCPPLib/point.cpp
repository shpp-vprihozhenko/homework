/*
 * File: point.cpp
 * ---------------
 * This file implements the point.h interface.
 * 
 * @version 2014/10/08
 * - removed 'using namespace' statement
 */

#include "point.h"
#include <string>
#include "strlib.h"

Pixel::Pixel() {
    x = 0;
    y = 0;
}

Pixel::Pixel(int x, int y) {
    this->x = x;
    this->y = y;
}

int Pixel::getX() const {
    return x;
}

int Pixel::getY() const {
    return y;
}

std::string Pixel::toString() const {
    return "(" + integerToString(x) + "," + integerToString(y) + ")";
}

bool Pixel::operator==(const Pixel & p2) const {
    return (x == p2.x) && (y == p2.y);
}

bool Pixel::operator!=(const Pixel & p2) const {
    return (x != p2.x) || (y != p2.y);
}

std::ostream & operator<<(std::ostream & os, const Pixel & pt) {
    return os << pt.toString();
}

int hashCode(const Pixel & pt) {
    int hash = 0;
    hash += abs(pt.getX() & 0x7fff) << 16;
    hash += abs(pt.getY());
    return hash;
}
