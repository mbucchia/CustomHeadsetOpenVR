#pragma once
#include <PVR.h>
#include <PVR_API.h>

#include "../Config/Config.h"

class PimaxCommon {
public:
	PimaxCommon();
	virtual ~PimaxCommon() = default;

protected:
	pvrSessionHandle GetPvrSession() const { return session; };
	bool IsOpenPortEnabled() const { return isOpenPortEnabled; }
	pvrHmdInfo GetHmdInfo() const { return hmdInfo; };
	pvrDisplayInfo GetDisplayInfo() const { return displayInfo; };

	Config::BaseHeadsetConfig& PatchDisplayConfig(Config::BaseHeadsetConfig& config) const;

private:
	pvrEnvHandle pvr = {};
	pvrSessionHandle session = {};
	bool isOpenPortEnabled = false;
	pvrHmdInfo hmdInfo = {};
	pvrDisplayInfo displayInfo = {};
	float cantingAngle = 0.f;
};
