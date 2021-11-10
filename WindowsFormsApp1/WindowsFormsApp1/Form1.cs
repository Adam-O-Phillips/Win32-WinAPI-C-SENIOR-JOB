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

namespace WindowsFormsApp1
{
    enum MouseClickPart
    {
        MB_NULL,
        MB_LEFT,
        MB_TOP,
        MB_RIGHT,
        MB_BOTTOM,
        MB_MID
    }
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

            rcAppRegion.X = 40;
            rcAppRegion.Y = 60;
            rcAppRegion.Width = 600;
            rcAppRegion.Height = 400;

            mouseMovePart = MouseClickPart.MB_NULL;
            textTxIP.Text = "127.0.0.1";
        }

        Rectangle rcAppRegion;
        UDPSocket c = null;
        
        MouseClickPart mouseMovePart;
        Point ptMouseClicked;
        Rectangle rcMouseClicked;

        private void Form1_MouseDown(object sender, MouseEventArgs e)
        {
            if (mouseMovePart != MouseClickPart.MB_NULL)
            {
                ptMouseClicked.X = e.X;
                ptMouseClicked.Y = e.Y;
                rcMouseClicked = rcAppRegion;
            }
            else
            {
                ptMouseClicked.X = ptMouseClicked.Y = -1;
            }
        }

        private void Form1_MouseMove(object sender, MouseEventArgs e)
        {
            int mouseX = e.X, mouseY = e.Y;
            if (e.Button == System.Windows.Forms.MouseButtons.None)
            {
                MouseClickPart nowPart = MouseClickPart.MB_NULL;
                if (mouseY >= rcAppRegion.Y && mouseY <= rcAppRegion.Y + rcAppRegion.Height)
                {
                    if (mouseX >= rcAppRegion.X - 5 && mouseX <= rcAppRegion.X + 5)
                        nowPart = MouseClickPart.MB_LEFT;
                    else if (mouseX >= rcAppRegion.X + rcAppRegion.Width - 5 && mouseX <= rcAppRegion.X + rcAppRegion.Width + 5)
                        nowPart = MouseClickPart.MB_RIGHT;
                }
                else if (mouseX >= rcAppRegion.X && mouseX <= rcAppRegion.X + rcAppRegion.Width)
                {
                    if (mouseY >= rcAppRegion.Y - 5 && mouseY <= rcAppRegion.Y + 5)
                        nowPart = MouseClickPart.MB_TOP;
                    else if (mouseY >= rcAppRegion.Y + rcAppRegion.Height - 5 && mouseY <= rcAppRegion.Y + rcAppRegion.Height + 5)
                        nowPart = MouseClickPart.MB_BOTTOM;
                }

                if (nowPart == MouseClickPart.MB_NULL && rcAppRegion.Contains(mouseX, mouseY))
                    nowPart = MouseClickPart.MB_MID;

                //Debug.WriteLine("X:" + mouseX + ", Y:" + mouseY + ": " + rcAppRegion.ToString());

                if (nowPart != mouseMovePart)
                {
                    mouseMovePart = nowPart;
                    Invalidate();
                }
            }
            else
            {
                bool bInvalidate = false;
                switch (mouseMovePart)
                {
                    case MouseClickPart.MB_LEFT:
                        {
                            int xgap = mouseX - ptMouseClicked.X;
                            if (rcMouseClicked.Width > xgap)
                            {
                                rcAppRegion.X = rcMouseClicked.X + xgap;
                                rcAppRegion.Width = rcMouseClicked.Width - xgap;
                                bInvalidate = true;
                            }
                        }
                        break;
                    case MouseClickPart.MB_TOP:
                        {
                            int ygap = mouseY - ptMouseClicked.Y;
                            if (rcMouseClicked.Height > ygap)
                            {
                                rcAppRegion.Y = rcMouseClicked.Y + ygap;
                                rcAppRegion.Height = rcMouseClicked.Height - ygap;
                                bInvalidate = true;
                            }
                        }
                        break;
                    case MouseClickPart.MB_RIGHT:
                        {
                            int xgap = mouseX - ptMouseClicked.X;
                            if (rcMouseClicked.Width + xgap > 0)
                            {
                                rcAppRegion.Width = rcMouseClicked.Width + xgap;
                                bInvalidate = true;
                            }
                        }
                        break;
                    case MouseClickPart.MB_BOTTOM:
                        {
                            int ygap = mouseY - ptMouseClicked.Y;
                            if (rcMouseClicked.Height + ygap > 0)
                            {
                                rcAppRegion.Height = rcMouseClicked.Height + ygap;
                                bInvalidate = true;
                            }
                        }
                        break;
                    case MouseClickPart.MB_MID:
                        {
                            int xgap = mouseX - ptMouseClicked.X;
                            int ygap = mouseY - ptMouseClicked.Y;
                            rcAppRegion.X = rcMouseClicked.X + xgap;
                            rcAppRegion.Y = rcMouseClicked.Y + ygap;
                            bInvalidate = true;
                        }
                        break;
                    default:
                        break;
                }

                if (bInvalidate)
                {
                    byte[] bData = new byte[16];

                    bData[0] = (byte)(rcAppRegion.X >> 24);
                    bData[1] = (byte)(rcAppRegion.X >> 16);
                    bData[2] = (byte)(rcAppRegion.X >> 8);
                    bData[3] = (byte)(rcAppRegion.X);

                    bData[4] = (byte)(rcAppRegion.Y >> 24);
                    bData[5] = (byte)(rcAppRegion.Y >> 16);
                    bData[6] = (byte)(rcAppRegion.Y >> 8);
                    bData[7] = (byte)(rcAppRegion.Y);

                    bData[8] = (byte)(rcAppRegion.Width >> 24);
                    bData[9] = (byte)(rcAppRegion.Width >> 16);
                    bData[10] = (byte)(rcAppRegion.Width >> 8);
                    bData[11] = (byte)(rcAppRegion.Width);

                    bData[12] = (byte)(rcAppRegion.Height >> 24);
                    bData[13] = (byte)(rcAppRegion.Height >> 16);
                    bData[14] = (byte)(rcAppRegion.Height >> 8);
                    bData[15] = (byte)(rcAppRegion.Height);

                    if (c != null)
                        c.SendBytes(bData);

                    Invalidate();
                }
            }
        }

        private void Form1_MouseUp(object sender, MouseEventArgs e)
        {

        }

        private void Form1_Paint(object sender, PaintEventArgs e)
        {
            Graphics g;
            g = this.CreateGraphics();

            SolidBrush sb = new SolidBrush(Color.DarkOrange);
            SolidBrush rb = new SolidBrush(Color.Red);
            Pen pb = new Pen(Color.Red, 2);
            
            g.FillRectangle(sb, rcAppRegion.X, rcAppRegion.Y, rcAppRegion.Width, rcAppRegion.Height);
            switch (mouseMovePart)
            {
                case MouseClickPart.MB_LEFT:
                    g.DrawLine(pb, rcAppRegion.X, rcAppRegion.Y, rcAppRegion.X, rcAppRegion.Y + rcAppRegion.Height);
                    break;
                case MouseClickPart.MB_TOP:
                    g.DrawLine(pb, rcAppRegion.X, rcAppRegion.Y, rcAppRegion.X + rcAppRegion.Width, rcAppRegion.Y);
                    break;
                case MouseClickPart.MB_RIGHT:
                    g.DrawLine(pb, rcAppRegion.X + rcAppRegion.Width, rcAppRegion.Y, rcAppRegion.X + rcAppRegion.Width, rcAppRegion.Y + rcAppRegion.Height);
                    break;
                case MouseClickPart.MB_BOTTOM:
                    g.DrawLine(pb, rcAppRegion.X, rcAppRegion.Y + rcAppRegion.Height, rcAppRegion.X + rcAppRegion.Width, rcAppRegion.Y + rcAppRegion.Height);
                    break;
                case MouseClickPart.MB_MID:
                    g.FillRectangle(rb, rcAppRegion.X, rcAppRegion.Y, rcAppRegion.Width, rcAppRegion.Height);
                    break;
            }
            g.Dispose();
        }

        private void SetIPBtn_Click(object sender, EventArgs e)
        {
            if (c != null)
                return;

            c = new UDPSocket();
            c.Client(textTxIP.Text, 27123);
        }

        Timer MyTimer = null;
        private void Form1_Load(object sender, EventArgs e)
        {
//             MyTimer = new Timer();
//             MyTimer.Interval = 40;
//             MyTimer.Tick += new EventHandler(MyTimer_Tick);
//             MyTimer.Start();
        }
        private void MyTimer_Tick(object sender, EventArgs e)
        {
            byte[] bData = new byte[1];
            bData[0] = 0;
            if (c != null)
                c.SendBytes(bData);
        }
    }
}
