# FRL New
A reprogrammed version of FRL (firestorm robotics library).
# Not complete!
It is still under development.
# How to use
Download use.py in the same directory as your robot project and add this to the end of build.gradle:
```
task loadFirestormRoboticsLibrary(type: Exec) {
    commandLine 'python', 'use.py'
}

build.dependsOn loadFirestormRoboticsLibrary
```
You may need to build it once just so it can install (it's a bug in Gradle).
Requires internet connection to install.