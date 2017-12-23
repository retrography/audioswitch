/*
 *  interface.h
 *  AudioSwitch
 *

Copyright (c) 2008 Devon Weller <wellerco@gmail.com>
Copyright (c) 2015 Ziga Zupanec <ziga.zupanec@gmail.com>
Copyright (c) 2016 Mahmood S. Zargar <mahmood@gmail.com>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

*/

#include <CoreServices/CoreServices.h>
#include <CoreAudio/CoreAudio.h>
#include "device.h"

#ifndef __INTERFACE_H_INCLUDED__   // if x.h hasn't been included yet...
#define __INTERFACE_H_INCLUDED__   //   #define this so the compiler knows it has been included

#define VOL_ARG_LEN 1024
#define VOL_DELIMITERS "=,:"

void printProperties(AudioDeviceID deviceID, ASDeviceType device_type, ASOutputFormat outputFormat);

void showUsage(const char *appName);

int runAudioSwitch(int argc, const char *argv[]);

#endif // __INTERFACE_H_INCLUDED__
