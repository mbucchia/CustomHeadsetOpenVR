#pragma once
#include "DistortionProfile.h"
#include <cstdint>
#include <string>

class MapUvDistortionProfile : public DistortionProfile {
public:
	MapUvDistortionProfile(const std::string& distortionProfile);

	virtual void Initialize() override;

	virtual void GetProjectionRaw(vr::EVREye eEye, float* pfLeft, float* pfRight, float* pfBottom, float* pfTop) override;

	virtual Point2D ComputeDistortion(vr::EVREye eEye, ColorChannel colorChannel, float fU, float fV) override;

	virtual void GetRecommendedRenderTargetSize(uint32_t* pnWidth, uint32_t* pnHeight) override;

	virtual bool GetDistortionResolutionOverride(uint32_t& distortionResolution) override;

private:
	const std::string distortionProfile;

	size_t distortionMapSize = 0;
	float renderInfo[11] = {};
	std::string distortionMap[3];
};
