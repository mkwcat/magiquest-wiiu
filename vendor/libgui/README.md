[![Publish Docker Image](https://github.com/wiiu-env/libgui/actions/workflows/push_image.yml/badge.svg)](https://github.com/wiiu-env/libgui/actions/workflows/push_image.yml)

# libgui
[![Build Status](https://travis-ci.org/Maschell/libgui.svg?branch=wut)](https://travis-ci.org/Maschell/libgui/tree/wut)  

## Usage
Following steps are required for initialization:
```C
libgui_memoryInitialize();			// Initialize memory management

//DO GUI STUFF HERE!

libgui_memoryRelease();
```

Link the application with:
```Makefile
-lgui -lfreetype -lgd -lpng -ljpeg -lz -lmad -lvorbisidec -logg -lbz2

```

You also need to add the freetype2  nclude path to your Makefile. Example:

```Makefile
-I$(PORTLIBS_PATH)/ppc/include/freetype2
```

TODO: provide more information

## Dependencies
To be able to use libgui, you need to install the following dependencies:

- [wut](https://github.com/devkitPro/wut/)
- Install the required portlibs via `(dkp-)pacman -Syu ppc-zlib ppc-libmad ppc-libogg ppc-libgd ppc-freetype ppc-libjpeg-turbo ppc-libpng ppc-libvorbisidec ppc-glm ppc-bzip2`

## Use this lib in Dockerfiles.
A prebuilt version of this lib can found on dockerhub. To use it for your projects, add this to your Dockerfile.
```
[...]
COPY --from=wiiuenv/libgui:[tag] /artifacts $DEVKITPRO
[...]
```
Replace [tag] with a tag you want to use, a list of tags can be found [here](https://hub.docker.com/r/wiiuenv/libgui/tags). 
It's highly recommended to pin the version to the **latest date** instead of using `latest`.

## Format the code via docker

`docker run --rm -v ${PWD}:/src wiiuenv/clang-format:13.0.0-2 -r ./source ./include -i`

# Credits
- Orignally based on https://github.com/dborth/libwiigui
- Wii U port / modification / new functions / sound / much more by dimok.
- Minor changes by Maschell
