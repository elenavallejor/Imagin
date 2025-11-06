/**
 * @file Byte.cpp
 * @brief Operators for bit level
 * @note To be implemented by students 
 * @note MP-DGIM, MP-IADE, MP-II (grupo B)
 */
#include <iostream>
#include <string>
#include "MPTools.h"
#include "Byte.h"

using namespace std;

const Byte Byte::MAX_BYTE_VALUE(255);
const Byte Byte::MIN_BYTE_VALUE(0);
    
void Byte::onBit(int pos){  
    if (pos >= MIN_BIT && pos < MAX_BIT){
        unsigned char mask(0x1 << pos);
        _data = (_data | mask );
    } 
    else {
        cout << "Error: valor de pos no valido" << endl;
    }
}

void Byte::offBit(int pos){
    if (pos >= MIN_BIT && pos < MAX_BIT){
        Byte mask(~( 0x1 << pos));
        _data = (_data & mask.getValue());
    } 
    else {
        cout << "OFFBIT Error: valor de pos no valido" << endl;
    }
}

bool Byte::getBit(int pos) const{
    if (pos >= MIN_BIT && pos < MAX_BIT){
        bool estado;
        Byte mask(0x1 << pos);
        if ( (_data & mask.getValue()) == 0x0 ){
            estado = 0;
        } 
        else {
            estado = 1;
        }
        return estado;
    } 
    else {
        cout << "GETBIT Error: valor de pos no valido" << endl;
    }
}

string Byte::to_string() const{
    string byte_string="";
    
    for (int i = 7; i >= 0; i--){
        if (getBit(i) == false){
            byte_string += "0";
        }
        else if (getBit(i) == true){
            byte_string += "1";
        }
        else {
            cout  << "Error: bit " << i << " distinto de 0 o 1";
        }
    }
    return byte_string;
}

void Byte::onByte(){
    _data = 0b11111111;
}

void Byte::offByte(){
    _data = 0b00000000;
}

void Byte::encodeByte(const bool v[]){
    for (int i = 0; i <= 7; i++){
        if(v[i] == false){
            offBit(7-i);
        }
        else {
            onBit(7-i);
        }
    }
}

void Byte::decodeByte(bool v[]){
    for (int i = 0; i <= 7; i++ ){
        if(getBit(i) == 1){
            v[7-i] = 1;
        }
        else if (getBit(i) == 0){
            v[7-i] = 0;
        }
        else {
            cout << "Error: bit " << i << " distinto de 0 o 1";
        }
    }
}

void Byte::decomposeByte(int posits[], int &n){
    n = 0;
    for(int i = 0; i <= 7; i++){
        if (getBit(i) == 1){
            posits[n] = i;
            n++;
        }
    }
}

void Byte::shiftRByte(int n){
    if (n >= 0 && n <= 7){
        _data = (_data >> n);
    }
    else {
        cout << "Error: valor de n no valido" << endl;
    }
}

void Byte::shiftLByte(int n){
    if (n >= 0 && n <= 7){
        _data = (_data << n);
    }
    else {
        cout << "Error: valor de n no valido" << endl;
    }
}

void Byte::mergeByte(Byte merge, int percentage){
    if (percentage > 0 || percentage < 100){
        _data = (_data*(100-percentage) + merge.getValue()*percentage)/100;
    }
    else {
        cout << "Error: porcentaje fuera de formato" << endl;
    }
 }
    