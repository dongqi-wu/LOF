#ifndef LOF_H
#define LOF_H

#endif // LOF_H
#include<QVector>


void getMeanDist(int,int,int);
double getMeanLoc(int,QVector<double>::iterator);
double getMeanLoc_norm(double,double,int,QVector<double>::iterator);
double getSTDDist_1(double,double);
double getSTDDist_2(double,double);
double getSTDLoc(int,QVector<double>::iterator);
double getSTDRateLoc(double,double);
