# Famicom Keyboard USB Adapter
The Family Computer (Famicom) Keyboard to USB HID Adapter using a Raspberry Pi RP2040.




## Wiring Diagram

| RP2040 | DA15 | Name | Function        |
| ------ | ---- | ---- | --------------- |
|      1 |   12 | OUT0 | Row Reset       |
|      2 |   10 | OUT2 | Keyboard Enable |  
|      3 |   11 | OUT1 | Column Select   |
|      5 |   07 | J2D1 | Data Pin 1      |
|      6 |   06 | J2D2 | Data Pin 2      |
|      7 |   05 | J2D3 | Data Pin 3      |
|      8 |   04 | J2D4 | Data Pin 4      |
|    GND |   01 |  GND | Ground          |
|    +3V |   15 |  +5V | Power           |




## Famicom Keyboard Port Pinout

Source: https://www.nesdev.org/wiki/Expansion_port
```
      Female DA-15 on peripheral
                 /\
                |   \
         Gnd -- | 01  \
                |   09 | -> /OE for joypad 2 ($4017 read strobe)
       SOUND <- | 02   |
                |   10 | -> OUT2 ($4016 write data, bit 2)
        /IRQ ?? | 03   |
                |   11 | -> OUT1 ($4016 write data, bit 1)
joypad 2 /D4 -> | 04   |
                |   12 | -> OUT0 ($4016 write data, bit 0, strobe on pads)
joypad 2 /D3 -> | 05   |
                |   13 | <- joypad 1 /D1
joypad 2 /D2 -> | 06   |
                |   14 | -> /OE for joypad 1 ($4016 read strobe)
joypad 2 /D1 -> | 07   |
                |   15 | -- +5V
joypad 2 /D0 ?? | 08  /
                |   /
                 \/
```




## Notes

The Famicom Keyboard was designed to work with the Famicom's 5V power and signalling. The console is comprised of just 3 ICs (Integrated Circuits / Chips): a 4017 *(Counter/Divider)*, a 4019 *(Quad AND/OR Select Gate)*, and a 4069 *(Hex Inverter)*. Each of these chips are designed to work at 5V, but have a minimum supply of 3V, the same as our RP2040.

* [Keyboard Schematic](https://ia800502.us.archive.org/5/items/fc-factory-cpu-schematic-scan/FC%20Family%20Basic%20Keyboard%20and%20Sound%20Circuit%20Schematic.png)
* [Keyboard Matrix](https://ia800502.us.archive.org/5/items/fc-factory-cpu-schematic-scan/FC%20Keyboard%20Key%20Schematic.png)
* [4017 Datasheet](https://www.ti.com/lit/ds/symlink/cd4017b.pdf)
* [4019 Datasheet](https://www.ti.com/lit/ds/symlink/cd4019b.pdf)
* [4069 Datasheet](https://www.ti.com/lit/ds/symlink/cd4069ub.pdf)
