//
// Created by Yonatan Deloro et Clément Riu, Projet MOPSI 2016-2017 ENPC
//

#include "library.h"
#include "imageProcessingFunc.h"
#include "shorterPath.h"

void
printResults(bool useEdge, bool useGCP, Img &imageL, Img &imageR, int largeur,
             int hauteur, Imagine::IntPoint2 position) {
    //! Print results for computation in the current window.
    int verticalOffSet = 5;
    int GCPWidth = 2 * useGCP;
    assert(verticalOffSet >= GCPWidth);
    std::cout << "Calcul d'une carte de disparité : " << std::endl;
    std::cout << "Edges use : " << useEdge << "  /  ";
    std::cout << "GCP use : " << useGCP << std::endl;

    Imagine::Image<byte, 2> imageLEdges(largeur, hauteur);
    Imagine::Image<byte, 2> imageREdges(largeur, hauteur);
    if (useEdge) {
        //! Affichage des bords des images
        edgeDetector(imageL, imageLEdges);
        display(grey(imageLEdges), Imagine::IntPoint2(largeur + 2, 1));
        edgeDetector(imageR, imageREdges);
        display(grey(imageREdges),
                Imagine::IntPoint2(largeur + 2, hauteur + 2));
    }

    //! Computation of disparity map with edges
    Imagine::Image<int, 2> disparity(largeur, hauteur);
    disparity = initDisparity(largeur, hauteur);
    int *dispMaxTab = new int[hauteur];
    int *dispMinTab = new int[hauteur];
    for (int ligne = 0; ligne < hauteur; ++ligne) {
        dispMaxTab[ligne] = -1;
        dispMinTab[ligne] = int(pow(2, 64));
    }


    // Time calculation to compare different algorithm
    struct timespec start, finish;
    double elapsed;
    std::cout << "Début du calcul..." << std::endl;
    clock_t time1 = clock_gettime(CLOCK_MONOTONIC, &start);

#pragma omp parallel for
    for (int ligne = verticalOffSet;
         ligne < hauteur - verticalOffSet; ++ligne) {
        disparityComputationG(largeur, imageL, imageR, ligne, disparity,
                              dispMaxTab[ligne], dispMinTab[ligne],
                              imageLEdges, imageREdges, useEdge, useGCP,
                              GCPWidth);
    }

    clock_t time2 = clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    std::cout << "Calcul fait en: " << (elapsed) << " s" << std::endl;

    int dispMax = -1;
    int dispMin = int(pow(2, 64));
    for (int ligne = 0; ligne < hauteur; ++ligne) {
        if (dispMaxTab[ligne] > dispMax) {
            dispMax = dispMaxTab[ligne];
        }
        if (dispMinTab[ligne] < dispMin) {
            dispMin = dispMinTab[ligne];
        }
    }

    //! Display the depth map with edge.
    Imagine::Image<Imagine::Color, 2> depth(largeur, hauteur);
    depth = disparityToDepth(disparity, dispMax, dispMin, largeur, hauteur);
    display(depth, position);
}

int main() {
    //!Chargement et affichage des images.
    Img imageL;
    Img imageR;
    // Chargement des images.
    // "cone" ou "tsuku"
    if (loadingImage(imageL, imageR, "cone") == 1) {
        std::cerr << "Mauvais chargement de fichier" << std::endl;
        return 0;
    }

    // Affichage d'information sur les images.
    int largeur = imageL.width(); // Identique pour les deux images
    int hauteur = imageL.height();
    std::cout << "Dimension de l'image : " << largeur << " * " << hauteur
              << std::endl;
    int taille = largeur * hauteur;
    std::cout << taille << std::endl;

    // Affichage des images.
    Imagine::openWindow(4 * largeur + 5, 2 * hauteur + 3, "Disparity map");
    display(grey(imageL), Imagine::IntPoint2(1, 1));
    display(grey(imageR), Imagine::IntPoint2(1, hauteur + 1));

    printResults(false, false, imageL, imageR, largeur, hauteur,
                 Imagine::IntPoint2(largeur * 2 + 3, 1));

    printResults(false, true, imageL, imageR, largeur, hauteur,
                 Imagine::IntPoint2(largeur * 2 + 3, hauteur + 2));

    printResults(true, false, imageL, imageR, largeur, hauteur,
                 Imagine::IntPoint2(largeur * 3 + 4, 1));

    printResults(true, true, imageL, imageR, largeur, hauteur,
                 Imagine::IntPoint2(largeur * 3 + 4, hauteur + 2));

    Imagine::endGraphics();

    return 0;
}
