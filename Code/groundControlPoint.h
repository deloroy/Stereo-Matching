//
// Created by Yonatan Deloro et Clément Riu, Projet MOPSI 2016-2017 ENPC
//

#ifndef PROJECT_GROUNDCONTROLPOINT_H
#define PROJECT_GROUNDCONTROLPOINT_H

#include "library.h"
#include "imageProcessingFunc.h"

//! Finds GCP and returns them.
std::map<int, std::vector<int> >
groundControlPointDetect(int largeur, const Img &image1, const Img &image2,
                         int ligne, Imagine::Image<int, 2> &cost, int GCPWidth);

//! Change the cost map in order to use GCP.
void groundControlPointCost(int largeur, const Img &image1, const Img &image2,
                            int ligne, Imagine::Image<int, 2> &cost,
                            int GCPWidth);

#endif //PROJECT_GROUNDCONTROLPOINT_H
