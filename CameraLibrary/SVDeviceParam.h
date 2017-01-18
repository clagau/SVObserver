// ******************************************************************************
// * COPYRIGHT (c) 2013 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDeviceParam
// * .File Name       : $Workfile:   SVDeviceParam.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:02:04  $
// ******************************************************************************

#pragma once

#pragma warning( disable: 4786 )	// identifier truncation 255 chars

#include "SVLoki/SVVisitor.h"
#include "SVTemplate.h"
#include "SVUtilityLibrary/SVString.h"

enum SVDeviceParamEnum;	// actual definition is in SVDeviceParams.h
enum SVDeviceParamDataTypeEnum;

class SVDeviceParam : public SVClonable, public Loki::BaseVisitable<HRESULT>
{
public:
	SVDeviceParam();
	SVDeviceParam( SVDeviceParamEnum e );
	virtual ~SVDeviceParam();
	static SVDeviceParam* Create(SVDeviceParamEnum eType);
	static CString GetParameterName( SVDeviceParamEnum e );
	SVDeviceParam* Clone() const {return static_cast< SVDeviceParam* >( CloneImpl() );}
	virtual SVClonable* CloneImpl() const = 0;

	virtual HRESULT GetValue( VARIANT& rv ) const;
	virtual HRESULT SetValue( const VARIANT& rv );
	virtual HRESULT SetMetadata(const SVDeviceParam* pBaseParam );

	const char* Name() const {return m_strName.c_str();}
	HRESULT SetName( const SVString& strName);

	const char* VisualName() const {return m_strVisualName.c_str();}
	HRESULT SetVisualName( const SVString& strName);

	const char* Description() const { return m_strDescription.c_str();}
	HRESULT SetDescription( const SVString& strDescription );

	long DetailLevel() const { return m_lDetailLevel;}
	HRESULT SetDetailLevel( long lDetailLevel );

	SVDeviceParamEnum Type() const {return m_eParam;}
	HRESULT SetType( SVDeviceParamEnum e );
	SVDeviceParamDataTypeEnum DataType() const { return m_eDataType; }

	int GetOrder() const {return m_iOrder;}
	void SetOrder(int iOrder) {m_iOrder = iOrder;}

	bool DeviceSupports() const {return m_bSupports;}
	bool Supports() const {return m_bSupports;}
	void Supports(bool bSupports) {m_bSupports = bSupports;}

	DEFINE_VISITABLE()

protected:
	void Init( const SVDeviceParam& rhs ) { m_eParam = rhs.m_eParam; }
	virtual void Init( SVDeviceParamEnum e );
	SVDeviceParamEnum m_eParam;
	SVDeviceParamDataTypeEnum m_eDataType;
	SVString m_strName;
	SVString m_strVisualName;
	SVString m_strDescription;
	int m_iOrder;
	long m_lDetailLevel;
	bool m_bSupports;
	
// disable assignment and copy constructor
protected:
	SVDeviceParam(const SVDeviceParam& );

private:
	const SVDeviceParam& operator = (const SVDeviceParam& );
};

typedef TTemporaryPointerWrapper<SVDeviceParam> SVDeviceParamTempWrapper;
typedef TValueSemantics<SVDeviceParam> SVDeviceParamWrapper;


