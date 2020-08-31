#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/utility.hpp>
#include <QTimer>
#include <QKeyEvent>
#include "qlabelmy.h"

using namespace cv;
using namespace std;

bool capstop=false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)

{
    ui->setupUi(this);
    ui->textRtspUrl->setText("/home/emre/Documents/video1.mp4");
    ui->textRecPath->setText("/home/emre/Documents/test.264");
    connect(ui->imgBox,SIGNAL(MousePressed(QMouseEvent*)),this,SLOT(MousePressed(QMouseEvent*)));



    string strpx1= to_string(px1);
    string strpx2= to_string(px2);
    string strpy1= to_string(py1);
    string strpy2= to_string(py2);

    ui->textpx1->setText(QString::fromStdString(strpx1));
    ui->textpx2->setText(QString::fromStdString(strpx2));
    ui->textpy1->setText(QString::fromStdString(strpy1));
    ui->textpy2->setText(QString::fromStdString(strpy2));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushRtspCapture_clicked()
{
    StartCapture();

}

void MainWindow::StartCapture(){
    QString text = ui->textRtspUrl->toPlainText();
    String url = text.toStdString();




    capture.open(url);
    ex= static_cast<int>(capture.get(CAP_PROP_FOURCC));
    if(!capture.isOpened()){

        cout<<"Can't file the source" <<endl;
    }
    sub=createBackgroundSubtractorMOG2(500,16,true);

    timer =new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(ProcessFrame()));
    timer->start(30);



}

void MainWindow::ProcessFrame(){

    capture.read(frame);
    if(frame.empty()){
    return;
    }
    px.x=px1;
    px.y=py1;

    py.x=px2;
    py.y=py2;



    //Background Subtrackter

    sub->apply(frame,mask);

    Mat kernelOpen = getStructuringElement(MORPH_RECT,Size(3,3),Point(-1,-1));
    Mat kernelClose= getStructuringElement(MORPH_RECT,Size(11,11),Point(-1,-1));
    Mat element = getStructuringElement(MORPH_CROSS,Size(3,3),Point(-1,-1));

    GaussianBlur(mask,mask,Size(13,13),1.5);
    morphologyEx(mask,mask,MORPH_OPEN,kernelOpen,Point(-1,-1),1,BORDER_DEFAULT,Scalar());
    morphologyEx(mask,mask,MORPH_CLOSE,kernelClose,Point(-1,-1),1,BORDER_DEFAULT,Scalar());
    dilate(mask,mask,element,Point(-1,-1),1,BORDER_REFLECT,Scalar());
    threshold(mask,mask,127,255,THRESH_BINARY_INV);

    findContours(mask,contours,RETR_TREE,CHAIN_APPROX_SIMPLE,Point(0,0));


    vector<vector<Point> > convexHulls(contours.size());

    for(unsigned int i=0; i< contours.size();i++){
        convexHull(contours[i],convexHulls[i]);
    }

    vector<Blob> currentFrameBlobs;
    vector<Blob> currentFrameBlobsUpdated;

    for (auto &convexHull : convexHulls) {
        Blob possibleBlob(convexHull);

        if (possibleBlob.currentBoundingRect.area() > 400 &&
            possibleBlob.dblCurrentAspectRatio > 0.2 &&
            possibleBlob.dblCurrentAspectRatio < 4.0 &&
            possibleBlob.currentBoundingRect.width > 30 &&
            possibleBlob.currentBoundingRect.height > 30 &&
            possibleBlob.dblCurrentDiagonalSize > 60.0 &&
            (cv::contourArea(possibleBlob.currentContour) / (double)possibleBlob.currentBoundingRect.area()) > 0.50) {
            currentFrameBlobs.push_back(possibleBlob);
        }
    }

    Mat imageBlobs(mask.size(),CV_8UC3,Scalar(0,0,0));
    std::vector<std::vector<cv::Point> > cnts;
    for(auto &blb : currentFrameBlobs){
        if(blb.blnStillBeingTracked==true){
            cnts.push_back(blb.currentContour);
        }
    }

    //aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
    drawContours(imageBlobs,cnts,-1,Scalar(255,255,255),-1);

    for (auto &convexHull : cnts) {
        Blob possibleBlob(convexHull);

        if (possibleBlob.currentBoundingRect.area() > 400 &&
            possibleBlob.dblCurrentAspectRatio > 0.2 &&
            possibleBlob.dblCurrentAspectRatio < 4.0 &&
            possibleBlob.currentBoundingRect.width > 30 &&
            possibleBlob.currentBoundingRect.height > 30 &&
            possibleBlob.dblCurrentDiagonalSize > 60.0 &&
            (cv::contourArea(possibleBlob.currentContour) / (double)possibleBlob.currentBoundingRect.area()) > 0.50) {
            if(contourArea(possibleBlob.currentContour)<20000 && contourArea(possibleBlob.currentContour)>1000){
                currentFrameBlobsUpdated.push_back(possibleBlob);
            }

        }
    }

    if(blnFirstFrame){
        for(auto &currentBlob : currentFrameBlobsUpdated){
            blobs.push_back(currentBlob);

        }
    }
    else{
        FindExistingBlob(blobs,currentFrameBlobsUpdated);

    }

    Mat FrameCopy = frame.clone();
    BlobInfo(blobs,FrameCopy);
    line(FrameCopy,px,py,Scalar(255,0,0),1);

    for(auto car: blobs){  
        int prevFrameIndex = (int)car.centerPositions.size() - 2;
        int currFrameIndex = (int)car.centerPositions.size() - 1;
if (car.blnStillBeingTracked == true && car.centerPositions.size() > 2){
    if(ui->checkBox->isChecked()){

          if(car.centerPositions[prevFrameIndex].y < px.y && car.centerPositions[currFrameIndex].y >= px.y && car.centerPositions[prevFrameIndex].x <= py.x && car.centerPositions[currFrameIndex].x > px.x){
            carCount++;
            line(FrameCopy,px,py,Scalar(0,255,0),2);

        }
    }
        else{

    if(car.centerPositions[prevFrameIndex].y > px.y && car.centerPositions[currFrameIndex].y <= px.y && car.centerPositions[prevFrameIndex].x <= py.x && car.centerPositions[currFrameIndex].x > px.x){
        carCount++;
        line(FrameCopy,px,py,Scalar(0,255,0),2);

      }
    }

        }
    }

    putText(FrameCopy,"Cars : "+to_string(carCount),Point(50,50),FONT_HERSHEY_SIMPLEX,1,Scalar(255,255,0),1);




    QImage image=QImage((uchar*)FrameCopy.data,FrameCopy.cols,FrameCopy.rows,FrameCopy.step,QImage::Format_RGB888);
    ui->imgBox->setGeometry(0,0,FrameCopy.cols,FrameCopy.rows);
    ui->imgBox->setPixmap(QPixmap::fromImage(image));

    writer.write(FrameCopy);

    frameSize.width=FrameCopy.cols;
    frameSize.height=FrameCopy.rows;


    currentFrameBlobs.clear();
    blnFirstFrame=false;

}

void MainWindow::BlobInfo(vector<Blob> &blobs, Mat &imgFrame2Copy){

    for (unsigned int i = 0; i < blobs.size(); i++) {

            if (blobs[i].blnStillBeingTracked == true) {

                rectangle(imgFrame2Copy, blobs[i].currentBoundingRect, Scalar(255,0,0), 1);

            }

    }
}

void MainWindow::FindExistingBlob(vector<Blob> &existingBlobs, vector<Blob> &currentFrameBlobs){

    for (auto &existingBlob : existingBlobs) {

            existingBlob.blnCurrentMatchFoundOrNewBlob = false;

            existingBlob.predictNextPosition();
        }

        for (auto &currentFrameBlob : currentFrameBlobs) {

            int intIndexOfLeastDistance = 0;
            double dblLeastDistance = 100000.0;

            for (unsigned int i = 0; i < existingBlobs.size(); i++) {

                if (existingBlobs[i].blnStillBeingTracked == true) {

                    double dblDistance = DistancePoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition);

                    if (dblDistance < dblLeastDistance) {
                        dblLeastDistance = dblDistance;
                        intIndexOfLeastDistance = i;

                    }
                }
            }

            if (dblLeastDistance < currentFrameBlob.dblCurrentDiagonalSize * 0.5) {
                AddBlobExistingOne(currentFrameBlob, existingBlobs, intIndexOfLeastDistance);
            }
            else {
                AddNewBlob(currentFrameBlob, existingBlobs);

            }

        }

        for (auto &existingBlob : existingBlobs) {

            if (existingBlob.blnCurrentMatchFoundOrNewBlob == false) {
                existingBlob.intNumOfConsecutiveFramesWithoutAMatch++;



            }

            if (existingBlob.intNumOfConsecutiveFramesWithoutAMatch >= 5) {
                existingBlob.blnStillBeingTracked = false;
            }

        }


}

void MainWindow::AddNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs){
    currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true;

    existingBlobs.push_back(currentFrameBlob);

}

void MainWindow::AddBlobExistingOne(Blob &currentFrameBlob, vector<Blob> &existingBlobs, int &intIndex){

        existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour;
        existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;

        existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());

        existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
        existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;

        existingBlobs[intIndex].blnStillBeingTracked = true;
        existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;
}

double MainWindow::DistancePoints(Point point1,Point point2){
    int intX = abs(point1.x - point2.x);
    int intY = abs(point1.y - point2.y);

    return(sqrt(pow(intX, 2) + pow(intY, 2)));
}

void MainWindow::on_pushCapStop_clicked()
{
    timer->stop();
    carCount=0;
    blobs.clear();
}

void MainWindow::on_pushSetLine_clicked()
{
    SetLocation();
}

void MainWindow::SetLocation(){
    QString qpx1 = ui->textpx1->toPlainText();
    QString qpx2 = ui->textpx2->toPlainText();
    QString qpy1 = ui->textpy1->toPlainText();
    QString qpy2 = ui->textpy2->toPlainText();

    px1=qpx1.toUInt();
    px2=qpx2.toUInt();
    py1=qpy1.toUInt();
    py2=qpy2.toUInt();
}
void MainWindow::MousePressed(QMouseEvent *ev)
{

    if(firstClick){


        px1=ev->x();
        py1=ev->y();

        firstClick=false;

    }
    else if(!firstClick){

        px2=ev->x();
        py2=ev->y();

        firstClick=true;
    }


    string strpx1= to_string(px1);
    string strpx2= to_string(px2);
    string strpy1= to_string(py1);
    string strpy2= to_string(py2);

    ui->textpx1->setText(QString::fromStdString(strpx1));
    ui->textpx2->setText(QString::fromStdString(strpx2));
    ui->textpy1->setText(QString::fromStdString(strpy1));
    ui->textpy2->setText(QString::fromStdString(strpy2));
    ui->label_5->setText(QString::fromStdString(strpx1));
}





void MainWindow::InitVideo(){
    QString pathtxt = ui->textRecPath->toPlainText();
    String path = pathtxt.toStdString();
    writer.open(path,ex,30,frameSize,true);

}


void MainWindow::on_pushStartRec_clicked()
{

    ui->pushStartRec->setText("Recording");

    isRecorded = !isRecorded;
    if(isRecorded)
        InitVideo();
}



void MainWindow::on_pushStopRec_clicked()
{
    writer.release();
    ui->pushStartRec->setText("Start Rec");
}
