#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/tracking.hpp>

#include "Blob.h"
#include <QMainWindow>
#include <QMouseEvent>
#include <QEvent>
#include <QKeyEvent>

using namespace std;
using namespace cv;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    VideoCapture capture;
    QTimer * timer;
    Mat mask;
    Mat frame;
    Ptr<BackgroundSubtractor> sub ;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    vector<Blob> blobs;
    bool blnFirstFrame = true;
    int carCount=0;
    bool firstClick=true;
    int ex;
    Size frameSize;
    VideoWriter writer;

    Ptr<MultiTracker> multiTracker = MultiTracker::create();
    vector<Rect> bboxes;
    vector<string> trackerTypes = {"BOOSTING", "MIL", "KCF", "TLD", "MEDIANFLOW", "GOTURN", "MOSSE", "CSRT"};
    Ptr<Tracker> createTrackerByName(string trackerType){
    Ptr<Tracker> tracker;

          if (trackerType ==  trackerTypes[0])
              tracker = TrackerBoosting::create();
          else if (trackerType == trackerTypes[1])
            tracker = TrackerMIL::create();
         else if (trackerType == trackerTypes[2])
            tracker = TrackerKCF::create();
          else if (trackerType == trackerTypes[3])
            tracker = TrackerTLD::create();
           else if (trackerType == trackerTypes[4])
            tracker = TrackerMedianFlow::create();
           else if (trackerType == trackerTypes[5])
            tracker = TrackerGOTURN::create();
          else if (trackerType == trackerTypes[6])
             tracker = TrackerMOSSE::create();
           else if (trackerType == trackerTypes[7])
            tracker = TrackerCSRT::create();
           else {
            cout << "Incorrect tracker name" << endl;
             cout << "Available trackers are: " << endl;
            for (vector<string>::iterator it = trackerTypes.begin() ; it != trackerTypes.end(); ++it)
              std::cout << " " << *it << endl;
     }
      return tracker;
}

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int px1=100;
    int px2=200;
    int py1=300;
    int py2=400;
    int t;
    Point px,py;
    int index=0;
    vector<int> carList;
    vector<int>::iterator it;
    int carId =0;
    bool recording;
    bool isRecorded;


private slots:
    void on_pushRtspCapture_clicked();

    void on_pushCapStop_clicked();

    void ProcessFrame();

    void StartCapture();

    void FindExistingBlob(vector<Blob> &existingBlobs, vector<Blob> &currentFrameBlobs);

    double DistancePoints(Point point1, Point point2);

    void AddBlobExistingOne(Blob &currentFrameBlob,vector<Blob> &existingBlobs, int &intIndex);

    void AddNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs);

    void BlobInfo(vector<Blob> &blobs, Mat &imgFrame2Copy);

    void on_pushSetLine_clicked();

    void SetLocation();

    void MousePressed(QMouseEvent *ev);

    void on_pushStartRec_clicked();

    void InitVideo();

    void on_pushStopRec_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
