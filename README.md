# ants
A simple visualisation of ant colony using a compute shader

Here are a few snapshots using different parameters in the shader:
![wind](https://user-images.githubusercontent.com/38104744/220429531-c92ae15b-c5bd-40eb-bd8e-85dd4be89cbd.jpg)
![wide](https://user-images.githubusercontent.com/38104744/220429542-d0a2745f-84ab-4ee4-8e67-3bc3500d779d.jpg)
![slim](https://user-images.githubusercontent.com/38104744/220429551-ceda90d6-f968-4853-adf5-bb0743137e43.jpg)
![layered](https://user-images.githubusercontent.com/38104744/220429559-7c42b18e-6bc0-4feb-8f6c-daed18ec2937.jpg)
![fromImage](https://user-images.githubusercontent.com/38104744/220429563-2cf410c5-b328-4ab0-b38f-bba3f5433ced.jpg)

The last one uses a image in a .bmp format and using black pixels in the image sets anchors for the ants, 
so the resulting visualisation looks somewhat like the image.


# Build
The code build only on windows OS and uses the compiler from Visual Studio. 
To build the code set correct path in shell.bat, then run the shell script from 
command line, and after that the build script. It should produce a build folder
and in it a executable.

# Run
After running the executable, by pressing the Alt key the simulation starts.
