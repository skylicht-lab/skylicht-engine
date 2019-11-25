
#ifndef __C_DIRECTX11_HARDWARE_BUFFER_H_INCLUDED__
#define __C_DIRECTX11_HARDWARE_BUFFER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_WINDOWS_

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#include "IHardwareBuffer.h"

namespace irr
{
namespace video
{

class CD3D11Driver;

class CD3D11HardwareBuffer : public IHardwareBuffer
{
	// Implementation of public methods
public:
	CD3D11HardwareBuffer(CD3D11Driver* driver, E_HARDWARE_BUFFER_TYPE type, scene::E_HARDWARE_MAPPING mapping,
		u32 size, u32 flags, const void* initialData = 0);
	CD3D11HardwareBuffer(scene::IIndexBuffer* indexBuffer, CD3D11Driver* driver);
	CD3D11HardwareBuffer(scene::IVertexBuffer* vertexBuffer, CD3D11Driver* driver);
	~CD3D11HardwareBuffer();

	bool update(const scene::E_HARDWARE_MAPPING mapping, const u32 size, const void* data) _IRR_OVERRIDE_;

	//! Lock function.
	void* lock(bool readOnly = false);

	//! Unlock function. Must be called after a lock() to the buffer.
	void unlock();

	//! Copy data from system memory
	void copyFromMemory(const void* sysData, u32 offset, u32 length);

	//! Copy data from another buffer
	void copyFromBuffer(IHardwareBuffer* buffer, u32 srcOffset, u32 descOffset, u32 length);

	//! return unordered access view
	ID3D11UnorderedAccessView* getUnorderedAccessView() const;

	//! return shader resource view
	ID3D11ShaderResourceView* getShaderResourceView() const;

	//! return DX 11 buffer
	ID3D11Buffer* getBuffer() const;

	void removeFromArray(bool status);

private:
	bool createInternalBuffer(const void* initialData);

	ID3D11Device* Device;
	ID3D11DeviceContext* Context;
	ID3D11Buffer* Buffer;
	ID3D11UnorderedAccessView* UAView;
	ID3D11ShaderResourceView* SRView;

	CD3D11Driver* Driver;
	CD3D11HardwareBuffer* TempStagingBuffer;

	bool UseTempStagingBuffer;
	D3D11_MAP LastMapDirection;

	bool RemoveFromArray;

	void* LinkedBuffer;
};

}
}

#endif
#endif
#endif