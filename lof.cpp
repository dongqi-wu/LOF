#include "lof.h"
#include "math.h"
#include "mainwindow.h"



double getMeanLoc(int dwLength, QVector<double>::iterator iter){
    double sum = 0;
    for (uint16_t i = 0;i < dwLength;i++){
        sum += *iter;
        iter++;
    }
    return sum / dwLength;
}

// Rewrote
double getMeanLoc_norm(double MLSO,double AMBT,int dwLength, QVector<double>::iterator iter){
    double MDS = 0;
    for (uint16_t i = 0;i < dwLength - 1;i++){
        if (fabs(*(iter+1) - *iter) > MDS){
            MDS = fabs(*(iter+1) - *iter);
        }
        iter++;
    }
    double MLS = std::max(fabs(MLSO/AMBT),fabs(AMBT/MLSO));
    MLS = pow(1e4,MLS);

    return (MLS * MDS);
}

double getSTDDist_1(double a,double b){
    return (fabs(a-b));
}

double getSTDDist_2(double a,double b){
    if (a == 0){
        a = 1e-5;
    }
    if (b == 0){
        b = 1e-5;
    }

    return std::max(fabs(a/b),fabs(b/a));
}

// Rewrote
double getSTDLoc(int dwLength, QVector<double>::iterator iter){
    double* diff_series = (double*)calloc(dwLength-1,sizeof(double));
    double sum = 0;
    double mean = 0;
    double std = 0;
    for (uint16_t i = 0;i < dwLength-1;i++){
        diff_series[i] = *(iter+1) - *iter;
        sum += diff_series[i];
        iter++;
    }
    mean = sum / (dwLength - 1);
    for (uint16_t i = 0;i < dwLength-1;i++){
        std += pow(diff_series[i] - mean, 2);
    }
    std = pow(std / (dwLength - 2),0.5);
    return std;
}

double getSTDRateLoc(double old,double curr){
    if (old == 0){
        old = 1e-5;
    }

    if (curr == 0){
        curr = 1e-5;
    }

    double SRL = fabs(curr - old) / fabs(std::max(curr,old));
    return SRL;
}
