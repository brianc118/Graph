/* Arduino/Teensy ILI9341 TFT Display Widget Library
 * 
 * AIM: 
 * 		To provide a flexible graphing library (predominantly line/scatter)
 *    for Arduino and ILI9341 or similar displays. Should be EASILY
 *		PORTED FOR OTHER HARDWARE by requiring minimal dependencies.
 *
 * FUNCTIONS:
 *		Border, line graph, bar graph, scatter graph, autoscalling, various
 *		axes settings
 *
 * Copyright (c) 2016 Brian Chen
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef GRAPH_h
#define GRAPH_h

#include <vector>
#include <algorithm>
#include "Arduino.h"
#include "ILI9341_t3.h"

#define TYPE_SCATTER 0
#define TYPE_LINE 1
#define TYPE_BAR  2

#define AXES_LEFT 0
#define AXES_RIGHT 1
#define AXES_CENTRE 2
#define AXES_TOP 3
#define AXES_BOTTOM 4

#define SCALE_AUTO 0
#define SCALE_MANUAL 1

#define RANGE_AUTO 0
#define RANGE_MANUAL 1

struct Point {
  float x;
  float y;
};

class Line {
public:
	Line(int type=TYPE_LINE, uint16_t point_color=ILI9341_WHITE, 
 	  uint16_t color=ILI9341_PINK);
	std::vector<Point> points_queue;
	std::vector<Point> points_ld;
	std::vector<Point> points_del_queue;

	uint16_t color, color_ld;
	uint16_t point_color, point_color_ld;
	int type, type_ld;

	void setType(int);
	void setColor(uint16_t);
	void setPointColor(uint16_t);

	bool isPoint(Point);
	void addPoint(Point);
	bool removePoint(Point);

	void update();
};

class Graph {
public:
  Graph(ILI9341_t3* ptr_tft);
  void addLine(int type=TYPE_LINE, uint16_t point_color=ILI9341_WHITE, 
    uint16_t color=ILI9341_PINK);
  void setBorderWidth(int border_width);
  void setBorderColor(uint16_t border_color);
  void setBackgroundColor(uint16_t background_color);
  void setAxesColor(uint16_t axes_color);
  void setArea(int x1, int y1, int dx, int dy);
  void setAxisType(int v_axis_type, int h_axis_type);
  void setXScaleType(int);
  void setYScaleType(int);
  void setXScale(int, int);
  void setYScale(int, int);
  void setXRangeType(int);
  void setXRange(float, float);
  void setYRangeType(int);
  void setYRange(float, float);
  void redraw(bool redraw=true);    // complete redraw
  void drawNewPoints();

  std::vector<Line> lines;
private:
  bool drawPoint(Point, Line*);
  void erasePoint(Point);
  void eraseSeg(Point, Point);
  Point mapPoint(Point p, bool current=true);
  void calcRange();

  int border_width;
  int border_width_ld;

  uint16_t border_color;
  uint16_t border_color_ld;

  uint16_t background_color;
  uint16_t background_color_ld;

  uint16_t axes_color;
  uint16_t axes_color_ld;

  int v_axis_type, h_axis_type;
  int v_axis_type_ld, h_axis_type_ld;
  int axes_x, axes_y;
  int axes_x_ld, axes_y_ld;

  int x_scale_type, y_scale_type;
  int x_scale_type_ld, y_scale_type_ld;

  int x_range_type, y_range_type;
  int x_range_type_ld, y_range_type_ld;

  int x1, y1, dx, dy;
  int x1_ld, y1_ld, dx_ld, dy_ld;
  int xmin, xmax;
  int xmin_ld, xmax_ld;
  int ymin, ymax;
  int ymin_ld, ymax_ld;
  float x_range_min, x_range_max;
  float x_range_min_ld, x_range_max_ld;
  float y_range_min, y_range_max;
  float y_range_min_ld, y_range_max_ld;

  ILI9341_t3 *ptr_tft;
};

#endif