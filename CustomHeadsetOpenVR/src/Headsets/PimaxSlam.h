#pragma once
#include "BaseHeadset.h"
#include "../Helpers/PimaxCommon.h"

class PimaxSlam : public BaseHeadsetShim, public PimaxCommon {
public:
	PimaxSlam(PimaxHeadsetType headsetType);
	virtual bool IsDesiredHeadset(std::string model, vr::PropertyContainerHandle_t container) override;

	virtual Config::BaseHeadsetConfig& GetConfig() override;
	virtual Config::BaseHeadsetConfig& GetConfigOld() override;

	virtual void PosTrackedDeviceActivate(uint32_t& unObjectId, vr::EVRInitError& returnValue) override;
	virtual void RunFrame() override;

private:
	const PimaxHeadsetType headsetType;
};
