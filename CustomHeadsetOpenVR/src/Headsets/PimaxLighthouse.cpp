#include "PimaxLighthouse.h"

bool PimaxLighthouseShim::IsDesiredHeadset(std::string model, vr::PropertyContainerHandle_t container){
	std::string trackingSystem = vr::VRProperties()->GetStringProperty(container, vr::Prop_TrackingSystemName_String);
	if(trackingSystem == "lighthouse"){
		if(model == Pimax::Crystal::LighthouseDeviceName){
			headsetType = Crystal;
			return true;
		} else if(model == Pimax::CrystalSuper::LighthouseDeviceName){
			headsetType = CrystalSuper;
			return true;
		} else if(model == Pimax::DreamAir::LighthouseDeviceName){
			headsetType = DreamAir;
			return true;
		}
	}
	return false;
}

Config::BaseHeadsetConfig& PimaxLighthouseShim::GetConfig(){
	switch (headsetType){
	case Crystal:
		return driverConfig.crystal;
	case CrystalSuper:
		return driverConfig.crystalSuper;
	case DreamAir:
	default:
		return driverConfig.dreamAir;
	}
}

Config::BaseHeadsetConfig& PimaxLighthouseShim::GetConfigOld(){
	switch (headsetType){
	case Crystal:
		return driverConfigOld.crystal;
	case CrystalSuper:
		return driverConfigOld.crystalSuper;
	case DreamAir:
	default:
		return driverConfigOld.dreamAir;
	}
}


void PimaxLighthouseShim::SubActivate(vr::PropertyContainerHandle_t container){
	eyeTracking.eyeRotation = defaultDriverConfig.dreamAir.eyeRotation;
	eyeTracking.enabled = GetConfig().enableEyeTracking;
	eyeTracking.Initialize();
}

void PimaxLighthouseShim::SubDeactivate(){
	eyeTracking.Shutdown();
}

void PimaxLighthouseShim::SubRunFrame(){
	eyeTracking.ipd = GetConfig().ipd + GetConfig().ipdOffset;
	eyeTracking.enabled = GetConfig().enableEyeTracking;
	// eyeTracking.eyeRotation = GetConfig().eyeRotation;
	eyeTracking.RunFrame();
}


extern "C" {
// cant be bothered, implement them here
void* zcalloc(void* opaque, unsigned int items, unsigned int size){
	return malloc(items * size);
}
void zcfree(void* opaque, void* address){
	free(address);
}
const char * z_errmsg[12]{"","","","","","","","","","","",""};
}