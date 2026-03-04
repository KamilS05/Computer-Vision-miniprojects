#include "fsiv_funcs.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <numeric>

using namespace cv;
using namespace std;

void fsiv_create_chessboard_3d_points(const Size& pattern_size,
                                      float square_size,
                                      vector<Point3f>& object_points)
{
    object_points.clear();
    for (int i = 0; i < pattern_size.height; ++i)
        for (int j = 0; j < pattern_size.width; ++j)
            object_points.emplace_back(j * square_size, i * square_size, 0.0f);
}

bool fsiv_find_chessboard_corners(const Mat& image,
                                  const Size& pattern_size,
                                  vector<Point2f>& corners,
                                  bool fast_preview)
{
    Mat gray;
    if (image.channels() == 3)
        cvtColor(image, gray, COLOR_BGR2GRAY);
    else
        gray = image;

    int flags = CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE | CALIB_CB_FILTER_QUADS;
    if (fast_preview)
        flags |= CALIB_CB_FAST_CHECK;

    bool found = findChessboardCorners(gray, pattern_size, corners, flags);
    if (found && !fast_preview)
    {
        TermCriteria term(TermCriteria::EPS | TermCriteria::COUNT, 30, 0.001);
        cornerSubPix(gray, corners, Size(7, 7), Size(-1, -1), term);
    }
    return found;
}

double fsiv_calibrate_camera(const vector<vector<Point3f>>& object_points_list,
                             const vector<vector<Point2f>>& image_points_list,
                             const Size& image_size,
                             Mat& camera_matrix, Mat& dist_coeffs,
                             vector<Mat>& rvecs, vector<Mat>& tvecs)
{
    camera_matrix = Mat::eye(3, 3, CV_64F);
    dist_coeffs = Mat::zeros(1, 8, CV_64F);
    int flags = 0; 
    flags |= CALIB_RATIONAL_MODEL;
    return calibrateCamera(object_points_list, image_points_list, image_size,
                           camera_matrix, dist_coeffs, rvecs, tvecs, flags);
}

double fsiv_compute_reprojection_error(const vector<vector<Point3f>>& object_points_list,
                                       const vector<vector<Point2f>>& image_points_list,
                                       const vector<Mat>& rvecs,
                                       const vector<Mat>& tvecs,
                                       const Mat& K, const Mat& dist)
{
    double total_err = 0.0;
    size_t total_points = 0;

    for (size_t i = 0; i < object_points_list.size(); ++i)
    {
        vector<Point2f> projected;
        projectPoints(object_points_list[i], rvecs[i], tvecs[i], K, dist, projected);
        double err = norm(image_points_list[i], projected, NORM_L2);
        total_err += err * err;
        total_points += object_points_list[i].size();
    }
    if (total_points == 0) return 0.0;
    return std::sqrt(total_err / static_cast<double>(total_points));
}

bool fsiv_save_calibration(const string& path,
                           const Mat& K, const Mat& dist,
                           const Size& image_size,
                           double reproj_error)
{
    FileStorage fs(path, FileStorage::WRITE);
    if (!fs.isOpened())
        return false;
    fs << "image_width" << image_size.width;
    fs << "image_height" << image_size.height;
    fs << "camera_matrix" << K;
    fs << "distortion_coefficients" << dist;
    fs << "error" << reproj_error;
    return true;
}

bool fsiv_load_calibration(const string& path,
                           Mat& K, Mat& dist)
{
    FileStorage fs(path, FileStorage::READ);
    if (!fs.isOpened())
        return false;
    fs["camera_matrix"] >> K;
    fs["distortion_coefficients"] >> dist;
    return !K.empty() && !dist.empty();
}

void fsiv_prepare_undistort_maps(const Mat& K, const Mat& dist,
                                 const Size& image_size,
                                 Mat& map1, Mat& map2)
{
    double alpha = 0.5; 
    Mat newK = getOptimalNewCameraMatrix(K, dist, image_size, alpha, image_size, nullptr, true);
    initUndistortRectifyMap(K, dist, Mat::eye(3,3,CV_64F), newK, image_size, CV_16SC2, map1, map2);
}

void fsiv_undistort_with_maps(const Mat& src, Mat& dst,
                              const Mat& map1, const Mat& map2)
{
    remap(src, dst, map1, map2, INTER_LINEAR);
}

bool fsiv_estimate_pose(const vector<Point3f>& object_points,
                        const vector<Point2f>& image_points,
                        const Mat& K, const Mat& dist,
                        Mat& rvec, Mat& tvec)
{
    if (object_points.empty() || image_points.empty())
        return false;
    return solvePnP(object_points, image_points, K, dist, rvec, tvec);
}

void fsiv_draw_axes(Mat& image, const Mat& K, const Mat& dist,
                    const Mat& rvec, const Mat& tvec, float axis_length)
{
    vector<Point3f> pts3 = { {0,0,0}, {axis_length,0,0}, {0,axis_length,0}, {0,0,-axis_length} };
    vector<Point2f> pts2;
    projectPoints(pts3, rvec, tvec, K, dist, pts2);
    auto O = pts2[0];
    line(image, O, pts2[1], Scalar(0,0,255), 2);
    line(image, O, pts2[2], Scalar(0,255,0), 2);
    line(image, O, pts2[3], Scalar(255,0,0), 2);
}


void fsiv_draw_cube(Mat& image, const Mat& K, const Mat& dist,
                    const Mat& rvec, const Mat& tvec, float s)
{
    vector<Point3f> v = {
        {0,0,0}, {s,0,0}, {s,s,0}, {0,s,0},
        {0,0,-s}, {s,0,-s}, {s,s,-s}, {0,s,-s}
    };
    vector<Point2f> p;
    projectPoints(v, rvec, tvec, K, dist, p);
    int A=0,B=1,C=2,D=3,E=4,F=5,G=6,H=7;
    line(image, p[A], p[B], Scalar(0,255,255), 2);
    line(image, p[B], p[C], Scalar(0,255,255), 2);
    line(image, p[C], p[D], Scalar(0,255,255), 2);
    line(image, p[D], p[A], Scalar(0,255,255), 2);
    line(image, p[E], p[F], Scalar(0,255,255), 2);
    line(image, p[F], p[G], Scalar(0,255,255), 2);
    line(image, p[G], p[H], Scalar(0,255,255), 2);
    line(image, p[H], p[E], Scalar(0,255,255), 2);
    line(image, p[A], p[E], Scalar(0,255,255), 2);
    line(image, p[B], p[F], Scalar(0,255,255), 2);
    line(image, p[C], p[G], Scalar(0,255,255), 2);
    line(image, p[D], p[H], Scalar(0,255,255), 2);
}
