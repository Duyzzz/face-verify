using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Diagnostics;
using System.IO;
using Newtonsoft.Json.Linq;

namespace UI
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void VerifyFaceWithReferences(string inputImagePath, List<string> referenceImagePaths)
        {
            ProcessStartInfo start = new ProcessStartInfo();
            start.FileName = "python.exe";  // Path to Python executable
                                            // Build arguments: script, input image, and reference images
            string arguments = string.Format("\"{0}\" \"{1}\"", "verify_faces.py", inputImagePath);
            foreach (var refImage in referenceImagePaths)
            {
                arguments += string.Format(" \"{0}\"", refImage);
            }

            start.Arguments = arguments;
            start.UseShellExecute = false;
            start.RedirectStandardOutput = true;
            start.CreateNoWindow = true;

            using (Process process = Process.Start(start))
            {
                using (StreamReader reader = process.StandardOutput)
                {
                    string result = reader.ReadToEnd();
                    dynamic verificationResult = JObject.Parse(result);  // Parse the JSON result

                    if (verificationResult.error != null)
                    {
                        MessageBox.Show(verificationResult.error.ToString());
                        return;
                    }

                    bool isVerified = verificationResult.verified;

                    // Display the result to the user
                    if (isVerified)
                    {
                        MessageBox.Show("Face verification successful: The input face matches one of the reference faces!");
                    }
                    else
                    {
                        MessageBox.Show("Face verification failed: The input face does not match any reference faces.");
                    }
                }
            }
        }
        private void button1_Click(object sender, EventArgs e)
        {
            // Open file dialog to select the input image
            OpenFileDialog inputFileDialog = new OpenFileDialog();
            inputFileDialog.Filter = "Image Files|*.jpg;*.png;*.bmp;*.gif";
            inputFileDialog.FilterIndex = 1;
            inputFileDialog.Title = "Select an Input Image";

            if (inputFileDialog.ShowDialog() == DialogResult.OK)
            {
                // Get the input image path
                string inputImagePath = inputFileDialog.FileName;

                // Display the selected input image in a PictureBox
                pictureBox1.Image = Image.FromFile(inputImagePath);

                // Now open another dialog for selecting multiple reference images
                OpenFileDialog referenceFileDialog = new OpenFileDialog();
                referenceFileDialog.Filter = "Image Files|*.jpg;*.png;*.bmp;*.gif";
                referenceFileDialog.FilterIndex = 1;
                referenceFileDialog.Title = "Select Reference Images";
                referenceFileDialog.Multiselect = true;  // Allow selecting multiple reference images

                if (referenceFileDialog.ShowDialog() == DialogResult.OK)
                {
                    // Get all selected reference image paths
                    string[] referenceImagePaths = referenceFileDialog.FileNames;

                    // Call the face verification method and pass in the input image and reference images
                    VerifyFaceWithReferences(inputImagePath, referenceImagePaths.ToList());
                }
            }

        }

    }
}
