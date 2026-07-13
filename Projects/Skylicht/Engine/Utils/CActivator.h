/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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

#include "Utils/CSingleton.h"

/**
 * @brief Register an activator type with its default constructor.
 *
 * The registered type must derive from `Skylicht::IActivatorObject` and be
 * default-constructible. The type is registered under its C++ type name string.
 *
 * @param type Concrete class type to register.
 */
#define ACTIVATOR_REGISTER(type)  \
	IActivatorObject* type##CreateFunc() { return new type(); } \
	bool type##_activator = CActivator::createGetInstance()->registerType(#type, &type##CreateFunc)

 /**
  * @brief Declare a virtual `getTypeName` implementation returning the class name.
  * @param type Concrete class type name.
  */
#define DECLARE_GETTYPENAME(type) \
	virtual const char* getTypeName() {return #type;}

namespace Skylicht
{
	/**
	 * @brief Base interface for objects created by `CActivator`.
	 * @ingroup Utilities
	 */
	class SKYLICHT_API IActivatorObject
	{
	public:
		/**
		 * @brief Virtual destructor for polymorphic activator objects.
		 */
		virtual ~IActivatorObject()
		{

		}
	};

	/**
	 * @brief Function pointer used to construct an activator object.
	 * @return Newly allocated activator object. The caller owns the returned pointer.
	 */
	typedef IActivatorObject* (*ActivatorCreateInstance)();

	/**
	 * @brief Runtime factory that creates registered objects by type name.
	 * @ingroup Utilities
	 */
	class SKYLICHT_API CActivator
	{
	public:
		DECLARE_SINGLETON(CActivator)

	protected:
		std::map<std::string, int> m_factoryName;
		std::vector<ActivatorCreateInstance> m_factoryFunc;

	public:
		/**
		 * @brief Register a type name and constructor callback.
		 * @param type Type name used as the factory key.
		 * @param func Constructor callback.
		 * @return True if the type was registered, false if the name already exists.
		 */
		bool registerType(const char* type, ActivatorCreateInstance func);

		/**
		 * @brief Create an instance of a registered type.
		 * @param type Type name used during registration.
		 * @return Newly allocated object, or null if the type is not registered.
		 */
		IActivatorObject* createInstance(const char* type);
	};
}
