#include <iostream>
#include <string>
#include "slidingWindow.h"
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

int main(){

    const String filename = "assets/ride.mp4";
    VideoCapture cap(filename);
    
    
    if (!cap.isOpened()){
        cout<<"Media not found at path "<<filename<<"\n";
        return 1; 
    }

    Point2f orgPoints[4];
    Point2f newPoints[4];
    
    // Points of interest area (for bird's eye view)
    
    orgPoints[0] = Point(1380, 1090);
    orgPoints[1] = Point(2280, 1090);
    orgPoints[2] = Point(3180, 1740);
    orgPoints[3] = Point(0, 1740);

    
    // orgPoints[0] = Point(0, 0);
    // orgPoints[1] = Point(3840, 0);
    // orgPoints[2] = Point(3840, 2160);
    // orgPoints[3] = Point(0, 2160);
    
    // Points of output
    newPoints[0] = Point(0, 0);
    newPoints[1] = Point(640, 0);
    newPoints[2] = Point(640, 480);
    newPoints[3] = Point(0, 480);
    
    // Matrix to store the transformed image
    Mat birdMatrix = getPerspectiveTransform(orgPoints, newPoints);
    Mat dst(480, 640, CV_8UC3);
    
    // Matrix to invert the transformation
    Mat invertedbirdMatrix;
    invert(birdMatrix, invertedbirdMatrix);

    Mat org; //Original image, modified only with result
    Mat img; //Working image
    
    while (true){
        
        // Read the  frame by frame
        cap.read(org);
        
        // Video ended
        if (org.empty()){
            break;
        }

        // Generate bird's eye view
        warpPerspective(org, dst, birdMatrix, dst.size(), INTER_LINEAR, BORDER_CONSTANT);
        
        // Convert from RGB to Gray
        cvtColor(dst, img, COLOR_RGB2GRAY);
        
        // Get the white areas
        Mat whiteArea;
        inRange(img, Scalar(195, 195, 195), Scalar(255, 255, 255), whiteArea);
        
        // Extract what matches
        Mat processed;
        bitwise_and(img, whiteArea, processed);
        imshow("Post processing: ", processed);

        //Blur the image so that edges/gaps become smoother
        const Size kernelSize = Size(9, 9);
        GaussianBlur(processed, processed, kernelSize, 0);

        // Fill the gaps
        Mat size = Mat::ones(14, 14, CV_8U);
        dilate(processed, processed, size);
        erode(processed, processed, size);
        morphologyEx(processed, processed, MORPH_CLOSE, size);

        // Keep white areas (above 150), remove black areas
        threshold(processed, processed, 150, 255, THRESH_BINARY);

        // <=========== Left Side of the lane ===========>

        // Sliding window algorithm, with starting rectangle
        vector<Point2f> pts = slidingWindow(processed, Rect(0, 420, 180, 60));
        
        vector<Point> allPts;
        // Store points for the polygon
        vector<Point2f> outPts;

        // Transform points back into original 
        perspectiveTransform(pts, outPts, invertedbirdMatrix);

        // Display the points
        for (int i = 0; i < outPts.size() - 1; i++){
            line(org, outPts[i], outPts[i + 1], Scalar(255, 0, 0), 3);
            allPts.push_back(Point(outPts[i].x, outPts[i].y));
        }
        allPts.push_back(Point(outPts[outPts.size() - 1].x, outPts[outPts.size() - 1].y));
        imshow("Post processing: ", processed);
        Mat out;
        // Convert from Gray to RGB
        cvtColor(processed, out, COLOR_GRAY2BGR);
        
        // Display the line on the processed image
        for (int i = 0; i < pts.size() - 1; ++i){
            line(out, pts[i], pts[i + 1], Scalar(255, 0, 0));
        }

        // <=========== Right Side of the lane ===========>

        // Sliding window algorithm, with starting rectangle
        pts = slidingWindow(processed, Rect(480, 420, 160, 60));
        imshow("Post processing: ", processed);
        perspectiveTransform(pts, outPts, invertedbirdMatrix);
        
        // Display the points
        for (int i = 0; i < outPts.size() - 1; i++){
            line(org, outPts[i], outPts[i + 1], Scalar(0, 0, 255), 3);
            allPts.push_back(Point(outPts[outPts.size() - i - 1].x, outPts[outPts.size() - i - 1].y));
        }
        
        allPts.push_back(Point(outPts[0].x - (outPts.size() - 1) , outPts[0].y));
        
        // Display the line on the processed image
        for (int i = 0; i < pts.size() - 1; ++i){
            line(out, pts[i], pts[i + 1], Scalar(0, 0, 255));
        }

        // Display the lane area 
        vector<vector<Point>> arr;
        arr.push_back(allPts);
        Mat overlay = Mat::zeros(org.size(), org.type());
        fillPoly(overlay, arr, Scalar(0, 255, 100));
        addWeighted(org, 1, overlay, 0.5, 0, org);

        //Show the result
        // imshow("Post processing: ", out);
        // imshow("Original: ", org);
        if (waitKey(50) > 0)
            break;
    }
    // release the capture
    cap.release();
        
    waitKey();
    return 0;
}

