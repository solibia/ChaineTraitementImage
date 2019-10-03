
/* Nom du fichier   : main.cpp
   Auteur           : PAZIMNA Solibia
   Date de creation : 31-03-2019
   Objet            : Fonction principal du programme.
*/

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <ctype.h>

#include <stdlib.h>
#include <stdio.h>

using namespace cv;
using namespace std;

void segmentation(string img_path);


int main(int argc, char** argv)
{
    std::string current_exec_name = argv[0]; // Name of the current exec program
       std::string first_arge;
       std::vector<std::string> all_args;
       if (argc > 1) {
         first_arge = argv[1];
         all_args.assign(argv + 1, argv + argc);//Regroupement des parametres de dans un vecteur de string
       }
       if (all_args.size()> 0) {
           segmentation(all_args.at(0));
       }else {
           cout<<"Veuillez entrer le path de l'image svp";
       }
    return 0;
}
