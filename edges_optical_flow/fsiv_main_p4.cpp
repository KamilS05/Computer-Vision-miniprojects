#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include "fsiv_funcs.hpp"

using namespace cv;
using namespace std;

static const char* KEYS =
    "{help h ? |      | print this message }"
    "{video    |      | path to input video file }"
    "{camera   | -1   | camera index (>=0) }"
    "{alpha    | 0.80 | running-average alpha in [0,1] }"
    "{tflow    | 0.50 | motion threshold (pixels/frame) }"
    "{blur     | 7    | Gaussian blur radius (0=no blur) }"
    "{canny_low| 50   | Canny low threshold [0..255] }"
    "{canny_high|150  | Canny high threshold [0..255] }"
    "{edge_dil | 2    | edge dilation radius (pixels) }"
    "{out      |      | output video path }";

int main(int argc, char** argv)
{
    CommandLineParser parser(argc, argv, KEYS);
    parser.about("FSIV Smart Background Filter (motion + edges)");

    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }

    const string kWinOut = "FSIV Output";
    const string kWinDbg = "FSIV Debug (flow|edges|mask)";

    VideoCapture cap;
    string video_path = parser.get<string>("video");
    int cam = parser.get<int>("camera");

    if (!video_path.empty()) cap.open(video_path);
    else                     cap.open(cam);

    if (!cap.isOpened())
    {
        cerr << "ERROR: Cannot open input.\n";
        return -1;
    }

    Mat frame;
    if (!cap.read(frame) || frame.empty())
    {
        cerr << "ERROR: Empty stream.\n";
        return -1;
    }

    Mat prev_gray;
    fsiv_to_grayscale(frame, prev_gray);
    Mat motion_smooth_f;  // running average state

    namedWindow(kWinOut);
    namedWindow(kWinDbg);

    int tr_alpha = int(parser.get<float>("alpha") * 100);
    int tr_tflow = int(parser.get<float>("tflow") * 10);
    int tr_blur  = parser.get<int>("blur");
    int tr_low   = parser.get<int>("canny_low");
    int tr_high  = parser.get<int>("canny_high");
    int tr_dil   = parser.get<int>("edge_dil");

    create_gui(kWinOut, tr_alpha, tr_tflow, tr_blur, tr_low, tr_high, tr_dil);

    //Output video writer
    string out_path = parser.get<string>("out");
    VideoWriter out;
    if (!out_path.empty())
    {
        double fps = cap.get(CAP_PROP_FPS);
        if (fps <= 0.0) fps = 30;
        out.open(out_path, VideoWriter::fourcc('a','v','c','1'), fps, frame.size());
        if (!out.isOpened())
            cerr << "WARNING: Cannot open output file.\n";
    }

    cout << "FSIV P4 FULL VERSION (motion + edges + blur)\n";
    cout << "Controls: ESC=exit  S=snapshot\n";

    // ----- MAIN LOOP -----
    while (true)
    {
        if (!cap.read(frame) || frame.empty())
            break;

        Mat outframe, dbg;

        fsiv_process_frame(frame,
                           prev_gray,
                           tr_alpha, tr_tflow, tr_blur,
                           tr_low, tr_high, tr_dil,
                           outframe, dbg,
                           motion_smooth_f);

        imshow(kWinOut, outframe);
        imshow(kWinDbg, dbg);

        if (out.isOpened())
            out << outframe;

        int key = waitKey(1) & 0xFF;
        if (key == 27) break;
        if (key == 's')
        {
            imwrite("snapshot_full.png", outframe);
            cout << "Saved snapshot_full.png\n";
        }
    }

    return 0;
}