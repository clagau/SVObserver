//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameValueObjectClass
//* .File Name       : $Workfile:   SVFileNameValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:38:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "FilesystemUtilities/FileHelper.h"
#include "SVStringValueObjectClass.h"
#pragma endregion Includes

namespace SvVol
{

class SVFileNameValueObjectClass : public SVStringValueObjectClass
{
	SV_DECLARE_CLASS

public:
	explicit SVFileNameValueObjectClass( LPCTSTR ObjectName );
	explicit SVFileNameValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVFILENAMEVALUEOBJECT );
	explicit SVFileNameValueObjectClass( const SVFileNameValueObjectClass& rhs);
	SVFileNameValueObjectClass& operator= ( const SVFileNameValueObjectClass& rhs );

	virtual ~SVFileNameValueObjectClass();

	virtual HRESULT SetValue( const std::string& rValue, int Index = -1 ) override;
	HRESULT SetDefaultValue( const std::string& rValue, bool bResetAll = true ) override;

protected:

private:
	void LocalInitialize();

	SvFs::FileHelper m_FileName;
};

} //namespace SvVol
