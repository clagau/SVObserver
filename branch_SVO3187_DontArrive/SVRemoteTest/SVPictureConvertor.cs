//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPictureConvertor
//* .File Name       : $Workfile:   SVPictureConvertor.cs  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   28 Jan 2015 13:36:16  $
//******************************************************************************
using System;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Security;
using stdole;

#region Ole Native Methods
internal static class OLENativeMethods
{
    #region PInvoke Signatures
    [DllImport("oleaut32.dll")]
    public static extern int OleLoadPicture([In, MarshalAs(UnmanagedType.Interface)] IStream pStorage, Int32 lSize, [In, MarshalAs(UnmanagedType.Bool)] bool fRunMode, ref Guid riid, [Out, MarshalAs(UnmanagedType.IUnknown)] out object ppvObj);
    #endregion
}
#endregion

public class SVPictureConvertor
{
    #region Member Variables
    // ImageConverter object used to convert byte arrays containing JPEG or PNG file images into 
    //  Bitmap objects. This is static and only gets instantiated once.
    private static readonly ImageConverter m_imgConv = new ImageConverter();
    #endregion Member Variables

    #region Image To IPicture
    static public stdole.StdPicture ImageToIPicture(System.Drawing.Image image)
    {
        stdole.StdPicture picture = null;

        if (null != image)
        {
            // Copy Image to memory stream
            SVMemoryStream memStream = new SVMemoryStream();

            image.Save(memStream, System.Drawing.Imaging.ImageFormat.Bmp);

            memStream.Seek(0, SeekOrigin.Begin);
            Int32 size = (Int32)memStream.Length;

            object a = null;
            Guid guid = typeof(stdole.IPicture).GUID;
            int ret = OLENativeMethods.OleLoadPicture(memStream, size, false, ref guid, out a);
            if (ret == 0)
            {
                picture = (stdole.StdPicture)a;
            }

            memStream.Close();
        }
        return picture;
    }
    #endregion

    #region IPictureToImage
    static public System.Drawing.Image IPictureToImage(stdole.IPictureDisp picture)
    {
        System.Drawing.Image image = null;

        if (picture != null)
        {
            IntPtr zero = IntPtr.Zero;

            stdole.IPicture pict = (stdole.IPicture)picture;
            int pictureType = picture.Type;
            if (pictureType == 1)
            {
                try
                {
                    zero = (IntPtr)picture.hPal;
                }
                catch (COMException)
                {
                    System.Diagnostics.Trace.WriteLine("SVPictureConvertor.IPictureToImage - COM Exception Occurred");
                }
            }
            image = GetPictureFromParams(pict, (IntPtr)picture.Handle, pictureType, zero, picture.Width, picture.Height);
        }
        else
        {
            System.Diagnostics.Trace.WriteLine("SVPictureConvertor.IPictureToImage stdole.StdPicture picture is null");
        }
        return image;
    }
    #endregion

    #region GetPictureFromParams
    private static Image GetPictureFromParams(object pict, IntPtr handle, int type, IntPtr paletteHandle, int width, int height)
    {
        switch (type)
        {
            case -1:
                return null;

            case 0:
                return null;

            case 1:
                {
                    if (handle == IntPtr.Zero)
                    {
                        System.Diagnostics.Trace.WriteLine("SVPictureConvertor.GetPictureFromParams - Bitmap Handle is NULL");
                    }
                    Image image = Image.FromHbitmap(handle, paletteHandle);
                    if (image == null)
                    {
                        System.Diagnostics.Trace.WriteLine("SVPictureConvertor.GetPictureFromParams - Image.FromHbitmap returned NULL");
                    }
                    return image;
                }

            /*case 2:
                {
                    WmfPlaceableFileHeader wmfHeader = new WmfPlaceableFileHeader();
                    wmfHeader.BboxRight = (short)width;
                    wmfHeader.BboxBottom = (short)height;
                    return (Image)new Metafile(handle, wmfHeader, false).Clone();
                }*/
            case 3:
                return (Image)Icon.FromHandle(handle).Clone();

            //case 4:
            //  return (Image)new Metafile(handle, false).Clone();
        }
        throw new ArgumentException("Unable to convert");
    }
    #endregion

    #region GetPictureFromByteArray
    static public Image GetPictureFromByteArray(byte[] bytes, ImageFormat imgFormat)
    {
        // GetImage as IPictureDisp doesn't work in SVRemteControl
        //return IPictureToImage( (stdole.IPictureDisp)(img.GetImage(false, 1.0, SVRemoteControlLib.SVImageFormatsEnum.IDisp)) );

        // The memory stream can't be closed until the image is destroyed (image destructor handles this)
        //System.IO.MemoryStream stream = new System.IO.MemoryStream(bytes);
        //return Image.FromStream(stream, true, false);

        // Use ImageConvertor instead to avoid memory stream issue.
        // This stil has the stream issue, it just hides it..
        return (Image)m_imgConv.ConvertFrom(bytes);
    }
    #endregion GetPictureFromByteArray
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVRemoteControl\SVRemoteTest\SVPictureConvertor.cs_v  $
 * 
 *    Rev 1.2   28 Jan 2015 13:36:16   sjones
 * Project: SVRemoteControl
 * Change Request (SCR) nbr: 21
 * SCR Title: Add Fail Status Streaming
 * Checked in by: sJones; Steve Jones
 * Change Description: Added PVCS header and footer
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
