
#ifndef FSIV_FUNCS_HPP
#define FSIV_FUNCS_HPP

#include <opencv2/opencv.hpp>
#include <string>
using namespace cv;
using namespace std;


Mat fsiv_contrast_enhance(const Mat& src01);
Mat fsiv_detail_enhance(const Mat& src01);
Mat fsiv_fuse(const Mat& contrast01, const Mat& detail01);
Mat fsiv_apply_gamma(const Mat& src01);
Mat fsiv_process_pipeline(const Mat& src01);
Mat fsiv_side_by_side_u8(const Mat& src_bgr_u8, const Mat& out_bgr_u8);

Mat processImage();
void updateImage(int, void*);
void interactiveMode(const string& save_path);
void normalMode(float alpha, float gain, float sigma, float gamma, float clip_limit, int tiles, const string& save_path);

#endif // FSIV_FUNCS_HPP