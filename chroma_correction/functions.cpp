#include "functions.h"


Mat processImage()
{
    
    // 1. CONVERT SLIDER VALUES
    float contrast = slider_contrast / 100.0f;          // 100 → 1.0
    float brightness = (slider_brightness - 100) / 100.0f;    // 100 → 0.0
    float gamma = slider_gamma / 100.0f;                // 100 → 1.0
    
    // 2. SIZE ADJUSTMENT
    Mat object = object_image.clone();
    resize(object, object, background_image.size());
    
    // 3. CONVERT TO HSV (for chroma key mask)
    Mat object_hsv;
    cvtColor(object, object_hsv, COLOR_BGR2HSV);
    
    // 4. CREATE CHROMA KEY MASK (green background)
    Scalar lower_threshold(h_min, s_min, v_min);
    Scalar upper_threshold(h_max, s_max, v_max);
    Mat mask;
    inRange(object_hsv, lower_threshold, upper_threshold, mask);
    
    // 5. LIGHTING CORRECTION
    Mat object_corrected;
    
    if (use_v_channel_only)
    {
        
        // MODE 1: Apply correction to V channel only (preserves hue and saturation)
        vector<Mat> hsv_channels;
        split(object_hsv, hsv_channels);  // Split into H, S, V
        
        Mat V_normalized, V_corrected;
        hsv_channels[2].convertTo(V_normalized, CV_32F, 1.0 / 255.0);
        
        // Apply formula: I' = c * I^g + b
        pow(V_normalized, gamma, V_corrected);
        V_corrected = contrast * V_corrected + brightness;
        
        // Limit values to range [0.0, 1.0]
        min(V_corrected, 1.0, V_corrected);  
        max(V_corrected, 0.0, V_corrected); 
        
        // Convert back to 0-255
        V_corrected.convertTo(hsv_channels[2], CV_8U, 255.0);
        
        // Merge channels and convert back to BGR
        merge(hsv_channels, object_hsv);
        cvtColor(object_hsv, object_corrected, COLOR_HSV2BGR);
    }
    else
    {
        
        // MODE 2: Apply correction directly to RGB channels
        Mat object_float;
        object.convertTo(object_float, CV_32F, 1.0 / 255.0);
        
        // Apply formula: I' = c * I^g + b on all BGR channels
        pow(object_float, gamma, object_float);
        object_float = contrast * object_float + brightness;
        
        // Limit values to range [0.0, 1.0]
        min(object_float, 1.0, object_float);
        max(object_float, 0.0, object_float);
        
        // Convert back to 0-255
        object_float.convertTo(object_corrected, CV_8U, 255.0);
    }
    
    // 6. COMBINE OBJECT WITH BACKGROUND
    Mat inverted_mask;
    bitwise_not(mask, inverted_mask);
    
    Mat object_part, background_part;
    object_corrected.copyTo(object_part, inverted_mask);
    background_image.copyTo(background_part, mask);
    
    Mat result = object_part + background_part;
    
    return result;
}

void updateImage(int, void*)
{
    Mat result = processImage();
    imshow("FSIV-ComposeIllum", result);
}

void interactiveMode(String save_path)
{
    
    namedWindow("FSIV-ComposeIllum");
    
    
    createTrackbar("Contrast", "FSIV-ComposeIllum", &slider_contrast, 200, updateImage);
    createTrackbar("Brightness", "FSIV-ComposeIllum", &slider_brightness, 200, updateImage);
    createTrackbar("Gamma", "FSIV-ComposeIllum", &slider_gamma, 200, updateImage);
    
    
    createTrackbar("H min", "FSIV-ComposeIllum", &h_min, 180, updateImage);
    createTrackbar("H max", "FSIV-ComposeIllum", &h_max, 180, updateImage);
    createTrackbar("S min", "FSIV-ComposeIllum", &s_min, 255, updateImage);
    createTrackbar("S max", "FSIV-ComposeIllum", &s_max, 255, updateImage);
    createTrackbar("V min", "FSIV-ComposeIllum", &v_min, 255, updateImage);
    createTrackbar("V max", "FSIV-ComposeIllum", &v_max, 255, updateImage);
    
    
    updateImage(0, 0);
    
   
    while (true)
    {
        int key = waitKey(30);
        
        
        if (key == 27)
        {
            cout << "Closing program...\n";
            break;
        }
        
        
        if (key == 's' || key == 'S')
        {
            Mat result = processImage();
            imwrite(save_path, result);
            cout << " Saved: " << save_path << "\n";
        }
    }
    
    destroyAllWindows();
}

void normalMode(float contrast, float brightness, float gamma, String save_path)
{
    // Set slider values based on parameters
    slider_contrast = (int)(contrast * 100);
    slider_brightness = (int)((brightness + 1.0) * 100);
    slider_gamma = (int)(gamma * 100);
    
    
    Mat result = processImage();
    
    
    imshow("Result", result);
    
    
    if (!save_path.empty())
    {
        imwrite(save_path, result);
        cout << "✓ Saved: " << save_path << "\n";
    }
    
    cout << "Press any key to exit...\n";
    waitKey(0);
    destroyAllWindows();
}