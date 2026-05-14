///////////////////////////////////////////////////////////////////////////////
// QPoptDriver.cpp
#include "TcPch.h"
#pragma hdrstop

#include "QPoptDriver.h"
#include "QPoptClassFactory.h"

DECLARE_GENERIC_DEVICE(QPOPTDRV)

IOSTATUS CQPoptDriver::OnLoad( )
{
	TRACE(_T("CObjClassFactory::OnLoad()\n") );
	m_pObjClassFactory = new CQPoptClassFactory();

	return IOSTATUS_SUCCESS;
}

VOID CQPoptDriver::OnUnLoad( )
{
	delete m_pObjClassFactory;
}

unsigned long _cdecl CQPoptDriver::QPOPTDRV_GetVersion( )
{
	return( (QPOPTDRV_Major << 8) | QPOPTDRV_Minor );
}

