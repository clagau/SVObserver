#pragma region Includes
#include "StdAfx.h"
#include "MonitorEntry.h"
#include "ObjectInterfaces\SVObjectTypeInfoStruct.h"
#include "SVMatroxLibrary\MatroxImageProps.h"
#pragma endregion Includes

namespace SvSml 
{
	MonitorEntry::MonitorEntry()
	{
		ObjectType = 0;		//<SVObjectTypeEnum
		InspectionStoreId = -1;
		ItemId = -1;	//<Index in the Inspection Store
		Store_Offset = 0;  ///offset in Inspection Store

		//Images 
		sizeX = 0;
		sizeY = 0;
		PitchByte = 0;
		Matrox_type = 0;
		Attrib = 0;
		BandSize = 0;
		ByteSize = 0;
	};

	MonitorEntry::MonitorEntry(const SVString& na, DWORD ty)
	{
		name = na;
		ObjectType = ty;
		InspectionStoreId = -1;
		ItemId = -1;	//<Index in the Inspection Store
		Store_Offset = 0;  ///offset in Inspection Store
		Pitch = 0;
		sizeX = 0;
		sizeY = 0;
		PitchByte = 0;
		Matrox_type = 0;
		Attrib = 0;
		BandSize = 0;
		ByteSize = 0;

	};

	MonitorEntry::MonitorEntry(const ShMonitorEntry &rentry)
	{
		name = rentry.name.c_str();
		ObjectType = rentry.ObjectType;
		InspectionStoreId = rentry.InspectionStoreId; ///no store Id in ShMonitorEntry
		ItemId = rentry.ItemId;
		Store_Offset = rentry.Store_Offset;
		//Images 
		Pitch = rentry.Pitch;
		sizeX = rentry.sizeX;
		sizeY = rentry.sizeY;
		PitchByte = rentry.PitchByte;
		Matrox_type = rentry.Matrox_type;
		Attrib = rentry.Attrib;

		BandSize = rentry.BandSize;
		ByteSize = rentry.ByteSize;
	}

	bool MonitorEntry::GetMatroxImageProps(MatroxImageProps &ImageProps)
	{
		if (ObjectType != SVImageObjectType)
			return false;

		ImageProps.Pitch = Pitch;
		ImageProps.sizeX = sizeX;
		ImageProps.sizeY = sizeY;
		ImageProps.PitchByte = PitchByte;
		ImageProps.Matrox_type = Matrox_type;
		ImageProps.Attrib = Attrib;

		ImageProps.Bandsize = BandSize;
		ImageProps.Bytesize = ByteSize;
		return true;

	}
	void MonitorEntry::SetMatroxImageProps(const MatroxImageProps  &rImageProps)
	{
		ObjectType = SVImageObjectType;
		Pitch = rImageProps.Pitch;
		sizeX = rImageProps.sizeX;
		sizeY = rImageProps.sizeY;
		PitchByte = rImageProps.PitchByte;
		Matrox_type = rImageProps.Matrox_type;
		Attrib = rImageProps.Attrib;

		ByteSize = rImageProps.Bytesize;
		BandSize = rImageProps.Bandsize;
	}

	ShMonitorEntry::ShMonitorEntry(const void_allocator &allocator) :name(allocator)
	{
		ObjectType = 0;		//<SVObjectTypeEnum
		ItemId = -1;	//<Index in the Inspection Store
		Store_Offset = 0;  ///offset in Inspection Store

		sizeX = 0;
		sizeY = 0;
		PitchByte = 0;
		Matrox_type = 0;
		Attrib = 0;
		BandSize = 0;
		ByteSize = 0;
	}
	ShMonitorEntry::ShMonitorEntry(const void_allocator &allocator, const MonitorEntry &rentry) :name(rentry.name.c_str(), allocator)
	{
		ObjectType = rentry.ObjectType;
		InspectionStoreId = rentry.InspectionStoreId; ///no store Id in ShMonitorEntry
		ItemId = rentry.ItemId;

		Store_Offset = rentry.Store_Offset;
		//Images 
		Pitch = rentry.Pitch;
		sizeX = rentry.sizeX;
		sizeY = rentry.sizeY;
		PitchByte = rentry.PitchByte;
		Matrox_type = rentry.Matrox_type;
		Attrib = rentry.Attrib;

		ByteSize = rentry.ByteSize;
		BandSize = rentry.BandSize;
	};

}
