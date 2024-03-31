#pragma once
#include "ComPtr.h"
#include <d3d12.h>
#include <cassert>
#include <windows.h>

class Device {
public:
	static ID3D12Device* GetDevice() {
		static ComPtr<ID3D12Device> device;
		static bool isFirst = true;
		if (isFirst) {
			// �f�o�C�X�̐���
			if (!SUCCEEDED(D3D12CreateDevice(0, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(device.GetAddressOf())))) {
				assert(!"�f�o�C�X�̐����Ɏ��s");
			}
			isFirst = false;
		}
		return device.Get();
	}
};
