//
// Created by Yonatan Deloro et Clément Riu, Projet MOPSI 2016-2017 ENPC
//

#include "imageProcessingFunc.h"

int loadingImage(Img &image1, Img &image2, const std::string &name) {
    //! Loads the images in image1 (left) and image2 (right).
    if (name == "cone") {
        if (!Imagine::load(image1, srcPath("cone/cone_L.png"))) {
            return 1;
        }
        if (!Imagine::load(image2, srcPath("cone/cone_R.png"))) {
            return 1;
        }
    }
    if (name == "tsuku") {
        if (!Imagine::load(image1, srcPath("tsukuba/tsuku_col1.ppm"))) {
            return 1;
        }
        if (!Imagine::load(image2, srcPath("tsukuba/tsuku_col5.ppm"))) {
            return 1;
        }
    }

    // Checking the images are the same size.
    if (image1.width() == image2.width() &&
        image1.height() == image2.height()) {
        std::cout << "Chargement réussi." << std::endl;
        return 0;
    } else {
        return 1;
    }
}


void edgeDetector(const Img &image, Img &imageOutput) {
    //! Find the edges of image using the Sobel method.
    byte seuil = 32; // Si le gradient est en deça de ce seuil, on met le bord
    // en noir, sinon en blanc.

    int hauteur = image.height();
    int largeur = image.width();

    Imagine::Image<double, 2> imageDouble(largeur, hauteur);

    int valX, valY = 0;
    double valSqrt;
    double max = 0;

    int gX[3][3]; // Sobel horizontal gradient operator.
    int gY[3][3]; // Sobel vertical gradient operator.
    // x mask
    gX[0][0] = 1;
    gX[0][1] = 0;
    gX[0][2] = -1;
    gX[1][0] = 2;
    gX[1][1] = 0;
    gX[1][2] = -2;
    gX[2][0] = 1;
    gX[2][1] = 0;
    gX[2][2] = -1;
    // y mask
    gY[0][0] = 1;
    gY[0][1] = 2;
    gY[0][2] = 1;
    gY[1][0] = 0;
    gY[1][1] = 0;
    gY[1][2] = 0;
    gY[2][0] = -1;
    gY[2][1] = -2;
    gY[2][2] = -1;


    // Edge detection :
    for (int ligne = 0; ligne < hauteur; ++ligne) {
        for (int colonne = 0; colonne < largeur; ++colonne) {

            // The border is set to 0.
            if ((ligne == 0) || (ligne == hauteur - 1) || (colonne == 0) ||
                (colonne == largeur - 1)) {
                valX = 0;
                valY = 0;
            } else {
                // Computing the X and Y convolutions
                valX = valY = 0;
                for (int caseVert = -1; caseVert <= 1; ++caseVert) {
                    for (int caseHori = -1; caseHori <= 1; ++caseHori) {
                        valX = valX +
                               image(colonne + caseHori, ligne + caseVert) *
                               gX[1 + caseVert][1 + caseHori];
                        valY = valY +
                               image(colonne + caseHori, ligne + caseVert) *
                               gY[1 + caseVert][1 + caseHori];
                    }
                }
            }
            valSqrt = sqrt(valX * valX + valY * valY);
            // Setting the image value in a double image
            imageDouble(colonne, ligne) = valSqrt;

            if (valSqrt > max) {
                max = valSqrt;
            }
        }
    }

    // The obtain results is 0 or 255.
    for (int ligne = 0; ligne < hauteur; ligne++) {
        for (int colonne = 0; colonne < largeur; ++colonne) {
            if (imageDouble(colonne, ligne) > seuil * max / 255) {
                imageOutput(colonne, ligne) = 255;
            } else {
                imageOutput(colonne, ligne) = 0;
            }
        }
    }
}

byte transform(int valeur, int valMax, int valMin) {
    //! Gives a value between 0 and 254 to a given value.
    return byte(255 * (valeur - valMin) / (valMax - valMin));
}

Imagine::Image<Imagine::Color, 2>
disparityToDepth(const Imagine::Image<int, 2> &disparity, int dispMax,
                 int dispMin, int largeur, int hauteur) {
    //! Gives back the depth map from a disparity map.
    Imagine::Image<Imagine::Color, 2> depth(largeur, hauteur);

    for (int ligne = 0; ligne < hauteur; ++ligne) {
        for (int colonne = 0; colonne < largeur; ++colonne) {
            int localDisparity = disparity(colonne, ligne);
            if (localDisparity == -1) {
                depth(colonne, ligne) = Imagine::Color(155, 0, 0);
            } else {
                if (localDisparity == 0) {
                    depth(colonne, ligne) = Imagine::Color(0, 255, 0);
                } else {
                    byte col = transform(disparity(colonne, ligne), dispMax,
                                         dispMin);
                    depth(colonne, ligne) = Imagine::Color(0, col, col);
                }
            }
        }
    }
    return depth;
}
