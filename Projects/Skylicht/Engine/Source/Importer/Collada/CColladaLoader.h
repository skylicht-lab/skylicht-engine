/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#pragma once

#include "pch.h"
#include "RenderMesh/CMesh.h"
#include "RenderMesh/CSkinnedMesh.h"

#include "Importer/CBaseMeshImporter.h"

#include "RenderMesh/CJointData.h"

namespace Skylicht
{
	// const for buffer
	const int k_positionBuffer = 0;
	const int k_normalBuffer = 1;
	const int k_texCoordBuffer = 2;

	// const for mesh
	const int k_mesh = 0;
	const int k_skinMesh = 1;

	struct SColladaImage
	{
		std::wstring id;
		std::wstring name;
		std::wstring fileName;
	};
	typedef std::vector<SColladaImage> ArrayImages;

	struct SEffectParam
	{
		std::wstring Name;
		std::wstring Source;
		std::wstring InitFromTexture;
	};
	typedef std::vector<SEffectParam> ArrayEffectParams;

	struct SEffect
	{
		video::SMaterial Mat;
		std::wstring Id;
		f32 Transparency;
		bool HasAlpha;
		bool TransparentAddColor;

		bool HasDiffuseMapping;
		bool HasBumpMapping;
		bool HasSpecularMapping;

		std::wstring DiffuseRef;
		std::wstring SpecularRef;
		std::wstring BumpRef;

		ArrayEffectParams EffectParams;

		inline bool operator< (const SEffect& other) const
		{
			return Id < other.Id;
		}
	};
	typedef std::vector<SEffect> ArrayEffects;


	struct SColladaMaterial
	{
		std::wstring ID;
		std::wstring Name;
		std::wstring InstanceEffect;
		SEffect Effect;
	};
	typedef std::vector<SColladaMaterial> ArrayMaterials;

	struct SBufferParam
	{
		std::wstring Name;
		float* FloatArray;
		int ArrayCount;
		int Type;
		int Strike;

		SBufferParam()
		{
			FloatArray = NULL;
		}
	};

	struct SVerticesParam
	{
		std::wstring Name;
		int PositionIndex;
		int NormalIndex;
		int TexCoord1Index;
		int TexCoord2Index;

		SVerticesParam()
		{
			PositionIndex = -1;
			NormalIndex = -1;
			TexCoord1Index = -1;
			TexCoord2Index = -1;
		}
	};

	struct STrianglesParam
	{
		int VerticesIndex;
		int EffectIndex;
		std::wstring Material;
		int NumPolygon;
		s32* VCount;
		s32* IndexBuffer;
		int NumElementPerVertex;
		int OffsetVertex;
		int OffsetNormal;
		int OffsetTexcoord1;
		int OffsetTexcoord2;

		STrianglesParam()
		{
			VerticesIndex = -1;
			EffectIndex = -1;
			NumPolygon = 0;
			IndexBuffer = NULL;
			VCount = NULL;
			NumElementPerVertex = 1;
			OffsetVertex = 0;
			OffsetNormal = 0;
			OffsetTexcoord1 = 0;
			OffsetTexcoord2 = 0;
		}
	};


	struct SWeightParam
	{
		u32 VertexID;
		f32 Strength;
	};

	struct SJointParam
	{
		std::wstring Name;
		core::matrix4 InvMatrix;
		std::vector<SWeightParam> Weights;
	};


	struct SMeshParam
	{
		std::wstring Name;
		std::wstring ControllerName;
		std::wstring ListJointName;

		std::vector<SBufferParam> Buffers;
		std::vector<SVerticesParam> Vertices;
		std::vector<STrianglesParam> Triangles;
		std::vector<SJointParam> Joints;

		std::vector<s32> JointVertexIndex;
		std::vector<s32> JointIndex;

		int Type;
		core::matrix4 BindShapeMatrix;
	};
	typedef std::vector<SMeshParam>	ArrayMeshParams;

	class CEntity;

	struct SNodeParam
	{
		std::wstring Name;
		std::wstring Type;
		std::wstring SID;
		std::wstring Instance;
		std::map<std::wstring, std::wstring> BindMaterial;

		core::matrix4 Transform;

		std::vector<SNodeParam*> Childs;
		SNodeParam* Parent;

		CEntity* Entity;
		int ChildLevel;

		SNodeParam()
		{
			Entity = NULL;
			ChildLevel = 0;
		}
	};
	typedef std::vector<SNodeParam*> ArrayNodeParams;

	// Add to support read DAE from FBX Converter
	struct SColladaMeshVertexMap
	{
		SMeshParam* meshId;
		s32 bufferId;
		s32 vertexId;

		bool operator<(const SColladaMeshVertexMap& another) const
		{
			if (meshId == another.meshId)
			{
				if (bufferId == another.bufferId)
				{
					if (vertexId == another.vertexId)
						return false;
					else
						return vertexId < another.vertexId;
				}
				else
					return bufferId < another.bufferId;
			}
			else
			{
				return meshId < another.meshId;
			}
		}
	};

	struct SColladaVertexIndex
	{
		s32 vertexId;
		s32 normalId;
		s32 texcoordId1;
		s32 texcoordId2;

		bool operator<(const SColladaVertexIndex& another) const
		{
			if (vertexId == another.vertexId)
			{
				if (normalId == another.normalId)
				{
					if (texcoordId1 == another.texcoordId1)
					{
						if (texcoordId2 == another.texcoordId2)
							return false;
						else
							return texcoordId2 < another.texcoordId2;
					}
					else
						return texcoordId1 < another.texcoordId1;
				}
				else
					return normalId < another.normalId;
			}
			else
			{
				return vertexId < another.vertexId;
			}
		}
	};

	class SKYLICHT_API CColladaLoader : public CBaseMeshImporter
	{
	public:
		static bool s_fixUV;

	protected:

		std::string m_meshFile;
		std::string m_meshName;

		std::string m_unit;
		float m_unitScale;
		core::matrix4 m_unitScaleMatrix;

	protected:

		bool m_zUp;
		bool m_flipOx;

		bool m_loadTexcoord2;
		bool m_createTangent;
		bool m_flipNormalMap;
		bool m_createBatchMesh;

		bool m_loadNormalMap;

		float m_maxUVTile;

		std::map<std::string, CJointData*> m_nameToJointData;
		std::map<std::string, CJointData*> m_sidToJointData;

	protected:

		ArrayImages m_listImages;
		ArrayEffects m_listEffects;
		ArrayMaterials m_listMaterial;
		ArrayMeshParams m_listMesh;
		ArrayNodeParams m_listNode;

		SNodeParam* m_colladaRoot;

		// Add to support read DAE from FBX Converter
		typedef std::vector<s32> ArrayShort;
		std::map<SColladaMeshVertexMap, ArrayShort> m_meshVertexIndex;

	public:
		CColladaLoader();

		virtual ~CColladaLoader();

		void setLoadTexCoord2(bool b)
		{
			m_loadTexcoord2 = b;
		}

		void setCreateBatchMesh(bool b)
		{
			m_createBatchMesh = b;
		}

		void setLoadNormalMap(bool b, bool flipNormalMap)
		{
			m_loadNormalMap = b;
			m_createTangent = b;
			m_flipNormalMap = flipNormalMap;
		}

		void setTextureFolder(std::vector<std::string>& folder)
		{
			m_textureFolder = folder;
		}

		virtual bool loadModel(const char* resource, CEntityPrefab* output, bool normalMap = true, bool flipNormalMap = true, bool texcoord2 = true, bool batching = false);

	protected:

		bool loadDae(const char* fileName, CEntityPrefab* output);

		void parseImageNode(io::IXMLReader* xmlRead, SColladaImage* image = NULL);

		void parseMaterialNode(io::IXMLReader* xmlRead, SColladaMaterial* material = NULL);

		void parseEffectNode(io::IXMLReader* xmlRead, SEffect* effect = NULL);

		void parseGeometryNode(io::IXMLReader* xmlRead);

		void parseControllerNode(io::IXMLReader* xmlRead);

		void parseSceneNode(io::IXMLReader* xmlRead);

		void parseUnit(io::IXMLReader* xmlRead);

	protected:

		SMeshParam* parseSkinNode(io::IXMLReader* xmlRead);

		SNodeParam* parseNode(io::IXMLReader* xmlRead, SNodeParam* parent);

		void updateEffectMaterial(SEffect* effect);

		void updateJointToMesh(SMeshParam* mesh, int numJoints, float* arrayWeight, float* arrayTransform, std::vector<s32>& vCountArray, std::vector<s32>& vArray);

	protected:

		CJointData* findJointData(const char* name);

		void loadEffectTexture();

		ITexture* getTextureResource(std::wstring& refName, ArrayEffectParams& params);

		void constructEntityPrefab(CEntityPrefab* output);

		CMesh* constructMesh(SMeshParam* mesh, SNodeParam* node);

		void constructSkinMesh(std::list<SNodeParam*>& nodes);

		void constructSkinMesh(SMeshParam* meshParam, CSkinnedMesh* mesh);

		IMeshBuffer* constructMeshBuffer(SMeshParam* mesh, STrianglesParam* tri, int bufferID, bool& needFixUVTile);

		void convertToLightMapVertices(IMeshBuffer* buffer, SMeshParam* mesh, STrianglesParam* tri);

		void cleanData();
	};

}