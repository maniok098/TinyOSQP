///////////////////////////////////////////////////////////////////////////////
// QPoptDriver.h

#ifndef __QPOPTDRIVER_H__
#define __QPOPTDRIVER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TcBase.h"

#define QPOPTDRV_NAME        "QPOPT"
#define QPOPTDRV_Major       1
#define QPOPTDRV_Minor       0

#define DEVICE_CLASS CQPoptDriver

#include "ObjDriver.h"

class CQPoptDriver : public CObjDriver
{
public:
	virtual IOSTATUS	OnLoad();
	virtual VOID		OnUnLoad();

	//////////////////////////////////////////////////////
	// VxD-Services exported by this driver
	static unsigned long	_cdecl QPOPTDRV_GetVersion();
	//////////////////////////////////////////////////////
	
};

Begin_VxD_Service_Table(QPOPTDRV)
	VxD_Service( QPOPTDRV_GetVersion )
End_VxD_Service_Table


#endif // ifndef __QPOPTDRIVER_H__