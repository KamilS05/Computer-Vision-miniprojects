# Chroma Key and Lighting Correction

This project implements chroma key compositing with optional lighting correction.

An object image (typically with a green background) is segmented using HSV color thresholds and combined with a different background image.  
The illumination of the object can be adjusted using contrast, brightness and gamma parameters.

The project was developed for educational purposes.

---

## Build

To compile the project, make sure you have **g++**, **make**, and **OpenCV** installed.

make

This will create the executable file.

---

## Usage

The program is executed from the command line and accepts the following arguments.

### Flags

`-i`  
interactive mode with sliders

`-l`  
apply lighting correction only to the HSV V channel

`-c`  
contrast value (range: 0.0 – 2.0, default: 1.0)

`-b`  
brightness value (range: -1.0 – 1.0, default: 0.0)

`-g`  
gamma value (range: 0.0 – 2.0, default: 1.0)

### Positional arguments


background_image object_image output_image


---

## Example

Normal mode:


./program background.jpg object.jpg result.jpg -c 1.2 -b 0.1 -g 1.1


Interactive mode:


./program -i background.jpg object.jpg result.jpg


In interactive mode you can adjust contrast, brightness, gamma and HSV thresholds using sliders.

Press **S** to save the current result.  
Press **ESC** to exit the program.

---

## Output

The program generates a composited image where the object replaces the chroma background and the illumination can be adjusted using the selected parameters.

---

## Author

Kamil Staniszewski