#include "MinimalAllocator.hpp"

#define MINIMAL_USE_PROCESSHEAPSTRING
#include "MinimalPath.hpp"

#define MINIMAL_USE_PROCESSHEAPARRAY
#include "MinimalArray.hpp"

#include "../../../Src/nnnEngine/nnn.h"

#include "Map.h"

namespace Nyan{
	class Scene
	{
	protected:
		Minimal::IMinimalAllocator *m_alloc;
		Map3D *map;
		NNN::Buffer::s_VertexBuffer *m_vb;
		NNN::Buffer::s_IndexBuffer *m_ib;
	public:
		typedef Minimal::MinimalArrayT < BYTE >  SaveFormat;
		void SaveScene(__out SaveFormat &bin);
		void LoadScene(__in SaveFormat& bin);
		Scene(Minimal::IMinimalAllocator *alloc);
	};
}