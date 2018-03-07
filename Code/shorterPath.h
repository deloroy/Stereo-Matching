//
// Created by Yonatan Deloro et Clément Riu, Projet MOPSI 2016-2017 ENPC
//

#ifndef PROJECT_SHORTERPATH_H
#define PROJECT_SHORTERPATH_H

#include "library.h"
#include "imageProcessingFunc.h"
#include "groundControlPoint.h"

//! Initialise the disparity map.
Imagine::Image<int, 2> initDisparity(int largeur, int hauteur);

//! Initialise the cost map for a row
Imagine::Image<int, 2>
initCost(int largeur, int ligne, const Img &image1, const Img &image2);

//! shorterPath computation with dynamic programming
Imagine::Image<int, 1>
shorterPathG(int largeur, const Imagine::Image<int, 2> &cost, int ligne,
             const Img &image1Edge, const Img &image2Edge, bool useEdge);

//! depth computation
void disparityComputationG(int largeur, const Img &image1, const Img &image2,
                           int ligne, Imagine::Image<int, 2> &disparity,
                           int &dispMax, int &dispMin, const Img &image1Edge,
                           const Img &image2Edge, bool useEdge, bool useGCP,
                           int GCPWidth);

#endif //PROJECT_SHORTERPATH_H
