#pragma once
#include <wrl/client.h>
// DirectX12�̐F��Ȍ^�p�̋��L�|�C���^�I�ȓz
template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;