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
using System.Net;
using System.Net.Sockets;
using Newtonsoft.Json;


namespace UI
{
    public partial class Form1 : Form
    {

        public Form1()
        {
            InitializeComponent();
            
        }

       private void button1_Click(object sender, EventArgs e)
        {
        }
        string hostIP = Host.IP; //Anhson
        //string hostIP = "192.168.217.149"; // Leduy
        //string hostIP = "192.168.242.149"; // Doan
        private void Form1_Load(object sender, EventArgs e)
        {
            pictureBox1.Image = Image.FromFile("D:\\university\\ky7_zz\\doAnDoLuong\\code_main\\UI\\image_show_load\\okr.jpg");
            //pictureBox1.Image = Image.FromFile("D:\\university\\ky7_zz\\doAnDoLuong\\code_main\\UI\\image_show_load\\okr.jpg");
            var client = new UdpClient();
            var serverEndpoint = new IPEndPoint(IPAddress.Parse(hostIP), Host.port);
            client.Send(Encoding.UTF8.GetBytes("CSharp"), 6, serverEndpoint);

            //IPEndPoint clientEndPoint = new IPEndPoint(IPAddress.Any, 65232);  // Listen on the fixed client port
            //byte[] receivedData = client.Receive(ref clientEndPoint);
            //string receivedMessage = Encoding.UTF8.GetString(receivedData);
            //Console.WriteLine("Received from server: " + receivedMessage);
            client.Close();
            warningLabel.Hide();
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {

        }

        private void button4_Click(object sender, EventArgs e)
        {
            var client = new UdpClient();
            var serverEndpoint = new IPEndPoint(IPAddress.Parse(hostIP), Host.port);
            client.Send(Encoding.UTF8.GetBytes("verify"), 6, serverEndpoint);

            IPEndPoint clientEndPoint = new IPEndPoint(IPAddress.Any, Winform.port);  // Listen on the fixed client port
            byte[] receivedData = client.Receive(ref clientEndPoint);
            client.Close();
            string receivedMessage = Encoding.UTF8.GetString(receivedData);
            string[] parts = receivedMessage.Split(new[] { ',' }, 2);
            string bo = parts[0];
            string Path = "D:\\university\\ky7_zz\\doAnDoLuong\\code_main\\" + parts[1];
            pictureBox1.Image = Image.FromFile(Path);
            if(bo == "f")
            {
                warningLabel.Text = "Sending image fail, please check the internet connection";
                warningLabel.BackColor = Color.Red;
                warningLabel.Show();
            }
            else if(bo == "n")
            {
                warningLabel.Text = "STRANGER WARNING!!!";
                warningLabel.BackColor = Color.Red;
                warningLabel.Show();
            }else
            {
                warningLabel.Text = "Member: " + bo + " verified";
                warningLabel.BackColor = Color.Green;
                warningLabel.Show();
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Form2 passForm = new Form2();

            passForm.ShowDialog();
            //var client = new UdpClient();
            //var serverEndpoint = new IPEndPoint(IPAddress.Parse(hostIP), 3333);
            //client.Send(Encoding.UTF8.GetBytes("adding"), 6, serverEndpoint);

            //IPEndPoint clientEndPoint = new IPEndPoint(IPAddress.Any, 6523);  // Listen on the fixed client port
            //byte[] receivedData = client.Receive(ref clientEndPoint);
            //client.Close();
            //string receivedMessage = Encoding.UTF8.GetString(receivedData);
            //string[] parts = receivedMessage.Split(new[] { ',' }, 2);
            //string bo = parts[0];
            //string Path = parts[1];
            //pictureBox1.Image = Image.FromFile(Path);
            //if(bo == "y")
            //{
            //    string json = File.ReadAllText("D:\\university\\ky7_zz\\doAnDoLuong\\code_main\\referenceData.json");
            //    warningLabel.Text = "adding to reference database";
            //    warningLabel.Show();
            //    //var element = JsonSerializer.Deserialize<List<Element>>(json);
            //}
            //else
            //{
            //    warningLabel.Text = "There is no face in the picture";
            //    warningLabel.Show();
            //}
        }
        private void Form1_TextChanged(object sender, EventArgs e)
        {
        }

        private void button3_Click(object sender, EventArgs e)
        {
            string url = "https://drive.google.com/drive/folders/1tlXlivK0ZYbZHVDvB0XnQSwmIOk26B_L"; // Replace with your desired URL
            try
            {
                Process.Start(new ProcessStartInfo(url) { UseShellExecute = true });
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Unable to open link. Error: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
    }
}
