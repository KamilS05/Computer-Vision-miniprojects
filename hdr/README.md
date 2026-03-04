# HDR-like Fusion (Contrast + Detail)

This project implements a simple HDR-like image enhancement pipeline based on two branches:

- **Contrast branch**: histogram equalization or CLAHE
- **Detail branch**: unsharp masking (detail enhancement)

Both branches are fused using a weighted sum, and an optional gamma correction is applied at the end.

The project was developed for educational purposes.

---

## Build

Make sure you have **g++**, **make**, and **OpenCV** installed.

make

---

## Usage


./program [options] input_image output_image


---

## Flags

`-i`  
interactive mode (trackbars)

`-l`  
process luma only (apply contrast enhancement to HSV V channel instead of full RGB)

`--clahe`  
use CLAHE instead of global histogram equalization

`-d`  
show debug windows (interactive mode only)

---

## Parameters

`--alpha=<float>`  
fusion weight toward contrast branch (0.0–1.0, default: 0.6)

`--gain=<float>`  
unsharp gain (0.0–10.0, default: 1.2)

`--sigma=<float>`  
Gaussian blur sigma for unsharp masking (0.1–20.0, default: 5.0)

`--gamma1=<float>`  
output gamma correction (0.1–3.0, default: 1.0)

`--clip=<float>`  
CLAHE clip limit (0.1–10.0, default: 3.0)

`--tiles=<int>`  
CLAHE grid size (2–32, default: 8)

---

## Examples

Normal mode (default parameters):


./program input.jpg output.jpg


Use CLAHE and process luma only:


./program --clahe -l input.jpg output.jpg


Stronger details + more contrast branch:


./program --gain=2.0 --sigma=3.0 --alpha=0.75 input.jpg output.jpg


Interactive mode with debug windows:


./program -i -d --clahe input.jpg output.jpg


Controls in interactive mode:
- `S` to save the output image
- `ESC` to exit

---

## Output

The program displays a **side-by-side view** (input | output).  
In debug mode it also shows separate windows for:
- contrast branch result
- detail branch result

---

## Author

Kamil Staniszewski