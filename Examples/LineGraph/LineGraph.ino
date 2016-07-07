#include <vector>
#include <algorithm>

#include <SPI.h>
#include <ILI9341_t3.h>
#include <Graph.h>

#define TFT_DC 15
#define TFT_CS 20
#define TFT_RST 16

ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, 11, 13, 12);

Graph graphScatt(&tft);
Graph graphLine(&tft);

void setup(){
	//while(!Serial);
	Serial.begin(115200);
	SPI.begin();
	tft.begin();
	
	graphScatt.setArea(0, 0, 240, 160);	
	graphLine.setArea(0, 160, 240, 160);

	graphScatt.addLine(TYPE_SCATTER, ILI9341_WHITE);
	// graphScatt.addLine(TYPE_SCATTER);
	// graphScatt.addLine(TYPE_SCATTER);
	
	// graphLine.addLine();

	graphLine.setBorderColor(ILI9341_DARKGREY);
	graphLine.setXRangeType(RANGE_MANUAL);
	graphLine.setXRange(-0.1, 2*PI+0.1);
	graphLine.setYRangeType(RANGE_MANUAL);
	graphLine.setYRange(-1.1, 1.1);
}

void loop(){
	float x = 0;
	unsigned long dt = millis();
	for (; x <= 2*PI; x += 2*PI/24){
		delay(50);
		graphScatt.lines[0].addPoint(Point{x, sinf(x)});
		//graphLine.lines[0].addPoint(Point{x, sinf(x)});
		// graphScatt.lines[1].addPoint(Point{x, 2*sinf(x)});
		// graphScatt.lines[2].addPoint(Point{x, -2*sinf(x)});
		graphScatt.redraw();
		//graphLine.redraw();
	}
	for (; x >= 0; x -= 2*PI/24){
		delay(50);
		graphScatt.lines[0].removePoint(Point({x, sinf(x)}));
		//graphLine.lines[0].removePoint(Point({x, sinf(x)}));
		// graphScatt.lines[1].removePoint(Point{x, 2*sinf(x)});
		graphScatt.redraw();
		//graphLine.redraw();
	}
	dt = millis() - dt;
	Serial.println("/****************DT****************/");
	Serial.println(dt);
	Serial.println("/**********************************/");
}