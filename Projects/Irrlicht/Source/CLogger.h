// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_LOGGER_H_INCLUDED__
#define __C_LOGGER_H_INCLUDED__

#include "ILogger.h"
#include "irrOS.h"
#include "irrString.h"
#include "IEventReceiver.h"

namespace irr
{

//! Class for logging messages, warnings and errors to stdout
class CLogger : public ILogger
{
public:

	CLogger(IEventReceiver* r);

	//! Returns the current set log level.
	virtual ELOG_LEVEL getLogLevel() const _IRR_OVERRIDE_;

	//! Sets a new log level.	virtual void setLogLevel(ELOG_LEVEL ll) _IRR_OVERRIDE_;
	virtual void setLogLevel(ELOG_LEVEL ll) _IRR_OVERRIDE_;

	//! Prints out a text into the log
	virtual void log(const c8* text, ELOG_LEVEL ll=ELL_INFORMATION) _IRR_OVERRIDE_;

	//! Prints out a text into the log
	virtual void log(const wchar_t* text, ELOG_LEVEL ll=ELL_INFORMATION) _IRR_OVERRIDE_;

	//! Prints out a text into the log
	virtual void log(const c8* text, const c8* hint, ELOG_LEVEL ll=ELL_INFORMATION) _IRR_OVERRIDE_;

	//! Prints out a text into the log
	virtual void log(const c8* text, const wchar_t* hint, ELOG_LEVEL ll=ELL_INFORMATION) _IRR_OVERRIDE_;

	//! Prints out a text into the log
	virtual void log(const wchar_t* text, const wchar_t* hint, ELOG_LEVEL ll=ELL_INFORMATION) _IRR_OVERRIDE_;

	//! Sets a new event receiver
	void setReceiver(IEventReceiver* r);

private:

	ELOG_LEVEL LogLevel;
	IEventReceiver* Receiver;
};

} // end namespace

#endif

