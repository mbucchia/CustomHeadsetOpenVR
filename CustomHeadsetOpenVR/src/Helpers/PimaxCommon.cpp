#include "PimaxCommon.h"

bool PimaxCommon::EnsureSession() {
	if (!pvr) {
		pvr_initialise(&pvr);
	}
	if (!pvrSession) {
		pvr_createSession(pvr, &pvrSession);
	}

	return !!pvrSession;
}

void PimaxCommon::GetHeadsetPose(vr::DriverPose_t& pose) {
	pose.qWorldFromDriverRotation.w = pose.qDriverFromHeadRotation.w = pose.qRotation.w = 1.0;
	pose.deviceIsConnected = !!pvrSession;
	pose.result = vr::TrackingResult_Running_OutOfRange;

	if (pose.deviceIsConnected) {
		const auto pvrNow = pvr_getTimeSeconds(pvr);
		pvrTrackingState state = {};
		pvr_getTrackingState(pvrSession, pvrNow, &state);
		if (state.HeadPose.StatusFlags & pvrStatus_OrientationTracked) {
			pose.vecPosition[0] = state.HeadPose.ThePose.Position.x;
			pose.vecPosition[1] = state.HeadPose.ThePose.Position.y;
			pose.vecPosition[2] = state.HeadPose.ThePose.Position.z;
			pose.qRotation.x = state.HeadPose.ThePose.Orientation.x;
			pose.qRotation.y = state.HeadPose.ThePose.Orientation.y;
			pose.qRotation.z = state.HeadPose.ThePose.Orientation.z;
			pose.qRotation.w = state.HeadPose.ThePose.Orientation.w;

			pose.vecVelocity[0] = state.HeadPose.LinearVelocity.x;
			pose.vecVelocity[1] = state.HeadPose.LinearVelocity.y;
			pose.vecVelocity[2] = state.HeadPose.LinearVelocity.z;

			pose.vecAngularVelocity[0] = state.HeadPose.AngularVelocity.x;
			pose.vecAngularVelocity[1] = state.HeadPose.AngularVelocity.y;
			pose.vecAngularVelocity[2] = state.HeadPose.AngularVelocity.z;

			pose.poseTimeOffset = state.HeadPose.TimeInSeconds - pvrNow;

			pose.poseIsValid = true;
			pose.result = vr::TrackingResult_Running_OK;
		}
	}
}
