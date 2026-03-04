// File: fsiv_main_p2.cpp

#if defined(_WIN32) && !defined(NOMINMAX)
#define NOMINMAX
#endif
#include "fsiv_funcs.hpp"

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;


 const char* kWin         = "HDR-like Fusion (Press ESC to exit; S key to save)";
 const char* kWinContrast = "Branch: Contrast (CLAHE / Equalize)";
 const char* kWinDetail   = "Branch: Detail (Unsharp)";

Mat input_image;  // Original image 

bool use_clahe = false;          
bool luma_only = false;           
bool show_debug = false;          
float alpha = 0.6f;              
float gamma1 = 1.0f;              
float gain = 1.2f;              
float sigma = 5.0f;              
float clip_limit = 3.0f;        
int tiles = 8;


int main(int argc, char** argv) {
    const String keys =
        "{help h ? |      | Show help }"
        "{i        |      | Interactive mode (trackbars) }"
        "{l        |      | Process luma channel only }"
        "{clahe    |      | use CLAHE instead of global equalization }"
        "{d        |      | Show debug windows (interactive only) }"
        "{alpha    | 0.6  | fusion weight toward contrast (0.0-1.0) }"
        "{gain     | 1.2  | unsharp gain (0.0-10.0) }"
        "{sigma    | 5.0  | Gaussian blur sigma (0.1-20.0) }"
        "{gamma1   | 1.0  | output gamma1 (0.1-3.0) }"
        "{clip     | 3.0  | CLAHE clip limit (0.1-10.0) }"
        "{tiles    | 8    | CLAHE grid size (2-32) }"
        "{@input   |      | input file }"
        "{@output  |      | output file }";
        

    CommandLineParser parser(argc, argv, keys);
    parser.about("FSIV Practical XXX");
    if (parser.has("help") || !parser.check()) {
        parser.printMessage();
        cout << "\nExamples:\n"
                  << "  " << argv[0]
                  << " -i "
                     "input.jpg output.jpg\n"
                  << "  " << argv[0]
                  << " -i --clahe input.jpg output.jpg \n";
        return 0;
    }

    // TODO: Write your code here
    //  . . . 
    bool interactive_mode = parser.has("i");
    luma_only = parser.has("l");
    use_clahe = parser.has("clahe");
    show_debug = parser.has("d");
     
    alpha = parser.get<float>("alpha");
    gain = parser.get<float>("gain");
    sigma = parser.get<float>("sigma");
    gamma1 = parser.get<float>("gamma1");
    clip_limit = parser.get<float>("clip");
    tiles  = parser.get<int>("tiles");

    String input_file = parser.get<String>("@input");
    String output_file = parser.get<String>("@output");
    
    if (!interactive_mode)
    {
        if (alpha < 0.0f || alpha > 1.0f)
        {
            cerr << "ERROR: alpha must be in range 0.0-1.0\n";
            return -1;
        }
        if (gain < 0.0f || gain > 10.0f)
        {
            cerr << "ERROR: gain must be in range 0.0-10.0\n";
            return -1;
        }
        if (sigma < 0.1f || sigma > 20.0f)
        {
            cerr << "ERROR: sigma must be in range 0.1-20.0\n";
            return -1;
        }
        if (gamma1 < 0.1f || gamma1 > 3.0f)
        {
            cerr << "ERROR: gamma1 must be in range 0.1-3.0\n";
            return -1;
        }
        if (clip_limit < 0.1f || clip_limit > 10.0f)
        {
            cerr << "ERROR: clip must be in range 0.1-10.0\n";
            return -1;
        }
        if (tiles < 2 || tiles > 32)
        {
            cerr << "ERROR: tiles must be in range 2-32\n";
            return -1;
        }
    }

    input_image = imread(input_file);
    if (input_image.empty())
    {
        cerr << "ERROR: Cannot open input image: " << input_file << "\n";
        return -1;
    }

     if (interactive_mode)
    {
        interactiveMode(output_file);
    }
    else
    {
        normalMode(alpha, gain, sigma, gamma1, clip_limit, tiles, output_file);
    }

    return 0;
}