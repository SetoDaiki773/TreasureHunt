#pragma once
#include <wrl/client.h>
// DirectX12の色んな型用の共有ポインタ的な奴
template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;