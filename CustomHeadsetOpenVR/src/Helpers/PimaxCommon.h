#pragma once
#define NOMINMAX
#include <openvr_driver.h>
#include <PVR.h>
#include <PVR_API.h>

#include "../Config/Config.h"

enum PimaxHeadsetType {
	DreamAir = 0,

	Invalid
};

struct PimaxHeadsetInfo {
	bool connected = false;
	bool isOpenPortEnabled = false;
	PimaxHeadsetType headsetType = Invalid;
	bool useSlamTracking = false;
};

class PimaxCommon {
public:
	PimaxCommon();
	virtual ~PimaxCommon() = default;
	static PimaxHeadsetInfo GetHeadsetInfo();
	static pvrSessionHandle GetPvrSession();
	static double GetPvrTime();

protected:
	pvrHmdInfo GetHmdInfo() const { return hmdInfo; };
	pvrDisplayInfo GetDisplayInfo() const { return displayInfo; };

	Config::BaseHeadsetConfig& PatchDisplayConfig(Config::BaseHeadsetConfig& config) const;

private:
	pvrHmdInfo hmdInfo = {};
	pvrDisplayInfo displayInfo = {};
	bool hasEyeTracking = false;
	float cantingAngle = 0.f;
};
