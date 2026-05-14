///////////////////////////////////////////////////////////////////////////////
// QPoptCtrl.h

#ifndef __QPOPTCTRL_H__
#define __QPOPTCTRL_H__

#include <atlbase.h>
#include <atlcom.h>


#include "resource.h"       // main symbols
#include "QPoptW32.h"
#include "TcBase.h"
#include "QPoptClassFactory.h"
#include "TcOCFCtrlImpl.h"

class CQPoptCtrl 
	: public CComObjectRootEx<CComMultiThreadModel>
	, public CComCoClass<CQPoptCtrl, &CLSID_QPoptCtrl>
	, public IQPoptCtrl
	, public ITcOCFCtrlImpl<CQPoptCtrl, CQPoptClassFactory>
{
public:
	CQPoptCtrl();
	virtual ~CQPoptCtrl();

DECLARE_REGISTRY_RESOURCEID(IDR_QPOPTCTRL)
DECLARE_NOT_AGGREGATABLE(CQPoptCtrl)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CQPoptCtrl)
	COM_INTERFACE_ENTRY(IQPoptCtrl)
	COM_INTERFACE_ENTRY(ITcCtrl)
	COM_INTERFACE_ENTRY(ITcCtrl2)
END_COM_MAP()

};

#endif // #ifndef __QPOPTCTRL_H__
