# Installing Xilinx XACT 5 inside DOSBox

The following steps describe how to install and run the old DOS-based Xilinx XACT 5.0 tools on a modern PC using DOSBox. This tutorial assumes the use of DOSBox 0.74-3 on Ubuntu 22.04 LTS.

Note that the `dosbox/c` directory in this repository already contains a pre-installed copy of the DOSBox C: drive with XACT and OrCAD pre-installed. There is no need to follow this tutorial again, unless you would like to install everything from scratch.

*NOTE: Xilinx XACT 5.0.0 and OrCAD 4.4 are proprietary software, but they have been discontinued and unsupported for decades and are now considered [abandonware](https://en.wikipedia.org/wiki/Abandonware). They are preserved here for historical/educational fair use, so that future generations can experience what it was like to design for the very first FPGAs.*

## Part I: Install and configure DOSBox

1. Install DOSBox via: `sudo apt-get install dosbox`
2. After installing, note that a default config file is created `~/.dosbox/dosbox-0.74-3.conf`
3. We will use folder `~/workspace/xc2064-clock/dosbox/c` to hold the contents of the virtual C: hard drive.
4. In `~/.dosbox/dosbox-0.74-3.conf`, after the `[autoexec]` section, add the line `mount c ~/workspace/xc2064-clock/dosbox/c`. This will auto-mount the virtual C: hard drive each time you start DOSBox.
5. In `~/.dosbox/dosbox-0.74-3.conf`, edit `machine=svga_s3` to `machine=svga_paradise`. This will allow us to use 800x600 resolution for OrCAD and XACT applications.

## Part II: Install OrCAD 4.4

Before installing Xilinx XACT, we need to install OrCAD to use as a schematic editor. (After some trial-and-error, it appears OrCAD needs to be installed *before* Xilinx XACT.)

The OrCAD 4.4 installer was originally found here:<br>
https://archive.org/details/1988-1996-orcad-100-701<br>
https://archive.org/download/1988-1996-orcad-100-701/1991_1994_orcad_42_44.7z<br>
MD5: `37ac2af0c616ec3a569a43fcd7d28731`<br>

1. Create a folder on the host at somewhere like `~/workspace/xc2064-clock/dosbox/a`. This directory will act as our virtual A: floppy drive. We will repeatedly change the contents of this folder to provide a new "floppy" to the DOS environment.
2. Copy contents of `1991_1994_orcad_42_44/1994_44/install/disk1` to `~/workspace/xc2064-clock/dosbox/a`.
3. Inside DOSBox, run `z:/mount a ~/workspace/xc2064-clock/dosbox/a`, then do `A:` to go to the virtual floppy, and run `install` to start the OrCAD installer.
4. At the prompt `Do you want to customize installation settings? NO`, press ENTER to go with the defaults.
5. At the prompt for video card type, use the spacebar to select `YES` for the types `VGA` and `Paradise Systems`, then press ENTER.
6. At the prompt for plotter type, select none, then press ENTER.
7. At the prompt for printer type, select none, then press ENTER.
8. The installer will ask if it can modify your `AUTOEXEC.BAT`. Press `Y` to allow this.
9. Next, the installer will bring up a `Welcome to OrCAD's product installation program...` screen. After this screen, it will show a list of tool sets to install. We need `OrCAD ESP Design Environment` and `OrCAD Schematic Design Tools`, use the spacebar to select `YES` for both and press ENTER to continue.
10. It will next ask you to place "ESP Disk" in drive A. Delete the existing contents of `~/workspace/xc2064-clock/dosbox/a` and replace them with the contents of `1991_1994_orcad_42_44/1994_44/sdt/disk1/`. Press CTRL-F4 to ask DOSBox to refresh the contents of the floppy.
11. It will ask `Is this a new installation of the ESP design environment?`, press `Y`, then press ENTER.
12. It will then ask for "SDT Disk 1", do the same thing using: `1991_1994_orcad_42_44/1994_44/sdt/disk1/`
13. It will then ask for "SDT Disk 2", do the same thing using: `1991_1994_orcad_42_44/1994_44/sdt/disk2/`
14. It will then ask for "SDT Disk 3", do the same thing using: `1991_1994_orcad_42_44/1994_44/sdt/disk3/`
15. It will then ask for "SDT Disk 4, do the same thing using: `1991_1994_orcad_42_44/1994_44/sdt/disk4/`, but note that Disk 4 is missing the `disk.id` file! Copy the `disk.id` file from the Disk 3 contents, and replace `Disk 3 of 5` line with `Disk 4 of 5`, then continue.
16. It will then ask for "SDT Disk 5, do the same thing using: `1991_1994_orcad_42_44/1994_44/sdt/disk5/`
17. After the installation exits, it will complain about about "Not enough environment space left." It seems like this error can be safely ignored.
18. Add the lines `C:` and `AUTOEXEC.BAT` to the end of `~/.dosbox/dosbox-0.74-3.conf` and restart DOSBox.
19. Next, we will create a driver file for OrCAD to use 800x600 resolution. Do `cd C:\ORCADESP\DRV\`, then run `GENDRIVE`, select `41: Paradise EGA or VGA 800x600 16 color`, then select `U - Use Settings above` and enter the name `PARADISE.DRV`.
20. Modify `~/workspace/xc2064-clock/dosbox/c/ORCAD/TEMPLATE/SDT.CFG` to `DD = 'PARADISE.DRV'`. (This is the template file for new OrCAD SDT projects.)

## Part III: Install Xilinx XACT 5.0.0

Next, we will install Xilinx XACT.

The Xilinx XACT 5.0.0 installer was originally found here:<br>
http://bitsavers.org/components/xilinx/xact/XACT.ver.5.0.0.English.zip<br>
Original ZIP MD5: `bd286a6e0f25ca882c297f44becca314`<br>
Recompressed 7Z MD5: `a818865cc3a22254dee3f2b023944b72`<br>

1. Delete existing contents of `~/workspace/xc2064-clock/dosbox/a` and replace with the contents of the archive above.
2. Run `Z:\mount a ~/workspace/xc2064-clock/dosbox/a` then do `A:` to go the virtual floppy.
3. Run `INSTALL` to start the XACT installer. It will wait at `Copying; Please wait...` for a minute or two.
4. It will then bring up a "Xilinx Installation Program" GUI page. (Note that the installer will grab the mouse from the host. To return the mouse back to the host, use CTRL-F10.)
5. Click "ACCEPT", then select "CUSTOM" install.
6. Under "Packages", select "ORCAD STD PKG", the continue through the installation prompts.
7. At the end of the installation, accept the changes to `AUTOEXEC.BAT`.
8. Next, we need to patch the XACT executables so they will work without a license. Copy the contents of `XACT.ver.5.0.0.English/PATCHES/*.EXE` over the existing executables in `~/workspace/xc2064-clock/dosbox/c/XACT`.
9. Add `SET GRMODE=VGAP16` to the end of `~/workspace/xc2064-clock/dosbox/c/AUTOEXEC.BAT`. This tells XACT programs to run at 800x600 with 16 colors.
10. Restart DOSBox.

You now have Xilinx XACT 5.0.0 and OrCAD 4.4 running inside DOSBox :)
