# Edges and Optical Flow (Smart Background Filter)

This project implements a simple **smart background filter** for video streams.

Foreground motion is detected using **Farneback optical flow** and refined using **Canny edge detection**.  
The final mask is used to keep the moving object sharp while applying **Gaussian blur** to the background.

The project was developed for educational purposes.

---

## Build

To compile the project, make sure you have **g++**, **make**, and **OpenCV** installed.

make

This will create the executable file.

---

## Usage

The program can process input from a **video file** or a **camera**.

### Input options

`--video=<path>`  
use a video file as input

`--camera=<index>`  
use a camera as input (0, 1, ...). Default: -1

### Main parameters

`--alpha=<float>`  
running-average smoothing for the motion mask (range: 0.0–1.0, default: 0.80)

`--tflow=<float>`  
motion threshold in pixels/frame (default: 0.50)

`--blur=<int>`  
Gaussian blur radius for the background (0 = no blur, default: 7)

`--canny_low=<int>`  
Canny low threshold (default: 50)

`--canny_high=<int>`  
Canny high threshold (default: 150)

`--edge_dil=<int>`  
edge dilation radius (in pixels) used to expand edges (default: 2)

### Output

`--out=<path>`  
optional output video path (writes the processed frames to a file)

---

## Examples

Process a video file:


./program --video=input.mp4


Use a webcam:


./program --camera=0


Stronger background blur + stricter motion threshold:


./program --video=input.mp4 --blur=12 --tflow=1.0


Save output video:


./program --video=input.mp4 --out=output.mp4


---

## Controls (during execution)

- `ESC` — exit
- `S` — save snapshot (`snapshot_full.png`)

---

## Output

The program shows two windows:

- **FSIV Output** — final video (foreground sharp, background blurred)
- **FSIV Debug** — diagnostic view: `flow magnitude | edges | final mask`

---

## Author

Kamil Staniszewski