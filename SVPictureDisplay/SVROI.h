//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVROI
//* .File Name       : $Workfile:   SVROI.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Jun 2014 16:28:56  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <map>
#include "SVPictureDisplayIdl.h"
#include "SVLibrary/DrawObject.h" // for ParamMap typedefs
#pragma endregion Includes

#pragma region Declarations
// The dispinterface uuid
const IID BASED_CODE IID_ISVROI =
{0xB5419935, 0x769D, 0x4527, {0x9D, 0x9B, 0x27, 0x76, 0xA1, 0x46, 0xFD, 0x4C}};
#pragma endregion Declarations

class SVROI : public CCmdTarget
{
public:
#pragma region enum
	enum 
	{
		dispidSetOverlay = 24L,
		dispidGroup = 1
	};
#pragma endregion enum

#pragma region Constructor
	SVROI();
	virtual ~SVROI();
#pragma endregion Constructor

	virtual void OnFinalRelease() override;

	static SVROI* GetCppObjectFromDispatchPointer(LPDISPATCH lpDispatch);

	// Implementation
public:
	HRESULT SetOverlay(VARIANT* pParameterList, VARIANT* pParameterValue, LONG bAllowEdit);

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	DECLARE_DYNCREATE(SVROI)
	DECLARE_OLECREATE(SVROI)

	// Generated message map functions
	//{{AFX_MSG(SVROI)
	//}}AFX_MSG

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(SVROI)
	//ROIType GetROIType() const;
	long GetParent() const;
	VariantParamMap GetParameterMap() const;
	//}}AFX_DISPATCH

	void ClearValues();

protected:
#pragma region Member Variables
	long m_Parent;
	VariantParamMap m_ParameterMap;
#pragma endregion Member Variables
};

