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


/**
 * @brief Aus den Daten ein Min und Max bilden und als Y-Achsenbeschreibung 
 * darstellen
 * 
 */
void GraphPanelClass::DrawYAxis()
{
    float max = GetMaxValue();
    float min = GetMinValue();

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

    float max = GetMaxValue();
    float min = GetMinValue();

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
    GD.ColorRGB(GetColor());

    for (int i = 0; i < c; i++)
    {

        int ypos = y + (m0 * (GetBufferValue(b[i]) - min));
        GD.Vertex2f((x), ypos);
        x = x + m1;
    }
}


float GraphPanelClass::GetBufferValue( BufferDataClass b) 
{
   float value = 0.0;
    MeasurementClass *p = System.GetSelectedMeasurement();
    if( p != nullptr) 
    {
        switch (p->Type)
        {
        case CurrentType:
            value = b.Current;
            break;
        case VoltageType:
            value = b.Voltage;
            break;        
        case ResistorType:
            value = b.Resistor;
            break;    
        case PowerType:
            value = b.Power;
            break;                          
        default:
            value = 0.0;
            break;
        }
       
    }
     return value;

}  



/**
 * @brief Delivers the maximum value from the buffer
 * 
 * @return float 
 */
float  GraphPanelClass::GetMaxValue() 
{
    float value = 0.0;
    MeasurementClass *p = System.GetSelectedMeasurement();
    if( p != nullptr) 
    {
        switch (p->Type)
        {
        case CurrentType:
            value = System.Buffer.GetCurrentMax();
            break;
        case VoltageType:
            value = System.Buffer.GetVoltageMax();
            break;        
        case ResistorType:
            value = System.Buffer.GetResistorMax();
            break;    
        case PowerType:
            value = System.Buffer.GetPowerMax();
            break;                          
        default:
            value = 0.0;
            break;
        }
       
    }
     return value;
}


/**
 * @brief Delivers the maximum value from the buffer
 * 
 * @return 
 */

uint  GraphPanelClass::GetColor() 
{
    uint value = THEME_CURRENT_LABEL_COLOR;
    MeasurementClass *p = System.GetSelectedMeasurement();
    if( p != nullptr) 
    {
        switch (p->Type)
        {
        case CurrentType:
            value = THEME_CURRENT_LABEL_COLOR;
            break;
        case VoltageType:
            value = THEME_VOLTAGE_LABEL_COLOR;
            break;        
        case ResistorType:
            value = THEME_RESISTOR_LABEL_COLOR ;
            break;    
        case PowerType:
            value = THEME_POWER_LABEL_COLOR;
            break;                          
        default:
            value = THEME_CURRENT_LABEL_COLOR;
            break;
        }
       
    }
     return value;
}



/**
 * @brief Delivers the minimum value from the buffer
 * 
 * @return float 
 */
float  GraphPanelClass::GetMinValue() 
{
    float value = 0.0;
    MeasurementClass *p = System.GetSelectedMeasurement();
    if( p != nullptr) 
    {
        switch (p->Type)
        {
        case CurrentType:
            value = System.Buffer.GetCurrentMin();
            break;
        case VoltageType:
            value = System.Buffer.GetVoltageMin();
            break;        
        case ResistorType:
            value = System.Buffer.GetResistorMin();
            break;    
        case PowerType:
            value = System.Buffer.GetPowerMin();
            break;                          
        default:
            value = 0.0;
            break;
        }
       
    }
     return value;
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

    DrawYAxis();

    DrawValues();
}
