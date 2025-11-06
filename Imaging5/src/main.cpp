/**
 * @file main.cpp
 * @note To be implemented by students.
 * @author MP-DGIM, MP-IADE, MP-II (grupo B) 
 */
#include <iostream>
#include <cstring>
#include <sstream>
#include "MPTools.h"
#include "Byte.h"
#include "Image.h"
#include "Histogram.h"

using namespace std;

static const int ERROR_ARGUMENTS = 100;

/**
 * @brief It waits until the key [INTRO] [RETURN] is pressed
 */
void pressReturnToContinue();

/**
 * @brief it shows help about the syntax of the command line
 */
void showHelp();

/**
 * @brief It breaks the program due to some error and shows a message
 * @param errorcode The type of error as declared in Image.h
 * @param errordata A message trying to describe the error
 */
void errorBreak(int errorcode, const string & errordata);

void readText(string file, char text[]){
    ifstream f(file, ios::in);
    if(f){
        char c;
        int n=0;
        while(f>>c){
            text[n++]=c;
        }
        f.close();
    }
    else {
        cerr << "Unable to open the file" << file << endl;
    }
}

int main(int nargs, char**args) {
    
    string input, copy, output, aux, hide_image, hide_text;

    Image im_input, im_output, im_copyfrom, im_bin, im_cut, im_histogram, im_collection[10], im_aux, collection, hidden_image,imagen;
    int res, n, x, y, w, h, k, z, max = 2000000;
     char hidden_text[max], text[max];
    bool hay_input = false, hay_output = false, hay_st = false, hay_ht = false, hay_si = false, hay_hi = false, correcto = true;
    
    for (int n = 1; n < nargs;) { // COMPROBACIONES
        aux = args[n];
        if (aux == "-i" && n < nargs) {
            n++;
            input = args[n++];
            hay_input = true;
        } else if (aux == "-o" && n < nargs) {
            n++;
            output = args[n++];
            hay_output = true;
        } else if (aux == "-p" && n < nargs) {
            n++;
            if (atoi(args[n])>=0 && atoi(args[n])<8){
                k = atoi(args[n++]);
            } else {
                errorBreak(ERROR_ARGUMENTS, "k must be greater than 0 and smaller than 8" );
            } 
        } else if (aux == "-ht" && n < nargs) {
            n++;
            hide_text = args[n++];
            hay_ht = true;
        } else if (aux == "-st" ) {
            n++;
            hay_st = true;
        } else if (aux == "-hi" && n < nargs) {
            n++;
            hide_image = args[n++];
            hay_hi = true;
        } else if (aux == "-si") {
            n++;
            hay_si = true;
        }
        else {
            errorBreak(ERROR_ARGUMENTS, " Unkown argument " + aux);
        }
    }
    
    if (!hay_input) {
        errorBreak(ERROR_ARGUMENTS, "Input file needed");
    } 
    else {
        int errorcode;
        errorcode = im_input.readFromFile(input.c_str());
        
        
        
//        cout << endl << "[im_input] " << im_input.inspect() << endl;
        
        
        if(hay_ht){
            readText(hide_text,text);
            correcto = im_input.hideText(text,k);  
            if (!correcto){
            cout << "ERROR: image input too small to hide the text given";
            }
        } else if(hay_st){
            correcto = im_input.showText(hidden_text,max,k);

            for (int i=0; i<strlen(hidden_text); i++ ){
                cout << hidden_text[i];
            }
            if (!correcto){
            cout << "ERROR: format error (the image input does not have a hidden text) or the text hidden has more characters than max";
            }
        } else if(hay_hi){
            correcto = im_input.hideImage(k,imagen);
            if (!correcto){
            cout << "ERROR: u";
            }
        
        } else if(hay_si){
            correcto = im_input.showImage(k, hidden_image);
            if (!correcto){
            cout << "ERROR: h";
            }
        }
    }
    
//    cout << endl << "[im_input] " << im_input.inspect() << endl;
//    im_input.showInWindow("input");

//        im_output.showInWindow("output");
        
        if (hay_output ) {
            im_output = im_input;
            im_output.saveToFile(output.c_str());
  //          cout << endl << "[im_output] " << im_output.inspect() << endl;
        }
     
    
    
    return 0;
}

void pressReturnToContinue() {
    char aux[10];
    cout << "Press [RETURN] to continue ...";
    cin.getline(aux, 1, '\n');
}

void showHelp() {
    cout << "Please use: -i <imageCarrier> [" << "-z" << " <-1|+1> " << "-p" << " <k=0> " << "-hi" << " <textSource> " << "-" << " " << "-hi" << " <imageSource> " << "-sh" <<"]" << endl;
    cout << "\n\t-i <imageCarrier> \t\tImage to carry hiden objets" << endl;
    cout << "\n\t-o <output> \t\t\t(OPT) File to store the result" << endl;
    cout << "\n\t" << "-z" << " <-1|+1> " << "\t\t\t(OPT) zooming image in (>0) and out (<0). Default value is 0" << endl;
    cout << "\n\t" << "-p" << " <k> " << " \t\t\t(OPT) bit-plane to deal with. Its default is 0" << endl;
    cout << "\n\t" << "-ht" << " <textSource> " << " \t\t(OPT) Hide text comtained in the file " << endl;
    cout << "\n\t" << "-st" << "\t\t\t\t(OPT) Showing the text encoded in the file " << endl;
    cout << "\n\t" << "-hi" << "imagesource" <<  "\t\t(OPT) Hide image comtained in the file " << endl;
    cout << "\n\t" << "-si" << "\t\t\t\t(OPT) Showing the image encoded in the file " << endl;
}
void errorBreak(int errorcode, const string & errordata){
    switch (errorcode) {
        case ERROR_ARGUMENTS:
            cout << endl << "Error in call: " << errordata << endl;
            showHelp();
            break;
        case Image::IMAGE_ERROR_OPEN:
            cout << endl << "Error opening file " << errordata << endl;
            break;
        case Image::IMAGE_ERROR_DATA:
            cout << endl << "Data error in file " << errordata << endl;
            break;
        case Image::IMAGE_ERROR_FORMAT:
            cout << endl << "Unrecognized format in file " << errordata << endl;
            break;
        case Image::IMAGE_TOO_LARGE:
            cout << endl << "The image is too large and does not fit into memory " << errordata << endl;
            break;
    }
    std::exit(1);
}


// if (hay_zoom){
//        if (z==-1){
//            im_input = im_input.zoomOut();
//        } else if (z==1){
//            im_input = im_input.zoomIn();
//        }   
//    }
//        
//        if (hay_copy) {
//            im_copyfrom.readFromFile(copy.c_str());
////            im_copyfrom.showInWindow("copyfrom");
// //           cout << endl << "[im_copyfrom] " << im_copyfrom.inspect() << endl;
//
//            w = im_copyfrom.width() + 5;
//            h = im_copyfrom.height() + 5;
//
//            im_bin = im_copyfrom.threshold();
//      //      im_bin.showInWindow("threshold");
// //           cout << endl << "[im_bin] " << im_bin.inspect() << endl;
//
//            x = 0;
//            im_input.pasteArea(x, 0, im_copyfrom);
//
//            im_copyfrom.extractObjects(im_collection, n, 3);
//            im_cut = im_collection[0];
//            im_collection[0].showInWindow("segmented");
//  //          cout << endl << "[im_collection[0]] " << im_collection[0].inspect() << endl;
//
//            x = x + w;
//            im_input.pasteArea(x, 0, im_cut);
//            im_input.pasteArea(x, h, im_bin);
//
//            x = x + w;
//            im_input.pasteArea(x, 0, im_cut, 64);
//            im_input.pasteArea(x, h, im_bin, 64);
//
//            x = x + w;
//            im_input.pasteArea(x, 0, im_cut, 64, 50);
//            im_input.pasteArea(x, h, im_bin, 64, 50);
//        }
//        im_output = im_input;
////        im_output.showInWindow("output");