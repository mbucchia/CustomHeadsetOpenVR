#include "MapUvDistortionProfile.h"
#include <algorithm>
#include <fstream>

MapUvDistortionProfile::MapUvDistortionProfile(const std::string& distortionProfile) : distortionProfile(distortionProfile) {
}

void MapUvDistortionProfile::Initialize() {
	const uint32_t length = vr::VRResources()->GetResourceFullPath((std::string("{CustomHeadsetOpenVR}/distortion/") + distortionProfile).c_str(), nullptr, nullptr, 0);
	std::string path;
	path.resize(length);
	vr::VRResources()->GetResourceFullPath((std::string("{CustomHeadsetOpenVR}/distortion/") + distortionProfile).c_str(), nullptr, path.data(), length);
	std::ifstream ifs(path, std::ios::binary);
	// TODO(mbucchia): Error handling.
	if (ifs.is_open()) {
		ifs.read((char*)renderInfo, sizeof(renderInfo));
		/*
			const float renderInfo[11] = {
				eyeRenderInfo[pvrEye_Left].Fov.UpTan,
				eyeRenderInfo[pvrEye_Left].Fov.DownTan,
				eyeRenderInfo[pvrEye_Left].Fov.LeftTan,
				eyeRenderInfo[pvrEye_Left].Fov.RightTan,
				eyeRenderInfo[pvrEye_Right].Fov.UpTan,
				eyeRenderInfo[pvrEye_Right].Fov.DownTan,
				eyeRenderInfo[pvrEye_Right].Fov.LeftTan,
				eyeRenderInfo[pvrEye_Right].Fov.RightTan,
				(float)viewportSize.w,
				(float)viewportSize.h,
				(float)s_mapSize,
			};
		*/
		distortionMapSize = (uint32_t)std::round(renderInfo[10]);
		const size_t size = distortionMapSize * distortionMapSize * sizeof(float) * 4;
		for (int channel = 0; channel < 3; channel++) {
			distortionMap[channel].resize(size);
			ifs.read(distortionMap[channel].data(), size);
		}
		ifs.close();
	}
}

void MapUvDistortionProfile::GetProjectionRaw(vr::EVREye eEye, float* pfLeft, float* pfRight, float* pfBottom, float* pfTop) {
	// Top and bottom are backwards per SteamVR documentation.
	*pfTop = -renderInfo[eEye == vr::Eye_Left ? 1 : 5];
	*pfBottom = renderInfo[eEye == vr::Eye_Left ? 0 : 4];
	*pfLeft = -renderInfo[eEye == vr::Eye_Left ? 2 : 6];
	*pfRight = renderInfo[eEye == vr::Eye_Left ? 3 : 7];
}

Point2D MapUvDistortionProfile::ComputeDistortion(vr::EVREye eEye, ColorChannel colorChannel, float fU, float fV) {
	const float minResolution = (float)std::min(resolutionX, resolutionY);
	fU = (fU * minResolution / (2.0f * resolutionY)) + 0.5f;
	fV = (fV * minResolution / (2.0f * resolutionX)) + 0.5f;

	const int32_t u = std::clamp((int32_t)std::round((eEye == vr::Eye_Left ? fU : (1.f - fU)) * (distortionMapSize - 1)), 0, (int32_t)distortionMapSize - 1);
	const int32_t v = std::clamp((int32_t)std::round(fV * (distortionMapSize - 1)), 0, (int32_t)distortionMapSize - 1);

	// Distortion contains RGBA samples, with R = U, G = V, B = unused, and A = validity
	const uint32_t rowPitch = distortionMapSize * 4;
	const auto getIndex = [&rowPitch](uint32_t u, uint32_t v) { return v * rowPitch + u * 4; };

	const uint32_t index = getIndex(u, v);
	const float* map = (float*)distortionMap[colorChannel].data();
	if (map[index + 3]) {
		float fX = ((eEye == vr::Eye_Left ? (1 - map[index + 0]) : map[index + 0]) - 0.5f) * 2.f;
		float fY = (map[index + 1] - 0.5f) * 2.f;
		return { fX, fY };
	}
	else {
		return { NAN, NAN };
	}
}

void MapUvDistortionProfile::GetRecommendedRenderTargetSize(uint32_t* pnWidth, uint32_t* pnHeight) {
	*pnWidth = (uint32_t)std::round(renderInfo[8]);
	*pnWidth = (uint32_t)std::round(renderInfo[9]);
}

bool MapUvDistortionProfile::GetDistortionResolutionOverride(uint32_t& distortionResolution) {
	distortionResolution = distortionMapSize;
	return true;
}
