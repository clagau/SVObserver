#pragma region Includes
#include "StdAfx.h"
#include "MonitorEntry.h"
#include "ObjectInterfaces\SVObjectTypeInfoStruct.h"
#include "SVMatroxLibrary\MatroxImageProps.h"
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace SvSml
{

	MonitorEntryData::MonitorEntryData()
	{
		ObjectType = 0;		//<SVObjectTypeEnum
		InspectionStoreId = -1;
		ItemId = -1;	//<Index in the Inspection Store
		Store_Offset = 0;  ///offset in Inspection Store

		variant_type = VT_EMPTY; ///vt value from variant
		isArray = FALSE; ;
		wholeArray = FALSE;
		arrayIndex = -1;
		m_MonitorListFlag = 0;


		//Images 
		sizeX = 0;
		sizeY = 0;
		PitchByte = 0;
		Matrox_type = 0;
		Attrib = 0;
		BandSize = 0;
		ByteSize = 0;
		Pitch = 0;
	}

	MonitorEntry::MonitorEntry() :data(), m_Guid(GUID_NULL)
	{

	};

	MonitorEntry::MonitorEntry(const SVString& na) :data(), m_Guid(GUID_NULL)
	{
		name = na;
	};

	MonitorEntry::MonitorEntry(const ShMonitorEntry &rentry) :data(), m_Guid(GUID_NULL)
	{
		name = rentry.name.c_str();
		data = rentry.data;
	}

	bool MonitorEntry::GetMatroxImageProps(MatroxImageProps &ImageProps)
	{
		if (data.ObjectType != SVImageObjectType)
			return false;

		ImageProps.Pitch = data.Pitch;
		ImageProps.sizeX = data.sizeX;
		ImageProps.sizeY = data.sizeY;
		ImageProps.PitchByte = data.PitchByte;
		ImageProps.Matrox_type = data.Matrox_type;
		ImageProps.Attrib = data.Attrib;
		ImageProps.Bandsize = data.BandSize;
		ImageProps.Bytesize = data.ByteSize;
		return true;
	}

	void MonitorEntry::SetMatroxImageProps(const MatroxImageProps  &rImageProps)
	{
		data.ObjectType = SVImageObjectType;
		data.Pitch = rImageProps.Pitch;
		data.sizeX = rImageProps.sizeX;
		data.sizeY = rImageProps.sizeY;
		data.PitchByte = rImageProps.PitchByte;
		data.Matrox_type = rImageProps.Matrox_type;
		data.Attrib = rImageProps.Attrib;
		data.ByteSize = rImageProps.Bytesize;
		data.BandSize = rImageProps.Bandsize;
	}

	bool MonitorEntry::GetValue(SVString& string, BYTE* ptr)
	{
		TCHAR Text[100];
		switch (data.variant_type)
		{
		case VT_I1: //CHAR
		{
			assert(data.ByteSize == 1);
			char* pValue = (char*)ptr;
			sprintf_s(Text, 100, _T("%c"), *pValue);
			string = Text;
			break;
		}
		case VT_UI1: //BYTE
		{
			assert(data.ByteSize == 1);
			BYTE value = *((BYTE*)ptr);
			sprintf_s(Text, 100, _T("0x%2.2x"), value);
			string = Text;
			break;
		}
		case VT_I2:
		{
			short* pShort = (short*)ptr;
			assert(data.ByteSize == 2);
			short   value = *pShort;
			string = std::to_string(value);
			break;
		}
		case VT_UI2:
		{
			assert(data.ByteSize == 2);
			WORD*   pWord = (WORD*)(ptr);
			sprintf_s(Text, 100, _T("0x%4.4x"), *pWord);
			string = Text;
			break;
		}
		case VT_I4:
		{
			int* pInt = (int*)ptr;
			assert(data.ByteSize == 4);
			INT32   value = *pInt;
			string = std::to_string(value);
			break;
		}
		case VT_UI4:
		{
			assert(data.ByteSize == 4);
			DWORD*   pDWord = (DWORD*)(ptr);
			sprintf_s(Text, 100, _T("0x%8.8x"), *pDWord);
			string = Text;
			break;
		}
		case VT_R4:
		{
			assert(data.ByteSize == 4);
			float* pfloat = (float*)ptr;
			string = std::to_string(*pfloat);
			break;
		}
		
		case VT_R8:
		{
			assert(data.ByteSize == 8);
			double* pDouble = (double*)ptr;
			string = std::to_string(*pDouble);
			break;
		}
		case VT_BSTR:
		{
			string.assign((char*)ptr, data.ByteSize - 1);
			break;
		}

		case VT_I8:
		{
			assert(data.ByteSize == 8);
			INT64*  pvalue = (INT64*)(ptr);
			sprintf_s(Text, 100, _T("%I64d"), *pvalue);
			string = Text;
			break;

		}
		case VT_UI8:
		{
			assert(data.ByteSize == 8);
			UINT64* pvalue = (UINT64*)(ptr);
			sprintf_s(Text, 100, _T("%I64d"), *pvalue);
			string = Text;
			break;
		}

		case VT_INT:
		{
			// BasicvalueObject converts VT_INT to VT_BOOL
			assert(data.ByteSize == 4);
			int*  pValue = (int*)(ptr);
			if (*pValue)
				string = "True";
			else
				string = "False";
			break;
		}
		case VT_UINT:
		{
			assert(data.ByteSize == 4);
			DWORD*   pDWord = (DWORD*)(ptr);
			sprintf_s(Text, 100, _T("0x%8.8x"), *pDWord);
			string = Text;
			break;
		}
		
		case VT_BOOL:
		{
			assert(data.ByteSize == 4);
			int*  pValue = (int*)(ptr);
			if (*pValue)
				string = "True";
			else
				string = "False";
			break;
		}
		case  VT_DPOINT:
		{
			assert(data.ByteSize == 16);
			double* px = (double*)(ptr);
			double* py = (double*)(ptr + 8);
			sprintf_s(Text, 100, _T("( %lf, %lf)"), *px, *py);
			string = Text;
			break;
		}
		case VT_POINT:
		{
			assert(data.ByteSize == 8);
			int* px = (int*)(ptr);
			int* py = (int*)(ptr + 4);

			sprintf_s(Text, 100, _T("( %i, %i)"), *px, *py);
			string = Text;
			break;
		}

		default:
			string = "???";
			break;
		}
		return true;
	}
	bool MonitorEntry::GetValue(_variant_t& val, BYTE* offset)
	{
		//@Todo[MEC][7.50] [17.07.2017] not implemented yet
		return false;
	}

	ShMonitorEntry::ShMonitorEntry(const void_allocator &allocator) :name(allocator), data()
	{

	}
	ShMonitorEntry::ShMonitorEntry(const void_allocator &allocator, const MonitorEntry &rentry) : name(rentry.name.c_str(), allocator)
	{
		data = rentry.data;
	};


}
