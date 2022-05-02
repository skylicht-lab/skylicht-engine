/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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

#include "Serializable/CObjectSerializable.h"

namespace Skylicht
{
	namespace Editor
	{
		enum EHistory
		{
			Create,
			Modify,
			Delete
		};

		struct SHistoryData
		{
			EHistory History;
			std::vector<std::string> Container;
			std::vector<std::string> ObjectID;
			std::vector<CObjectSerializable*> DataModified;
			std::vector<CObjectSerializable*> Data;
		};

		class CHistory
		{
		protected:
			std::vector<SHistoryData*> m_history;
			std::vector<SHistoryData*> m_redo;

		public:
			CHistory();

			virtual ~CHistory();

			void clearHistory();

			void clearRedo();

			void addHistory(EHistory history,
				std::vector<std::string>& container,
				std::vector<std::string>& id,
				std::vector<CObjectSerializable*>& dataModified,
				std::vector<CObjectSerializable*>& data);

			virtual void undo() = 0;

			virtual void redo() = 0;
		};
	}
}