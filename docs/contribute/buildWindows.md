## Load and install MSVC Commandline Compiler

[visualstudio.microsoft.com/de/downloads/](https://visualstudio.microsoft.com/de/downloads/) -> Buildtools für Visual Studio 2019

Install Workloads:
* C++ build tools
* Universal Windows Plattform Buildtools
* add C++ CMake Tools for Windows 

Note: If you want to upgrade from a previous version: The "Visual Studio Installer" can update your build tools to the latest version.


## Load and install git

[gitforwindows.org/](https://gitforwindows.org/)


## Prepare dependencies

Run `bootstrap.ps1`. (Work in progress...)

***OR***

1. Go to [vcpkg.io/en/getting-started.html](https://vcpkg.io/en/getting-started.html) and follow the instructions to install vcpkg.
0. In a terminal navigate to your plagn root directory and run `[pathToVcpkgExecutable] install`. This will install the dependencies listed in `vcpkg.json`.
0. Use the vscode tasks to configure and build Plag'n. They may also inspire you about how to configure and build from console.
