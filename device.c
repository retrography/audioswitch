/*
 *  device.h
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

#include "device.h"
#include "interface.h"

AudioDeviceID getCurrentlySelectedDeviceID(ASDeviceType typeRequested) {
    UInt32 propertySize;
    AudioDeviceID deviceID = kAudioDeviceUnknown;

    AudioObjectPropertyAddress pa;
    pa.mScope = kAudioObjectPropertyScopeGlobal;
    pa.mElement = kAudioObjectPropertyElementMaster;

    // get the default output device
    propertySize = sizeof(deviceID);
    switch (typeRequested) {
        case kAudioTypeInput:
            pa.mSelector = kAudioHardwarePropertyDefaultInputDevice;
            break;
        case kAudioTypeOutput:
            pa.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
            break;
        case kAudioTypeSystemOutput:
            pa.mSelector = kAudioHardwarePropertyDefaultSystemOutputDevice;
            break;
        default:
            break;
    }
    AudioObjectGetPropertyData(kAudioObjectSystemObject, &pa, 0, NULL, &propertySize, &deviceID);
    return deviceID;
}

void getDeviceName(AudioDeviceID deviceID, char *deviceName) {
    UInt32 propertySize = 256;
    AudioObjectPropertyAddress pa;
    pa.mSelector = kAudioDevicePropertyDeviceName;
    pa.mScope = kAudioObjectPropertyScopeGlobal;
    pa.mElement = kAudioObjectPropertyElementMaster;

    AudioObjectGetPropertyData(deviceID, &pa, 0, NULL, &propertySize, deviceName);
}

void getDeviceVolume(AudioDeviceID deviceID, float *vol_left, float *vol_right) {
    OSStatus err;
    UInt32 size;
    UInt32 channel[2];

    AudioObjectPropertyAddress pa;
    pa.mSelector = kAudioDevicePropertyPreferredChannelsForStereo;
    pa.mScope = kAudioDevicePropertyScopeOutput;
    pa.mElement = kAudioObjectPropertyElementMaster;

    size = sizeof(channel);
    err = AudioObjectGetPropertyData(deviceID, &pa, 0, NULL, &size, &channel);
    if (err != noErr) {
        return;
    }

    *vol_right = -1.0;
    *vol_left = -1.0;
    pa.mSelector = kAudioDevicePropertyVolumeScalar;
    size = sizeof(float);

    pa.mElement = channel[0];
    err = AudioObjectGetPropertyData(deviceID, &pa, 0, NULL, &size, vol_left);

    pa.mElement = channel[1];
    err |= AudioObjectGetPropertyData(deviceID, &pa, 0, NULL, &size, vol_right);

    if (err != noErr) {
        return;
    }
}

void getStreamSampleRate(AudioDeviceID deviceID, Float64 *sampleRate) {
    OSStatus err;
    UInt32 size;
    AudioStreamBasicDescription format;
    size = sizeof(AudioStreamBasicDescription);

    AudioObjectPropertyAddress pa;
    pa.mSelector = kAudioDevicePropertyStreamFormat;
    pa.mScope = kAudioDevicePropertyScopeOutput;
    pa.mElement = kAudioObjectPropertyElementMaster;

    err = AudioObjectGetPropertyData(deviceID, &pa, 0, NULL, &size, &format);
    if (err != noErr) {
        return;
    }
    *sampleRate = format.mSampleRate;
}

void getStreamBitDepth(AudioDeviceID deviceID, Float64 *bitDepth) {
    OSStatus err;
    UInt32 size = sizeof(AudioStreamBasicDescription);
    AudioStreamBasicDescription format;

    AudioObjectPropertyAddress pa;
    pa.mSelector = kAudioDevicePropertyStreamFormat;
    pa.mScope = kAudioDevicePropertyScopeOutput;
    pa.mElement = kAudioObjectPropertyElementMaster;

    err = AudioObjectGetPropertyData(deviceID, &pa, 0, NULL, &size, &format);
    if (err != noErr) {
        return;
    }
    *bitDepth = format.mBitsPerChannel;
}

void setSampleRate(AudioDeviceID deviceID, Float64 sampleRate) {
    OSStatus err;
    UInt32 size;

    AudioObjectPropertyAddress pa;
    pa.mSelector = kAudioDevicePropertyNominalSampleRate;
    pa.mScope = kAudioDevicePropertyScopeOutput;
    pa.mElement = kAudioObjectPropertyElementMaster;

    size = sizeof(Float64);
    err = AudioObjectSetPropertyData(deviceID, &pa, 0, NULL, size, &sampleRate);
    if (err != noErr) {
        return;
    }
}

void setDeviceVolume(AudioDeviceID deviceID, float vol_left, float vol_right) {
    OSStatus err;
    UInt32 size;
    UInt32 channel[2];

    AudioObjectPropertyAddress pa;
    pa.mSelector = kAudioDevicePropertyPreferredChannelsForStereo;
    pa.mScope = kAudioDevicePropertyScopeOutput;
    pa.mElement = kAudioObjectPropertyElementMaster;

    size = sizeof(channel);
    err = AudioObjectGetPropertyData(deviceID, &pa, 0, NULL, &size, &channel);
    if (err != noErr) {
        return;
    }

    pa.mSelector = kAudioDevicePropertyVolumeScalar;
    size = sizeof(Float32);

    pa.mElement = channel[0];
    err = AudioObjectSetPropertyData(deviceID, &pa, 0, NULL, size, &vol_left);

    pa.mElement = channel[1];
    err |= AudioObjectSetPropertyData(deviceID, &pa, 0, NULL, size, &vol_right);

    if (err != noErr) {
        printf("error setting volume of channel %u or %u\n", (unsigned int) channel[0], (unsigned int) channel[1]);
        return;
    }
}

typedef UInt8 CAAudioHardwareDeviceSectionID;
#define  kAudioDeviceSectionGlobal  ((CAAudioHardwareDeviceSectionID)0x00)

void getAggregateDeviceSubDeviceList(AudioDeviceID deviceID, AudioObjectID *sub_device, UInt32 *outSize) {
    OSStatus err;

    AudioObjectPropertyAddress pa;
    pa.mSelector = kAudioAggregateDevicePropertyActiveSubDeviceList;
    pa.mScope = kAudioObjectPropertyScopeGlobal;
    pa.mElement = kAudioObjectPropertyElementMaster;

    err = AudioObjectGetPropertyData(deviceID, &pa, 0, NULL, outSize, sub_device);
    if (err != noErr) {
        printf("error getting subdevice list of an aggregate device - %u\n", (unsigned int) deviceID);
        return;
    }
}

// returns kAudioTypeInput or kAudioTypeOutput
ASDeviceType getDeviceType(AudioDeviceID deviceID) {
    UInt32 propertySize = 256;

    AudioObjectPropertyAddress pa;
    pa.mSelector = kAudioDevicePropertyStreams;
    pa.mElement = kAudioObjectPropertyElementMaster;

    // if there are any output streams, then it is an output
    pa.mScope = kAudioDevicePropertyScopeOutput;
    AudioObjectGetPropertyDataSize(deviceID, &pa, 0, NULL, &propertySize);
    if (propertySize > 0) return kAudioTypeOutput;

    // if there are any input streams, then it is an input
    pa.mScope = kAudioDevicePropertyScopeInput;
    AudioObjectGetPropertyDataSize(deviceID, &pa, 0, NULL, &propertySize);
    if (propertySize > 0) return kAudioTypeInput;

    return kAudioTypeUnknown;
}

void getDeviceTransportType(AudioDeviceID deviceID, AudioDevicePropertyID *transportType) {
    AudioObjectPropertyAddress pa;
    pa.mSelector = kAudioDevicePropertyTransportType;
    pa.mScope = kAudioObjectPropertyScopeGlobal;
    pa.mElement = kAudioObjectPropertyElementMaster;

    UInt32 size = sizeof(transportType);

    AudioObjectGetPropertyData(deviceID, &pa, 0, 0, &size, transportType);
}


bool isAnOutputDevice(AudioDeviceID deviceID) {
    UInt32 propertySize = 256;

    AudioObjectPropertyAddress pa;
    pa.mSelector = kAudioDevicePropertyStreams;
    pa.mScope = kAudioDevicePropertyScopeOutput;
    pa.mElement = kAudioObjectPropertyElementMaster;

    // if there are any output streams, then it is an output
    AudioObjectGetPropertyDataSize(deviceID, &pa, 0, NULL, &propertySize);
    if (propertySize > 0) return true;

    return false;
}

bool isAnInputDevice(AudioDeviceID deviceID) {
    UInt32 propertySize = 256;

    AudioObjectPropertyAddress pa;
    pa.mSelector = kAudioDevicePropertyStreams;
    pa.mScope = kAudioDevicePropertyScopeInput;
    pa.mElement = kAudioObjectPropertyElementMaster;

    // if there are any input streams, then it is an input
    AudioObjectGetPropertyDataSize(deviceID, &pa, 0, NULL, &propertySize);
    if (propertySize > 0) return kAudioTypeInput;

    return false;
}

char *deviceTypeName(ASDeviceType device_type) {
    switch (device_type) {
        case kAudioTypeInput:
            return "input";
        case kAudioTypeOutput:
            return "output";
        case kAudioTypeSystemOutput:
            return "system";
        default:
            return "unknown";
    }
}

bool hasSubdevices(AudioDevicePropertyID device_type) {
    switch (device_type) {
        case kAudioDeviceTransportTypeAggregate:
            return true;
        default:
            return false;
    }
}

void showCurrentlySelectedDeviceID(ASDeviceType typeRequested) {
    AudioDeviceID currentDeviceID = kAudioDeviceUnknown;
    currentDeviceID = getCurrentlySelectedDeviceID(typeRequested);

    printProperties(currentDeviceID, typeRequested);
}

UInt32 getNumberOfDevices(AudioDeviceID *dev_array) {
    UInt32 propertySize;
    AudioObjectPropertyAddress pa;
    pa.mSelector = kAudioHardwarePropertyDevices;
    pa.mScope = kAudioObjectPropertyScopeGlobal;
    pa.mElement = kAudioObjectPropertyElementMaster;

    AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &pa, 0, NULL, &propertySize);
    //printf("propertySize1: %d\n", propertySize);

    AudioObjectGetPropertyData(kAudioObjectSystemObject, &pa, 0, NULL, &propertySize, dev_array);
    //printf("numberOfDevices: %lu\n", (propertySize / sizeof(AudioDeviceID)));

    return (propertySize / sizeof(AudioDeviceID));
}

AudioDeviceID getRequestedDeviceID(char *requestedDeviceName, ASDeviceType typeRequested) {
    AudioDeviceID dev_array[64];
    int numberOfDevices = 0;
    char deviceName[DEVICE_NAME_LEN];

    numberOfDevices = getNumberOfDevices(dev_array);

    for (int i = 0; i < numberOfDevices; ++i) {
        switch (typeRequested) {
            case kAudioTypeInput:
                if (!isAnInputDevice(dev_array[i])) continue;
                break;

            case kAudioTypeOutput:
                if (!isAnOutputDevice(dev_array[i])) continue;
                break;

            case kAudioTypeSystemOutput:
                if (getDeviceType(dev_array[i]) != kAudioTypeOutput) continue;
                break;
            default:
                break;
        }

        getDeviceName(dev_array[i], deviceName);
        // printf("For device %d, id = %d and name is %s\n",i,dev_array[i],deviceName);
        if (strcmp(requestedDeviceName, deviceName) == 0) {
            return dev_array[i];
        }
    }

    return kAudioDeviceUnknown;
}

AudioDeviceID getNextDeviceID(AudioDeviceID currentDeviceID, ASDeviceType typeRequested) {
    AudioDeviceID dev_array[64];
    int numberOfDevices = 0;
    AudioDeviceID first_dev = kAudioDeviceUnknown;
    int found = -1;

    numberOfDevices = getNumberOfDevices(dev_array);

    for (int i = 0; i < numberOfDevices; ++i) {
        switch (typeRequested) {
            case kAudioTypeInput:
                if (!isAnInputDevice(dev_array[i])) continue;
                break;

            case kAudioTypeOutput:
                if (!isAnOutputDevice(dev_array[i])) continue;
                break;

            case kAudioTypeSystemOutput:
                if (getDeviceType(dev_array[i]) != kAudioTypeOutput) continue;
                break;
            default:
                break;
        }

        if (first_dev == kAudioDeviceUnknown) {
            first_dev = dev_array[i];
        }
        if (found >= 0) {
            return dev_array[i];
        }
        if (dev_array[i] == currentDeviceID) {
            found = i;
        }
    }

    return first_dev;
}

void setDevice(AudioDeviceID newDeviceID, ASDeviceType typeRequested) {
    UInt32 propertySize = sizeof(UInt32);

    AudioObjectPropertyAddress pa;
    pa.mScope = kAudioObjectPropertyScopeGlobal;
    pa.mElement = kAudioObjectPropertyElementMaster;

    switch (typeRequested) {
        case kAudioTypeInput:
            pa.mSelector = kAudioHardwarePropertyDefaultInputDevice;
            break;
        case kAudioTypeOutput:
            pa.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
            break;
        case kAudioTypeSystemOutput:
            pa.mSelector = kAudioHardwarePropertyDefaultSystemOutputDevice;
            break;
        default:
            break;
    }
    AudioObjectSetPropertyData(kAudioObjectSystemObject, &pa, 0, NULL, propertySize, &newDeviceID);
}

