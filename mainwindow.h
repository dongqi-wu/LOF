<<<<<<< HEAD
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <math.h>
#include <cstring>
#include <QString>
#include <time.h>
#include "lof.h"

#define Threshold_LOF_Spatial_1 10
#define Threshold_LOF_Spatial_2 1e+2
#define float_comp_accuracy 1e-7

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_pbImport_clicked();

    void on_pbRun_clicked();

    void on_pbLoad_clicked();

    void on_sbBadDataNum_valueChanged(int arg1);

private:
    QVector<double>::iterator dwIter;
    uint32_t currentDW;
    struct arr4sort{
        double data;
        uint16_t index;
    };
    // UI Functions
    Ui::MainWindow *ui;
    void loadData();
    QVector<double> Arr2QVec(double*,uint32_t);
    QColor getClr(int);
    // LOF Functions
    void lofMain();
    void varAlloc();
    void varClear();
    void dataCompare();
    void printResults();
    void plotResult(uint16_t, uint16_t, uint16_t);
    // Data Parameters
    clock_t runtime;
    QVector<int> badDataIndex;
    bool tson;
    QString Path;
    QStringList dataList;
    QVector<double> time;
    double range_l,range_h;
    uint32_t RowSize;
    unsigned long dataNum,ColumnSize;
    uint32_t Iter_Cnt = 0;
    // LOF Variables
    QVector<QVector<double>> dataArray;
    uint16_t MinPts_Spatial;
    uint32_t dwLength,dwStep;
    QVector<QVector<double>> lof_time_1,lof_time_2,std_time,std_orig_time,std_rate_time;
    QVector<QVector<int16_t>> badData_flag;
    QVector<QVector<double>> avg_std_time,avg_mean_time,mean_orig_time,mean_by_time;
    QVector<double> total_Const_Num,total_Abnor_Num;
	// dataCompare
    double Sum_Reach_Distance_Spatial_1,Sum_Reach_Distance_Spatial_2;
    double sumstd,summean;
    uint32_t goodDataNum;
    double Sum_LRD_Spatial_1,Sum_LRD_Spatial_2;
    QVector<QVector<double>> TDS_1,TDS_2,RDS_1,RDS_2;
    QVector<QVector<double>> bdfId;
    QVector<QVector<double>> N_MinPts_Spatial_1,N_MinPts_Spatial_2;
    QVector<int> Card_N_MinPts_Spatial_1,Card_N_MinPts_Spatial_2;
    uint16_t bdfSum;
    QVector<double> LRD_MinPts_Spatial_1,LRD_MinPts_Spatial_2,LOF_MinPts_Spatial_1,LOF_MinPts_Spatial_2;
    QVector<double> STD_Location_Spatial_Original,STD_Location_Spatial,STD_Rate_Location_Spatial;
    QVector<double> Mean_Location_Spatial_Original,Mean_Location_Spatial;
    QVector<double> MinPts_Distance_Spatial_1,MinPts_Distance_Spatial_2;
    QVector<int16_t> Mean_Rate_Location_Spatial;
    QVector<int> True_Distance_Same_Idx_Spatial_1,True_Distance_Same_Idx_Spatial_2;
    arr4sort** TDS_1_Sorted,**TDS_2_Sorted;
	
};

#endif // MAINWINDOW_H
=======
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <math.h>
#include <cstring>
#include <QString>
#include <time.h>
#include "lof.h"

#define Threshold_LOF_Spatial_1 10
#define Threshold_LOF_Spatial_2 1e+2
#define float_comp_accuracy 1e-7

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_pbImport_clicked();

    void on_pbRun_clicked();

    void on_pbLoad_clicked();

    void on_sbBadDataNum_valueChanged(int arg1);

private:
    QVector<double>::iterator dwIter;
    uint32_t currentDW;
    struct arr4sort{
        double data;
        uint16_t index;
    };
    // UI Functions
    Ui::MainWindow *ui;
    void loadData();
    QVector<double> Arr2QVec(double*,uint32_t);
    QColor getClr(int);
    // LOF Functions
    void lofMain();
    void varAlloc();
    void varClear();
    void dataCompare();
    void printResults();
    void plotResult(uint16_t, uint16_t, uint16_t);
    // Data Parameters
    clock_t runtime;
    QVector<int> badDataIndex;
    bool tson;
    QString Path;
    QStringList dataList;
    QVector<double> time;
    double range_l,range_h;
    uint32_t RowSize;
    unsigned long dataNum,ColumnSize;
    uint32_t Iter_Cnt = 0;
    // LOF Variables
    QVector<QVector<double>> dataArray;
    uint16_t MinPts_Spatial;
    uint32_t dwLength,dwStep;
    QVector<QVector<double>> lof_time_1,lof_time_2,std_time,std_orig_time,std_rate_time;
    QVector<QVector<int16_t>> badData_flag;
    QVector<QVector<double>> avg_std_time,avg_mean_time,mean_orig_time,mean_by_time;
    QVector<double> total_Const_Num,total_Abnor_Num;
	// dataCompare
    double Sum_Reach_Distance_Spatial_1,Sum_Reach_Distance_Spatial_2;
    double sumstd,summean;
    uint32_t goodDataNum;
    double Sum_LRD_Spatial_1,Sum_LRD_Spatial_2;
    QVector<QVector<double>> TDS_1,TDS_2,RDS_1,RDS_2;
    QVector<QVector<double>> bdfId;
    QVector<QVector<double>> N_MinPts_Spatial_1,N_MinPts_Spatial_2;
    QVector<int> Card_N_MinPts_Spatial_1,Card_N_MinPts_Spatial_2;
    uint16_t bdfSum;
    QVector<double> LRD_MinPts_Spatial_1,LRD_MinPts_Spatial_2,LOF_MinPts_Spatial_1,LOF_MinPts_Spatial_2;
    QVector<double> STD_Location_Spatial_Original,STD_Location_Spatial,STD_Rate_Location_Spatial;
    QVector<double> Mean_Location_Spatial_Original,Mean_Location_Spatial;
    QVector<double> MinPts_Distance_Spatial_1,MinPts_Distance_Spatial_2;
    QVector<int16_t> Mean_Rate_Location_Spatial;
    QVector<int> True_Distance_Same_Idx_Spatial_1,True_Distance_Same_Idx_Spatial_2;
    arr4sort** TDS_1_Sorted,**TDS_2_Sorted;
	
};

#endif // MAINWINDOW_H
>>>>>>> e5d2c2e2fddaa84d9c73fed1c409b95c25a23704
