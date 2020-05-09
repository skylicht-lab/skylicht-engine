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

#include "pch.h"
#include "CShader.h"

#include "Utils/CStringImp.h"

#include "ShaderCallback/CShaderLighting.h"
#include "ShaderCallback/CShaderCamera.h"
#include "ShaderCallback/CShaderMaterial.h"
#include "ShaderCallback/CShaderShadow.h"
#include "ShaderCallback/CShaderSH.h"

namespace Skylicht
{
	CShader::CShader() :
		m_listVSUniforms(NULL),
		m_listFSUniforms(NULL),
		m_numVSUniform(0),
		m_numFSUniform(0),
		m_deferred(false)
	{
		// builtin callback
		addCallback<CShaderLighting>();
		addCallback<CShaderCamera>();
		addCallback<CShaderMaterial>();
		addCallback<CShaderShadow>();
		addCallback<CShaderSH>();
	}

	CShader::~CShader()
	{
		for (IShaderCallback *cb : m_callbacks)
		{
			delete cb;
		}
		m_callbacks.clear();

		deleteAllUI();
		deleteAllResource();
	}

	void CShader::deleteAllUI()
	{
		for (u32 i = 0, n = m_ui.size(); i < n; i++)
		{
			delete m_ui[i];
		}

		m_ui.clear();
	}

	void CShader::deleteAllResource()
	{
		for (u32 i = 0, n = m_resources.size(); i < n; i++)
		{
			delete m_resources[i];
		}

		m_resources.clear();
	}

	EUniformType CShader::getUniformType(const char *name)
	{
		const char *uniformString[] = {
			"VIEW_PROJECTION",
			"WORLD_VIEW_PROJECTION",
			"VIEW",
			"WORLD",
			"WORLD_INVERSE",
			"WORLD_INVERSE_TRANSPOSE",
			"WORLD_TRANSPOSE",
			"BONE_MATRIX",
			"SHADOW_MAP_MATRIX",
			"SHADOW_MAP_DISTANCE",
			"CAMERA_POSITION",
			"WORLD_CAMERA_POSITION",
			"LIGHT_COLOR",
			"LIGHT_AMBIENT",
			"LIGHT_DIRECTION",
			"WORLD_LIGHT_DIRECTION",
			"POINT_LIGHT_COLOR",
			"POINT_LIGHT_POSITION",
			"POINT_LIGHT_ATTENUATION",
			"SPOT_LIGHT_CUTOFF",
			"OBJECT_PARAM",
			"MATERIAL_COLOR",
			"MATERIAL_PARAM",
			"DEFAULT_VALUE",
			"SHADER_VEC2",
			"SHADER_VEC3",
			"SHADER_VEC4",
			"SH_CONST",
			"CUSTOM_VALUE",
			"CONFIG_VALUE",
			"BPCEM_MIN",
			"BPCEM_MAX",
			"BPCEM_POS",
			"TEXTURE_MIPMAP_COUNT",
			"FOG_PARAMS",
			"SSAO_KERNEL",
			"DEFERRED_VIEW",
			"DEFERRED_PROJECTION",
			"NULL"
		};

		for (u32 i = 0, n = NUM_SHADER_TYPE; i < n; i++)
		{
			if (strcmp(uniformString[i], name) == 0)
				return (EUniformType)i;
		}

		return NUM_SHADER_TYPE;
	}

	E_MATERIAL_TYPE CShader::getBaseShaderByName(const char *name)
	{
		std::string type = name;

		if (type == "SOLID")
			return EMT_SOLID;
		else if (type == "TRANSPARENT_ADD_COLOR")
			return EMT_TRANSPARENT_ADD_COLOR;
		else if (type == "TRANSPARENT_ALPHA_CHANNEL")
			return EMT_TRANSPARENT_ALPHA_CHANNEL;

		return EMT_SOLID;
	}

	void CShader::parseUI(io::IXMLReader *xmlReader, SUniformUI *parent)
	{
		const wchar_t *wtext;
		char text[1024];

		const char* uiName[] = {
			"UITexture",
			"UIColor",
			"UISlider",
			"UIFloat",
			"UIFloat2",
			"UIFloat3",
			"UIFloat4",
			"UIGroup"
		};

		SUniformUI *uniform = new SUniformUI(this);

		wtext = xmlReader->getAttributeValue(L"control");
		CStringImp::convertUnicodeToUTF8(wtext, text);

		EUIControlType controlType = UIGroup;
		for (u32 i = 0, n = UICount; i < n; i++)
		{
			if (strcmp(text, uiName[i]) == 0)
			{
				uniform->ControlType = (EUIControlType)i;
				break;
			}
		}

		wtext = xmlReader->getAttributeValue(L"name");
		CStringImp::convertUnicodeToUTF8(wtext, text);
		uniform->Name = text;

		wtext = xmlReader->getAttributeValue(L"autoReplace");
		if (wtext != NULL)
		{
			CStringImp::convertUnicodeToUTF8(wtext, text);
			CStringImp::splitString(text, ";", uniform->AutoReplace);
		}

		wtext = xmlReader->getAttributeValue(L"step");
		if (wtext != NULL)
		{
			CStringImp::convertUnicodeToUTF8(wtext, text);
			uniform->SliderStep = atoi(text);
		}

		if (parent == NULL)
		{
			m_ui.push_back(uniform);
		}
		else
		{
			// add child
			uniform->Parent = parent;
			parent->Childs.push_back(uniform);
		}

		if (xmlReader->isEmptyElement() == false)
		{
			while (xmlReader->read())
			{
				if (xmlReader->getNodeType() == io::EXN_ELEMENT)
				{
					std::wstring nodeName = xmlReader->getNodeName();
					if (nodeName == L"ui")
						parseUI(xmlReader, uniform);
				}
				else if (xmlReader->getNodeType() == io::EXN_ELEMENT_END)
				{
					std::wstring nodeName = xmlReader->getNodeName();
					if (nodeName == L"ui")
						break;
				}
			}
		}
	}

	void CShader::parseUniformUI(io::IXMLReader *xmlReader)
	{
		while (xmlReader->read())
		{
			if (xmlReader->getNodeType() == io::EXN_ELEMENT)
			{
				std::wstring nodeName = xmlReader->getNodeName();
				if (nodeName == L"ui")
					parseUI(xmlReader, NULL);
			}
			else if (xmlReader->getNodeType() == io::EXN_ELEMENT_END)
			{
				std::wstring nodeName = xmlReader->getNodeName();
				if (nodeName == L"customUI")
					return;
			}
		}
	}

	void CShader::parseResources(io::IXMLReader *xmlReader)
	{
		const wchar_t *wtext;
		char text[1024];

		while (xmlReader->read())
		{
			if (xmlReader->getNodeType() == io::EXN_ELEMENT)
			{
				std::wstring nodeName = xmlReader->getNodeName();
				if (nodeName == L"resource")
				{
					SResource *res = new SResource();
					m_resources.push_back(res);

					wtext = xmlReader->getAttributeValue(L"name");
					CStringImp::convertUnicodeToUTF8(wtext, text);
					res->Name = text;

					wtext = xmlReader->getAttributeValue(L"type");
					CStringImp::convertUnicodeToUTF8(wtext, text);
					const char *type[] =
					{
						"Texture",
						"CubeTexture",
						"StaticCubeTexture",
						"ShadowMapTexture"
					};

					for (u32 i = 0, n = ResourceCount; i < n; i++)
					{
						if (CStringImp::comp<const char>(text, type[i]) == 0)
						{
							res->Type = (EResourceType)i;
							break;
						}
					}

					wtext = xmlReader->getAttributeValue(L"path");
					if (wtext != NULL)
					{
						CStringImp::convertUnicodeToUTF8(wtext, text);
						res->Path = text;
					}
				}
			}
			else if (xmlReader->getNodeType() == io::EXN_ELEMENT_END)
			{
				std::wstring nodeName = xmlReader->getNodeName();
				if (nodeName == L"resources")
					return;
			}
		}
	}

	void CShader::parseUniform(io::IXMLReader *xmlReader)
	{
		const wchar_t *wtext;
		char text[1024];

		bool inVS = true;

		while (xmlReader->read())
		{
			switch (xmlReader->getNodeType())
			{
			case io::EXN_ELEMENT:
			{
				std::wstring nodeName = xmlReader->getNodeName();
				if (nodeName == L"vs")
					inVS = true;
				else if (nodeName == L"fs")
					inVS = false;
				else if (nodeName == L"uniform")
				{
					SUniform *uniform = NULL;
					if (inVS == true)
					{
						m_vsUniforms.push_back(SUniform());
						uniform = &m_vsUniforms.getLast();
					}
					else
					{
						m_fsUniforms.push_back(SUniform());
						uniform = &m_fsUniforms.getLast();
					}

					wtext = xmlReader->getAttributeValue(L"name");
					CStringImp::convertUnicodeToUTF8(wtext, text);
					uniform->Name = text;

					wtext = xmlReader->getAttributeValue(L"type");
					CStringImp::convertUnicodeToUTF8(wtext, text);
					uniform->Type = getUniformType(text);

					if (uniform->Type == NUM_SHADER_TYPE)
					{
						sprintf(text, "[CShader] %s: '%s' have unknown type", m_name.c_str(), uniform->Name.c_str());
						os::Printer::log(text);
					}

					wtext = xmlReader->getAttributeValue(L"float");
					CStringImp::convertUnicodeToUTF8(wtext, text);
					uniform->FloatSize = atoi(text);

					wtext = xmlReader->getAttributeValue(L"array");
					if (wtext != NULL)
					{
						// Array Uniform use in BONES
						CStringImp::convertUnicodeToUTF8(wtext, text);
						uniform->ArraySize = atoi(text);
					}

					uniform->SizeOfUniform = uniform->FloatSize * uniform->ArraySize;

					wtext = xmlReader->getAttributeValue(L"matrix");
					if (wtext != NULL)
					{
						CStringImp::convertUnicodeToUTF8(wtext, text);
						if (strcmp(text, "true") == 0)
							uniform->IsMatrix = true;
						else
							uniform->IsMatrix = false;
					}

					wtext = xmlReader->getAttributeValue(L"normal");
					if (wtext != NULL)
					{
						CStringImp::convertUnicodeToUTF8(wtext, text);
						if (strcmp(text, "true") == 0)
							uniform->IsNormal = true;
						else
							uniform->IsNormal = false;
					}

					wtext = xmlReader->getAttributeValue(L"valueIndex");
					if (wtext != NULL)
					{
						CStringImp::convertUnicodeToUTF8(wtext, text);
						uniform->ValueIndex = atoi(text);
					}

					wtext = xmlReader->getAttributeValue(L"value");
					if (wtext != NULL)
					{
						CStringImp::convertUnicodeToUTF8(wtext, text);

						std::vector<std::string> listValue;
						CStringImp::splitString(text, ",", listValue);
						int numValue = listValue.size();
						if (numValue > uniform->FloatSize)
							numValue = uniform->FloatSize;

						memset(uniform->Value, 0, sizeof(float) * 16);

						for (int i = 0; i < numValue; i++)
							uniform->Value[i] = (float)atof(listValue[i].c_str());
					}

					wtext = xmlReader->getAttributeValue(L"openGL");
					if (wtext != NULL)
					{
						CStringImp::convertUnicodeToUTF8(wtext, text);

						if (strcmp(text, "true") == 0)
							uniform->OpenGL = true;
						else
							uniform->OpenGL = false;
					}

					wtext = xmlReader->getAttributeValue(L"directX");
					if (wtext != NULL)
					{
						CStringImp::convertUnicodeToUTF8(wtext, text);

						if (strcmp(text, "true") == 0)
							uniform->DirectX = true;
						else
							uniform->DirectX = false;
					}

					wtext = xmlReader->getAttributeValue(L"min");
					if (wtext != NULL)
					{
						CStringImp::convertUnicodeToUTF8(wtext, text);
						uniform->Min = (float)atof(text);
					}

					wtext = xmlReader->getAttributeValue(L"max");
					if (wtext != NULL)
					{
						CStringImp::convertUnicodeToUTF8(wtext, text);
						uniform->Max = (float)atof(text);
					}
				}
			}
			case io::EXN_ELEMENT_END:
			{
				std::wstring nodeName = xmlReader->getNodeName();
				if (nodeName == L"uniforms")
					return;
			}
			}
		}
	}

	void CShader::initShader(io::IXMLReader *xmlReader, const char *shaderFolder)
	{
		const wchar_t *wtext;
		char text[1024];

		while (xmlReader->read())
		{
			switch (xmlReader->getNodeType())
			{
			case io::EXN_ELEMENT:
			{
				std::wstring nodeName = xmlReader->getNodeName();
				if (nodeName == L"shaderConfig")
				{
					wtext = xmlReader->getAttributeValue(L"name");
					CStringImp::convertUnicodeToUTF8(wtext, text);
					m_name = text;

					wtext = xmlReader->getAttributeValue(L"baseShader");
					CStringImp::convertUnicodeToUTF8(wtext, text);
					m_baseShader = getBaseShaderByName(text);

					wtext = xmlReader->getAttributeValue(L"writeDepthMaterial");
					if (wtext != NULL)
					{
						CStringImp::convertUnicodeToUTF8(wtext, text);
						m_writeDepth = text;
					}

					wtext = xmlReader->getAttributeValue(L"deferred");
					if (wtext != NULL)
					{
						CStringImp::convertUnicodeToUTF8(wtext, text);
						if (CStringImp::comp<const char>(text, "true") == 0)
							m_deferred = true;
					}
				}
				else if (nodeName == L"uniforms")
				{
					parseUniform(xmlReader);
				}
				else if (nodeName == L"customUI")
				{
					parseUniformUI(xmlReader);
				}
				else if (nodeName == L"resources")
				{
					parseResources(xmlReader);
				}
				else if (nodeName == L"shader")
				{
					wtext = xmlReader->getAttributeValue(L"type");
					CStringImp::convertUnicodeToUTF8(wtext, text);
					std::string shaderType = text;

					wtext = xmlReader->getAttributeValue(L"vs");
					CStringImp::convertUnicodeToUTF8(wtext, text);
					std::string vs = text;

					wtext = xmlReader->getAttributeValue(L"fs");
					CStringImp::convertUnicodeToUTF8(wtext, text);
					std::string fs = text;

					if (shaderType == "HLSL")
					{
						m_hlsl.VertexShader = shaderFolder + vs;
						m_hlsl.FragmentShader = shaderFolder + fs;
					}
					else
					{
						m_glsl.VertexShader = shaderFolder + vs;
						m_glsl.FragmentShader = shaderFolder + fs;
					}
				}
				else if (nodeName == L"map")
				{
					SAttributeMapping m;

					wtext = xmlReader->getAttributeValue(L"uniformName");
					CStringImp::convertUnicodeToUTF8(wtext, text);
					m.UniformName = text;

					wtext = xmlReader->getAttributeValue(L"attributeName");
					CStringImp::convertUnicodeToUTF8(wtext, text);
					m.AttributeName = text;

					// add mapping
					m_attributeMapping.push_back(m);
				}
			}
			break;
			case io::EXN_TEXT:
			{
			}
			break;
			default:
				break;
			}
		};

		m_listVSUniforms = m_vsUniforms.pointer();
		m_listFSUniforms = m_fsUniforms.pointer();

		m_numVSUniform = (int)m_vsUniforms.size();
		m_numFSUniform = (int)m_fsUniforms.size();

		// todo build shader
		buildShader();

		// update editor ui uniform
		buildUIUniform();
	}

	void CShader::buildUIUniform()
	{
		for (u32 i = 0, n = m_ui.size(); i < n; i++)
		{
			SUniformUI* ui = m_ui[i];
			buildUIUniform(ui);
		}
	}

	void CShader::buildUIUniform(SUniformUI *ui)
	{
		if (ui->ControlType != UIGroup)
		{
			ui->UniformInfo = getVSUniform(ui->Name.c_str());
			if (ui->UniformInfo == NULL)
				ui->UniformInfo = getFSUniform(ui->Name.c_str());
		}

		for (u32 i = 0, n = ui->Childs.size(); i < n; i++)
		{
			SUniformUI* child = ui->Childs[i];
			buildUIUniform(child);
		}
	}

	CShader::SUniformUI* CShader::getUniformUIByName(const char *name)
	{
		for (u32 i = 0, n = m_ui.size(); i < n; i++)
		{
			if (m_ui[i]->Name == name)
				return m_ui[i];

			if (m_ui[i]->Childs.size() > 0)
			{
				SUniformUI* r = getUniformUIByName(name, m_ui[i]);
				if (r != NULL)
					return r;
			}
		}

		return NULL;
	}

	CShader::SUniformUI* CShader::getUniformUIByName(const char *name, CShader::SUniformUI *group)
	{
		for (u32 i = 0, n = group->Childs.size(); i < n; i++)
		{
			SUniformUI *ui = group->Childs[i];

			if (ui->Name == name)
				return ui;

			if (ui->Childs.size() > 0)
			{
				SUniformUI* r = getUniformUIByName(name, ui);
				if (r != NULL)
					return r;
			}
		}

		return NULL;
	}

	SUniform* CShader::getVSUniform(const char *name)
	{
		for (int i = 0; i < m_numVSUniform; i++)
		{
			if (m_listVSUniforms[i].Name == name)
				return &m_listVSUniforms[i];
		}

		return NULL;
	}

	SUniform* CShader::getFSUniform(const char *name)
	{
		for (int i = 0; i < m_numFSUniform; i++)
		{
			if (m_listFSUniforms[i].Name == name)
				return &m_listFSUniforms[i];
		}

		return NULL;
	}

	std::string CShader::getVSShaderFileName()
	{
		std::string ret;

		IVideoDriver *driver = getVideoDriver();
		if (driver->getDriverType() == video::EDT_DIRECT3D11)
			ret = m_hlsl.VertexShader;
		else
			ret = m_glsl.VertexShader;

		return ret;
	}

	std::string CShader::getFSShaderFileName()
	{
		std::string ret;

		IVideoDriver *driver = getVideoDriver();
		if (driver->getDriverType() == video::EDT_DIRECT3D11)
			ret = m_hlsl.FragmentShader;
		else
			ret = m_glsl.FragmentShader;

		return ret;
	}

	void CShader::buildShader()
	{
		std::string vs = getVSShaderFileName();
		std::string fs = getFSShaderFileName();

		if (vs.empty() || fs.empty())
		{
			setMaterialRenderID(-1);
		}
		else
		{
			int matID = getVideoDriver()->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles
			(
				vs.c_str(), "main", video::EVST_VS_4_0,
				fs.c_str(), "main", video::EPST_PS_4_0,
				this,
				m_baseShader
			);

			setMaterialRenderID(matID);
		}
	}

	bool CShader::isUniformAvaiable(SUniform& uniform)
	{
		IVideoDriver *driver = getVideoDriver();

		if (driver->getDriverType() == video::EDT_DIRECT3D11)
		{
			return uniform.DirectX;
		}
		else
		{
			return uniform.OpenGL;
		}

		return true;
	}

	void CShader::OnSetConstants(video::IMaterialRendererServices* services, s32 userData, bool updateTransform)
	{
		IVideoDriver *driver = services->getVideoDriver();
		IMaterialRenderer *matRender = driver->getMaterialRenderer(m_materialRenderID);

		// init for first time
		if (m_initCallback == true)
		{
			for (int i = 0; i < m_numVSUniform; i++)
			{
				SUniform& uniform = m_listVSUniforms[i];
				if (isUniformAvaiable(uniform) == true)
				{
					// query uniform
					uniform.UniformShaderID = matRender->getShaderVariableID(uniform.Name.c_str(), video::EST_VERTEX_SHADER);

					// todo fix array uniform
					if (uniform.UniformShaderID == -1 && uniform.ArraySize > 1)
					{
						std::string name = uniform.Name;
						name += "[0]";

						uniform.UniformShaderID = matRender->getShaderVariableID(name.c_str(), video::EST_VERTEX_SHADER);
					}
				}
			}

			for (int i = 0; i < m_numFSUniform; i++)
			{
				SUniform& uniform = m_listFSUniforms[i];
				if (isUniformAvaiable(uniform) == true)
				{
					// query uniform
					uniform.UniformShaderID = matRender->getShaderVariableID(uniform.Name.c_str(), video::EST_PIXEL_SHADER);

					// todo fix array uniform
					if (uniform.UniformShaderID == -1 && uniform.ArraySize > 1)
					{
						std::string name = uniform.Name;
						name += "[0]";

						uniform.UniformShaderID = matRender->getShaderVariableID(name.c_str(), video::EST_PIXEL_SHADER);
					}
				}

#if _DEBUG
				if (uniform.UniformShaderID == -1)
				{
					if (!(uniform.DirectX == false && getVideoDriver()->getDriverType() == EDT_DIRECT3D11))
					{
						char log[512];
						sprintf(log, "Warning: Shader: %s - Uniform: %s is not use", m_name.c_str(), uniform.Name.c_str());
						os::Printer::log(log);
					}

				}
#endif
			}

			m_initCallback = false;
		}

		// todo set vertex shader
		for (int i = 0; i < m_numVSUniform; i++)
		{
			SUniform& uniform = m_listVSUniforms[i];
			if (uniform.UniformShaderID >= 0)
			{
				// builtin callback
				if (setUniform(uniform, matRender, true, updateTransform) == false)
				{
					// plugin callback
					for (IShaderCallback *cb : m_callbacks)
					{
						cb->OnSetConstants(this, &uniform, matRender, true);
					}
				}

			}
		}

		// todo set pixel shader
		for (int i = 0; i < m_numFSUniform; i++)
		{
			SUniform& uniform = m_listFSUniforms[i];
			if (uniform.UniformShaderID >= 0)
			{
				// builtin callback
				if (setUniform(uniform, matRender, false, updateTransform) == false)
				{
					// plugin callback
					for (IShaderCallback *cb : m_callbacks)
					{
						cb->OnSetConstants(this, &uniform, matRender, false);
					}
				}
			}
		}
	}

	bool CShader::setUniform(SUniform &uniform, IMaterialRenderer* matRender, bool vertexShader, bool updateTransform)
	{
		IVideoDriver *driver = getVideoDriver();
		CShaderManager *shaderManager = CShaderManager::getInstance();

		switch (uniform.Type)
		{
		case VIEW_PROJECTION:
		{
			if (updateTransform == true)
			{
				const core::matrix4& viewProj = driver->getTransform(video::ETS_VIEW_PROJECTION);

				if (vertexShader == true)
					matRender->setShaderVariable(uniform.UniformShaderID, viewProj.pointer(), uniform.SizeOfUniform, video::EST_VERTEX_SHADER);
				else
					matRender->setShaderVariable(uniform.UniformShaderID, viewProj.pointer(), uniform.SizeOfUniform, video::EST_PIXEL_SHADER);
			}
		}
		break;
		case WORLD_VIEW_PROJECTION:
		{
			if (updateTransform == true)
			{
				const core::matrix4& worldViewProj = driver->getTransform(video::ETS_WORLD_VIEW_PROJECTION);

				if (vertexShader == true)
					matRender->setShaderVariable(uniform.UniformShaderID, worldViewProj.pointer(), uniform.SizeOfUniform, video::EST_VERTEX_SHADER);
				else
					matRender->setShaderVariable(uniform.UniformShaderID, worldViewProj.pointer(), uniform.SizeOfUniform, video::EST_PIXEL_SHADER);
			}
		}
		break;
		case VIEW:
		{
			if (updateTransform == true)
			{
				const core::matrix4& view = driver->getTransform(video::ETS_VIEW);

				if (vertexShader == true)
					matRender->setShaderVariable(uniform.UniformShaderID, view.pointer(), uniform.SizeOfUniform, video::EST_VERTEX_SHADER);
				else
					matRender->setShaderVariable(uniform.UniformShaderID, view.pointer(), uniform.SizeOfUniform, video::EST_PIXEL_SHADER);
			}
		}
		break;
		case WORLD:
		{
			if (updateTransform == true)
			{
				const core::matrix4& world = driver->getTransform(video::ETS_WORLD);

				if (vertexShader == true)
					matRender->setShaderVariable(uniform.UniformShaderID, world.pointer(), uniform.SizeOfUniform, video::EST_VERTEX_SHADER);
				else
					matRender->setShaderVariable(uniform.UniformShaderID, world.pointer(), uniform.SizeOfUniform, video::EST_PIXEL_SHADER);
			}
		}
		break;
		case WORLD_INVERSE:
		{
			if (updateTransform == true)
			{
				const core::matrix4& worldInv = driver->getTransform(video::ETS_WORLD_INVERSE);

				if (vertexShader == true)
					matRender->setShaderVariable(uniform.UniformShaderID, worldInv.pointer(), uniform.SizeOfUniform, video::EST_VERTEX_SHADER);
				else
					matRender->setShaderVariable(uniform.UniformShaderID, worldInv.pointer(), uniform.SizeOfUniform, video::EST_PIXEL_SHADER);
			}
		}
		break;
		case WORLD_INVERSE_TRANSPOSE:
		{
			if (updateTransform == true)
			{
				const core::matrix4& worldIT = driver->getTransform(video::ETS_WORLD_INVERSE_TRANSPOSE);
				if (vertexShader == true)
					matRender->setShaderVariable(uniform.UniformShaderID, worldIT.pointer(), uniform.SizeOfUniform, video::EST_VERTEX_SHADER);
				else
					matRender->setShaderVariable(uniform.UniformShaderID, worldIT.pointer(), uniform.SizeOfUniform, video::EST_PIXEL_SHADER);
			}
		}
		break;
		case WORLD_TRANSPOSE:
		{
			if (updateTransform == true)
			{
				const core::matrix4& world = driver->getTransform(video::ETS_WORLD);
				core::matrix4 worldTranspose = world.getTransposed();

				if (vertexShader == true)
					matRender->setShaderVariable(uniform.UniformShaderID, worldTranspose.pointer(), uniform.SizeOfUniform, video::EST_VERTEX_SHADER);
				else
					matRender->setShaderVariable(uniform.UniformShaderID, worldTranspose.pointer(), uniform.SizeOfUniform, video::EST_PIXEL_SHADER);
			}
		}
		break;
		case DEFAULT_VALUE:
		case CONFIG_VALUE:
		{
			if (vertexShader == true)
				matRender->setShaderVariable(uniform.UniformShaderID, uniform.Value, uniform.SizeOfUniform, video::EST_VERTEX_SHADER);
			else
				matRender->setShaderVariable(uniform.UniformShaderID, uniform.Value, uniform.SizeOfUniform, video::EST_PIXEL_SHADER);
		}
		break;
		case BONE_MATRIX:
		{
			if (vertexShader == true && shaderManager->BoneMatrix != NULL)
				matRender->setShaderVariable(uniform.UniformShaderID, shaderManager->BoneMatrix, uniform.SizeOfUniform, video::EST_VERTEX_SHADER);
		}
		break;
		case SHADER_VEC2:
		{
			CShaderManager *material = shaderManager;
			int paramID = (int)uniform.Value[0];
			float v[2];
			v[0] = material->ShaderVec2[paramID].X;
			v[1] = material->ShaderVec2[paramID].Y;

			if (vertexShader == true)
				matRender->setShaderVariable(uniform.UniformShaderID, v, uniform.SizeOfUniform, video::EST_VERTEX_SHADER);
			else
				matRender->setShaderVariable(uniform.UniformShaderID, v, uniform.SizeOfUniform, video::EST_PIXEL_SHADER);
		}
		break;
		case SHADER_VEC3:
		{
			CShaderManager *material = shaderManager;
			int paramID = (int)uniform.Value[0];
			float v[3];
			v[0] = material->ShaderVec3[paramID].X;
			v[1] = material->ShaderVec3[paramID].Y;
			v[2] = material->ShaderVec3[paramID].Z;

			if (vertexShader == true)
				matRender->setShaderVariable(uniform.UniformShaderID, v, uniform.SizeOfUniform, video::EST_VERTEX_SHADER);
			else
				matRender->setShaderVariable(uniform.UniformShaderID, v, uniform.SizeOfUniform, video::EST_PIXEL_SHADER);
		}
		break;
		case SHADER_VEC4:
		{
			CShaderManager *material = shaderManager;
			int paramID = (int)uniform.Value[0];
			float v[4];
			v[0] = material->ShaderVec4[paramID].X;
			v[1] = material->ShaderVec4[paramID].Y;
			v[2] = material->ShaderVec4[paramID].Z;
			v[3] = material->ShaderVec4[paramID].W;

			if (vertexShader == true)
				matRender->setShaderVariable(uniform.UniformShaderID, v, uniform.SizeOfUniform, video::EST_VERTEX_SHADER);
			else
				matRender->setShaderVariable(uniform.UniformShaderID, v, uniform.SizeOfUniform, video::EST_PIXEL_SHADER);

			break;
		}
		case TEXTURE_MIPMAP_COUNT:
		{
			SMaterial *material = shaderManager->getCurrentMaterial();
			int textureID = (int)uniform.Value[0];
			ITexture *texture = material->TextureLayer[textureID].Texture;

			if (texture != NULL)
			{
				float count = 1;

				if (texture->hasMipMaps() == true)
				{
					const core::dimension2du& size = texture->getSize();
					int width = size.Width;
					while (width > 2)
					{
						width = width >> 1;
						count++;
					}
				}

				if (vertexShader == true)
					matRender->setShaderVariable(uniform.UniformShaderID, &count, uniform.SizeOfUniform, video::EST_VERTEX_SHADER);
				else
					matRender->setShaderVariable(uniform.UniformShaderID, &count, uniform.SizeOfUniform, video::EST_PIXEL_SHADER);
			}
			else
			{
				float count = 11.0f;

				if (vertexShader == true)
					matRender->setShaderVariable(uniform.UniformShaderID, &count, uniform.SizeOfUniform, video::EST_VERTEX_SHADER);
				else
					matRender->setShaderVariable(uniform.UniformShaderID, &count, uniform.SizeOfUniform, video::EST_PIXEL_SHADER);
			}
		}
		break;
		/*
		case OBJECT_PARAM:
		{
		}
		break;
		case BPCEM_MIN:
		case BPCEM_MAX:
		{
		}
		break;
		case BPCEM_POS:
		{
		}
		break;
		case FOG_PARAMS:
		{
		}
		break;
		case SSAO_KERNEL:
		{
		}
		break;
		case DEFERRED_VIEW:
		{
		}
		break;
		case DEFERRED_PROJECTION:
		{
		}
		break;
		case CUSTOM_VALUE:
		{
			// todo
			// set params in callback
		}
		break;
		*/
		default:
			return false;
		}
		return true;
	}
}