#include "PimaxCommon.h"

#include <PVR_Math.h>

#include "../Driver/DriverLog.h"

PimaxCommon::PimaxCommon() {
	pvr_initialise(&pvr);
	if (pvr_createSession(pvr, &session) == pvr_success) {
		// Don't enable parallel projections.
		pvr_setIntConfig(GetPvrSession(), "view_rotation_fix", 0);

		// Cache useful immutable state.
		isOpenPortEnabled = pvr_getIntConfig(GetPvrSession(), "no_render", 0);
		pvr_getHmdInfo(GetPvrSession(), &hmdInfo);
		pvrHmdTrackingStyle trackingStyle = pvrHmdTrackingStyle_Unknown;
		trackingStyle = (pvrHmdTrackingStyle)pvr_getTrackedDeviceIntProperty(
			GetPvrSession(),
			pvrTrackedDevice_HMD,
			pvrTrackedDeviceProp_Prop_HmdTrackingStyle_Int,
			pvrHmdTrackingStyle_Unknown);
		DriverLog("Detected headset '%s' (%04x) with %s tracking", GetHmdInfo().ProductName, GetHmdInfo().ProductId, trackingStyle == pvrHmdTrackingStyle_InsideOutCameras ? "SLAM" : "Lighthouse");
		pvr_getEyeDisplayInfo(GetPvrSession(), pvrEye_Left, &displayInfo);
		pvrEyeRenderInfo eyeInfo[pvrEye_Count] = {};
		pvr_getEyeRenderInfo(GetPvrSession(), pvrEye_Left, &eyeInfo[pvrEye_Left]);
		pvr_getEyeRenderInfo(GetPvrSession(), pvrEye_Right, &eyeInfo[pvrEye_Right]);
		cantingAngle = PVR::Quatf{ eyeInfo[pvrEye_Left].HmdToEyePose.Orientation }.Angle(eyeInfo[pvrEye_Right].HmdToEyePose.Orientation) / 2.f;
	}
}

Config::BaseHeadsetConfig& PimaxCommon::PatchDisplayConfig(Config::BaseHeadsetConfig& config) const {
	config.edidProductId = GetDisplayInfo().edid_pid;
	config.resolutionX = GetDisplayInfo().width / 2;
	config.resolutionY = GetDisplayInfo().height;
	config.displayRotation = GetDisplayInfo().eye_rotate;
	config.eyeRotation = cantingAngle;
	return config;
}
