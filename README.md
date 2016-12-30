# Skyghost-Touch

Skyghost Touch is a free (as in freedom) retro 80s-style asteroid shooter designed for Android tablets and large Android smartphones.

It's _much_ easier to install from the .apk but if you insist on compiling from source these steps should help you:

* Install Ubuntu or similar OS on your computer
* Install Android platform 19 and the NDK
* Download SDL2, and its extensions: image, ttf, and mixer
* Download glm for C++ matrix ops
* $ git clone https://www.github.com/clockworkdevstudio/Skyghost-Touch.git Skyghost\ Touch
* (Note: filenames may vary as versions of the libs vary)
* $ tar xzf SDL2-2.0.4.tar.gz
* $ tar xzf SDL2_image-2.0.1.tar.gz
* $ tar xzf SDL2_mixer-2.0.1.tar.gz
* $ tar xzf SDL2_ttf-2.0.14.tar.gzi
* $ mv SDL2-2.0.4/ SDL
* $ mv SDL2_image-2.0.1/ SDL2_image
* $ mv SDL2_ttf-2.0.14/ SDL2_ttf
* $ mv SDL2_mixer-2.0.1/ SDL2_mixer
* $ rm SDL2-2.0.4.tar.gz
* $ rm SDL2_image-2.0.1.tar.gz
* $ rm SDL2_mixer-2.0.1.tar.gz
* $ rm SDL2_ttf-2.0.14.tar.gz
* $ mv SDL* Skyghost\ Touch/jni/
* $ unzip glm-0.9.8.1.zip
* $ cp -r glm/glm Skyghost\ Touch/jni/SDL/include/
* $ gedit Skyghost-Touch/jni/SDL2_mixer/Android.mk
* Change the first 3 "SUPPORT" lines to false
* Run the next command in the project's root directory:
* $ android update project -p . -t android-19
