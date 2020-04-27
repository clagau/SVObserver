//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeBufferCreator
//* .File Name       : $Workfile:   SVMatroxGigeBufferCreator.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:39:12  $
//******************************************************************************

template <SVMatroxBufferAttributeEnum Attribute>
SVMatroxGigeBufferCreator<Attribute>::SVMatroxGigeBufferCreator()
{
}

template <SVMatroxBufferAttributeEnum Attribute>
SVMatroxGigeBufferCreator<Attribute>::~SVMatroxGigeBufferCreator()
{
}

template <SVMatroxBufferAttributeEnum Attribute>
HRESULT SVMatroxGigeBufferCreator<Attribute>::Create(const SVMatroxSystem& rSystem, unsigned long p_ulWidth, unsigned long p_ulHeight, unsigned long p_ulFormat, SVMatroxGigeBufferStruct& rBufferStruct)
{
	HRESULT l_hr = S_OK;

	rBufferStruct.m_Buffer = new SVMatroxBuffer;
	rBufferStruct.m_ulWidth = p_ulWidth;
	rBufferStruct.m_ulHeight = p_ulHeight;
	rBufferStruct.m_ulFormat = p_ulFormat;

	SVMatroxBufferCreateStruct createStruct;
	l_hr = BuildCreateStruct(p_ulWidth, p_ulHeight, p_ulFormat, createStruct);
	
	if (S_OK == l_hr)
	{
		HRESULT l_Code = SVMatroxBufferInterface::Create( rSystem, *(rBufferStruct.m_Buffer.get()), createStruct);
		if (l_Code == S_OK)
		{
			l_Code = SVMatroxBufferInterface::GetHostAddress(&(rBufferStruct.m_ulAddress), *(rBufferStruct.m_Buffer.get()));
		}
		l_hr = l_Code;
	}
	return l_hr;
}

template <SVMatroxBufferAttributeEnum Attribute>
HRESULT SVMatroxGigeBufferCreator<Attribute>::BuildCreateStruct(unsigned long p_ulWidth, unsigned long p_ulHeight, unsigned long p_ulFormat, SVMatroxBufferCreateStruct& rCreateStruct)
{
	HRESULT l_hr = S_OK;

	rCreateStruct.m_eAttribute = Attribute;

	rCreateStruct.m_lSizeX = p_ulWidth;
	rCreateStruct.m_lSizeY = p_ulHeight;

	switch (p_ulFormat)
	{
		case SvDef::SVImageFormatMono8:
			rCreateStruct.m_lSizeBand = 1;
			rCreateStruct.m_eType = SV8BitUnsigned;
			break;

		case SvDef::SVImageFormatBGR888:
		case SvDef::SVImageFormatBGR888X:
			rCreateStruct.m_lSizeBand = 3;
			rCreateStruct.m_eType = SV8BitUnsigned;
			break;

		default:
			l_hr = S_FALSE;
			break;
	}
	return l_hr;
}

template <SVMatroxBufferAttributeEnum Attribute>
HRESULT SVMatroxGigeBufferCreator<Attribute>::Destroy(SVMatroxGigeBufferStruct& rBufferStruct)
{
	HRESULT l_hr = S_OK;

	if( nullptr != rBufferStruct.m_Buffer.get() )
	{
		rBufferStruct.m_Buffer.get()->clear();
	}
	else
	{
		l_hr = S_FALSE;
	}

	return l_hr;
}

