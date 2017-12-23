/*
 *  interface.c
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

void printProperties(AudioDeviceID deviceID, ASDeviceType typeRequested, ASOutputFormat outputFormat) {
    char deviceName[DEVICE_NAME_LEN];
    float vol_left, vol_right;
    ASDeviceType device_type;
    UInt32 transportType;

    switch (typeRequested) {
        case kAudioTypeInput:
            if (!isAnInputDevice(deviceID)) return;
            device_type = kAudioTypeInput;
            break;

        case kAudioTypeOutput:
            if (!isAnOutputDevice(deviceID)) return;
            device_type = kAudioTypeOutput;
            break;

        case kAudioTypeSystemOutput:
            device_type = getDeviceType(deviceID);
            if (device_type != kAudioTypeOutput) return;
            break;
        default:
            break;
    }

    getDeviceName(deviceID, deviceName);
    getDeviceVolume(deviceID, &vol_left, &vol_right);
    getDeviceTransportType(deviceID, &transportType);

    switch (outputFormat) {
      case kOutputFormatName:
        printf("%s\n", deviceName);
        break;
      default:
        printf("[%3u] - %6s %-26s", (unsigned int) deviceID, deviceTypeName(device_type), deviceName);
        if (vol_left < -0.1 || vol_right < -0.1) {
          printf("\n");
        }
        else {
          printf(" :: [%.3f:%.3f]\n", vol_left, vol_right);
        }

        if (transportType == kAudioDeviceTransportTypeAggregate) {
          AudioObjectID sub_device[32];
          UInt32 outSize = sizeof(sub_device);
          getAggregateDeviceSubDeviceList(deviceID, sub_device, &outSize);
          for (int j = 0; j < outSize / sizeof(AudioObjectID); j++) {
            printf("\t");
            printProperties(sub_device[j], device_type, outputFormat);
          }
        }
    }

}

void showUsage(const char *appName) {
    printf("Usage: %s [-a] [-c] [-t type] [-n] -s device_name\n"
                   "  -a     : shows all devices\n"
                   "  -c     : shows current device\n"
                   "  -t type  : device type (input/output/system).  Defaults to all.\n"
                   "  -f format : output format (name/full).  Defaults to full.\n"
                   "  -n     : cycles the audio device to the next one\n"
                   "  -s device_name : sets the audio device to the given device by name\n"
                   "  -e device_id1=vol1,vol2:device_id2=vol1,vol2 : sets audio device volume for 1st and 2nd channel. Multiple device can be separated with ':'.\n\n",
           appName);
}

int runAudioSwitch(int argc, const char *argv[]) {
    char requestedDeviceName[DEVICE_NAME_LEN];
    char volume_arg[VOL_ARG_LEN];
    AudioDeviceID chosenDeviceID = kAudioDeviceUnknown;
    ASDeviceType typeRequested = kAudioTypeUnknown;
    ASOutputFormat outputFormat = kOutputFormatDefault;
    int function = 0;

    int c;
    while ((c = getopt(argc, (char **) argv, "hacnf:t:s:e:")) != -1) {
        switch (c) {
            case 'a':
                // show all
                function = kFunctionShowAll;
                break;
            case 'c':
                // get current device
                function = kFunctionShowCurrent;
                break;

            case 'e':
                // set device volume
                function = kFunctionSetVolume;
                strncpy(volume_arg, optarg, VOL_ARG_LEN);
                break;

            case 'h':
                // show help
                function = kFunctionShowHelp;
                break;

            case 'n':
                // cycle to the next audio device
                function = kFunctionCycleNext;
                break;

            case 's':
                // set the requestedDeviceName
                function = kFunctionSetDevice;
                strncpy(requestedDeviceName, optarg, DEVICE_NAME_LEN);
                break;

            case 'f':
                // set the output format
                if (strcmp(optarg, "full") == 0) {
                    outputFormat = kOutputFormatDefault;
                }
                else if (strcmp(optarg, "name") == 0) {
                    outputFormat = kOutputFormatName;
                }
                else {
                    printf("Invalid output format \"%s\" specified.\n", optarg);
                    showUsage(argv[0]);
                    return 1;
                }
                break;

            case 't':
                // set the requestedDeviceName
                if (strcmp(optarg, "input") == 0) {
                    typeRequested = kAudioTypeInput;
                }
                else if (strcmp(optarg, "output") == 0) {
                    typeRequested = kAudioTypeOutput;
                }
                else if (strcmp(optarg, "system") == 0) {
                    typeRequested = kAudioTypeSystemOutput;
                }
                else {
                    printf("Invalid device type \"%s\" specified.\n", optarg);
                    showUsage(argv[0]);
                    return 1;
                }
                break;
        }
    }

    if (function == kFunctionShowAll) {
        switch (typeRequested) {
            case kAudioTypeInput:
            case kAudioTypeOutput:
                showAllDevices(typeRequested, outputFormat);
                break;
            case kAudioTypeSystemOutput:
                showAllDevices(kAudioTypeOutput, outputFormat);
                break;
            default:
                showAllDevices(kAudioTypeInput, outputFormat);
                showAllDevices(kAudioTypeOutput, outputFormat);
        }
        return 0;
    }
    if (function == kFunctionShowHelp) {
        showUsage(argv[0]);
        return 0;
    }
    if (function == kFunctionShowCurrent) {
        if (typeRequested == kAudioTypeUnknown) typeRequested = kAudioTypeOutput;
        showCurrentlySelectedDeviceID(typeRequested, outputFormat);
        return 0;
    }
    if (function == kFunctionSetVolume) {
        double vol1, vol2;
        int dev_id;
        int i = 0;
        char *pch = strtok(volume_arg, VOL_DELIMITERS);

        while (pch != NULL) {
            if (i % 3 == 0) {
                dev_id = atoi(pch);
            }
            else if (i % 3 == 2) {
                i = -1;
                vol2 = atof(pch);
                setDeviceVolume(dev_id, vol1, vol2);
            }
            else {
                vol1 = atof(pch);
            }

            pch = strtok(NULL, VOL_DELIMITERS);
            i++;
        }

        return 0;
    }

    if (typeRequested == kAudioTypeUnknown) typeRequested = kAudioTypeOutput;

    if (function == kFunctionCycleNext) {
        // get current device of requested type
        chosenDeviceID = getCurrentlySelectedDeviceID(typeRequested);
        if (chosenDeviceID == kAudioDeviceUnknown) {
            printf("Could not find current audio device of type %s.  Nothing was changed.\n",
                   deviceTypeName(typeRequested));
            return 1;
        }

        // find next device to current device
        chosenDeviceID = getNextDeviceID(chosenDeviceID, typeRequested);
        if (chosenDeviceID == kAudioDeviceUnknown) {
            printf("Could not find next audio device of type %s.  Nothing was changed.\n",
                   deviceTypeName(typeRequested));
            return 1;
        }

        // choose the requested audio device
        setDevice(chosenDeviceID, typeRequested);
        getDeviceName(chosenDeviceID, requestedDeviceName);
        printf("%s audio device set to \"%s\"\n", deviceTypeName(typeRequested), requestedDeviceName);

        return 0;
    }

    if (function != kFunctionSetDevice) {
        printf("Please specify audio device.\n");
        showUsage(argv[0]);
        return 1;
    }

    // find the id of the requested device
    chosenDeviceID = getRequestedDeviceID(requestedDeviceName, typeRequested);
    if (chosenDeviceID == kAudioDeviceUnknown) {
        printf("Could not find an audio device named \"%s\" of type %s.  Nothing was changed.\n", requestedDeviceName,
               deviceTypeName(typeRequested));
        return 1;
    }

    // choose the requested audio device
    setDevice(chosenDeviceID, typeRequested);
    printf("%s audio device set to \"%s\"\n", deviceTypeName(typeRequested), requestedDeviceName);
    return 0;

}
