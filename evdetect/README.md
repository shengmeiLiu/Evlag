# EvDetect

Detect the event number for particular device.

## Usage

```
  usage: evdetect.sh [-qh] {str}
         str - device string to match
         -q quiet
         -h this help message
  example: evdetect.sh -q "GIGABYTE USB-HID Keyboard"
```

## Details

EvDetect parses `/proc/bus/input/devices` and searches the device
names for a match to the indicated string (`str`).  Since the devices
are presented sequentially, the first line that matches the string
(minus one) is returned as the event number.  The full path (in
`/dev/input`) is displayed as output.

Example (quiet):
```
% evdetect.sh -q "GIGABYTE USB-HID Keyboard"
/dev/input/event11
```

Example (verbose):

```
Option: verbose
Device: 'GIGABYTE USB-HID Keyboard'
Names:
     1	N: Name="Lid Switch"
     2	N: Name="Sleep Button"
     3	N: Name="Sleep Button"
     4	N: Name="Power Button"
     5	N: Name="Power Button"
     6	N: Name="AT Translated Set 2 keyboard"
     7	N: Name="ETPS/2 Elantech Touchpad"
     8	N: Name="Logitech Gaming Mouse G502"
     9	N: Name="Logitech Gaming Mouse G502 Keyboard"
    10	N: Name="Logitech Gaming Mouse G502 Consumer Control"
    11	N: Name="Logitech Gaming Mouse G502 System Control"
    12	N: Name="GIGABYTE USB-HID Keyboard"
    13	N: Name="GIGABYTE USB-HID Keyboard Mouse"
    14	N: Name="GIGABYTE USB-HID Keyboard System Control"
    15	N: Name="GIGABYTE USB-HID Keyboard Consumer Control"
    16	N: Name="GIGABYTE USB-HID Keyboard"
    17	N: Name="GIGABYTE USB-HID Keyboard"
    18	N: Name="GIGABYTE USB-HID Keyboard Wireless Radio Control"
    19	N: Name="USB2.0 HD UVC WebCam: USB2.0 HD"
    20	N: Name="Video Bus"
    21	N: Name="Video Bus"
    22	N: Name="HDA Intel PCH Headphone"
    23	N: Name="HDA Intel PCH HDMI/DP,pcm=3"
    24	N: Name="HDA Intel PCH HDMI/DP,pcm=7"
    25	N: Name="HDA Intel PCH HDMI/DP,pcm=8"
    26	N: Name="HDA Intel PCH HDMI/DP,pcm=9"
    27	N: Name="HDA Intel PCH HDMI/DP,pcm=10"
    28	N: Name="HDA NVidia HDMI/DP,pcm=3"
    29	N: Name="HDA NVidia HDMI/DP,pcm=7"
Match:      12	N: Name="GIGABYTE USB-HID Keyboard"
Event: 11
/dev/input/event11
``

## Files

This folder includes two sample files:

+ `evtest.sample` - a list of devices as reported by `evtest` on an
  example Linux laptop

+ `devices.sample` - the contents of `/proc/bus/input/devices` on that
  same laptop


--------------------------

Happy detecting!

-- Mark


