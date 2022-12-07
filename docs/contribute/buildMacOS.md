## Install XCode

If you haven't already, please go to the AppStore and install XCode.

Afterwards you'll need to install the command line tools by opening the terminal and running

`xcode-select --install`

## Install cmake

We are using CMake for our build chain. The easiest way is to install [homebrew](https://brew.sh/) first. Because then you just need to run

`brew install cmake`

from terminal.

## Install Ninja

Ninja is a slightly faster builder/linker. You don't need to, but will have more of an out-of-the-box experience, should you install it. Easiest way to install is by running

`brew install ninja`

from terminal.

## Install pkg-config

The pkg-config program is used to retrieve information about installed libraries in the system. It is typically used to compile and link against one or more libraries. You'll need this tool for building the dependencies downloaded from VCPKG.

`brew install pkg-config`

from terminal.

## Install Visual Studio Code

We are using Visual Studio Code to collaborate, as it has some nice features for exactly that. So we suggest, you'll use it, too. You don't have to, of course.

If you know, what you're doing and don't like Visual Studio Code: Use whatever tool chain you prefer. Just make sure to exclude the results of your tool chain by adapting your `.gitignore` accordingly.

Everyone else may go to [the download page of Visual Studio Code](https://code.visualstudio.com/Download) and proceed with the typical installation processs.

## Prepare dependencies

Open you terminal in your Plag'n root directory.

Run `bootstrap.sh`.

***OR***

1. Go to [vcpkg.io/en/getting-started.html](https://vcpkg.io/en/getting-started.html) and follow the instructions to install vcpkg.
0. In a terminal navigate to your plagn root directory and run `[pathToVcpkgExecutable] install`. This will install the dependencies listed in `vcpkg.json`.
0. Use the vscode tasks to configure and build Plag'n. They may also inspire you about how to configure and build from console.

## Test your setup

If you did, as we suggested: Open Visual Studio Code.

Then open the `plagn.code-workspace` file in Plag'n's root directory.

Once everything is loaded, you should be able to run a build task by pressing

`Cmd + Shift + B`

which will execute the debug build. Or you select a build task by going

`Terminal -> Run Task...`

in the menu bar and selecting your target task.

Either be happy or trouble shoot.
