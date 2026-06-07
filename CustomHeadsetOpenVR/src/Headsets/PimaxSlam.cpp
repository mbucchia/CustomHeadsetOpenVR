#include "PimaxSlam.h"

PimaxSlam::PimaxSlam(PimaxHeadsetType headsetType) : headsetType(headsetType) {
}

bool PimaxSlam::IsDesiredHeadset(std::string model, vr::PropertyContainerHandle_t container) {
	return true;
}

Config::BaseHeadsetConfig& PimaxSlam::GetConfig() {
	switch (headsetType) {
	case Crystal:
		return driverConfig.crystal;
	case CrystalSuper:
		return driverConfig.crystalSuper;
	case DreamAir:
	default:
		return driverConfig.dreamAir;
	}
}

Config::BaseHeadsetConfig& PimaxSlam::GetConfigOld() {
	switch (headsetType) {
	case Crystal:
		return driverConfigOld.crystal;
	case CrystalSuper:
		return driverConfigOld.crystalSuper;
	case DreamAir:
	default:
		return driverConfigOld.dreamAir;
	}
}

void PimaxSlam::PosTrackedDeviceActivate(uint32_t& unObjectId, vr::EVRInitError& returnValue) {
	vr::PropertyContainerHandle_t container = vr::VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);
	switch (headsetType) {
	case Crystal:
		vr::VRProperties()->SetStringProperty(container, vr::Prop_ModelNumber_String, Pimax::Crystal::PrettyName);
		break;
	case CrystalSuper:
		vr::VRProperties()->SetStringProperty(container, vr::Prop_ModelNumber_String, Pimax::CrystalSuper::PrettyName);
		break;
	case DreamAir:
		vr::VRProperties()->SetStringProperty(container, vr::Prop_ModelNumber_String, Pimax::DreamAir::PrettyName);
		break;
	}

	returnValue = vr::VRInitError_None;
	BaseHeadsetShim::PosTrackedDeviceActivate(unObjectId, returnValue);
}

// TODO(mbucchia): Add shims for eye tracking


void PimaxSlam::RunFrame() {
	BaseHeadsetShim::RunFrame();
	if (!isActive) {
		return;
	}

	if (!EnsureSession()) {
		return;
	}

	vr::DriverPose_t pose = {};
	GetHeadsetPose(pose);
	vr::VRServerDriverHost()->TrackedDevicePoseUpdated(deviceIndex, pose, sizeof(pose));
}
