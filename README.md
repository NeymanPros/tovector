**Tovector**
---
**Description**

Console app, uses OpenCV library. Takes a path to some image and creates a .svg description of its contours. Resulting image can contain lines and ellipse arcs. 

The program binarized colors, extracts the contours, turns them into split vectors which desribe the contours using coordinates, and finally looks for patters inside a vector to turn it into arcs and lines, which are understandable by svg format.

Formulas for arcs are mathimatically correct, and calculated using first and 2 last points in a sequence. 

---

**Testing**

There is a testing tool which creates images from each step, and you can track if each transform is correct. It gets built along with the main project, and works with the 2 images in /test file.

---
**Usage**

The app can be launched using cmake + make, 
```cd /path/to/project/
mkdir ./build
cd ./build
cmake ../
make
./to_vector /path/to/image
```

