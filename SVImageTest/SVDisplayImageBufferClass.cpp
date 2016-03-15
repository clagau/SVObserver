// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDisplayImageBufferClass
// * .File Name       : $Workfile:   SVDisplayImageBufferClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:08:32  $
// ******************************************************************************

#include "stdafx.h"
#include "SVDisplayImageBufferClass.h"
#include "SVHBitmapUtilitiesLibrary\SVImageFormatEnum.h"

SVDisplayImageBufferClass::SVDisplayImageBufferClass()
{
	m_pucImageData = NULL;
}

SVDisplayImageBufferClass::~SVDisplayImageBufferClass()
{
	DestroyDisplayBuffer();
}

bool SVDisplayImageBufferClass::empty() const
{
	return ( m_pucImageData == NULL );
}

void SVDisplayImageBufferClass::clear()
{
	DestroyDisplayBuffer();
}

SVPOINT SVDisplayImageBufferClass::GetPositionPoint() const
{
	return SVPOINT();
}

SVBitmapInfo SVDisplayImageBufferClass::GetBitmapInfo() const
{
	return m_BitmapInfo;
}

unsigned char* SVDisplayImageBufferClass::GetBufferAddress() const
{
	return m_pucImageData;
}

HRESULT SVDisplayImageBufferClass::GetData( SVImageBufferHandleData& p_rData ) const
{
	return E_FAIL;
}

bool SVDisplayImageBufferClass::UpdateDisplayBufferInfo( long bufWidth, long bufHeight, int iFormat )
{
	bool l_bOk = DestroyDisplayBuffer();

	if( l_bOk )
	{
		WORD wClrBits = 8;
		
		if ( iFormat == SVImageFormatRGB888 )
			wClrBits = 24;
		else if ( iFormat == SVImageFormatRGB8888 )
			wClrBits = 32;
	
		m_BitmapInfo.Assign( bufWidth, bufHeight, wClrBits, SVBitmapInfo::GetDefaultColorTable( wClrBits ) );

		l_bOk = !( m_BitmapInfo.empty() );

		if( l_bOk )
		{
			m_pucImageData = new unsigned char[ m_BitmapInfo.GetBitmapImageSizeInBytes() ];

			l_bOk = m_pucImageData != NULL;
		}

		if( l_bOk )
		{
			switch( iFormat )
			{
				case  SVImageFormatMono8:
				{
					UINT32 i,j,Width,Height;
					UINT8  Value,*pMem;

					Width=abs( m_BitmapInfo.GetWidth() );
					Height=abs( m_BitmapInfo.GetHeight() );
					pMem=m_pucImageData;

					for(i=0;i<Height;i++)
					{
						Value=(UINT8)(i&0xFF);
						for(j=0;j<Width;j++)
						 *pMem++=Value++;
					}

					break;
				}
				case SVImageFormatRGB888 :
				{
					UINT32  i,j,Width,Height;
					UINT32  Pattern,Offset;
					UINT32  StartPattern;
					UINT8  *pMem;

					Width=abs( m_BitmapInfo.GetWidth() );
					Height=abs( m_BitmapInfo.GetHeight() );

					// Clear whole memory
					memset(m_pucImageData,0,Width*Height*3);
					pMem=m_pucImageData;
					StartPattern=0;

					for(i=0;i<Height;i++)
					{
						Pattern=StartPattern;
						Offset=(Pattern>>8)%3;

						for(j=0;j<Width;j++)
						{
							pMem[Offset]=(UINT8)(Pattern&0xFF);
							pMem+=3;

							Pattern+=2;
							Offset=(Pattern>>8)%3;
						}

						StartPattern+=2;
					}

					break;
				}
				case SVImageFormatRGB8888 :
				{
					UINT32  i,j,Width,Height;
					UINT32  Pattern,Offset;
					UINT32  StartPattern;
					UINT8  *pMem;

					Width=abs( m_BitmapInfo.GetWidth() );
					Height=abs( m_BitmapInfo.GetHeight() );

					// Clear whole memory
					memset(m_pucImageData,0,Width*Height*4);
					pMem=m_pucImageData;
					StartPattern=0;

					for(i=0;i<Height;i++)
					{
						Pattern=StartPattern;
						Offset=(Pattern>>8)%4;

						for(j=0;j<Width;j++)
						{
							pMem[Offset]=(UINT8)(Pattern&0xFF);
							pMem+=4;

							Pattern+=2;
							Offset=(Pattern>>8)%4;
						}

						StartPattern+=2;
					}

					break;
				}
				case SVImageFormatUnknown :
				{
					break;
				}
				default:
				{
					break;
				}
			}
		}
	}

	if( ! l_bOk )
	{
		DestroyDisplayBuffer();
	}

	return l_bOk;
}

bool SVDisplayImageBufferClass::DestroyDisplayBuffer()
{
	bool l_bOk = true;

	m_BitmapInfo.clear();

	if( m_pucImageData != NULL )
	{
		delete[] m_pucImageData;
		m_pucImageData = NULL;
	}

	return l_bOk;
}

