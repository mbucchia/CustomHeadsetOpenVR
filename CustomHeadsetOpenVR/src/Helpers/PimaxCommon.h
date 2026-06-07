#pragma once
#define NOMINMAX
#include <openvr_driver.h>
#include <PVR.h>
#include <PVR_API.h>

namespace Pimax {
	static constexpr uint16_t UsbVendorId = 0x34A4;
	static constexpr uint16_t EdidVendorId = 53826; // PVR

	namespace Crystal {
		// TODO(mbucchia): Check name (I don't own the LH version).
		static const char* LighthouseDeviceName = "Pimax Crystal";
		static const char* PrettyName = "Pimax Crystal";
		static constexpr uint16_t UsbProductId = 0x0012;
	};
	namespace CrystalSuper {
		// TODO(mbucchia): Check name (I don't own the LH version).
		static const char* LighthouseDeviceName = "Pimax Crystal Super";
		static const char* PrettyName = "Pimax Crystal Super";
		static constexpr uint16_t UsbProductId = 0x0040;
	};
	namespace DreamAir {
		static const char* LighthouseDeviceName = "Pimax Dream Air";
		static const char* PrettyName = "Pimax Dream Air";
		static constexpr uint16_t UsbProductId = 0x0044;
	};
};

enum PimaxHeadsetType {
	Crystal = 0,
	CrystalSuper,
	DreamAir,

	Invalid
};

class PimaxCommon {
protected:
	bool EnsureSession();
	void GetHeadsetPose(vr::DriverPose_t& pose);

	pvrEnvHandle pvr = {};
	pvrSessionHandle pvrSession = {};
};
