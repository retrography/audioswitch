AudioSwitch (for OSX)
===============

A command-line utility to switch the audio source on Mac OS X.

Description
-----------

This utility for Mac OS X switches the audio source and/or adjusts volume of input/output devices.

You specify the name of the audio source, such as Built-in Digital Output, and the utility switches the source immediately without any GUI interaction.

This is a command-line utility only and has no graphical user interface.  Works with Mac OS 10.7 Lion and 10.8 Mountain Lion.

Requirements & Installation
-----------------------------------
AudioSwitch requires `cmake` to build.

    brew install --HEAD https://raw.githubusercontent.com/retrography/homebrew-tap/audioswitch.rb

Usage
-----

audioswitch [-a] [-c] [-t type] [-n] -s device_name  
or  
audioswitch -e device_id1=0.5,0.5:device_id2=0.7,0.8

 - **-a**               : Lists all devices along with their device IDs.
 - **-c**               : Shows current device.
 - **-t** _type_        : Device type (input/output/system). Defaults to output.
 - **-n**               : Cycles the audio device to the next one.
 - **-s** _device_name_ : Sets the audio device to the given device by name.
 - **-e** _device_id1_=_vol1_,_vol2_:_device_id2_=_vol1_,_vol2_ : Sets the volume of audio device given by ID, followed by volume of first and second channel respectively. Multiple device can be separated with colon.

License
-------

MIT License, see license.txt
Copyright (c) 2008-2011 Devon Weller <wellerco@gmail.com>  
Copyright (c) 2011 Christian Zuckschwerdt <zany@triq.net>  
Copyright (c) 2015 [Ziga Zupanec](https://github.com/agiz/) <ziga.zupanec@gmail.com>
Copyright (c) 2016 Mahmood S. Zargar <mahmood@gmail.com>