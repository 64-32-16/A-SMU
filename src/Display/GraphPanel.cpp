#include "GraphPanel.h"
#include "Display.h"


//-------------------------------------------------------------------
// GraphPanelClass
//-------------------------------------------------------------------


GraphPanelClass::GraphPanelClass(int x, int y, int w, int h)
{
    X = x;
    Y = y;
    W = w;
    H = h;

    Title = "GRAPH";
}

void GraphPanelClass::DrawMinMax()
{
    float max = System.Buffer.GetCurrentMax();
    float min = System.Buffer.GetCurrentMin();

    float step = (max - min) / 6.0;

    int x = GetX();
    int y = GetY();

    for (int i = 0; i < 7; i++)
    {
        GD.cmd_text(x, y, 26, OPT_CENTERY, String(max, 5).c_str());
        max = max - step;
        y = y + RowHeight();
    }
}

void GraphPanelClass::DrawValues()
{

    if (GetX() < 0 || (GetX() + W) > 800)
        return;

    float max = System.Buffer.GetCurrentMax();
    float min = System.Buffer.GetCurrentMin();

    if ((max - min) == 0.0)
        return;

    float m0 = (180.0 / (max - min));
    float m1 = 700.0 / 200.0;

    int x = GetX() + 100;
    int y = GetY();

    BufferDataClass b[200];

    int c = System.Buffer.GetData(b, 200);

    /* Punkte zeichnen
    x = GetX()+100;
    GD.Begin(POINTS);
    GD.PointSize(2*16);
    GD.ColorRGB( C1);
    GD.ColorA(0xFF);

    for( int i=0; i< c ; i++)
    {
        int ypos = y + (m0 * ( b[i].Current - min) );
        GD.Vertex2f( (x)  , ypos );
        x = x + m1;
    }
    */

    x = GetX() + 100;

    GD.Begin(LINE_STRIP);
    GD.LineWidth(10);
    GD.ColorRGB(C1);

    for (int i = 0; i < c; i++)
    {

        int ypos = y + (m0 * (b[i].Current - min));
        GD.Vertex2f((x), ypos);
        x = x + m1;
    }
}

void GraphPanelClass::Render()
{

    if (Visible == false)
        return;

    //Serial.println(" GraphPanelClass::Render()");

    int h = 180;
    int y = GetY();
    int x = GetX() + 100;
    int x1 = W;

    // Gitterlinien zeichnen
    if (GetX() < 0)
        x1 = W + GetX();

    GD.Begin(LINES);
    GD.ColorRGB(0xFFFFFF);
    GD.LineWidth(4);

    for (int r = 0; r < 7; r++)
    {
        GD.Vertex2f(x, y);
        GD.Vertex2f(x1, y);
        y = y + RowHeight();
    }

    y = GetY();

    for (int c = 0; c < 11; c++)
    {

        GD.Vertex2f(x, y);
        GD.Vertex2f(x, (h + y));
        x = x + 70;
    }

    DrawMinMax();

    DrawValues();
}
