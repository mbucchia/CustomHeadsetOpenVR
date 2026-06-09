#include "PimaxCommon.h"

#include <PVR_Math.h>

#include "../Driver/DriverLog.h"

static pvrEnvHandle s_pvr = {};
static pvrSessionHandle s_pvrSession = {};
static PimaxHeadsetInfo s_headsetInfo = {};

static bool EnsurePvrSession() {
	if (!s_pvr) {
		if (pvr_initialise(&s_pvr) != pvr_success) {
			return false;
		}
	}
	if (!s_pvrSession) {
		if (pvr_createSession(s_pvr, &s_pvrSession) != pvr_success) {
			return false;
		}

		// This is what the OpenPort toggle sets in Pimax EVO.
		s_headsetInfo.isOpenPortEnabled = pvr_getIntConfig(s_pvrSession, "no_render", 0);

		pvrHmdInfo info = {};
		pvr_getHmdInfo(s_pvrSession, &info);
		// TODO: Add other Pimax headsets here.
		switch (info.ProductId) {
		case 0x0044:
			s_headsetInfo.headsetType = DreamAir;
			break;
		}
		s_headsetInfo.connected = s_headsetInfo.headsetType != Invalid;
		if (s_headsetInfo.connected) {
			pvrHmdTrackingStyle trackingStyle = pvrHmdTrackingStyle_Unknown;
			trackingStyle = (pvrHmdTrackingStyle)pvr_getTrackedDeviceIntProperty(
				s_pvrSession,
				pvrTrackedDevice_HMD,
				pvrTrackedDeviceProp_Prop_HmdTrackingStyle_Int,
				pvrHmdTrackingStyle_Unknown);
			s_headsetInfo.useSlamTracking = trackingStyle == pvrHmdTrackingStyle_InsideOutCameras;

			DriverLog("Detected headset '%s' (%04x) with %s tracking", info.ProductName, info.ProductId, s_headsetInfo.useSlamTracking ? "SLAM" : "Lighthouse");
		}
	}
	return true;
}

PimaxHeadsetInfo PimaxCommon::GetHeadsetInfo() {
	EnsurePvrSession();
	return s_headsetInfo;
}

pvrSessionHandle PimaxCommon::GetPvrSession() {
	EnsurePvrSession();
	return s_pvrSession;
}

double PimaxCommon::GetPvrTime() {
	return pvr_getTimeSeconds(s_pvr);
}

PimaxCommon::PimaxCommon() {
	// Don't enable parallel projections.
	pvr_setIntConfig(GetPvrSession(), "view_rotation_fix", 0);

	// Cache useful immutable state.
	pvr_getHmdInfo(GetPvrSession(), &hmdInfo);
	pvr_getEyeDisplayInfo(GetPvrSession(), pvrEye_Left, &displayInfo);
	hasEyeTracking = // Crystal OG, Crystal Super, Dream Air SE, Dream Air.
		GetHmdInfo().ProductId == 0x0012 || GetHmdInfo().ProductId == 0x0040 ||
		GetHmdInfo().ProductId == 0x0042 || GetHmdInfo().ProductId == 0x0044;
	pvrEyeRenderInfo eyeInfo[pvrEye_Count] = {};
	pvr_getEyeRenderInfo(GetPvrSession(), pvrEye_Left, &eyeInfo[pvrEye_Left]);
	pvr_getEyeRenderInfo(GetPvrSession(), pvrEye_Right, &eyeInfo[pvrEye_Right]);
	cantingAngle = PVR::Quatf{ eyeInfo[pvrEye_Left].HmdToEyePose.Orientation }.Angle(eyeInfo[pvrEye_Right].HmdToEyePose.Orientation) / 2.f;
}

Config::BaseHeadsetConfig& PimaxCommon::PatchDisplayConfig(Config::BaseHeadsetConfig& config) const {
	config.edidProductId = GetDisplayInfo().edid_pid;
	config.resolutionX = GetDisplayInfo().width / 2;
	config.resolutionY = GetDisplayInfo().height;
	// The PimaxDistortionProfile already accounts for rotation, but the others one don't.
	if (config.distortionProfile == "Pimax Builtin") {
		config.displayRotation = 0;
	} else {
		config.displayRotation = GetDisplayInfo().eye_rotate;
	}
	config.eyeRotation = cantingAngle;
	return config;
}
