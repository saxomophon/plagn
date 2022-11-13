$buildPathDebug = ".\build-win64-debug"
If(!(test-path -PathType container $buildPathDebug))
{
    Write-Output "Creating debug build directory..."
    New-Item -Path $buildPathDebug -ItemType "directory"
}

$buildPathRelease = ".\build-win64-release"
If(!(test-path -PathType container $buildPathRelease))
{
    Write-Output "Creating release build directory..."
    New-Item -Path $buildPathRelease -ItemType "directory"
}

$vcpkgPath = ".\vcpkg\vcpkg.exe"
If(!(test-path -PathType leaf $vcpkgPath))
{
    Write-Output "Cloning vcpkg.git..."
    git clone https://github.com/Microsoft/vcpkg.git
    Write-Output "Bootstrapping vcpkg..."
    .\vcpkg\bootstrap-vcpkg.bat -disableMetrics
}

Write-Output "Running vcpkg install..."
.\vcpkg\vcpkg.exe install
