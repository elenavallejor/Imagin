/**
 * @file Image.cpp
 * @note To be implemented by students
 * @author MP-DGIM, MP-IADE, MP-II (grupo B)
 */


#include<iostream>
#include <fstream>
#include<cstring>
#include "MPTools.h"
#include "Byte.h"
#include "Image.h"

using namespace std;

Image::Image() {
    _width = 0;
    _height = 0;
    _data = nullptr;
}

Image::Image(int width, int height) {
    setImage(height, width);
    flatten(Byte(0x0));
}

int Image::height() const {
    return _height;
}

int Image::width() const {
    return _width;
}

void Image::setPixel(int x, int y, const Byte &v) {
    if (x < _width && x >= 0 && y < _height && y >= 0) {
        _data[y][x] = v;
    }
}

int Image::getPixel(int x, int y) const {
    if (x < _width && x >= 0 && y < _height && y >= 0) {
        return _data[y][x].getValue();
    } else {
        return -1;
    }
}

void Image::setPos(int i, const Byte &v) {
    if (i >= 0 && i < _height * _width) {
        setPixel(i % _width, i / _width, v);
    }
}

int Image::getPos(int i) const {
    if (i >= 0 && i < _height * _width) {
        return getPixel(i % _width, i / _width);
    } else {
        return -1;
    }
}

void Image::flatten(const Byte &b) {
    for (int i = 0; i < _width; i++) {
        for (int j = 0; j < _height; j++) {
            setPixel(i, j, b);
        }
    }
}

void Image::showInWindow(string title) const {
    string tempname = "data" + SLASH + ".hidden_" + title + ".pgm";

    mpSavePGMImage(tempname, (const unsigned char *) _data[0], _width, _height);
    mpDisplayImage(tempname, title);
}

string Image::inspect() const {
    string result = "";
    result = std::to_string(_width) + "x" + std::to_string(_height) + " " + std::to_string(mphash((const unsigned char*) _data[0], _height * _width));
    return result;
}

int Image::readFromFile(const char filename[]) {
    int h, w, aux;
    Byte b;
    ifstream fentrada;
    char c1, c2;

    cout << endl << "...Reading image from " << filename << endl;
    fentrada.open(filename);
    if (!fentrada) {
        return IMAGE_ERROR_OPEN;
    }
    fentrada >> c1 >> c2;
    if (c1 != 'P') {
        return IMAGE_ERROR_FORMAT;
    }
    if (c2 != '2') {
        return IMAGE_ERROR_FORMAT;
    }
    fentrada >> w >> h >> aux;

    if (w * h > IMAGE_MAX_SIZE) {
        return IMAGE_TOO_LARGE;
    }
    setSize(h, w);
    cout << w << "x" << h << endl;
    for (int y = 0; y < height(); y++) {
        for (int x = 0; x < width(); x++) {

            fentrada >> aux;
            this->setPixel(x, y, Byte(aux));
        }
    }
    if (!fentrada) {
        return IMAGE_ERROR_DATA;
    }
    fentrada.close();
    return IMAGE_DISK_OK;

}

int Image::saveToFile(const char filename[]) const {
    ofstream fsalida;
    cout << endl << "...Saving image into " << filename << endl;
    if (height() == 0 || width() == 0) {
        return IMAGE_DISK_OK;
    }
    fsalida.open(filename);
    if (!fsalida) {
        return IMAGE_ERROR_OPEN;
    }
    fsalida << "P2" << endl;
    fsalida << width() << " " << height() << endl;
    fsalida << "255" << endl;
    for (int y = 0; y < height(); y++) {
        for (int x = 0; x < width(); x++) {
            fsalida << (int) this->getPixel(x, y) << " ";
        }
    }
    if (!fsalida) {
        return IMAGE_ERROR_DATA;
    }
    fsalida.close();
    return IMAGE_DISK_OK;
}

Histogram Image::getHistogram() const {
    Histogram histograma;
    for (int i = 0; i < _width * _height; i++) {
        histograma.setLevel(Byte(getPos(i)), histograma.getLevel(getPos(i)) + 1);
    }
    return histograma;
}

Image Image::depictsHistogram() const {
    Histogram histograma = getHistogram();

    int width = 256, height = 160, border = 1, foot_tone = 9, a = height - (border + foot_tone) - 2;
    int normal;

    Image result(width, height);

    // compute max
    int max = histograma.getMaxLevel();
    // build white image
    result.flatten(Byte(255));

    for (int x = 0; x < 256; x++) {
        // compute the relative point from which the pixel is black on the image
        normal = histograma.getLevel(Byte(x)) * a / max;

        // set the pixel
        for (int j = height - foot_tone - 2; j >= height - foot_tone - 2 - normal; j--) {

            result.setPixel(x, j, Byte(0));
        }

        for (int y = 0; y < foot_tone; y++) { // the foot tone in the image
            result.setPixel(x, height - border - y, x);
        }
    }
    return result;
}

void Image::extractObjects(Image set [], int &nimages, int maximages, double tolerance) {
    Histogram histograma = getHistogram();

    nimages = 0;
    int max = histograma.getMaxLevel();
    int tono_max, tono_min = -1;
    int tono = 255;

    Image imagen(_width, _height);

    while (tono >= 0) {
        if (tolerance * max <= histograma.getLevel(Byte(tono))) {
            tono_max = tono;
            while (tolerance * max <= histograma.getLevel(Byte(tono))) {
                tono--;
            }
            cout << "start " << tono_min << endl;
            tono_min = tono;

            for (int i = 0; i < _height * _width; i++) {
                if (getPos(i) >= tono_min && getPos(i) <= tono_max) {
                    imagen.setPos(i, Byte(getPos(i)));
                } else {
                    imagen.setPos(i, Byte(0));
                }
            }
            set[nimages] = imagen;
            if (tono_min >= 0 && tono_max <= 255) {
                cout << "Found object " << nimages << " in [" << tono_min << "," << tono_max << "]" << endl;
                nimages++;
                tono = tono_min;
            }
        } else {
            tono--;
        }
    }
    cout << "Found " << nimages << " objects" << endl;
}

Image Image::threshold(int t) const {
    Image result(_width, _height);
    if (t < 0 || t > 255) {
        t = getHistogram().getBalancedLevel();
    }

    for (int i = 0; i < _width; i++) {
        for (int j = 0; j < _height; j++) {
            if (getPixel(i, j) > t) {
                result.setPixel(i, j, Byte(255));
            }
        }
    }
    cout << "Thresholding to level " << t << endl;
    return result;
}

Image Image::copyArea(int x, int y, int w, int h) const { //
    Image result(w, h);
    for (int i = x; i < x + w && i < _width; i++) {
        for (int j = y; j < y + h && j < _height; j++) {
            if (i >= 0 && i < _width && j >= 0 && j < _height) {
                result.setPixel(i, j, getPixel(i, j));
            }
        }
    }
    return result;
}

void Image::pasteArea(int x, int y, const Image &from, int toneup, int merge) {
    if (toneup < 0 || toneup > 255) {
        toneup = -1;
    }
    if (merge < 0 || merge > 100) {
        merge = 100;
    }
    int k = 0, l;
    for (int i = x; i < x + from.width(); i++) {
        l = 0;
        for (int j = y; j < y + from.height(); j++) {
            if (i >= 0 && i < _width && j >= 0 && j < _height && from.getPixel(k, l) > toneup) {
                setPixel(i, j, Byte((from.getPixel(k, l) * merge + getPixel(k, l)*(100 - merge)) / 100));
            }
            l++;
        }
        k++;
    }
}

Image::~Image() {
    clear();
}

Image::Image(const Image &from) {
    setImage(from.height(), from.width());
    for (int i = 0; i < from.width(); i++) {
        for (int j = 0; j < from.height(); j++) {
            setPixel(i, j, Byte(from.getPixel(i, j)));
        }
    }
}

Image &Image::operator=(const Image &rhs) {
    if (this != &rhs) {
        copy(rhs);
    }
    return *this;
}

Image Image::zoomIn() const {
    Image zoom(2 * _width, 2 * _height);
    for (int i = 0; i < zoom.width(); i++) {
        for (int j = 0; j < zoom.height(); j++) {
            if (i % 2 == 0 || j % 2 == 0) {
                zoom.setPixel(i, j, Byte(getPixel(i / 2, j / 2)));
            } else if (i % 2 != 0 && j % 2 != 0) {
                zoom.setPixel(i, j, Byte(copyArea(i / 2 - 1, j / 2 - 1, 3, 3).getHistogram().getBalancedLevel()));
            }
        }
    }
    cout << "...Zooming in" << endl;
    return zoom;
}

Image Image::zoomOut() const {
    Image zoom(_width / 2, _height / 2);
    for (int i = 0; i < zoom.width(); i++) {
        for (int j = 0; j < zoom.height(); j++) {
            zoom.setPixel(i, j, Byte(copyArea(i * 2 - 1, j * 2 - 1, 3, 3).getHistogram().getBalancedLevel()));
        }
    }
    cout << "...Zooming out" << endl;
    return zoom;
}


bool Image::showImage(int k, Image &result) const {
    bool correcto = true;
    bool codificado[Byte::NUM_BITS];
    Byte pixel;
    int w,h, modificados;

   
    for ( int i=0; i<5*Byte::NUM_BITS && correcto; i++ ){
        codificado[i % Byte::NUM_BITS] = Byte(getPos(i)).getBit(k);
        
        if (i==Byte::NUM_BITS-1){ // CASO I
            pixel.encodeByte(codificado);
            correcto = true;
            if (char(pixel.getValue()!='I')){
                cout << "a";
                correcto = false;
            }  
   }      
         if ( i==2*Byte::NUM_BITS-1){ // CASO M
            pixel.encodeByte(codificado);
            correcto = true;
            if (char(pixel.getValue()!='M')){
                cout << "aaa";
                correcto = false;
            }
        }
        else if ( i==3*Byte::NUM_BITS-1){ // CASO G
            pixel.encodeByte(codificado);
            correcto = true;
            if (char(pixel.getValue()!='G')){
                correcto = false;
            }
        }
        else if ( i==4*Byte::NUM_BITS-1){ // WIDTH
            pixel.encodeByte(codificado);
            w = pixel.getValue();
        }
        else if ( i==5*Byte::NUM_BITS-1){ // HEIGHT
            pixel.encodeByte(codificado);
            h = pixel.getValue();
        } 
        modificados++;
    }
    
    if( height()*width() < (w*h+5)*8 ){
        correcto = false;
    }
    if (correcto){
        Image local(w,h);
        
        for ( int i=5*Byte::NUM_BITS ; modificados<w*h*8 && correcto; i++ ){
            codificado[i % Byte::NUM_BITS] = Byte(getPos(i)).getBit(k);
            if (i%8== 7) {
                pixel.encodeByte(codificado);
                local.setPos(modificados,pixel);
                modificados++;
            }
        }
        result = local;
    } 
    return correcto;
}



bool Image::hideImage(int k, const Image &from) {
    bool correcto = true;
    int n = (from.width() * from.height() + 5)*8;
    if ( from.width() > 255 || from.height() > 255 || n > width()*height() ) {
        correcto = false;      
    }
    
    if (correcto) {
        int modificados = 0;
        Byte temporal(73);
        for (int j = 0; j<4*Byte::NUM_BITS - 1; j++) {
            if (modificados == Byte::NUM_BITS){
                temporal.setValue(77);
            }
            else if (modificados == 2*Byte::NUM_BITS){
                temporal.setValue(71);
            }
            else if (modificados == 3*Byte::NUM_BITS){
                temporal.setValue(from.width());
            }
            
            else if (modificados == 4*Byte::NUM_BITS){
                temporal.setValue(from.height());
            }
            
            
            if (temporal.getBit(j%Byte::NUM_BITS)) {
                _data[modificados/width()][modificados%width()].onBit(k);
            }
            else {
                _data[modificados/width()][modificados%width()].offBit(k);
            }
            modificados++;
        }
        
        for (int i = 0; i < from.width() * from.height(); i++) {
            for (int j = Byte::NUM_BITS - 1; j >= 0; j--) {
                temporal.setValue(from.getPos(i));

                if (temporal.getBit(j)) {
                    _data[modificados / width()][modificados % width()].onBit(k);
                }
                else {
                    _data[modificados / width()][modificados % width()].offBit(k);
                }
                modificados++;
            }
        }
    }
    return correcto;
}

bool Image::hideText(char source [], int k) {
    bool correcto = true;
    Byte caracter;
    if (width() * height() < (strlen(source) + 1)*8 ) {
        correcto = false;
    }
    
    if (correcto) {
        int n = 0;
        caracter.setValue(84); // CASO T
        for (int j = 0; j<3*Byte::NUM_BITS - 1; j++) {
            if (n == Byte::NUM_BITS){ // CASO X
                caracter.setValue(88);
            }
            else if (n == 2*Byte::NUM_BITS){ // CASO T
                caracter.setValue(84);
            }
            

        }
        for (int i =0; i <= strlen(source); i++) {
            caracter.setValue(source[i]);
            for (int j = Byte::NUM_BITS - 1; j >= 0; j--) {
                if (caracter.getBit(j)) {
                    _data[n / width()][n % width()].onBit(k);
                } else {
                    _data[n / width()][n % width()].offBit(k);
                }
                n++;
            }
        }
    }
    return correcto;
}

bool Image::showText(char hidden [], int max, int k) const {
    bool correcto = true;
    int n_caracteres = 0;
    bool codificado[Byte::NUM_BITS];
    Byte caracter;
    for (int i=0; i<3*Byte::NUM_BITS ;i++){
        codificado[i%Byte::NUM_BITS] = Byte(getPos(i)).getBit(k);
        if (i == Byte::NUM_BITS - 1 || i == 3 * Byte::NUM_BITS - 1) {
            caracter.encodeByte(codificado);
            if (char(caracter.getValue()) != 'T') {
                correcto = false;
            }
        } else if (i == 2*Byte::NUM_BITS-1) {
            caracter.encodeByte(codificado);
            if (char(caracter.getValue()) != 'X') {
                correcto = false;
            }
        }
    }
    
    for (int i=3*Byte::NUM_BITS; i < width()*height() && correcto; i++) {
        codificado[i%Byte::NUM_BITS] = Byte(getPos(i)).getBit(k);
         
        if (i % 8 == 7) {
            caracter.encodeByte(codificado);
            hidden[n_caracteres] = char(caracter.getValue());
            n_caracteres++;
            if (n_caracteres > max) {
                correcto = false;
            }

        }
    }
    
    return correcto;
}

void Image::clear() {
    if (_data != nullptr) {
        delete [] _data[0];
        delete [] _data;
        _data = nullptr;
    }
}

void Image::copy(const Image &rhs) {
    if (_height != rhs.height() || _width != rhs.width()) {
        setSize(rhs.height(), rhs.width());
    }
    for (int i = 0; i < rhs.width(); i++) {
        for (int j = 0; j < rhs.height(); j++) {
            setPixel(i, j, Byte(rhs.getPixel(i, j)));
        }
    }
}

void Image::setSize(int height, int width) {
    if (height != _height || width != _width) {
        clear();
        setImage(height, width);
    }
}

void Image::setImage(int height, int width) {
    if (height > 0 && width > 0) {
        _width = width;
        _height = height;
        _data = new Byte * [_height];
        _data[0] = new Byte [_height * _width];
        for (int i = 1; i < height; ++i) {
            _data[i] = _data[i - 1] + _width;
        }
    }
}


// Es mas sencillo que el parametro 'p' solo se lea una vez

// Hacer la funcion de desbordamiento para mas nota!