#ifndef _CGAMESHADERPARAMS_H_
#define _CGAMESHADERPARAMS_H_

#define MAX_SHADERPARAMS	20

namespace Skylicht
{
	class CShaderParams
	{
	protected:
		SVec4	m_shaderParam[MAX_SHADERPARAMS];

	public:
		CShaderParams();
		virtual ~CShaderParams();

		// getParam
		SVec4& getParam(int i)
		{
			return m_shaderParam[i];
		}

		// getParamData
		float* getParamData(int i)
		{
			return &m_shaderParam[i].X;
		}

		// setShader
		void setShader(int i, SVec4& value)
		{
			m_shaderParam[i] = value;
		}
	};
}

#endif
