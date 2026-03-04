#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream> 
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc/imgproc.hpp> 

using namespace std;
using namespace cv;

// Global variable declarations
extern Mat object_image;
extern Mat background_image;
extern int slider_contrast;
extern int slider_brightness;
extern int slider_gamma;
extern int h_min, h_max;
extern int s_min, s_max;
extern int v_min, v_max;
extern bool use_v_channel_only; 

// Function declarations
Mat processImage();
void updateImage(int, void*);
void interactiveMode(String save_path);
void normalMode(float contrast, float brightness, float gamma, String save_path);

#endif