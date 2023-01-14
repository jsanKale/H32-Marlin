# Marlin-H32
This is a build of a more recent release of Marlin for H32-based 3D printers, predominantly used by the Voxelab Aquila X2.

The H32, specifically the Huada Semiconductor (HDSC) HC32F460PETB, is built on the ARM Cortex-M4 platform, and is mostly compatible with the STM32F1. Disappointingly, this chipset relies on an SDK that HDSC only provides to direct customers of its chips, so it is not officially supported by the open source PlatformIO build system used by the Marlin project. Thanks to Voxelab’s Marlin source release, and alexqzd’s initial work, we have a working (if a little clunky) environment to build Marlin for H32.

On the other hand, as of 2.0.9.3 Marlin now officially supports the full-color LCDs used by the Ender 3 v2 and Aquila X2, so fewer patches are required to get Marlin working on these printers.

**Current version:** Marlin 2.1.1 (kirb/Marlin-H32 rev. 2)

## Current status
* No known issues

## Disclaimer
**Install at your own risk.** I’m not responsible if your prints come out terrible, or turn into a pile of spaghetti, or your printer catches fire, or whatever else.

## Installation
### Which build do I want?
There are several builds available. Which one you use depends on your configuration.

* **ManualMesh:** Recommended if you have a stock Aquila printer. Provides a built-in tool for manually creating a mesh of the bed for higher print accuracy.
* **BLTouch:** If your printer has a BLTouch probe installed, use this build.
  * **HS:** This moves the Z axis faster while leveling, but presumably can lead to reduced accuracy.
* **NoProbe:** Provides no bed leveling features at all. This matches Voxelab’s official build configuration, but is not recommended.
* **UBL:** These builds enable Unified Bed Leveling. UBL takes significantly longer to create a mesh, but this leads to much higher accuracy, and doesn’t need to be redone as often as bilinear bed leveling.

The numeric portion of each firmware filename is the leveling mesh size. If you’re looking for the simplest and fastest option, use 3×3. If you’re looking for the best accuracy, at the cost of bed leveling taking much longer, use 9×9.

### Main firmware
1. Download the appropriate firmware for your machine configuration from [releases](https://github.com/kirb/Marlin-H32/releases)
2. In the root of your SD card, create the folder **firmware**. Paste the **.bin** file into this folder.
3. Turn off the printer, insert the SD card, and turn it back on. You should see a red bar appear on the LCD, then the printer will boot the new firmware.
4. If you’re on a Mac, remove the SD card, insert it into your PC again, and delete the firmware folder. If you don’t do this, hidden files created by Finder will be incorrectly detected as a firmware update the next time you turn on the printer. (If this happens to you, just follow these steps again.)

### LCD firmware
You shouldn’t need to do this with every firmware update. If you have weird issues with the LCD, you probably need to update its firmware.

1. If you’re not sure how your SD card is formatted, format it as FAT32 with cluster size set to 4096 (4K). The display is more fussy about this than the main firmware.
2. Download the display firmware (DWIN_SET) from the [Marlin configurations repo](https://github.com/MarlinFirmware/Configurations/archive/refs/heads/release-2.1.1.zip).
3. Extract the zip file, and navigate to **config/examples/Creality/Ender-3 V2**. Copy **DWIN_SET** to the root of your SD card.
5. With the printer off, unscrew the 4 screws on the rear of the display. Carefully open the back, ideally using a plastic pry tool to not damage the plastic.
6. Insert the SD card into the slot found on the LCD board.
7. Turn the printer on. The LCD will display blue if an update was successfully recognised. It will switch to red upon success.
8. Turn the printer off, remove the SD card, and turn it back on.
# H32-Marlin
# H32-Marlin
