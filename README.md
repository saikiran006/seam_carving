# Implementation of Seam Carving for image resizing.

## Description
- Seam Carving algorithm resizes an image by preserving the important contents in the original image. 
- To decrease the size of the image height or width a horizontal or vertical seam with the lowest energy is calculated and removed by shifting the pixels in the image. This process is repeated until the height or width of the image reaches the desired height or width.
- To increase the size of the image height or width by k pixels, k seams with the least energy are calculated and for each pixel in the k seams, it is averaged with its right pixel and duplicated. 
- To calculate the energy of a pixel we use the following formula

    - Energy(i, j) = sqrt((Gradient_x(i, j)^2) + (Gradient_y(i, j)^2))

- After calculating the energy for each pixel in the image, we use dynamic programming to calculate the cumulative minimum energy for each pixel. This allows us to identify the seam (a connected path of low energy pixels from top to bottom or left to right) with the lowest energy. Here is a step-by-step outline of the process:
    - ### Intitialization 
        - Create a 2D array M of the same size as the image to store the cumulative minimum energy.
        - Initialize the first row of M with the corresponding energy values from the energy matrix.
    - ### Cumulative minimum energy calculation
        - For each pixel (i, j) in the image (excluding the first row), compute the cumulative minimum energy using the values from the previous row.
        - The value of M(i, j) is given by:
        M(i,j)=Energy(i,j)+min(M(i−1,j−1),M(i−1,j),M(i−1,j+1)) 
        
    - ### Seam Identification 
        - Start from the pixel in the last row with the smallest cumulative energy.
        - Trace back to the first row by always moving to the pixel with the smallest cumulative energy among the three possible pixels above.

## Requirements
 - OpenCV
 - GCC Compiler

## Compilation 
```sh
- g++ -o seam_carving.out seam_carving.cpp `pkg-config --cflags --libs opencv4`
```