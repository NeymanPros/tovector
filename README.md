**Tovector**
---
**Description**

Console app, uses OpenCV library. Takes a path to some image and creates a .svg description of its contours. Resulting image can contain lines and ellipse arcs. 

---
**Usage**

After installing the files, replace the path inside CMakeFiles.txt, leading to Opencv with the coorect one.

The app can be launched using cmake + make, 
```cd /path/to/project/
mkdir ./build
cd ./build
cmake ../
make
./to_vector /path/to/image
```

