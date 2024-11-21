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
        
        private void Form1_Load(object sender, EventArgs e)
        {
            pictureBox1.Image = Image.FromFile("D:\\university\\ky7_zz\\doAnDoLuong\\code_main\\UI\\image_show_load\\okr.jpg");
            var client = new UdpClient();
            var serverEndpoint = new IPEndPoint(IPAddress.Parse("192.168.1.162"), 3333);
            client.Send(Encoding.UTF8.GetBytes("CSharp"), 6, serverEndpoint);

            IPEndPoint clientEndPoint = new IPEndPoint(IPAddress.Any, 65232);  // Listen on the fixed client port
            byte[] receivedData = client.Receive(ref clientEndPoint);
            string receivedMessage = Encoding.UTF8.GetString(receivedData);
            Console.WriteLine("Received from server: " + receivedMessage);
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
            var serverEndpoint = new IPEndPoint(IPAddress.Parse("192.168.1.162"), 3333);
            client.Send(Encoding.UTF8.GetBytes("verify"), 6, serverEndpoint);

            IPEndPoint clientEndPoint = new IPEndPoint(IPAddress.Any, 6523);  // Listen on the fixed client port
            byte[] receivedData = client.Receive(ref clientEndPoint);
            client.Close();
            string receivedMessage = Encoding.UTF8.GetString(receivedData);
            string[] parts = receivedMessage.Split(new[] { ',' }, 2);
            string bo = parts[0];
            string Path = "D:\\university\\ky7_zz\\doAnDoLuong\\code_main\\" + parts[1];
            pictureBox1.Image = Image.FromFile(Path);
            if(bo == "n")
            {
                warningLabel.Text = "STRANGER WARNING!!!";
                warningLabel.BackColor = Color.Red;
                warningLabel.Show();
            }else
            {
                warningLabel.Text = "Member: " + bo + "  verified";
                warningLabel.BackColor = Color.Green;
                warningLabel.Show();
            }
        }
    }
}
