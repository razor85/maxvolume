#include "stdafx.h"

#include <windows.h>
#include <mmeapi.h>

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

// Multimedia library
#pragma comment(lib, "winmm")
#define logInfo(X) \
  do {\
    std::stringstream __str;\
    __str << X;\
    __str << "\n";\
    OutputDebugStringA((LPCSTR)__str.str().c_str());\
  } while (false)

#define throwException(X) \
  do {\
    std::stringstream __str;\
    __str << X;\
    __str << "\n";\
    OutputDebugStringA((LPCSTR)__str.str().c_str());\
    throw std::runtime_error(__str.str().c_str());\
  } while (false)


int main(int argc, char* argv[]) {
  CoInitialize(NULL);

  IMMDeviceEnumerator *deviceEnumerator = NULL;
  HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, 
    __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);

  IMMDeviceCollection* deviceCollection = NULL;
  hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &deviceCollection);
  if (hr != S_OK)
    throwException("Failed to enumerate audio endpoints");

  unsigned int deviceCount;
  deviceCollection->GetCount(&deviceCount);

  for (unsigned int deviceId = 0; deviceId < deviceCount; ++deviceId) {
    IMMDevice* device = NULL;
    deviceCollection->Item(deviceId, &device);

    IAudioEndpointVolume* endpointVolume = NULL;
    device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
    device->Release();
    device = NULL;

    float currentVolume = 0;
    endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
    logInfo("Current volume as a scalar is: " << currentVolume);

    endpointVolume->SetMasterVolumeLevelScalar(1.0f, NULL);
    endpointVolume->Release();
  }

  IMMDevice* defaultDevice = NULL;
  deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
  deviceEnumerator->Release();
  deviceEnumerator = NULL;

  IAudioEndpointVolume *endpointVolume = NULL;
  defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
  defaultDevice->Release();
  defaultDevice = NULL;

  float currentVolume = 0;
  endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
  logInfo("Current volume as a scalar is: " << currentVolume);

  endpointVolume->SetMasterVolumeLevelScalar(1.0f, NULL);
  endpointVolume->Release();

  CoUninitialize();
  return 0;
}
