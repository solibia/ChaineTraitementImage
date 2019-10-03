
/* Nom du fichier   : fonction.cpp
   Auteur           : PAZIMNA Solibia
   Date de creation : 31-03-2019
   Objet            : Fonction principal du programme.
*/

#include <stdio.h>
#include <stdlib.h>
#include <opencv/cv.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include "opencv2/opencv.hpp"
#include <string>


using namespace cv;
using namespace std;

Mat fonctionLineaireSaturation(Mat image, Point point1, Point point2){
    Mat imageFinale(image.rows, image.cols, CV_8UC3, Scalar( 255,255,255));

    for(int i = 0; i < image.rows; i++){

        for(int j = 0; j < image.cols; j++){

            for(int c = 0; c < 3; c++){

					int val = image.at<Vec3b>(i,j)[c];

					if(val>=0 && val <= point1.x) val = 0;
    				else if(point1.x < val && val <= point2.x)
						val =(int) (255/(point2.x - point1.x)) * (val - point1.x);
    				else if(point2.x < val && val <= 255) val = 255;
               imageFinale.at<Vec3b>(i,j)[c] = saturate_cast<uchar>(val);
            }
        }
    }

    return imageFinale;
}

void saveImg(string fileName, Mat img){
    if(!imwrite(fileName, img))
    cout<<"Erreur lors de l'enregistrement";
}
//Fonction d'affichage du message d'accueil
string getIDNomImage()
/*
    Objet           : Fonction permettant de générer un identifiant
                        aléatoire a partir de l'heure système
    Type de retour  : string
*/
{
    time_t seconds;
    time(&seconds);
    stringstream ss;
    ss << seconds;
    string ts = ss.str();
    return "_"+ts;
}//Fin de la fonction getIDNomImage


void segmentation(string img_path){
    Point p1, p2;
    p1.x = 10;
    p1.y = 0;
    p2.x = 120;
    p2.y = 255;

    Mat inputImg_brut = imread(img_path, CV_LOAD_IMAGE_COLOR);
    //controle si l'image est valide ou non
    if (!inputImg_brut.data) {
        cout << "\n** Veuillez renseigner une image valide! **\n";
    }else {
        //application de l'algoritme du contraste à l'image
        string timeStamp = getIDNomImage();
        Mat inputImg = fonctionLineaireSaturation(inputImg_brut, p1, p2);
        imshow("Image Contraste ", inputImg);
        // enregistrement de l'image contrasté dans un dossier
        saveImg("images_contrast/Contrast_Img_"+timeStamp+".png", inputImg);

        Mat otsu_Img;
        // conversion de l'image couleur en image en niveaux de gris
        cvtColor(inputImg, otsu_Img, COLOR_RGB2GRAY);

        // application de l'algorithme d'OTSU
        threshold(otsu_Img, otsu_Img, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);

        /// fin de la phase de Segmentation

        /// début de la phase de post-Segmentation

        // définition des éléments structurants
        Mat struct_element_1 = getStructuringElement(MORPH_RECT, Size(16, 16));
        Mat struct_element_2 = getStructuringElement(MORPH_RECT, Size(7, 7));
        Mat transform_Img;

        /// dilatation et erosion
        dilate(otsu_Img, transform_Img, struct_element_1);
        dilate(otsu_Img, transform_Img, struct_element_1);
        erode(transform_Img, transform_Img, struct_element_2);
        erode(transform_Img, transform_Img, struct_element_2);

        // déclaration d'un vecteur pour contenir les contours des régions détectées
        vector<vector<Point> > contours;

        // recherche des contours des régions détectées
        findContours(transform_Img, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

        // etiquetage des régions
        Mat markers = Mat::zeros(transform_Img.size(), CV_32SC1);
        for (size_t i = 0; i < contours.size(); i++)
            drawContours(markers, contours, static_cast<int>(i),
                    Scalar::all(static_cast<int>(i) + 1), -1);

        /// génération aléatoire de couleurs
        vector<Vec3b> colors;
        for (size_t i = 0; i < contours.size(); i++) {
            int b = theRNG().uniform(0, 255);
            int g = theRNG().uniform(0, 255);
            int r = theRNG().uniform(0, 255);
            colors.push_back(Vec3b((uchar) b, (uchar) g, (uchar) r));
        }

        // coloration des régions avec des couleurs différentes
        Mat post_seg_img = Mat::zeros(markers.size(), CV_8UC3);
        for (int i = 0; i < markers.rows; i++) {
            for (int j = 0; j < markers.cols; j++) {
                int index = markers.at<int>(i, j);
                if (index > 0 && index <= static_cast<int>(contours.size()))
                    post_seg_img.at<Vec3b>(i, j) = colors[index - 1];
                else
                    post_seg_img.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
            }
        }

        // ET logique entre l'image entrée et le masque obtenu après post-segmentation
        Mat seg_img;
        inputImg.copyTo(seg_img, post_seg_img);

        /// fin de la phase de post-Segmentation

        /// affichage des résultats
        // image originale
        imshow("Image Originale", inputImg_brut);

        // image ayant subi l'algo OTSU
        imshow("Image Segmentation OTSU", otsu_Img);
        // enregistrement de l'image dans un dossier
        saveImg("output/OTSU_Segment_Img_"+timeStamp+".png", otsu_Img);

        // image post-segmentation
        imshow("Image Post-Segmentation", post_seg_img);
        // enregistrement de l'image dans un dossier
        saveImg("output/Post_Segment_Img.png_"+timeStamp+".png", post_seg_img);

        // image segmentee
        imshow("Image Segmentee", seg_img);
        // enregistrement de l'image dans un dossier
        saveImg("output/Segment_Img.png_"+timeStamp+".png", seg_img);

        waitKey(0);
    }
}
