#include <iostream> 
#include <opencv2/core/utility.hpp> 
#include "functions.h"

using namespace std;
using namespace cv;



// Images
Mat object_image;  // Original image with object
Mat background_image;      // Background image

int slider_contrast = 100;   
int slider_brightness = 100;    
int slider_gamma = 100;      

// Processing mode flag
bool use_v_channel_only = false;  

// Chroma key HSV parameters
int h_min = 40;    // Hue 
int h_max = 80;    
int s_min = 50;    // Saturation 
int s_max = 255;   
int v_min = 50;    // Value
int v_max = 255;   


int main(int argc, char** argv)
{
    // -----------------------------
    // 1. ARGUMENT DEFINITION
    // -----------------------------
    
    const char* keys =
        "{help h         |      | show help }"
        "{i interactive  |      | interactive mode }"
        "{l              |      | apply correction to V channel only }"
        "{c contrast     | 1.0  | contrast (0.0-2.0) }"
        "{b brightness   | 0.0  | brightness (-1.0-1.0) }"
        "{g gamma        | 1.0  | gamma (0.0-2.0) }"
        "{@background    |      | background image file }"
        "{@object        |      | object image file }"
        "{@output        |      | output file }";
    
    CommandLineParser parser(argc, argv, keys);
    parser.about("Chroma Key with lighting correction");
    
   
    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }
    
    // -----------------------------
    // 2. PARAMETER READING
    // -----------------------------
    
    bool interactive_mode = parser.has("i");
    use_v_channel_only = parser.has("l");
    float contrast = parser.get<float>("c");
    float brightness = parser.get<float>("b");
    float gamma = parser.get<float>("g");
    String background_file = parser.get<String>("@background");
    String object_file = parser.get<String>("@object");
    String output_file = parser.get<String>("@output");
    
    if (!parser.check())
    {
        parser.printErrors();
        return -1;
    }
    
    // -----------------------------
    // 3. VALIDATION (only for normal mode)
    // -----------------------------
    
    if (!interactive_mode)
    {
        if (contrast < 0.0f || contrast > 2.0f)
        {
            cerr << "ERROR: Contrast must be in range 0.0-2.0\n";
            return -1;
        }
        if (brightness < -1.0f || brightness > 1.0f)
        {
            cerr << "ERROR: Brightness must be in range -1.0-1.0\n";
            return -1;
        }
        if (gamma < 0.0f || gamma > 2.0f)
        {
            cerr << "ERROR: Gamma must be in range 0.0-2.0\n";
            return -1;
        }
    }
    
    // -----------------------------
    // 4. IMAGE LOADING
    // -----------------------------
    
    background_image = imread(background_file);
    object_image = imread(object_file);
    
    if (background_image.empty())
    {
        cerr << "ERROR: Cannot open background image: " << background_file << "\n";
        return -1;
    }
    
    if (object_image.empty())
    {
        cerr << "ERROR: Cannot open object image: " << object_file << "\n";
        return -1;
    }

    
    // -----------------------------
    // 5. MODE SELECTION
    // -----------------------------
    
    if (interactive_mode)
    {
        interactiveMode(output_file);
    }
    else
    {
        normalMode(contrast, brightness, gamma, output_file);
    }
    
    return 0;
}