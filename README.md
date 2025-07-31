# MagiQuest Wii U

Recreations of MagiQuest Legacy encounters, including the never released Xavier encounter, on the Wii U. The following encounters are included:
- Dragon / Charlock
- Goblin King (does not contain the guard or princess... yet)
- Ice Dragon / Winterra
- Silver Dragon
- Heroic Dragon (unfinished, contains an edited clip)
- Xavier
- Golem / Kalo (from MagiQuest Online)

Assets source: [The MagiQuest Archive Rev0 on archive.org](https://archive.org/details/magiquestarchive)

## Building

To compile you'll need:

- devkitPPC (with the DEVKITPPC environment variable set)
- [wut](https://github.com/devkitPro/wut/)
- Install the required portlibs via `(dkp-)pacman -Syu ppc-zlib ppc-libogg ppc-libgd ppc-freetype ppc-libjpeg-turbo ppc-libpng ppc-glm ppc-bzip2`

After installing the prerequisites, simply run `make`. The output executable (rpx) will be in the created `code` directory. To package into a wuhb file, run `make wuhb`.

## License

This software is licensed under the GNU General Public License version 2 (or any
later version). The full license can be found in the LICENSE file.
