#mkdir debug dir
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    if [ -d "./build-linux-debug" ]; then
        echo "debug directory exists."
    else
        echo "creating debug build directory..."
        mkdir ./build-linux-debug
    fi
elif [[ "$OSTYPE" == "darwin"* ]]; then
    if [ -d "./build-macOS-debug" ]; then
        echo "debug directory exists."
    else
        echo "creating debug build directory..."
        mkdir ./build-macOS-debug
    fi
else
    echo "Your OS is not yet supported. Sorry."
    exit 0
fi

#mkdir release dir
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    if [ -d "./build-linux-release" ]; then
        echo "release directory exists."
    else
        echo "creating release build directory..."
        mkdir ./build-linux-release
    fi
elif [[ "$OSTYPE" == "darwin"* ]]; then
    if [ -d "./build-macOS-release" ]; then
        echo "release directory exists."
    else
        echo "creating debug release directory..."
        mkdir ./build-macOS-release
    fi
else
    echo "Did you change OS while the script was running?"
    exit 0
fi

#install vcpkg inside plagn
if [[ "$OSTYPE" == "linux-gnu"* ]] || [[ "$OSTYPE" == "darwin"* ]]; then
    if [ -d "./vcpkg" ]; then
        echo "Updating vcpkg..."
        cd ./vcpkg
        git pull
        cd ..
    else
        echo "Cloning vcpkg..."
        git clone https://github.com/Microsoft/vcpkg.git
    fi
    echo "Installing vcpkg..."
    ./vcpkg/bootstrap-vcpkg.sh --disableMetrics
else
    echo "Did you change OS while the script was running?"
    exit 0
fi

echo "Done."