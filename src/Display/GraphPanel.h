#ifndef _Graph_Panel_h
#define _Graph_Panel_h


#include "arduino.h"
#include <GD2.h>
#include "Controls.h"
#include "Components.h"



class GraphPanelClass: public ContainerClass 
{
	public:
		GraphPanelClass( int x, int y,  int w, int h);
		const char* Classname() override {return "GraphClass";}
		void Render()  override;

	

		int RowHeight() { return 30;}
	protected: 

		void DrawYAxis(); 
		void DrawValues();
        float GetMaxValue();
        float GetMinValue();
        uint GetColor();
        const char* GetUnit();

		float GetBufferValue( BufferDataClass b) ;

};

#endif

