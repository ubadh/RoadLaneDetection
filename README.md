# 🛣️ &nbsp; Real Time Road Lane Detection &nbsp;  🛣️

## Abstract

With the emergence of Autonomous cars and the evolution of the Advanced Driver Assistance Systems (ADASs), Road Lane Detection has been playing a major role in lessening road accidents and increasing driving safety. The detection of ego lanes with their corresponding right and left boundaries is of great importance as they provide contextual information, improve lane-based navigation, and help keep self-driving cars within their proper lanes. Hence why, throughout this project, I attempted to implement a real-time algorithm that would detect lanes on a highway.

## Requirements:

### Homebrew

If on macOS, use the following command to install Homebrew:

```
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

### OpenCV:

If on macOS, follow the instructions to install openCV:

1. Install OpenCV

```
brew install opencv
```

2. Install pkg-config

```
brew install pkg-config
```

## Run the Program:

Use the following commands

```
g++ $(pkg-config --cflags --libs opencv) -std=c++11  main.cpp -o output
./output
```

## Algorithm & Techniques:

### Overview:

The program follows a rigorous approach to detecting the road lanes in real-time. As it is very complex to deal with curved/distorted camera angles, the program starts by converting the original camera angle into a bird's eye view (view from the top). Then, the information is extracted by filtering the image using colors. It only extracts the white segments and ignores the surroundings. As a way to clear up the image, a few image processing techniques are used such as Gaussian blur to remove the noise and unnecessary gaps, and a thresholding function to make everything belonging to the lane white, and everything else black. To get the lane data, the program uses the Sliding Window algorithm which returns the points needed to mark the lane. In the end, it converts the points back to the initial camera angle and displays the road with the lane filled with a chosen color, and ego-lane boundaries drawn in another color. A more in-depth explanation is provided right below.

### Converting the angle to a bird's eye view:

Starting with this technique, we define the region of interest in each frame of the original image using a trial and error method, then set the size for th image to be processed. In here, I have chosen 640x480px. We then call getPerspectiveTransform() that provides us with the transformation matrix. We also have to prepare an inverted transformation matrix that we’ll need to transform the points that we will find on the bird’s eye image back into the original size.

```cpp
// Points of interest region (for bird's eye view)   
orgPoints[0] = Point(1380, 1090);
orgPoints[1] = Point(2280, 1090);
orgPoints[2] = Point(3180, 1740);
orgPoints[3] = Point(0, 1740);

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
```

### Image Processing:

#### Masking the white regions:

To mask the white regions, I have used the inRange() function to get all pixels with the colors ranging from very light gray Scalar(195,195,195) to white Scalar(255, 255, 255).

```cpp
// Get the white areas
Mat whiteArea;
inRange(img, Scalar(195, 195, 195), Scalar(255, 255, 255), whiteArea);
Mat processed;
bitwise_and(img, whiteArea, processed);
```

#### Gaussian blur:

```cpp
const Size kernel = Size(9, 9);
GaussianBlur(processed, processed, kernel, 0);
```

#### Erosion:

```cpp
Mat size = Mat::ones(14, 14, CV_8U);
dilate(processed, processed, size);
```

#### Dilation:

```cpp
Mat size = Mat::ones(14, 14, CV_8U);
erode(processed, processed, size);
```

### Sliding Window Algorithm

The main algorithm used to detect the road lane is a variation of the sliding window algorithm. It follows three main steps.
1. Locate all non-zero (white) pixels in a region passed as an argument.
2. Calculate the average x-coordinate of the white pixels (if none, use same x-coordinate from before), and store it.
3. Iterate until reaching the top of the image by moving the slider.
<br/>
The sliding window algorithm is used twice, one for the left edge and the second for the right edge of the lane.

```cpp

vector<Point2f> slidingWindow(Mat image, Rect rec){
    vector<Point2f> points;
    bool done = false;
    
    while (!done){
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
```

### Visualization:

To visualize the road lane, we use the inverted transformation matrix to convert the points into the initial angle, then fill the region in a chosen color, with the edges with different colors.

```cpp
// Display the line on the processed image
for (int i = 0; i < pts.size() - 1; ++i){
    line(out, pts[i], pts[i + 1], Scalar(0, 0, 255));
}

// Display the lane region 
vector<vector<Point>> arr;
arr.push_back(allPts);
Mat overlay = Mat::zeros(org.size(), org.type());
fillPoly(overlay, arr, Scalar(0, 255, 100));
addWeighted(org, 1, overlay, 0.5, 0, org);
```

## Improvements:

### Least square fitting algorithm:
In the future, to improve the curvature of the road lane detection (as right now, it only shows it as straight lines), I'll be using the least square fitting algorithm and use a polynomial curve.

### Combine with Traffic Sign Recognition / Cars detection:
As a further step, I'll be combining this project with the traffic sign recongition Machine Learning Project.

## References:

[1] Tejas Mahale, Chaoran Chen, Wenhui Zhan. “End to End Video Segmentation for Driving: Lane Detection for Autonomous Car”.
<br /><br />
[2] Shrutika Pokale, Deeplaxmi Niture. "On-board Camera-based Lane and Traffic Sign Detection for 2 Wheeler HeadLamp", 2022 IEEE 7th International conference for Convergence in Technology, pp.1-6.










