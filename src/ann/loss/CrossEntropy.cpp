/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this template
 */

/* 
 * File:   CrossEntropy.cpp
 * Author: ltsach
 * 
 * Created on August 25, 2024, 2:47 PM
 */

#include "loss/CrossEntropy.h"
#include "ann/functions.h"

CrossEntropy::CrossEntropy(LossReduction reduction): ILossLayer(reduction){
    
}

CrossEntropy::CrossEntropy(const CrossEntropy& orig):
ILossLayer(orig){
}

CrossEntropy::~CrossEntropy() {
}

double CrossEntropy::forward(xt::xarray<double> X, xt::xarray<double> t){
    //YOUR CODE IS HERE
    m_aCached_Ypred = X;
    m_aYtarget = t;

    // Calculate log(y_i)
    auto log_yi = xt::log(X + 1e-7);

    // Calculate the element-wise product t_i . log(y_i)
    auto product = t * log_yi;

    // Check if product has more than one dimension before applying axis
    double CE;
    if (product.dimension() > 1) {
        CE = -xt::mean(xt::sum(product, {1}))();  // Sum across classes and mean across batch
    } else {
        CE = -xt::mean(product)();  // No axis specified if 1D
    }

    return CE;
}
xt::xarray<double> CrossEntropy::backward() {
    //YOUR CODE IS HERE
    const double EPSILON = 1e-7;
    int N_norm = m_aCached_Ypred.shape()[0];

    // Compute the gradient according to the formula
    xt::xarray<double> gradient = - (m_aYtarget / (m_aCached_Ypred + EPSILON)) / N_norm;
    return gradient;
}