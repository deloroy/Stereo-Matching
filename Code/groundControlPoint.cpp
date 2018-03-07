//
// Created by Yonatan Deloro et Clément Riu, Projet MOPSI 2016-2017 ENPC
//

#include "groundControlPoint.h"

std::map<int, std::vector<int> >
groundControlPointDetect(int largeur, const Img &image1, const Img &image2,
                         int ligne, Imagine::Image<int, 2> &cost,
                         int GCPWidth) {
    //! Finds GCP and returns them.

    std::map<int, std::vector<int> > pointsConfiance; // Recense les paires de
    // (pixels de l'image 1, vecteur de pixels de l'image 2 associables)
    int sum;

    for (int colL = GCPWidth; colL < largeur - GCPWidth; ++colL) {
        for (int colR = GCPWidth; colR < largeur - GCPWidth; ++colR) {
            sum = 0;
            for (int cross = -GCPWidth; cross < GCPWidth + 1; ++cross) {
                sum += abs(image1(colL + cross, ligne) -
                           image2(colR + cross, ligne));
                sum += abs(image1(colL, (ligne + cross)) -
                           image2(colR, (ligne + cross)));
            }
            if (sum == 0) {
                std::map<int, std::vector<int> >::iterator it_points =
                        pointsConfiance.find(colL);
                if (it_points == pointsConfiance.end()) {
                    pointsConfiance[colL] = std::vector<int>();
                    pointsConfiance[colL].push_back(colR);
                } else {
                    (*it_points).second.push_back(colR);
                }
            }
        }
    }
    return pointsConfiance;
}

void groundControlPointCost(int largeur, const Img &image1, const Img &image2,
                            int ligne, Imagine::Image<int, 2> &cost,
                            int GCPWidth) {

    //! Change the cost map in order to use GCP.
    std::map<int, std::vector<int> > pointsConfiance = groundControlPointDetect(
            largeur, image1, image2, ligne, cost, GCPWidth);

    std::map<int, std::vector<int> >::iterator
            it_points = pointsConfiance.begin();

    for (; it_points != pointsConfiance.end(); it_points++) {

        int colL = (*it_points).first; //le pixel de gauche est associé
        bool confiant;
        // surement à un pixel de l'image de droite
        for (int colR = 0; colR < largeur; ++colR) {
            confiant = false;
            for (int ptConfInd = 0;
                 ptConfInd < (*it_points).second.size(); ++ptConfInd) {
                if ((*it_points).second[ptConfInd] == colR) {
                    confiant = true;
                }
            }
            if (!confiant) {   //on ne peut pas associer au pixel de gauche
                // un des autres pixels de l'image de droite que ceux sélectionnées
                cost(colL, colR) = 100000;
            }
        }

    }
}
