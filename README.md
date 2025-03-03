# Hydrobox

Hydrobox is my latest project that I've just started working on, an interactive 2D fluid simulator based on Smoothed Particle Hydrodynamics (SPH).

## Building

I'm writing these down so I don't forget later on, I'm not yet sure if there are any additional dependencies that need to be installed

### Dependencies

- Git
- CMake version >= 3.20
- C++ compiler (GCC, Clang, MSVC, etc.)

Clone the repository with `--recurse-submodules`, then go into the directory
```
git clone --recurse-submodules https://github.com/Yilmaz4/Hydrobox.git
cd Hydrobox
```

Generate the build files with CMake and build
```
cmake -S . -B build
cmake --build build
```

You can then find the executable in the `bin` directory

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

All third-party libraries used in this project are distributed under their respective licenses, which can be found in the lib directory within each library's folder.
