# Mirza's Model Viewer 

## Build

```shell

# Windows
vcpkg/vcpkg.exe install --triplet x64-windows
cmake --preset=default # or release for release version
cmake --build build
```

## Usage

1. Download [glTF-Sample-Assets](https://github.com/KhronosGroup/glTF-Sample-Assets)
1. Find a sample like DamagedHelment, and copy the path to the gltf file.
    1. DamagedHelmet: `C:\Tools\glTF-Sample-Assets\Models\DamagedHelmet\glTF\DamagedHelmet.gltf`
    1. FlightHelmet: `C:\Tools\glTF-Sample-Assets\Models\FlightHelmet\glTF\FlightHelmet.gltf`
1. Run `ogl-pbr-viewer.exe`
1. Paste the path in the text box and hit load.

## Screenshots
![Screenshot](screenshots/damaged-helmet.png?raw=true "Damaged Helmet")
