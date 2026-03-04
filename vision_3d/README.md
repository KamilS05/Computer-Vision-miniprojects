# 3D Vision: Camera Calibration and Simple AR (Axes / Cube)

This project implements a basic 3D vision workflow using a chessboard pattern:

- **Camera calibration** from multiple chessboard views
- Saving calibration parameters to a YAML file
- **Undistortion** using the calibration model
- **Pose estimation (solvePnP)**
- Simple augmented reality: drawing **3D axes** or a **3D cube** on the chessboard

The project was developed for educational purposes.

---

## Build

Make sure you have **g++**, **make**, and **OpenCV** installed.

make

---

## Usage

The program supports two modes:

- `--calibrate` : capture chessboard views and generate calibration file
- `--run` : load calibration and run AR overlay (axes/cube)

Input can be taken from:

- a camera (`--camera`)
- a video file (`--video`)

---

## Parameters

### Input

`--camera=<int>`  
camera index (default: 0)

`--video=<path>`  
use a video file instead of a camera

### Chessboard settings

`--rows=<int>`  
number of inner corners per column (default: 6)

`--cols=<int>`  
number of inner corners per row (default: 9)

`--square=<float>`  
square size in your chosen units (default: 25.0)

### Files

`--out=<path>`  
output calibration file (default: `camera.yml`)

`--params=<path>`  
path to calibration file used in `--run` mode

### Drawing

`--draw=axes` or `--draw=cube`  
default: axes

---

## Examples

### Calibration from camera (save to camera.yml)


./program --calibrate --camera=0 --rows=6 --cols=9 --square=25 --out=camera.yml


### Calibration from video


./program --calibrate --video=calib.mp4 --rows=6 --cols=9 --square=25 --out=camera.yml


### Run AR overlay (axes)


./program --run --camera=0 --params=camera.yml --draw=axes


### Run AR overlay (cube)


./program --run --camera=0 --params=camera.yml --draw=cube


---

## Controls

### Calibration mode (`--calibrate`)
- `SPACE` : capture a chessboard view (only if detected)
- `d`     : toggle fast preview mode
- `c`     : calibrate and save to file
- `r`     : reset captured views
- `ESC`   : exit

### AR mode (`--run`)
- `a`     : draw axes
- `u`     : draw cube
- `d`     : toggle drawing chessboard corners
- `s`     : save snapshot (`ar_XXXX.png`)
- `ESC`   : exit

---

## Output

Calibration mode prints RMS and mean reprojection error and saves camera parameters to a YAML file.

AR mode displays an undistorted video stream with an overlay (axes or cube) aligned to the detected chessboard.

---

## Author

Kamil Staniszewski