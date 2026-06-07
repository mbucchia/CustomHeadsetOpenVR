#include "UsbUtils.h"
#define NOMINMAX
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>

#pragma comment(lib, "setupapi.lib")

bool FindUsbDeviceByVidPid(uint16_t vendorId, uint16_t productId) {
    HDEVINFO deviceInfoSet = SetupDiGetClassDevsW(
        NULL,
        L"USB",
        NULL,
        DIGCF_ALLCLASSES | DIGCF_PRESENT
    );

    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        return false;
    }

    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    DWORD deviceIndex = 0;
    bool deviceFound = false;

    // Build the query string to search for inside the hardware ID
    std::wstringstream ss;
    ss << L"VID_" << std::hex << std::setfill(L'0') << std::setw(4) << vendorId << L"&PID_" << std::hex << std::setfill(L'0') << std::setw(4) << productId;
    std::wstring formatQuery = ss.str();
    // Transform to uppercase to guarantee case-insensitive matches
    std::transform(formatQuery.begin(), formatQuery.end(), formatQuery.begin(), ::towupper);

    while (SetupDiEnumDeviceInfo(deviceInfoSet, deviceIndex, &deviceInfoData)) {
        deviceIndex++;
        DWORD dataType;
        DWORD bufferSize = 0;

        SetupDiGetDeviceRegistryPropertyW(
            deviceInfoSet,
            &deviceInfoData,
            SPDRP_HARDWAREID,
            &dataType,
            NULL,
            0,
            &bufferSize
        );

        if (bufferSize == 0) {
            continue;
        }

        std::wstring hardwareIdBuffer(bufferSize / sizeof(wchar_t), L'\0');

        // Call again to actually retrieve the hardware ID multi-string
        if (SetupDiGetDeviceRegistryPropertyW(
            deviceInfoSet,
            &deviceInfoData,
            SPDRP_HARDWAREID,
            &dataType,
            reinterpret_cast<PBYTE>(&hardwareIdBuffer[0]),
            bufferSize,
            NULL)) {

            std::transform(hardwareIdBuffer.begin(), hardwareIdBuffer.end(), hardwareIdBuffer.begin(), ::towupper);
            if (hardwareIdBuffer.find(formatQuery) != std::wstring::npos) {
                deviceFound = true;
                break;
            }
        }
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return deviceFound;
}
