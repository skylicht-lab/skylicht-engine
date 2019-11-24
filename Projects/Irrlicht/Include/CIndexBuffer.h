// Copyright (C) 2012 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_INDEX_BUFFER_H_INCLUDED__
#define __C_INDEX_BUFFER_H_INCLUDED__

#include "IIndexBuffer.h"

namespace irr
{
namespace scene
{
	class CIndexBuffer : public IIndexBuffer
	{
	public:
		CIndexBuffer(video::E_INDEX_TYPE type = video::EIT_16BIT) : Type(type), HardwareMappingHint(EHM_NEVER), ChangedID(1), Indices(0)
		{
#ifdef _DEBUG
			setDebugName("CIndexBuffer");
#endif
			if(Type == video::EIT_32BIT)
				Indices = new CIndexList<u32>();
			else // EIT_16BIT
				Indices = new CIndexList<u16>();
		}

		CIndexBuffer(const CIndexBuffer &indexBuffer) : Type(video::EIT_16BIT), HardwareMappingHint(EHM_NEVER), ChangedID(1), Indices(0)
		{
			Type = indexBuffer.Type;

			HardwareMappingHint = indexBuffer.HardwareMappingHint;

			if (Type == video::EIT_32BIT)
				Indices = new CIndexList<u32>();
			else // EIT_16BIT
				Indices = new CIndexList<u16>();

			const u32 ibCount = indexBuffer.Indices->size();

			Indices->reallocate(ibCount);

			for (u32 i = 0; i < ibCount; ++i)
				addIndex(indexBuffer.getIndex(i));
		}

		virtual ~CIndexBuffer()
		{
			delete Indices;
		}

		virtual void clear()
		{
			Indices->clear();
		}

		virtual u32 getLast()
		{
			return (u32)Indices->getLast();
		}

		virtual void set_used(u32 used)
		{
			Indices->set_used(used);
		}

		virtual void reallocate(u32 size)
		{
			Indices->reallocate(size);
		}

		virtual u32 allocated_size() const
		{
			return Indices->allocated_size();
		}

		virtual s32 linear_reverse_search(const u32& element) const
		{
			return Indices->linear_reverse_search(element);
		}

		virtual video::E_INDEX_TYPE getType() const
		{
			return Type;
		}

		virtual void setType(video::E_INDEX_TYPE type)
		{
			if (Type == type)
				return;

			Type = type;

			IIndexList* IndicesList = 0;

			switch (Type)
			{
				case video::EIT_16BIT:
				{
					IndicesList = new CIndexList<u16>();
					break;
				}
				case video::EIT_32BIT:
				{
					IndicesList = new CIndexList<u32>();
					break;
				}
			}

			if (Indices)
			{
				IndicesList->reallocate(Indices->size());

				for(u32 i = 0; i < Indices->size(); ++i)
					IndicesList->addIndex(Indices->getIndex(i));

				delete Indices;
			}

			Indices = IndicesList;
		}

		virtual E_HARDWARE_MAPPING getHardwareMappingHint() const
		{
			return HardwareMappingHint;
		}

		virtual void setHardwareMappingHint(E_HARDWARE_MAPPING hardwareMappingHint)
		{
			if (HardwareMappingHint != hardwareMappingHint)
				setDirty();

			HardwareMappingHint = hardwareMappingHint;
		}

		virtual void addIndex(const u32& index)
		{
			Indices->addIndex(index);
		}

		virtual u32 getIndex(u32 id) const
		{
			return Indices->getIndex(id);
		}

		virtual void* getIndices()
		{
			return Indices->pointer();
		}

		virtual u32 getIndexCount() const
		{
			return Indices->size();
		}

		virtual u32 getIndexSize() const
		{
			if(Type == video::EIT_32BIT)
				return sizeof(u32);

			return sizeof(u16);
		}

		virtual void setIndex(u32 id, u32 index)
		{
			Indices->setIndex(id, index);
		}		

		virtual void setDirty()
		{
			if (HardwareBuffer)
				HardwareBuffer->requestUpdate();

			++ChangedID;
		}

		virtual u32 getChangedID() const
		{
			return ChangedID;
		}

	protected:
		class IIndexList
		{
		public:
			virtual ~IIndexList() {};
			virtual void clear() = 0;
			virtual void* pointer() = 0;
			virtual u32 size() const = 0;
			virtual u32 getLast() = 0;
			virtual void set_used(u32 used) = 0;
			virtual void reallocate(u32 size) = 0;
			virtual u32 allocated_size() const = 0;
			virtual s32 linear_reverse_search(const u32& element) const = 0;
			virtual void addIndex(const u32& index) = 0;
			virtual u32 getIndex(u32 id) const = 0;
			virtual void setIndex(u32 id, u32 index) = 0;
		};

		template <class T>
		class CIndexList : public IIndexList
		{
		public:
			CIndexList() : Data(0)
			{
			}

			CIndexList(const CIndexList &indexList) : Data(0)
			{
				const u32 ilCount = indexList.Data.size();

				Data.reallocate(ilCount);

				for (u32 i = 0; i < ilCount; ++i)
					Data.push_back(indexList.Data[i]);
			}

			~CIndexList()
			{
			}

			virtual void clear()
			{
				Data.clear();
			}

			virtual void* pointer()
			{
				return Data.pointer();
			}

			virtual u32 size() const
			{
				return Data.size();
			}

			virtual u32 getLast()
			{
				return (u32)Data.getLast();
			}

			virtual void set_used(u32 used)
			{
				Data.set_used(used);
			}

			virtual void reallocate(u32 size)
			{
				Data.reallocate(size);
			}

			virtual u32 allocated_size() const
			{
				return Data.allocated_size();
			}

			virtual s32 linear_reverse_search(const u32& element) const
			{
				return Data.linear_reverse_search(element);
			}

			virtual void addIndex(const u32& index)
			{
				Data.push_back(index);
			}

			virtual u32 getIndex(u32 id) const
			{
				if (id < Data.size())
					return Data[id];

				return 0;
			}

			virtual void setIndex(u32 id, u32 index)
			{
				if (id < Data.size())
					Data[id] = (T)index;
			}	

		protected:
			core::array<T> Data;
		};

		video::E_INDEX_TYPE Type;

		E_HARDWARE_MAPPING HardwareMappingHint;

		u32 ChangedID;

		IIndexList* Indices;
	};

	//typedef CIndexList<u16> SIndexBuffer16;
	//typedef CIndexList<u32> SIndexBuffer32;
}
}

#endif