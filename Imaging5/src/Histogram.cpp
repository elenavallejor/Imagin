/**
 * @file Histogram.cpp
 * @author MP
 */
#include<iostream>
#include<string>
#include "MPTools.h"
#include "Byte.h"
#include "Histogram.h"

using namespace std;

const double Histogram::HISTOGRAM_TOLERANCE=0.01; ///< Value of the default tolerance

Histogram::Histogram(){
    for (int i=0; i < HISTOGRAM_LEVELS; i++){
        _data[i] = 0;
    }
}

int Histogram::size() const{
    return HISTOGRAM_LEVELS;
}

void Histogram::clear(){
    for (int i=0; i < HISTOGRAM_LEVELS; i++){
        _data[i] = 0;
    }
}

int Histogram::getLevel(Byte level) const{
    return _data[level.getValue()];
}

void Histogram::setLevel(Byte level, int npixeles){
    _data[level.getValue()] = npixeles;
}

int Histogram::getMaxLevel() const{
    int max = 0;
    for (int i=0; i < HISTOGRAM_LEVELS; i++){
        if (_data[i] > max){
            max = _data[i];
        }
    }
    return max;
}

int Histogram::getAverageLevel() const{
    int media = 0;
    for (int i=0; i < HISTOGRAM_LEVELS; i++){
        media += _data[i];
    }
    media /= size();
    return media;
}

int Histogram::getBalancedLevel() const {
    int sum = 0, t, tope;
    for (int i = 0; i < size(); i++)
        sum += getLevel(i);
    
    tope=sum/2;
    sum=0;
    for (t=0; t<size() && sum < tope; t++) 
        sum+=getLevel(t);
    return t-1;
}

string Histogram::inspect() const{ 
    std::string result = "";
    for (int i = 0; i < HISTOGRAM_LEVELS; i++) {
        result += to_string(_data[i]);
    }
    result = std::to_string(mphash((unsigned char*)result.c_str(), result.length()));
    result = std::to_string(HISTOGRAM_LEVELS) + " " + result;
    return result;
}

