using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.IO;

namespace UI
{
    public partial class Form2 : Form
    {
        string hostIP = "192.168.1.162"; //Anhson
        //string hostIP = "192.168.217.149"; // Leduy
        //string hostIP = "192.168.242.149"; // Doan
        public Form2()
        {
            InitializeComponent();
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void passButton_Click(object sender, EventArgs e)
        {
            if(passTextBox.Text == "123456")
            {
                button2.Hide();
                Image img = Image.FromStream(new MemoryStream(File.ReadAllBytes("D:\\university\\ky7_zz\\doAnDoLuong\\code_main\\UI\\image_show_load\\okr.jpg")));

                Image thumbnail = img.GetThumbnailImage(533, 400, null, IntPtr.Zero);
                pictureBox1.Image = thumbnail;
                //img.Dispose();
                Console.WriteLine("load okr");
                pictureBox1.Show();
                label1.BackColor = Color.Green;
                label1.Text = "Logged in";
                var client = new UdpClient();
                var serverEndpoint = new IPEndPoint(IPAddress.Parse(hostIP), 3333);
                client.Send(Encoding.UTF8.GetBytes("adding"), 6, serverEndpoint);
                client.Close();

                Socket udpSocket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
                // Bind the socket to the IP and port
                IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("192.168.1.162"), 6523);
                udpSocket.Bind(endPoint);
                // Buffer to store received data
                byte[] buffer = new byte[1024];

                EndPoint remoteEndPoint = new IPEndPoint(IPAddress.Any, 3333);
                int receivedBytes = udpSocket.ReceiveFrom(buffer, ref remoteEndPoint);
                string receivedMessage_t = Encoding.UTF8.GetString(buffer, 0, receivedBytes);
                Console.WriteLine($"Received message: {receivedMessage_t} from {remoteEndPoint}");
                udpSocket.Close();

                //IPEndPoint clientEndPoint = new IPEndPoint(IPAddress.Any, 6523);  // Listen on the fixed client port
                //byte[] receivedData = client.Receive(ref clientEndPoint);
                //client.Close();
                //string receivedMessage = Encoding.UTF8.GetString(receivedData);
                string[] parts = receivedMessage_t.Split(new[] { ',' }, 2);
                string bo = parts[0];
                if(bo == "f")
                {
                    label3.Text = "Fail when sending image, check network connection";
                }
                else
                {
                    Image img_ = Image.FromStream(new MemoryStream(File.ReadAllBytes("D:\\university\\ky7_zz\\doAnDoLuong\\code_main\\imageSaveTemp\\temp.jpg")));
                    
                    Image thumbnail_ = img_.GetThumbnailImage(533, 400, null, IntPtr.Zero);
                    pictureBox1.Image = thumbnail_;
                    //img_.Dispose();
                    //pictureBox1.Image = Image.FromFile("D:\\university\\ky7_zz\\doAnDoLuong\\code_main\\imageSaveTemp\\temp.jpg");
                    if (bo == "y")
                    {
                        button2.Show();
                        label3.Text = "Do you want to save this person";
                    }else
                    {
                        button2.Hide();
                        label3.Text = "can not recognize any face in the picture";
                    }
                }

            }
            else
            {
                label1.Text = "Wrong password";
                label1.BackColor = Color.Red;
            }
            Console.WriteLine("out");
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

        private void button2_Click(object sender, EventArgs e)
        {
            var client = new UdpClient();
            var serverEndpoint = new IPEndPoint(IPAddress.Parse(hostIP), 3333);
            client.Send(Encoding.UTF8.GetBytes($"{textBox1.Text}"), textBox1.Text.Length, serverEndpoint);
            client.Close();
        }

        private void Form2_Load(object sender, EventArgs e)
        {
            button2.Hide();
        }
    }
}
