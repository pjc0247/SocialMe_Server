using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.IO;

using System.Net;
using System.Net.Sockets;

using System.Threading;
using System.Threading.Tasks;

namespace SocialMeFileServer
{
    public partial class Form1 : Form
    {
        const int SERVER_PORT = 9918;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            Thread serv = new Thread(Server);
            serv.Start();
        }
        public void _log(string msg)
        {
            output.Items.Add(msg);
        }


        static void log(string msg)
        {
            Program.program._log(msg);
        }
        static void Server()
        {
            TcpListener server = null;

            try
            {
                server = new TcpListener(SERVER_PORT);
                server.Start();
            }
            catch (SocketException exc)
            {
                log("error " + exc.Message);
            }
            log("server started at " + SERVER_PORT.ToString() + "port");

            while (true)
            {
                TcpClient client = null;
                Thread ct = null;

                client = server.AcceptTcpClient();

                ct = new Thread(new ParameterizedThreadStart(ClientProcessor));
                ct.Start(client);
            }
        }
        static void ClientProcessor(object arg)
        {
            TcpClient client = arg as TcpClient;
            NetworkStream stream = client.GetStream();

            byte[] buffer = new byte[128];
            string path;

            FileStream file;
            FileInfo fi = new FileInfo(path);

            stream.Read(buffer, 0, 128);
            path = Encoding.Default.GetString(buffer);

            file = File.Open(path, FileMode.Open);

            if (file == null)
                stream.Write(Encoding.Default.GetBytes("ER"), 0, 3);
            else
            {
                stream.Write(Encoding.Default.GetBytes("OK"), 0, 3);

                

                file.Close();
            }

            stream.Close();
            client.Close();
        }
    }
}
