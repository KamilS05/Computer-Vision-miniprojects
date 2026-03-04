// File: fsiv_funcs.cpp
// (c) mjmarin

#include "fsiv_funcs.hpp"
#include <algorithm>
#include <iostream>

using namespace cv;
using namespace std;

// External global variables from main
extern Mat input_image;
extern bool use_clahe;
extern bool luma_only;
extern bool show_debug;
extern float alpha;
extern float gamma1;
extern float gain;
extern float sigma;
extern float clip_limit;
extern int tiles;


extern const char* kWin;
extern const char* kWinContrast;
extern const char* kWinDetail;


Mat fsiv_contrast_enhance(const Mat& src01) {
    Mat result;
    
    if (src01.channels() == 1) {
        // Grayscale image
        Mat src_8u;
        src01.convertTo(src_8u, CV_8U, 255.0);
        
        if (use_clahe) {
            Ptr<CLAHE> clahe = createCLAHE(clip_limit, Size(tiles, tiles));
            clahe->apply(src_8u, src_8u);
        } else {
            equalizeHist(src_8u, src_8u);
        }
        
        src_8u.convertTo(result, CV_32F, 1.0/255.0);
    }
    else {
        // Color image
        if (luma_only) {
            // Process V channel in HSV
            Mat src_8u;
            src01.convertTo(src_8u, CV_8U, 255.0);
            
            Mat hsv;
            cvtColor(src_8u, hsv, COLOR_BGR2HSV);
            
            vector<Mat> hsv_channels;
            split(hsv, hsv_channels);
            
            // Process V channel
            if (use_clahe) {
                Ptr<CLAHE> clahe = createCLAHE(clip_limit, Size(tiles, tiles));
                clahe->apply(hsv_channels[2], hsv_channels[2]);
            } else {
                equalizeHist(hsv_channels[2], hsv_channels[2]);
            }
            
            merge(hsv_channels, hsv);
            cvtColor(hsv, src_8u, COLOR_HSV2BGR);
            src_8u.convertTo(result, CV_32F, 1.0/255.0);
        }
        else {
            // Process each BGR channel independently
            Mat src_8u;
            src01.convertTo(src_8u, CV_8U, 255.0);
            
            vector<Mat> bgr_channels;
            split(src_8u, bgr_channels);
            
            for (int i = 0; i < 3; i++) {
                if (use_clahe) {
                    Ptr<CLAHE> clahe = createCLAHE(clip_limit, Size(tiles, tiles));
                    clahe->apply(bgr_channels[i], bgr_channels[i]);
                } else {
                    equalizeHist(bgr_channels[i], bgr_channels[i]);
                }
            }
            
            merge(bgr_channels, src_8u);
            src_8u.convertTo(result, CV_32F, 1.0/255.0);
        }
    }
    
    return result;
}

Mat fsiv_detail_enhance(const Mat& src01) {
    Mat blurred;
    
    // Calculate kernel size from sigma
    int ksize = (int)(ceil(sigma * 3) * 2 + 1);
    if (ksize < 3) ksize = 3;
    
    GaussianBlur(src01, blurred, Size(ksize, ksize), sigma);
    
    // Unsharp mask: Out = (1+gain)*I - gain*Blur
    Mat result = (1.0 + gain) * src01 - gain * blurred;
    
    // Clamp to [0,1]
    min(result, 1.0, result);
    max(result, 0.0, result);
    
    return result;
}

Mat fsiv_fuse(const Mat& contrast01, const Mat& detail01) {
    
    Mat result;
    addWeighted(contrast01, alpha, detail01, 1.0 - alpha, 0.0, result);
    
    // Clamp to [0,1]
    min(result, 1.0, result);
    max(result, 0.0, result);
    
    return result;
}

Mat fsiv_apply_gamma(const Mat& src01) {
    if (abs(gamma1 - 1.0f) < 0.001f) {
        return src01.clone();
    }
    
    Mat result;
    pow(src01, gamma1, result);
    
    
    min(result, 1.0, result);
    max(result, 0.0, result);
    
    return result;
}

Mat fsiv_process_pipeline(const Mat& src01) {
    // Step 1: Contrast branch
    Mat contrast01 = fsiv_contrast_enhance(src01);
    
    // Step 2: Detail branch
    Mat detail01 = fsiv_detail_enhance(src01);
    
    // Step 3: Fusion
    Mat fused01 = fsiv_fuse(contrast01, detail01);
    
    // Step 4: gamma1 correction
    Mat result01 = fsiv_apply_gamma(fused01);
    
    return result01;
}

Mat fsiv_side_by_side_u8(const Mat& src_bgr_u8, const Mat& out_bgr_u8) {
    Mat result;
    hconcat(src_bgr_u8, out_bgr_u8, result);
    return result;
}


Mat processImage()
{
    // Convert input image to float [0,1]
    Mat src01;
    input_image.convertTo(src01, CV_32F, 1.0/255.0);
    
    // Process through pipeline
    Mat result01 = fsiv_process_pipeline(src01);
    
    // Convert back to 8-bit
    Mat result_u8;
    result01.convertTo(result_u8, CV_8U, 255.0);
    
    return result_u8;
}


void updateImage(int, void*)
{
    Mat result = processImage();
    
   
    Mat side_by_side = fsiv_side_by_side_u8(input_image, result);
    imshow(kWin, side_by_side);
    
    if (show_debug) {
        Mat src01;
        input_image.convertTo(src01, CV_32F, 1.0/255.0);
        
        Mat contrast01 = fsiv_contrast_enhance(src01);
        Mat detail01 = fsiv_detail_enhance(src01);
        
        Mat contrast_u8, detail_u8;
        contrast01.convertTo(contrast_u8, CV_8U, 255.0);
        detail01.convertTo(detail_u8, CV_8U, 255.0);
        
        imshow(kWinContrast, contrast_u8);
        imshow(kWinDetail, detail_u8);
    }
}


void interactiveMode(const string& output_file)
{
    namedWindow(kWin);
    
    if (show_debug) {
        namedWindow(kWinContrast);
        namedWindow(kWinDetail);
    }
    
    
    int alpha_trackbar = static_cast<int>(alpha * 1000);
    int gain_trackbar = static_cast<int>(gain * 100);
    int sigma_trackbar = static_cast<int>((sigma - 0.1) / 19.9 * 1000);
    int gamma_trackbar = static_cast<int>((gamma1 - 0.1) / 2.9 * 1000);
    int clip_trackbar = static_cast<int>((clip_limit - 0.1) / 9.9 * 1000);
    
    createTrackbar("Alpha x1000", kWin, &alpha_trackbar, 1000, updateImage);
    createTrackbar("Gain x100", kWin, &gain_trackbar, 1000, updateImage);
    createTrackbar("Sigma", kWin, &sigma_trackbar, 1000, updateImage);
    createTrackbar("Gamma", kWin, &gamma_trackbar, 1000, updateImage);
    createTrackbar("CLAHE clip", kWin, &clip_trackbar, 1000, updateImage);
    createTrackbar("CLAHE tiles", kWin, &tiles, 32, updateImage);
    
    
    updateImage(0, nullptr);
    
    while (true) {
        int key = waitKey(30);
        
        // Update float values from trackbars
        alpha = alpha_trackbar / 1000.0f;
        gain = 10.0f * gain_trackbar / 1000.0f;
        sigma = 0.1f + 19.9f * sigma_trackbar / 1000.0f;
        gamma1 = 0.1f + 2.9f * gamma_trackbar / 1000.0f;
        clip_limit = 0.1f + 9.9f * clip_trackbar / 1000.0f;
        if (tiles < 2) tiles = 2;
        
        if (key == 27) { // ESC
            cout << "Closing program...\n";
            break;
        }
        
        if (key == 's' || key == 'S') {
            Mat result = processImage();
            imwrite(output_file, result);
            cout << "Saved: " << output_file << "\n";
        }
    }
    
    destroyAllWindows();
}


void normalMode(float alpha_param, float gain_param, float sigma_param, float gamma_param, 
                float clip_limit_param, int tiles_param, const string& output_file)
{
    // Set global variables
    alpha = alpha_param;
    gain = gain_param;
    sigma = sigma_param;
    gamma1 = gamma_param;
    clip_limit = clip_limit_param;
    tiles = tiles_param;
    
    Mat result = processImage();
    

    Mat side_by_side = fsiv_side_by_side_u8(input_image, result);
    
    imshow("Result", side_by_side);
    
    if (!output_file.empty()) {
        imwrite(output_file, result);
        cout << "Saved: " << output_file << "\n";
    }
    
    cout << "Press any key to exit...\n";
    waitKey(0);
    destroyAllWindows();
}