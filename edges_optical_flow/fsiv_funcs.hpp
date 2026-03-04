
#ifndef FSIV_FUNCS_HPP
#define FSIV_FUNCS_HPP

#include <opencv2/opencv.hpp>
#include <string>  


template<typename T>
inline T fsiv_clamp(T v, T lo, T hi)
{
    return (v < lo) ? lo : ((v > hi) ? hi : v);
}


void fsiv_to_grayscale(const cv::Mat& bgr, cv::Mat& gray);


void fsiv_compute_optical_flow_farneback(
    const cv::Mat& prev_gray,
    const cv::Mat& gray,
    cv::Mat& flow,
    double pyr_scale=0.5, int levels=3, int winsize=15,
    int iterations=3, int poly_n=5, double poly_sigma=1.2);

void fsiv_flow_magnitude(const cv::Mat& flow, cv::Mat& mag);


void fsiv_motion_mask_from_mag(const cv::Mat& mag, float t_flow, cv::Mat& mask);


void fsiv_update_running_mask(
    cv::Mat& prev_mask_f, const cv::Mat& curr_mask_u, float alpha, cv::Mat& out_mask_u);


void fsiv_compute_edges(const cv::Mat& gray, int low, int high, cv::Mat& edges);


void fsiv_refine_foreground_mask(
    const cv::Mat& motion_u, const cv::Mat& edges_u, int dilate_radius, cv::Mat& refined_u);


void fsiv_apply_background_blur(
    const cv::Mat& bgr, const cv::Mat& fg_mask_u, int blur_radius, cv::Mat& out_bgr);

void fsiv_process_frame(const cv::Mat& frame,
                        cv::Mat& prev_gray,
                        int tr_alpha, int tr_tflow, int tr_blur,
                        int tr_low, int tr_high, int tr_dil,
                        cv::Mat& outframe, cv::Mat& dbg,
                        cv::Mat& motion_smooth_f);

void create_gui(const std::string& win,
                int& tr_alpha, int& tr_tflow, int& tr_blur,
                int& tr_low, int& tr_high, int& tr_dil);


#endif // FSIV_FUNCS_HPP
