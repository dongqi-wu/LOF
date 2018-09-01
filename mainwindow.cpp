#include "mainwindow.h"
#include "ui_mainwindow.h"
int sortCompare(const void*, const void*);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // initialization
    ui->setupUi(this);
    dataArray.clear();
    ui->progressBar->setValue(0);
    ui->sbWindowLength->setMinimum(1);
    ui->sbWindowStep->setMinimum(0);
    ui->sbWindowLength->setValue(20);
    ui->sbWindowStep->setValue(1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Select the path of the data file
void MainWindow::on_pbImport_clicked()
{
    Path = QFileDialog::getOpenFileName(this, tr("Open CSV File"), "", tr("Comma Seperated Values (*.csv)"));
    ui->textDataPath->setPlainText(Path);
    return;
}

// Initialize and run LOF analysis
void MainWindow::on_pbRun_clicked()
{
    // Get Current Settings
    dwLength = ui->sbWindowLength->value();
    dwStep = ui->sbWindowStep->value();

    // Initialize LOF variables
    this->varClear();

    // Generate LOF results
    lofMain();
    runtime = clock() - runtime;

    // Output Results
    MainWindow::printResults();

    return;
}

// Load data from the selected datapath
void MainWindow::on_pbLoad_clicked()
{
    MainWindow::loadData();
    return;
}

// Load data from the selected datapath
void MainWindow::loadData(){
    // Initialization
    range_l = 99999;
    range_h = -99999;
    tson = ui->cbTimeStamp->isChecked(); // if the to-be-imported data contains timestamp
    uint32_t k;
    Path = ui->textDataPath->toPlainText(); // path of selected csv file

    // Load the selected CSV data
    QFile inputFile(Path);
    if (!inputFile.open(QIODevice::ReadOnly)) {
            qDebug() << inputFile.errorString();
            return;
    }
    QString data = inputFile.readAll();
    inputFile.close();

    // Split the data by line and by ','
    dataList = data.split("\r\n");
    ColumnSize = dataList.size() - 1;       // number of measurements for each channel
    RowSize = dataList[0].split(',').size(); // number of PMU channels
    ui->progressBar->setMaximum(RowSize);
    ui->progressBar->setMinimum(0);
    time.clear();
    time.resize(ColumnSize);

    // Allocate memory space for dataArray and assign value to each element
    // i is ColumnNum, j is RowNum
    if (dataArray.size() > 0){
        dataArray.clear();
    }
    dataArray.resize(RowSize);
    for (uint32_t i = 0;i < RowSize;i++){
        ui->progressBar->setValue(i+1);
        if (tson){
            k = i - 1;
        }
        else{
            k = i;
        }
        if ((!tson) || (tson && (i > 0))){
            //dataArray[k] = (double*)malloc(ColumnSize*sizeof(double));
            dataArray[k].resize(ColumnSize);
        }
        for (uint32_t j = 0;j < ColumnSize;j++){
            if (tson && (i==0)){
                time[j] = dataList[j].split(',').first().toDouble();
            }
            else{
                dataArray[k][j] = dataList[j].split(',').at(i).toDouble();
                if (dataArray[k][j] > range_h){
                    range_h = dataArray[k][j];
                }
                if (dataArray[k][j] < range_l){
                    range_l = dataArray[k][j];
                }
            }
        }
    }
    if (tson){
        RowSize--;
    }

    // Plot original data before processing
    QColor Clr;
    if (!tson){
        time.resize(ColumnSize);
        // if no timestamp provided, plot data versus index
        for (uint32_t i = 0;i < ColumnSize;i++){
            time[i] = i + 1;
        }
    }

    // Generate graphs
    ui->progressBar->setMaximum(RowSize - (uint16_t)tson);
    for (unsigned long i = 0;i < (RowSize - (uint16_t)tson);i++){
        ui->progressBar->setValue(i+1);
        Clr = MainWindow::getClr(i);
        ui->plotOriginalData->addGraph();
        ui->plotOriginalData->graph(i)->setData(time,dataArray[i]);
        ui->plotOriginalData->graph(i)->setPen(QPen(Clr));
    }

    // Edit axis range and label
    if (tson){
        ui->plotOriginalData->xAxis->setLabel("Time (sec)");
    }
    else{
        ui->plotOriginalData->xAxis->setLabel("Sample Index");
    }
    ui->plotOriginalData->yAxis->setLabel("Data");
    ui->plotOriginalData->xAxis->setRange(time.first(),time.last());
    ui->plotOriginalData->yAxis->setRange(range_l - fabs(0.05*range_l),range_h + fabs(0.05*range_h));
    ui->plotOriginalData->replot();

    return;
}

// Perform LOF analysis
void MainWindow::lofMain(){
    runtime = clock();
    MinPts_Spatial = (uint16_t)ceil((double)RowSize / 2);
    uint16_t main_flag = 1;
    MainWindow::varAlloc();
    Iter_Cnt = 0;
    ui->progressBar->setMaximum((ColumnSize - dwLength)/dwStep + 1);
    ui->progressBar->setValue(0);
    while (main_flag){
        ui->progressBar->setValue(Iter_Cnt+1);
        // move current data window
        if ((dwLength + Iter_Cnt * dwStep) < ColumnSize){
            currentDW = Iter_Cnt * dwStep;          // std::iterator pointing to the first index of current time window
        }
        else{
            main_flag = 0;

        }
        // remove mean from current data window
        // NOT IMPLEMENTED

        // remove mean value from original data window
        // NOT IMPLEMENTED

        // compare data in spatial and temporal dimension
        MainWindow::dataCompare();
        Iter_Cnt++;
    }
}

// Analyze one datawindow
void MainWindow::dataCompare(){
    // initialization
    for (uint16_t i = 0;i < RowSize;i++){
        TDS_1[i].fill(-1);
        TDS_2[i].fill(-1);
        RDS_1[i].fill(-1);
        RDS_2[i].fill(-1);
        N_MinPts_Spatial_1[i].fill(-1);
        N_MinPts_Spatial_2[i].fill(-1);
    }
    Card_N_MinPts_Spatial_1.fill(MinPts_Spatial);
    Card_N_MinPts_Spatial_2.fill(MinPts_Spatial);
    LRD_MinPts_Spatial_1.fill(-1);
    LRD_MinPts_Spatial_2.fill(-1);
    LOF_MinPts_Spatial_1.fill(-1);
    LOF_MinPts_Spatial_2.fill(-1);
    STD_Location_Spatial_Original.fill(-1);
    STD_Location_Spatial.fill(-1);
    STD_Rate_Location_Spatial.fill(-1);
    Mean_Location_Spatial_Original.fill(-1);
    Mean_Location_Spatial.fill(-1);
    Mean_Rate_Location_Spatial.fill(-1);


    // iteration
    for(uint16_t pmu = 0;pmu < RowSize;pmu++){
        dwIter = dataArray[pmu].begin() + Iter_Cnt * dwStep;

        STD_Location_Spatial_Original[pmu] = getSTDLoc(dwLength,dwIter);
        std_orig_time[pmu][Iter_Cnt] = STD_Location_Spatial_Original[pmu];

        Mean_Location_Spatial_Original[pmu] = getMeanLoc(dwLength,dwIter);
        mean_orig_time[pmu][Iter_Cnt] = Mean_Location_Spatial_Original[pmu];

        if(Iter_Cnt == 0){
            avg_std_time[pmu][Iter_Cnt] = STD_Location_Spatial_Original[pmu];
            avg_mean_time[pmu][Iter_Cnt] = Mean_Location_Spatial_Original[pmu];
        }
        else{
            // line 196, SIGNIFICANTLY MODIFIED
            sumstd = 0;
            summean = 0;
            goodDataNum = 0;
            for (uint16_t i = 0;i < ColumnSize;i++){
                if(badData_flag[pmu][i] == 0){
                    goodDataNum++;
                    sumstd += std_orig_time[pmu][i];
                    summean += mean_orig_time[pmu][i];
                }
            }
            avg_std_time[pmu][Iter_Cnt] = sumstd / goodDataNum;
            avg_mean_time[pmu][Iter_Cnt] = summean / goodDataNum;
        }
        STD_Location_Spatial[pmu] = STD_Location_Spatial_Original[pmu] / avg_std_time[pmu][Iter_Cnt];
        std_time[pmu][Iter_Cnt] = STD_Location_Spatial[pmu];
        Mean_Location_Spatial[pmu] = getMeanLoc_norm(Mean_Location_Spatial_Original[pmu],avg_mean_time[pmu][Iter_Cnt],dwLength,dwIter);
        mean_by_time[pmu][Iter_Cnt] = Mean_Location_Spatial[pmu];

        if (Iter_Cnt > 0){
            STD_Rate_Location_Spatial[pmu] = getSTDRateLoc(std_time[pmu][Iter_Cnt-1],std_time[pmu][Iter_Cnt]);
        }
        else {
            STD_Rate_Location_Spatial[pmu] = STD_Location_Spatial[pmu];
        }
        std_rate_time[pmu][Iter_Cnt] = STD_Rate_Location_Spatial[pmu];
    }
    // line 218
    for (uint16_t ii = 0;ii < RowSize;ii++){
        TDS_1[ii][ii] = 1e10;
        TDS_2[ii][ii] = 1e10;
        for(uint16_t jj = ii;jj < RowSize;jj++){
            if (ii != jj){
                TDS_1[ii][jj] = getSTDDist_1(STD_Location_Spatial[ii],STD_Location_Spatial[jj]);
                TDS_2[ii][jj] = getSTDDist_2(STD_Location_Spatial[ii],STD_Location_Spatial[jj]);
                TDS_1[jj][ii] = TDS_1[ii][jj];
                TDS_2[jj][ii] = TDS_2[ii][jj];
            }
        }
    }

    // obtain N_Minpts and Card_N_MinPts and MinPts_Distance (SIGNIFICANTLY MODIFIED)
    // copy TDS_1/2 to TDS_1/2_Sorted
    for (uint16_t si = 0;si < RowSize;si++){
        for (uint16_t sj = 0;sj < RowSize;sj++){
            TDS_1_Sorted[si][sj].data = TDS_1[si][sj];
            TDS_1_Sorted[si][sj].index = sj;
            TDS_2_Sorted[si][sj].data = TDS_2[si][sj];
            TDS_2_Sorted[si][sj].index = sj;
        }
    }

    // sort TDS_1/2_Sorted
    for (uint16_t si = 0;si < RowSize;si++){
        std::qsort(TDS_1_Sorted[si],RowSize,sizeof(arr4sort),sortCompare);
        std::qsort(TDS_2_Sorted[si],RowSize,sizeof(arr4sort),sortCompare);
    }

    // obtain N_MinPts_Spatial
    for (uint16_t si = 0;si < RowSize;si++){
        for (uint16_t sj = 0;sj < MinPts_Spatial;sj++){
            N_MinPts_Spatial_1[si][sj] = TDS_1_Sorted[si][sj].index;
            N_MinPts_Spatial_2[si][sj] = TDS_2_Sorted[si][sj].index;
        }
    }
    for (uint16_t si = 0;si < RowSize;si++){
        MinPts_Distance_Spatial_1[si] = TDS_1_Sorted[si][MinPts_Spatial-1].data;
        MinPts_Distance_Spatial_2[si] = TDS_2_Sorted[si][MinPts_Spatial-1].data;
    }
    // line 244

    for (uint16_t jj = 0;jj < RowSize;jj++){ // SIGNIFICANTLY MODIFIED
        True_Distance_Same_Idx_Spatial_1.clear();
        True_Distance_Same_Idx_Spatial_2.clear();
        for (uint16_t kk = 0;kk < RowSize;kk++){
            if (fabs(TDS_1[jj][kk] - TDS_1[0][TDS_1_Sorted[jj][MinPts_Spatial-1].index]) < float_comp_accuracy){
                True_Distance_Same_Idx_Spatial_1.append(kk);
            }
            if (fabs(TDS_2[jj][kk] - TDS_2[0][TDS_2_Sorted[jj][MinPts_Spatial-1].index]) < float_comp_accuracy){
                True_Distance_Same_Idx_Spatial_2.append(kk);
            }
        }
        // 248 & 249
        for (uint16_t kk = 0;kk < True_Distance_Same_Idx_Spatial_1.length();kk++){

            if ((True_Distance_Same_Idx_Spatial_1[kk] == TDS_1_Sorted[jj][MinPts_Spatial-1].index) ||
                 (True_Distance_Same_Idx_Spatial_1[kk] == jj)){
                True_Distance_Same_Idx_Spatial_1.remove(kk);
            }
        }
        for (uint16_t kk = 0;kk < True_Distance_Same_Idx_Spatial_2.length();kk++){
            if ((True_Distance_Same_Idx_Spatial_2[kk] == TDS_2_Sorted[jj][MinPts_Spatial-1].index) ||
                 (True_Distance_Same_Idx_Spatial_2[kk] == jj)){
                True_Distance_Same_Idx_Spatial_2.remove(kk);
            }
        }
        // 251
        if(!True_Distance_Same_Idx_Spatial_1.isEmpty()){
            for (uint16_t kk = MinPts_Spatial;kk < (MinPts_Spatial + True_Distance_Same_Idx_Spatial_1.length());kk++){
                N_MinPts_Spatial_1[jj][kk] = True_Distance_Same_Idx_Spatial_1[kk - MinPts_Spatial];
            }
            Card_N_MinPts_Spatial_1[jj] += True_Distance_Same_Idx_Spatial_1.length();
        }
        // 257
        if(!True_Distance_Same_Idx_Spatial_2.isEmpty()){
            for (uint16_t kk = MinPts_Spatial;kk < (MinPts_Spatial + True_Distance_Same_Idx_Spatial_2.length());kk++){
                N_MinPts_Spatial_2[jj][kk] = True_Distance_Same_Idx_Spatial_2[kk - MinPts_Spatial];
            }
            Card_N_MinPts_Spatial_2[jj] += True_Distance_Same_Idx_Spatial_2.length();
        }
    }

    // obtain Reach_Distance
    for (uint16_t ii = 0;ii < RowSize;ii++){
        for (uint16_t jj = 0;jj < RowSize;jj++){
            RDS_1[ii][jj] = std::max(MinPts_Distance_Spatial_1[jj],TDS_1[ii][jj]);
            RDS_2[ii][jj] = std::max(MinPts_Distance_Spatial_2[jj],TDS_2[ii][jj]);
        }
    }
    // obtain LRD_MinPts (line 272)
    for (uint16_t ii = 0;ii < RowSize;ii++){
        Sum_Reach_Distance_Spatial_1 = 0;
        Sum_Reach_Distance_Spatial_2 = 0;
        // 274
        for (uint16_t kk = 0;kk < Card_N_MinPts_Spatial_1[ii];kk++){
            Sum_Reach_Distance_Spatial_1 += RDS_1[ii][N_MinPts_Spatial_1[ii][kk]];
        }
        // 275
        for (uint16_t kk = 0;kk < Card_N_MinPts_Spatial_2[ii];kk++){
            Sum_Reach_Distance_Spatial_2 += RDS_2[ii][N_MinPts_Spatial_2[ii][kk]];
        }
        // 277
        LRD_MinPts_Spatial_1[ii] = Card_N_MinPts_Spatial_1[ii] / Sum_Reach_Distance_Spatial_1;
        LRD_MinPts_Spatial_2[ii] = Card_N_MinPts_Spatial_2[ii] / Sum_Reach_Distance_Spatial_2;
    }
    // obtain LOF (line 281)
    for (uint16_t ii = 0;ii < RowSize;ii++){
        Sum_LRD_Spatial_1 = 0;
        Sum_LRD_Spatial_2 = 0;
        for (uint16_t jj = 0;jj < Card_N_MinPts_Spatial_1[ii];jj++){
            Sum_LRD_Spatial_1 += LRD_MinPts_Spatial_1[N_MinPts_Spatial_1[ii][jj]];
        }
        for (uint16_t jj = 0;jj < Card_N_MinPts_Spatial_2[ii];jj++){
            Sum_LRD_Spatial_2 += LRD_MinPts_Spatial_2[N_MinPts_Spatial_2[ii][jj]];
        }
        LOF_MinPts_Spatial_1[ii] = Sum_LRD_Spatial_1 / (Card_N_MinPts_Spatial_1[ii] * LRD_MinPts_Spatial_1[ii]);
        LOF_MinPts_Spatial_2[ii] = Sum_LRD_Spatial_2 / (Card_N_MinPts_Spatial_2[ii] * LRD_MinPts_Spatial_2[ii]);

        lof_time_1[ii][Iter_Cnt] = LOF_MinPts_Spatial_1[ii];
        lof_time_2[ii][Iter_Cnt] = LOF_MinPts_Spatial_2[ii];

        if (LOF_MinPts_Spatial_1[ii] > Threshold_LOF_Spatial_1){
            badData_flag[ii][Iter_Cnt] = 1;
        }

        if (LOF_MinPts_Spatial_2[ii] > Threshold_LOF_Spatial_2){
            badData_flag[ii][Iter_Cnt] = 2;
        }

        if (badData_flag[ii][Iter_Cnt] == -1){
            badData_flag[ii][Iter_Cnt] = 0;
        }
        // line 304
        if (Iter_Cnt > dwLength){
            bdfSum = 0;
            for (unsigned int jj = (Iter_Cnt - dwLength - 1);jj < Iter_Cnt+1;jj++){
                bdfSum += badData_flag[ii][jj];
            }
            if (((badData_flag[ii][Iter_Cnt] == 1) && (bdfSum >= (dwLength - 3))) || (badData_flag[ii][Iter_Cnt] == 2)){
                bdfId[ii][0] = 1;
                if (bdfId[ii][1] == 0){
                    bdfId[ii][1] = dwLength + Iter_Cnt * dwStep;
                    if (badData_flag[ii][Iter_Cnt] == 1){
                        bdfId[ii][2] = LOF_MinPts_Spatial_1[ii];
                    }
                    else if(badData_flag[ii][Iter_Cnt] == 2){
                        bdfId[ii][2] = LOF_MinPts_Spatial_2[ii];
                    }
                }
                else {
                    bdfId[ii][3] = dwLength + Iter_Cnt * dwStep;
                    if (badData_flag[ii][Iter_Cnt] == 1){
                        bdfId[ii][4] = LOF_MinPts_Spatial_1[ii];
                    }
                    else if (badData_flag[ii][Iter_Cnt] == 2){
                        bdfId[ii][4] = LOF_MinPts_Spatial_2[ii];
                    }
                }
            }
        }
    }
}

// Display LOF detection results in textbox
void MainWindow::printResults(){
    // Result TextBox
    badDataIndex.clear();
    QString txt = QString("Itertion Number: %1").arg(Iter_Cnt);
    ui->tbResult->setPlainText(txt);
    txt = QString("Total Runtime: %1 sec").arg((double)runtime / CLOCKS_PER_SEC);
    ui->tbResult->append(txt);
    txt = QString("Runtime Per Iteration: %1 sec").arg((double)runtime / CLOCKS_PER_SEC / Iter_Cnt);
    ui->tbResult->append(txt);
    txt = QString("===================================Bad Data Detection Result=================================");
    ui->tbResult->append(txt);
    txt = QString("Bad PMU Index | Snapshot of 1st Bad Data Point | LOF at 1st Bad Data Point | Snapshot of Last Bad Data Point | LOF at Last Bad Data Point");
    ui->tbResult->append(txt);
    for (uint8_t i = 0;i < RowSize;i++){
        if (bdfId[i][0] == 1){
            badDataIndex.append(i);
            if (i < 9){
                txt = QString("          0%1           |                      %2                        |               %3               |                         %4                       |                   %5                ")
                        .arg(i+1).arg(bdfId[i][1]).arg(bdfId[i][2]).arg(bdfId[i][3]).arg(bdfId[i][4]);
            }
            else{
                txt = QString("          %1           |                      %2                        |               %3               |                         %4                       |                   %5                ")
                        .arg(i+1).arg(bdfId[i][1]).arg(bdfId[i][2]).arg(bdfId[i][3]).arg(bdfId[i][4]);
            }
            ui->tbResult->append(txt);
        }
    }
    txt = QString("===================================================================================");
    ui->tbResult->append(txt);
    // Bad Data Plot
    if (!badDataIndex.isEmpty()){
        ui->sbBadDataNum->setMaximum(badDataIndex.size());
        ui->sbBadDataNum->setMinimum(1);
        ui->sbBadDataNum->setValue(1);
        MainWindow::plotResult(badDataIndex[0],bdfId[badDataIndex[0]][1] - 2*dwLength,bdfId[badDataIndex[0]][3] + 2*dwLength);
    }
    else{
        ui->sbBadDataNum->setValue(0);
        MainWindow::plotResult(10000,0,0);
    }

    return;
}

// Plot detected bad data
void MainWindow::plotResult(uint16_t pmu, uint16_t start, uint16_t end){
    if (pmu < 9999){ // if any bad data is detected, plot the zoomed-in data curve
        QVector<double> dataBad(end - start + 1);
        QVector<double> timeBad(end - start + 1);
        for (uint16_t i = start;i < end+1;i++){
            timeBad[i-start] = time[i];
            dataBad[i-start] = dataArray[pmu][i];
        }
        ui->plotBadData->addGraph();
        ui->plotBadData->graph(0)->setData(timeBad,dataBad);
        // Edit axis range and label
        if (tson){
            ui->plotBadData->xAxis->setLabel("Time (sec)");
        }
        else{
            ui->plotBadData->xAxis->setLabel("Sample Index");
        }
        ui->plotBadData->yAxis->setLabel("Bad Data");
        ui->plotBadData->xAxis->setRange(timeBad.first(),timeBad.last());
        ui->plotBadData->yAxis->setRange(range_l - fabs(0.05*range_l),range_h + fabs(0.05*range_h));
        ui->plotBadData->replot();
    }
    else{
        ui->plotBadData->clearGraphs();
        ui->plotBadData->replot();
    }


    return;
}

// Allocate variables to be used in LOF
void MainWindow::varAlloc(){
    // lofMain
    lof_time_1.resize(RowSize);
    lof_time_2.resize(RowSize);
    std_time.resize(RowSize);
    std_orig_time.resize(RowSize);
    std_rate_time.resize(RowSize);
    badData_flag.resize(RowSize);
    avg_std_time.resize(RowSize);
    avg_mean_time.resize(RowSize);
    mean_orig_time.resize(RowSize);
    mean_by_time.resize(RowSize);
    total_Abnor_Num.resize(RowSize);
    total_Const_Num.resize(RowSize);

    TDS_1.resize(RowSize);
    TDS_2.resize(RowSize);
    RDS_1.resize(RowSize);
    RDS_2.resize(RowSize);
    N_MinPts_Spatial_1.resize(RowSize);
    N_MinPts_Spatial_2.resize(RowSize);
    Card_N_MinPts_Spatial_1.resize(RowSize);
    Card_N_MinPts_Spatial_2.resize(RowSize);
    LRD_MinPts_Spatial_1.resize(RowSize);
    LRD_MinPts_Spatial_2.resize(RowSize);
    LOF_MinPts_Spatial_1.resize(RowSize);
    LOF_MinPts_Spatial_2.resize(RowSize);
    STD_Location_Spatial_Original.resize(RowSize);
    STD_Location_Spatial.resize(RowSize);
    STD_Rate_Location_Spatial.resize(RowSize);
    Mean_Location_Spatial_Original.resize(RowSize);
    Mean_Location_Spatial.resize(RowSize);
    Mean_Rate_Location_Spatial.resize(RowSize);
    TDS_1_Sorted = (arr4sort**)malloc(RowSize*sizeof(arr4sort*));
    TDS_2_Sorted = (arr4sort**)malloc(RowSize*sizeof(arr4sort*));
    MinPts_Distance_Spatial_1.resize(RowSize);
    MinPts_Distance_Spatial_2.resize(RowSize);
    bdfId.resize(RowSize);

    for (uint16_t i = 0;i < RowSize;i++){
        lof_time_1[i].resize(ColumnSize+100);
        lof_time_2[i].resize(ColumnSize+100);
        std_time[i].resize(ColumnSize+100);
        std_orig_time[i].resize(ColumnSize+100);
        std_rate_time[i].resize(ColumnSize+100);
        badData_flag[i].resize(ColumnSize+100); // D
        badData_flag[i].fill(-1);
        avg_std_time[i].resize(ColumnSize+100);
        avg_mean_time[i].resize(ColumnSize+100);
        mean_by_time[i].resize(ColumnSize+100);
        mean_orig_time[i].resize(ColumnSize+100);
        TDS_1_Sorted[i] = (arr4sort*)malloc(RowSize*sizeof(arr4sort));
        TDS_2_Sorted[i] = (arr4sort*)malloc(RowSize*sizeof(arr4sort));
        TDS_1[i].resize(RowSize);
        TDS_2[i].resize(RowSize);
        RDS_1[i].resize(RowSize);
        RDS_2[i].resize(RowSize);
        N_MinPts_Spatial_1[i].resize(2*RowSize);
        N_MinPts_Spatial_2[i].resize(2*RowSize);
        bdfId[i].resize(5);
    }

}

// Clear result containers before each run
void MainWindow::varClear(){
    badData_flag.clear();
    bdfId.clear();
}

// Convert floating point array to QVector for Plotting
//QVector<double> MainWindow::Arr2QVec(double* Arr, uint32_t size){
//    QVector<double> v(size);
//    for (uint32_t i=0;i<size;i++){
//        v[i] = Arr[i];
//    }
//    return v;
//}

// Switch between baddata plots
void MainWindow::on_sbBadDataNum_valueChanged(int arg1)
{
    if (!badDataIndex.isEmpty()){
        MainWindow::plotResult(badDataIndex[arg1-1],bdfId[badDataIndex[arg1-1]][1] - 2*dwLength,bdfId[badDataIndex[arg1-1]][3] + 2*dwLength);
    }
    else{
        MainWindow::plotResult(10000,0,0);
    }
}

// Compare elements in TDS_1, TDS_2
int sortCompare(const void* a, const void* b){
    struct arr4sort{
        double data;
        uint16_t index;
    };
    double ad = ((arr4sort*)a)->data;
    uint16_t ai = ((arr4sort*)a)->index;
    double bd = ((arr4sort*)b)->data;
    uint16_t bi = ((arr4sort*)b)->index;

    if (((ad - bd) < -float_comp_accuracy) || ((fabs(ad - bd) < float_comp_accuracy) && ai < bi)){
        return -1;
    }
    else if (((ad - bd) > float_comp_accuracy) || ((fabs(ad - bd) < float_comp_accuracy) && ai > bi)){
        return 1;
    }
    else{
        return 0;
    }

}

// Return a different color for different data channel
QColor MainWindow::getClr(int index){
    switch(index){
        case 0:
            return Qt::blue;
        case 1:
            return Qt::red;
        case 2:
            return Qt::green;
        case 3:
            return Qt::yellow;
        case 4:
            return Qt::magenta;
        case 5:
            return Qt::cyan;
        case 6:
            return QColor(0x99,0x99,0xff,255);
        case 7:
            return QColor(0xff,0x99,0x33,255);
        case 8:
            return QColor(0x99,0x00,0x99,255);
        case 9:
            return QColor(0xff,0x99,0xff,255);
        default:
            return QColor(rand()%0xff,rand()%0xff,rand()%0xff,255);
    }
}
