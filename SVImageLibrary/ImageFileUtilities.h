//*****************************************************************************
/// \copyright COPYRIGHT (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Contains declarations of utility functions for image file operations
//******************************************************************************


/// \param FilePath full file path of the bitmap file 
/// \param Context device context from which the bitmap file is to be created
/// \param Area defines the size of the bitmap file
/// \param BitsPerPixel (self-explanatory)
/// \returns true if the bitmap file was successfully created, otherwise false.
bool CreateBmpFileFromHdc(const char* FilePath, HDC Context, RECT Area, uint16_t BitsPerPixel = 24);
