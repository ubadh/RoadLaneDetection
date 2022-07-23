#include <iostream>
#include <string>
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

vector<Point2f> slidingWindow(Mat image, Rect rec){
    vector<Point2f> points;
    bool done = false;
    
    while (!done){
        //rectangle(image,rec,Scalar(255,0,0),1,8,0);
        float currentX = rec.x + rec.width * 0.5f;
        
        // Get the region of interest
        Mat roi = image(rec);
        vector<Point2f> locations;
        
        // Get all the white results
        findNonZero(roi, locations);
        
        float avgX = 0.0f;
        
        // Get the average of the rectangle
        for (int i = 0; i < locations.size(); ++i){
            avgX += rec.x + locations[i].x;
        }
        avgX = locations.empty() ? currentX : avgX / locations.size();
        
        Point point(avgX, rec.y + rec.height * 0.5f);
        points.push_back(point);
        
        // Move the rec up
        rec.y -= rec.height;
        
        // For the uppermost position
        if (rec.y < 0){
            rec.y = 0;
            done = true;
        }
        
        // Move x position
        rec.x += (point.x - currentX);
        
        // Check if rec is within coordinates of frame
        if (rec.x < 0){
            rec.x = 0;
        }
        if (rec.x + rec.width >= image.size().width){
            rec.x = image.size().width - rec.width - 1;
        }
    }
    return points;
}