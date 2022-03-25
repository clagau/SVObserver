//******************************************************************************
/// COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Form for Displaying/Loading an Image
//******************************************************************************
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace SVRemoteTest
{
    public partial class ImageDisplayForm : Form
    {
        #region Member Variables
        private Image m_Image = null;
        #endregion member variables

        #region Properties
        public Image DisplayedImage
        {
             get { return m_Image; }
        }
        #endregion Properties

        #region Select Image
        private void SelectImage()
        {
            String imageFilename = String.Empty;

            // prompt for directory/format
            OpenFileDialog openDlg = new OpenFileDialog();

            openDlg.FileName = "Image.bmp";
            openDlg.Filter = "Bitmap files (*.bmp)|*.bmp";
            
            if (DialogResult.OK == openDlg.ShowDialog())
            {
                m_Image = Image.FromFile(openDlg.FileName, true);
            }
        }
        #endregion Select Image

        #region OnSaveImage
        private void OnSaveImage(object sender, EventArgs e)
        {
            // prompt for directory/format
            SaveFileDialog savefile = new SaveFileDialog();

            if (pictureBox.Image.RawFormat.Equals(System.Drawing.Imaging.ImageFormat.Png))
            {
                // set a default file name
                savefile.FileName = "Image.png";
                savefile.Filter = "PNG files (*.png)|*.png";
            }
            else
            {
                // set a default file name
                savefile.FileName = "Image.bmp";
                savefile.Filter = "Bitmap files (*.bmp)|*.bmp";
            }
            if (DialogResult.OK == savefile.ShowDialog())
            {
                /* 
                System.IO.FileStream stream = new System.IO.FileStream(savefile.FileName, System.IO.FileMode.Create);
                System.Windows.Media.Imaging.BitmapEncoder encoder = new System.Windows.Media.Imaging.BmpBitmapEncoder();
                System.Windows.Media.Imaging.BitmapSource source = SVPictureConvertor.BitmapToSource(pictureBox.Image as Bitmap);
                encoder.Frames.Add(System.Windows.Media.Imaging.BitmapFrame.Create(source, System.Windows.Media.Imaging.BitmapCreateOptions.PreservePixelFormat, System.Windows.Media.Imaging.BitmapCacheOption.None));
                encoder.Save(stream);
                */
                pictureBox.Image.Save(savefile.FileName, pictureBox.Image.RawFormat);
            }
        }
        #endregion OnSaveImage

        #region SetImageToDisplay
        private void SetImageToDisplay()
        {
            pictureBox.Image = m_Image;
            if (null != m_Image)
            {
                // Set maximum size and add scroll bars if greateer
                int w = Math.Min(640, m_Image.Size.Width);
                int h = Math.Min(480, m_Image.Size.Height + 32);
                if (w < m_Image.Size.Width || h < (m_Image.Size.Height + 32))
                {
                    // scrollbars are needed
                }
                this.Size = new Size(w, h);
                /* @Future - when pixel format is retained on both load and save
                ContextMenu mnu = new ContextMenu();
                MenuItem mnuSave = new MenuItem("Save");
                mnuSave.Click += new EventHandler(OnSaveImage);
                mnu.MenuItems.Add(mnuSave);
                pictureBox.ContextMenu = mnu; */
            }
        }
        #endregion SetImageToDisplay

        #region Constructor
        public ImageDisplayForm(string name, Image image, bool bSelect = false)
        {
            this.Text = name;
            m_Image = image;
            InitializeComponent();
            if (bSelect)
            {
                SelectImage();
            }
            SetImageToDisplay();
        }
        #endregion Constructors
    }
}
