//
// Created by Yonatan Deloro et Clément Riu, Projet MOPSI 2016-2017 ENPC
//

#include "shorterPath.h"


Imagine::Image<int, 2> initDisparity(int largeur, int hauteur) {
    //! Initialise the disparity map.
    Imagine::Image<int, 2> disparity(largeur, hauteur);
    for (int ligne = 0; ligne < hauteur; ++ligne) {
        for (int colonne = 0; colonne < largeur; ++colonne) {
            disparity(colonne, ligne) = -1;
        }
    }
    return disparity;
}


Imagine::Image<int, 2>
initCost(int largeur, int ligne, const Img &image1, const Img &image2) {
    //! Initialise the cost map for a row
    Imagine::Image<int, 2> cost(largeur, largeur);
    for (int colR = 0; colR < largeur; ++colR) {
        for (int colL = 0; colL < largeur; ++colL) {
            cost(colL, colR) = abs(image1(colL, ligne) - image2(colR, ligne));
        }
    }
    return cost;
}

Imagine::Image<int, 1>
shorterPathG(int largeur, const Imagine::Image<int, 2> &cost, int ligne,
             const Img &image1Edge, const Img &image2Edge, bool useEdge) {
    //! shorterPath computation with dynamic programming with edges
    // association gives the pixels associated to the corresponding pixel.
    Imagine::Image<int, 1> association(largeur);
    for (int colL = 0; colL < largeur; ++colL) {
        association(colL) = -1; // -1 means there are no association.
    }

    Imagine::Image<float, 2> distance(largeur, largeur); // distance[i][j] sera
    // la distance minimale permettant associer les pixels situés dans
    // (ligne,:i) dans l'image gauche avec les pixels situés dans (ligne,:j)
    // dans l'image droite.

    //! Initial distance computation :
    for (int col = 0; col < largeur; ++col) {
        distance(0, col) = cost(0, col);
        distance(col, 0) = cost(col, 0);
    }
    int edgeCost1 = 0;
    int edgeCost2 = 0;


    //! Computation between prefixes
    for (int colR = 1; colR < largeur; ++colR) {
        if (useEdge) {
            edgeCost2 = 10000000 * (1 - image2Edge(colR, ligne) / 255);
        }
        for (int colL = 1; colL < largeur; colL++) {
            if (useEdge) {
                edgeCost1 = 10000000 * (1 - image1Edge(colL, ligne) / 255);
            }
            distance(colL, colR) = std::min(
                    std::min(distance(colL, colR - 1) + useEdge * edgeCost1,
                             distance(colL - 1, colR) + useEdge * edgeCost2),
                    distance(colL - 1, colR - 1)) + cost(colL, colR);
        }
    }


    std::pair<int, int> etape = std::make_pair(largeur - 1, largeur - 1);
    association(etape.second) = etape.first; // on associe les deux derniers
    // pixels. etape est un couple d'indices caractérisant le nouveau
    // sous-problème auquel on se ramène, qui appartient au chemin optimal
    // si etape=(i,j), le prochain sous-problème du chemin optimal consistera à
    // évaluer la distance entre les pixels de 0 à i-1 de l'image 1 et de 0 à
    // j-1 de l'image 2.

    float D_antecedent;
    while ((etape.first > 0) && (etape.second > 0)) {
        D_antecedent = distance(etape.second, etape.first) -
                       cost(etape.second, etape.first);
        if (D_antecedent == distance(etape.second - 1, etape.first - 1)) {
            etape.first--;
            etape.second--;
            association(etape.second) = etape.first;
        } else {
            if (D_antecedent == distance(etape.second - 1, etape.first)) {
                association(etape.second) = -1;
                etape.second--;
                association(etape.second) = etape.first;
            } else {
                if (D_antecedent == distance(etape.second, etape.first - 1)) {
                    etape.first--;
                    association(etape.second) = etape.first;
                }
            }
        }
    }
    return association;
}

void disparityComputationG(int largeur, const Img &image1, const Img &image2,
                           int ligne, Imagine::Image<int, 2> &disparity,
                           int &dispMax, int &dispMin, const Img &image1Edge,
                           const Img &image2Edge, bool useEdge, bool useGCP,
                           int GCPWidth) {
    //! depth computation with edges

    Imagine::Image<int, 2> cost = initCost(largeur, ligne, image1, image2);

    if (useGCP) {
        groundControlPointCost(largeur, image1, image2, ligne, cost, GCPWidth);
    }

    Imagine::Image<int, 1> association = shorterPathG(largeur, cost, ligne,
                                                      image1Edge, image2Edge,
                                                      useEdge);

    int localDisparity;
    for (int colL = 0; colL < largeur; ++colL) {
        if (association(colL) != -1) {
            localDisparity = abs(colL - association(colL));
            disparity(colL, ligne) = localDisparity;
            if (localDisparity > dispMax) {
                dispMax = localDisparity;
            }
            if (localDisparity < dispMin) {
                dispMin = localDisparity;
            }
        }

    }
}
