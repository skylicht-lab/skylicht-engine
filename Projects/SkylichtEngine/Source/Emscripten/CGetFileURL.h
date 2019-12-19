#ifndef _EM_GETFILE_H_
#define _EM_GETFILE_H_

namespace Skylicht
{
	class CGetFileURL
	{
	public:
		enum EMethod
		{
			Get = 0,
			Post
		};

		enum EState
		{
			None,
			Downloading,
			Finish,
			Error
		};

	private:
		std::string m_params;
		std::string m_url;
		std::string m_fileName;

		EState m_state;
		int m_errorCode;

	public:
		CGetFileURL(const char *url, const char *fileName);

		~CGetFileURL();

		void setParams(const char *params)
		{
			m_params = params;
		}

		void download(EMethod method);

		void onLoad(unsigned int size, const char *path);

		void onStatus(unsigned int size, int status);

		void onProcess(unsigned int size, int percent);

		inline EState getState()
		{
			return m_state;
		}

		inline int getErrorCode()
		{
			return m_errorCode;
		}
	};
}

#endif