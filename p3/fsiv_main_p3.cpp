#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include "fsiv_funcs.hpp"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    CommandLineParser parser(argc, argv,
        "{help h||}"
        "{calibrate||}"
        "{run||}"
        "{camera|0|}"
        "{video||}"
        "{rows|6|}"
        "{cols|9|}"
        "{square|25.0|}"
        "{out|camera.yml|}"
        "{params||}"
        "{draw|axes|}"); 

    if (parser.has("help"))
    {
        cout << "FSIV P3\n"
             << "  --calibrate : capture views + calibration\n"
             << "  --run       : AR (axes/cube)\n";
        return 0;
    }

    const bool do_calib = parser.has("calibrate");
    const bool do_run   = parser.has("run");
    const int cam_idx   = parser.get<int>("camera");
    const string video  = parser.get<string>("video");
    const int rows      = parser.get<int>("rows");
    const int cols      = parser.get<int>("cols");
    const float sq      = parser.get<float>("square");
    const string out    = parser.get<string>("out");
    const string params = parser.get<string>("params");
    string draw_kind    = parser.get<string>("draw");  // "axes" lub "cube"

    const Size pattern_size(cols, rows);

    // Open video
    VideoCapture cap;
    if (video.empty()) cap.open(cam_idx);
    else               cap.open(video);
    if (!cap.isOpened())
    {
        cerr << "Cannot open input.\n";
        return -1;
    }

    // 3D points 
    vector<Point3f> obj_tmpl;
    fsiv_create_chessboard_3d_points(pattern_size, sq, obj_tmpl);

    namedWindow("Chessboard");

    // CALIBRATION MODE

    if (do_calib)
    {
        vector<vector<Point3f>> all_obj;
        vector<vector<Point2f>> all_img;
        Size img_size;
        bool preview_on = false;

        cout << "Calibration: SPACE=add  d=preview  c=calibrate  r=reset  ESC=exit\n";

        while (true)
        {
            Mat frame;
            cap >> frame;
            if (frame.empty()) break;
            img_size = frame.size();

            // Line views
            Mat view = frame.clone();
            if (preview_on)
            {
                vector<Point2f> fast;
                bool ok = fsiv_find_chessboard_corners(frame, pattern_size, fast, true);
                drawChessboardCorners(view, pattern_size, fast, ok);
            }

            putText(view, format("views: %zu", all_img.size()),
                    Point(10,25), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0,255,0), 2);

            imshow("Chessboard", view);
            int key = waitKey(10) & 0xFF;

            if (key == 27) break; // ESC
            if (key == 'd' || key == 'D') preview_on = !preview_on;

            if (key == ' ')
            {
                vector<Point2f> corners;
                if (fsiv_find_chessboard_corners(frame, pattern_size, corners, false))
                {
                    all_img.push_back(corners);
                    all_obj.push_back(obj_tmpl);
                    cout << "Captured " << all_img.size() << " views\n";
                }
                else
                {
                    cout << "Chessboard not found.\n";
                }
            }

            if (key == 'r' || key == 'R')
            {
                all_img.clear();
                all_obj.clear();
                cout << "Reset.\n";
            }

            if (key == 'c' || key == 'C')
            {
                if (all_img.size() < 3)
                {
                    cout << "Need at least 3 views.\n";
                    continue;
                }

                Mat K, dist; vector<Mat> rvecs, tvecs;
                double rms = fsiv_calibrate_camera(all_obj, all_img, img_size, K, dist, rvecs, tvecs);
                double err = fsiv_compute_reprojection_error(all_obj, all_img, rvecs, tvecs, K, dist);

                bool ok = fsiv_save_calibration(out, K, dist, img_size, err);
                cout << "RMS=" << rms << " mean_error=" << err << (ok ? " [saved]\n" : " [save failed]\n");
            }
        }
    }

    //AR MODE

    else if (do_run)
    {
        if (params.empty())
        {
            cerr << "Need --params.\n";
            return -1;
        }

        Mat K, dist;
        if (!fsiv_load_calibration(params, K, dist))
        {
            cerr << "Cannot read calibration file.\n";
            return -1;
        }

        Mat map1, map2;
        bool maps_ready = false;

        bool draw_axes_mode;
        if (draw_kind == "axes")
            draw_axes_mode = true;
        else
            draw_axes_mode = false;
        bool draw_corners   = true;
        int  snap_id        = 0;

        cout << "AR: a=axes  u=cube  d=corners  s=snap  ESC=exit\n";

        while (true)
        {
            Mat frame;
            cap >> frame;
            if (frame.empty()) break;

            if (!maps_ready)
            {
                fsiv_prepare_undistort_maps(K, dist, frame.size(), map1, map2);
                maps_ready = true;
            }

            Mat undist;
            fsiv_undistort_with_maps(frame, undist, map1, map2);

            vector<Point2f> corners;
            bool found = fsiv_find_chessboard_corners(undist, pattern_size, corners, false);

            if (draw_corners)
                drawChessboardCorners(undist, pattern_size, corners, found);

            if (found)
            {
                Mat rvec, tvec;
                if (fsiv_estimate_pose(obj_tmpl, corners, K, Mat::zeros(dist.size(), dist.type()), rvec, tvec))
                {
                    if (draw_axes_mode)
                        fsiv_draw_axes(undist, K, Mat::zeros(dist.size(), dist.type()), rvec, tvec, 2.0f * sq);
                    else
                        fsiv_draw_cube(undist, K, Mat::zeros(dist.size(), dist.type()), rvec, tvec, sq);
                }
            }

            imshow("Chessboard", undist);
            int key = waitKey(10) & 0xFF;

            if (key == 27) break; 
            if (key == 'a' || key == 'A') draw_axes_mode = true;
            if (key == 'u' || key == 'U') draw_axes_mode = false;
            if (key == 'd' || key == 'D') draw_corners = !draw_corners;
            if (key == 's' || key == 'S')
            {
                string name = format("ar_%04d.png", snap_id++);
                imwrite(name, undist);
                cout << "Saved: " << name << "\n";
            }
        }
    }
    else
    {
        cout << "Nothing to do. Use --calibrate or --run.\n";
    }

    destroyAllWindows();
    return 0;
}
