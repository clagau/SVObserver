//*****************************************************************************
/// \copyright (c) 2023,2023 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SVMatroxImageFile.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class holds an imagebuffer for an image read from file 
//********************************************************************************
#include "stdafx.h"
#include "SVMatroxImageFile.h"
#include "FilesystemUtilities/FilepathUtilities.h"

SVMatroxImageFile::SVMatroxImageFile()
{
    

}

SVMatroxImageFile::~SVMatroxImageFile()
{
    if (m_ImageBuffer)
    {
        MbufFree(m_ImageBuffer);

    }
   
}

SIZE SVMatroxImageFile::GetSize()
{
    SIZE ret {0,0};
    if (m_ImageBuffer != M_NULL)
    {
        ret.cx = static_cast<long>( MbufInquire(m_ImageBuffer, M_SIZE_X, M_NULL));
        ret.cy = static_cast<long>(MbufInquire(m_ImageBuffer, M_SIZE_Y, M_NULL));
           
    }
    return ret;

}

MIL_INT SVMatroxImageFile::GetType()
{
    MIL_INT ret {0};
    if (m_ImageBuffer != M_NULL)
    {
        ret = MbufInquire(m_ImageBuffer, M_TYPE, M_NULL);
    }
    return ret;
}

MIL_INT SVMatroxImageFile::GetSizeBand()
{
    MIL_INT ret {0};
    if (m_ImageBuffer != M_NULL)
    {
        ret = MbufInquire(m_ImageBuffer, M_SIZE_BAND, M_NULL);
    }
    return ret;
}

MIL_INT SVMatroxImageFile::GetSizeBit()
{
    MIL_INT ret {0};
    if (m_ImageBuffer != M_NULL)
    {
        ret = MbufInquire(m_ImageBuffer, M_SIZE_BIT, M_NULL);
    }
    return ret;
}

bool SVMatroxImageFile::Load(LPCSTR filename)
{
    if (m_ImageBuffer)
    {
        MbufFree(m_ImageBuffer);
        m_ImageBuffer = M_NULL;
        m_Filename.clear();
    }
    m_ImageBuffer = MbufImport(filename, M_DEFAULT, M_RESTORE, M_DEFAULT_HOST, M_NULL);
    if (m_ImageBuffer)
    {
        m_Filename = filename;
        return true;
    }
    return false;
}

bool  SVMatroxImageFile::LoadFirstFile(LPCTSTR Name, ImageFileFormat fileFormat)
{
    std::string FileName;

    if (FILE_ATTRIBUTE_DIRECTORY == (::GetFileAttributes(Name) & FILE_ATTRIBUTE_DIRECTORY))
    {
        std::vector<std::string> fileList {SvFs::getFileList(Name, fileFormat, false)};
        auto iter = fileList.begin();
        if (fileList.end() != iter)
        {
            FileName = *iter;
        }
    }
    else
    {
        FileName = Name;
    }

    return Load(FileName.c_str());
}




std::string SVMatroxImageFile::GetDebugString()
{
    auto type = GetType();
    SIZE  size = GetSize();
    
    MIL_INT bit = GetSizeBit();
    MIL_INT band = GetSizeBand();

    
    std::string ret = std::format("{}: X: {} Y : {} Bands: {} Bits {}  Type {} \n ", m_Filename, size.cx, size.cy, band, bit, type);
    return ret;
}

bool SVMatroxImageFile::IsColor()
{
    return GetSizeBand() >= 3;
}
