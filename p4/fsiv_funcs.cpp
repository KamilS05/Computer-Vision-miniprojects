/**
 * @file fsiv_funcs.cpp
 * @brief Definitions for Smart Background Filtering helpers (motion + edges).
 */

#include "fsiv_funcs.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void fsiv_to_grayscale(const Mat& bgr, Mat& gray)
{
    if (bgr.channels() == 3)
        cvtColor(bgr, gray, COLOR_BGR2GRAY);
    else
        gray = bgr.clone();
}

void fsiv_compute_optical_flow_farneback(
    const Mat& prev_gray,
    const Mat& gray,
    Mat& flow,
    double pyr_scale, int levels, int winsize,
    int iterations, int poly_n, double poly_sigma)
{
    calcOpticalFlowFarneback(prev_gray, gray, flow,
                             pyr_scale, levels, winsize,
                             iterations, poly_n, poly_sigma, 0);
}

void fsiv_flow_magnitude(const Mat& flow, Mat& mag)
{
    vector<Mat> ch(2);
    split(flow, ch);
    magnitude(ch[0], ch[1], mag);
}


void fsiv_motion_mask_from_mag(const Mat& mag, float t_flow, Mat& mask)
{
   

    Mat tmp;
    threshold(mag, tmp, t_flow, 255.0, THRESH_BINARY);
    tmp.convertTo(mask, CV_8U);
}

void fsiv_update_running_mask(Mat& prev_mask_f, const Mat& curr_mask_u, float alpha, Mat& out_mask_u)
{
    

    Mat curr_f;
    curr_mask_u.convertTo(curr_f, CV_32F, 1.0 / 255.0);

    if (prev_mask_f.empty() || prev_mask_f.size() != curr_mask_u.size() || prev_mask_f.type() != CV_32F)
        prev_mask_f = curr_f.clone();
    else
        prev_mask_f = alpha * prev_mask_f + (1.0f - alpha) * curr_f;

    Mat tmp;
    prev_mask_f.convertTo(tmp, CV_8U, 255.0);
    out_mask_u = tmp;
}

void fsiv_compute_edges(const Mat& gray, int low, int high, Mat& edges)
{
    
    Canny(gray, edges, low, high);
}

void fsiv_refine_foreground_mask(const cv::Mat& motion_u,
                                 const cv::Mat& edges_u,
                                 int dilate_radius,
                                 cv::Mat& refined_u)
{
    cv::Mat edges_dil = edges_u.clone();
    if (dilate_radius > 0)
    {
        int k = 2 * dilate_radius + 1;
        cv::Mat se = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(k, k));
        cv::dilate(edges_u, edges_dil, se);
    }

    cv::bitwise_or(motion_u, edges_dil, refined_u);
}
void fsiv_apply_background_blur(const Mat& bgr, const Mat& fg_mask_u, int blur_radius, Mat& out_bgr)
{
    
    int ksz = 2 * blur_radius + 1;
    Mat blurred;
    GaussianBlur(bgr, blurred, Size(ksz, ksz), 0);

    out_bgr = blurred.clone();
    bgr.copyTo(out_bgr, fg_mask_u);
}

void create_gui(const string& win,
                int& tr_alpha, int& tr_tflow, int& tr_blur,
                int& tr_low, int& tr_high, int& tr_dil)
{
    createTrackbar("alpha x100", win, &tr_alpha, 100);
    createTrackbar("tflow x0.1", win, &tr_tflow, 100);
    createTrackbar("blur",      win, &tr_blur, 20);
    createTrackbar("cLow",      win, &tr_low, 255);
    createTrackbar("cHigh",     win, &tr_high,255);
    createTrackbar("edgeDil",   win, &tr_dil, 20);
}


void fsiv_process_frame(const Mat& frame,
                        Mat& prev_gray,
                        int tr_alpha, int tr_tflow, int tr_blur,
                        int tr_low, int tr_high, int tr_dil,
                        Mat& outframe, Mat& dbg,
                        Mat& motion_smooth_f)
{
    Mat gray;
    fsiv_to_grayscale(frame, gray);

    float alpha = tr_alpha / 100.0f;
    float tflow = tr_tflow / 10.0f;

    Mat flow;
    fsiv_compute_optical_flow_farneback(prev_gray, gray, flow);

    Mat mag;
    fsiv_flow_magnitude(flow, mag);

    Mat motion_mask;
    fsiv_motion_mask_from_mag(mag, tflow, motion_mask);

    Mat motion_smooth_u;
    fsiv_update_running_mask(motion_smooth_f, motion_mask, alpha, motion_smooth_u);

    Mat edges;
    fsiv_compute_edges(gray, tr_low, tr_high, edges);

    Mat refined;
    fsiv_refine_foreground_mask(motion_smooth_u, edges, tr_dil, refined);

    fsiv_apply_background_blur(frame, refined, tr_blur, outframe);

    
    Mat mag_norm, mag_u8;
    normalize(mag, mag_norm, 0, 255, NORM_MINMAX);
    mag_norm.convertTo(mag_u8, CV_8U);

    Mat mag_u8_3, edges3, refined3;
    
    cvtColor(mag_u8, mag_u8_3, COLOR_GRAY2BGR);
    cvtColor(edges, edges3, COLOR_GRAY2BGR);
    cvtColor(refined, refined3, COLOR_GRAY2BGR);


    hconcat(vector<Mat>{ mag_u8_3, edges3, refined3 }, dbg);

    prev_gray = gray.clone();
}

