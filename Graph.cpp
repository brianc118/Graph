#ifndef GRAPH_cpp
#define GRAPH_cpp

#include <vector>
#include <algorithm>
#include "Arduino.h"
#include "ILI9341_t3.h"
#include "Graph.h"

#define EPSILON ((double)0.000999)

namespace std {
  void __throw_bad_alloc(){
    while(true){
      Serial.println("Unable to allocate memory");
    }
  }
  void __throw_length_error(char const*e){
    while(true){
      Serial.print("Length Error :");
      Serial.println(e);
    }
  }
}

static float fmap(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static bool approx(float a, float b){
  return abs(a - b) < EPSILON;
}

static void printP(Point p){
  Serial.print("(");
  Serial.print(p.x);
  Serial.print(", ");
  Serial.print(p.y);
  Serial.print(")");
}

Line::Line(int type, uint16_t point_color, uint16_t color){
  setType(type);
  setColor(color);
  setPointColor(point_color);
}

void Line::setType(int type){
  this->type = type;
}

void Line::setColor(uint16_t color){
  this->color = color;
}

void Line::setPointColor(uint16_t point_color){
  this->point_color = point_color;
}

bool Line::isPoint(Point p){
  for (uint16_t i = 0; i < points_queue.size(); i++){
    if (approx(p.x, points_queue[i].x) && approx(p.y, points_queue[i].y)){
      return true;
    }
  }
  for (uint16_t i = 0; i < points_ld.size(); i++){
    if (approx(p.x, points_ld[i].x) && approx(p.y, points_ld[i].y)){
      return true;
    }
  }
  return false;
}

void Line::addPoint(Point p){
  if (!isPoint(p)){
    // Serial.print("Queued (");
    // printP(p); Serial.println();
    points_queue.push_back(p);
  }
}

bool Line::removePoint(Point p){
  bool out = false;
  for (uint16_t i = 0; i < points_queue.size(); i++){
    if (approx(p.x, points_queue[i].x) && approx(p.y, points_queue[i].y)){
      points_queue.erase(points_queue.begin() + i);
      out = true;
    }
  }
  for (uint16_t i = 0; i < points_ld.size(); i++){
    if (approx(p.x, points_ld[i].x) && approx(p.y, points_ld[i].y)){
      //points_ld.erase(points_ld.begin() + i);
      points_del_queue.push_back(p);
      out = true;
    }
  }

  if (out){
    // Serial.print("REMOVED: ");
    // printP(p);
    // Serial.println();
  }
  return out;
}

void Line::update(){
  type_ld = type;
  color_ld = color;
  point_color_ld = point_color;
}

Graph::Graph(ILI9341_t3* ptr_tft){
  this->ptr_tft = ptr_tft;
  setBorderWidth(10);
  setBorderColor(ILI9341_BLACK);
  setBackgroundColor(ILI9341_PURPLE);
  setAxesColor(ILI9341_BLUE);
  setAxesType(AXES_CENTRE, AXES_CENTRE);
  setXScaleType(SCALE_AUTO);
  setYScaleType(SCALE_AUTO);
  setXRangeType(RANGE_AUTO);
  setYRangeType(RANGE_AUTO);
}

void Graph::addLine(int type, uint16_t point_color, uint16_t color){
  lines.push_back(Line(type, point_color, color));
}

void Graph::setBorderWidth(int border_width){
  this->border_width = border_width;
}

void Graph::setBorderColor(uint16_t border_color){
  this->border_color = border_color;
}

void Graph::setBackgroundColor(uint16_t background_color){
  this->background_color = background_color;
}

void Graph::setAxesColor(uint16_t axes_color){
  this->axes_color = axes_color;
}

void Graph::setArea(int x1, int y1, int dx, int dy){
  this->x1 = x1;
  this->y1 = y1;
  this->dx = dx;
  this->dy = dy;
}

void Graph::setAxisType(int v_axis_type, int h_axis_type){
  this->v_axis_type = v_axis_type;
  this->h_axis_type = h_axis_type;
}

void Graph::setXScaleType(int x_scale_type){
  this->x_scale_type = x_scale_type;
}

void Graph::setYScaleType(int y_scale_type){
  this->y_scale_type = y_scale_type;
}

void Graph::setXScale(int xmin, int xmax){
  this->xmin = xmin;
  this->xmax = xmax;
}

void Graph::setYScale(int ymin, int ymax){
  this->ymin = ymin;
  this->ymax = ymax;
}

void Graph::setXRangeType(int x_range_type){
  this->x_range_type = x_range_type;
}

void Graph::setYRangeType(int y_range_type){
  this->y_range_type = y_range_type;
}

void Graph::setXRange(float x_range_min, float x_range_max){
  this->x_range_min = x_range_min;
  this->x_range_max = x_range_max;
}

void Graph::setYRange(float y_range_min, float y_range_max){
  this->y_range_min = y_range_min;
  this->y_range_max = y_range_max;
}

void Graph::redraw(bool redraw){
  calcRange();

  bool draw_body = (border_width != border_width_ld
                 || x1 != x1_ld
                 || y1 != y1_ld
                 || dx != dx_ld
                 || dy != dy_ld);
  bool draw_x_axis = false;
  bool draw_y_axis = false;

  if (draw_body){
    // Draw border
    //ptr_tft->fillRect(x1, y1, dx, dy, border_color);
    for (int i = border_width - 1; i >= 0; i--){
      ptr_tft->drawRect(x1+i, y1+i, dx-2*i, dy-2*i, border_color);
    }

    // Draw background
    ptr_tft->fillRect(x1+border_width, y1+border_width, dx-2*border_width, dy-2*border_width, background_color);

    draw_x_axis = true;
    draw_y_axis = true;
  }

  // Draw axes
  axes_x = 0, axes_y = 0;
  
  if (v_axis_type == AXES_CENTRE){
    axes_x = 0;
  }
  else if (v_axis_type == AXES_LEFT){
    axes_x = 0;
  }
  else if (v_axis_type == AXES_RIGHT){
    axes_x = 0;
  }

  if (axes_x != axes_x_ld || draw_x_axis){
    ptr_tft->drawFastVLine(axes_x, y1+border_width, dy-2*border_width, axes_color);
  }
  if (axes_y != axes_y_ld || draw_y_axis){
    ptr_tft->drawFastHLine(x1+border_width, axes_y, dx-2*border_width, axes_color);
  } 

  bool range_scale_same = (xmin == xmin_ld
                        && xmax == xmax_ld
                        && ymin == ymin_ld
                        && ymax == ymax_ld
                        && x_range_min == x_range_min_ld
                        && x_range_max == x_range_max_ld
                        && y_range_min == y_range_min_ld
                        && y_range_max == y_range_max_ld);
  for (uint16_t i = 0; i < lines.size(); i++){
    // if body has been redrawn, points are already erased
    // so clear points_del_queue from memory
    // a redraw of all points is now required

    // if range_scale_same==False, then all points need to be redrawn 

    if (true || draw_body || !range_scale_same){
      // copy all points from points_ld to del_queue and queue.
      while (!lines[i].points_ld.empty()){
        lines[i].points_del_queue.push_back(lines[i].points_ld.back());
        lines[i].points_queue.push_back(lines[i].points_ld.back());
        lines[i].points_ld.pop_back();
      }

      // // clear points_del_queue and corresponding points in points_queue
      // while (!lines[i].points_del_queue.empty()){
      //   lines[i].removePoint(lines[i].points_del_queue.back());
      //   lines[i].points_del_queue.pop_back();
      // }
    }
    Serial.print("LINE "); Serial.print(i); Serial.print(": del_queue size: ");
    Serial.print(lines[i].points_del_queue.size());
    Serial.print("queue size: ");
    Serial.print(lines[i].points_queue.size());
    Serial.print("points_ld size: ");
    Serial.print(lines[i].points_ld.size());
    Serial.println();

    // Erase points
    while (!lines[i].points_del_queue.empty()){
      if (lines[i].type_ld == TYPE_LINE){
        if (lines[i].points_del_queue.size() >= 2){
          eraseSeg(lines[i].points_del_queue.back(), lines[i].points_del_queue.end()[-2]);
        }
      }
      else{
        erasePoint(lines[i].points_del_queue.back());
      }
      lines[i].points_del_queue.pop_back();
    }

    // Draw points
    while (!lines[i].points_queue.empty()){
      drawPoint(lines[i].points_queue.back(), &lines[i]);
      lines[i].points_queue.pop_back();
    }
    lines[i].update();
  }

  /*
  for (uint16_t i = 0; i < lines.size(); i++){
    Serial.print("LINE "); Serial.print(i); Serial.println(": ");

    if (!range_scale_same && !draw_body){
      // if (lines[i].type_ld == TYPE_LINE){
      //   for (uint16_t j = 0; j < lines[i].points_ld.size(); j++){
      //     Serial.print("ERASING: ");
      //     printP(lines[i].points_ld[j]);
      //     Serial.println();
      //     if (j + 1 <= lines[i].points_ld.size()){
      //       eraseSeg(lines[i].points_ld[j], lines[i].points_ld[j + 1]);
      //     }
      //   }
      // } 
      // else{
      //   for (uint16_t j = 0; j < lines[i].points_ld.size(); j++){
      //     erasePoint(lines[i].points_ld[j]);
      //   }
      // }    
    }
    if (!range_scale_same || draw_body){
      // Add all previously drawn points to queue
      // while (!lines[i].points_ld.empty()){
      //   lines[i].points_queue.push_back(lines[i].points_ld.back());
      //   lines[i].points_ld.pop_back();
      // }
    }
    // Erase points
    while (!lines[i].points_del_queue.empty()){
      if (lines[i].type_ld == TYPE_LINE){
        if (lines[i].points_del_queue.size() >= 2){
          eraseSeg(lines[i].points_del_queue.back(), lines[i].points_del_queue.end()[-2]);
        }
      }
      else{
        erasePoint(lines[i].points_del_queue.back());
      }
      lines[i].points_del_queue.pop_back();
    }

    // Draw points
    while (!lines[i].points_queue.empty()){
      drawPoint(lines[i].points_queue.back(), &lines[i]);
      lines[i].points_queue.pop_back();
    }
    lines[i].update();
  }
  */

  border_width_ld = border_width;
  
  x1_ld = x1;
  y1_ld = y1;
  dx_ld = dx;
  dy_ld = dy;

  v_axis_type_ld = v_axis_type;
  h_axis_type_ld = h_axis_type;

  axes_x_ld = axes_x;
  axes_y_ld = axes_y;

  x_scale_type_ld = x_scale_type;
  y_scale_type_ld = y_scale_type;

  x_range_type_ld = x_range_type;
  y_range_type_ld = y_range_type;

  xmin_ld = xmin;
  xmax_ld = xmax;
  ymin_ld = ymin;
  ymax_ld = ymax;

  x_range_min_ld = x_range_min;
  x_range_max_ld = x_range_max;
  y_range_min_ld = y_range_min;
  y_range_max_ld = y_range_max;

  // Draw labels
}

// void Graph::clearLine(){

// }


bool Graph::drawPoint(Point p, Line* l){
  if (p.x > x_range_max || p.x < x_range_min || p.y > y_range_max || p.y < y_range_min){
    return false;
  }
  
  Point mp = mapPoint(p);

  if (l->type == TYPE_LINE && l->points_ld.size()){
    Point pp = mapPoint(l->points_ld.back());
    ptr_tft->drawLine(pp.x, pp.y, mp.x, mp.y, l->color);
    ptr_tft->drawPixel(pp.x, pp.y, l->point_color);
  }
  
  ptr_tft->drawPixel(mp.x, mp.y, l->point_color);

  // Serial.print("drew (");
  // printP(p);
  // Serial.println();

  l->points_ld.push_back(p);
  return true;
}

void Graph::erasePoint(Point p){
  Point mp = mapPoint(p, false);
  ptr_tft->drawPixel(mp.x, mp.y, background_color);
}

void Graph::eraseSeg(Point p1, Point p2){
  Point mp1 = mapPoint(p1, false);
  Point mp2 = mapPoint(p2, false);
  ptr_tft->drawLine(mp1.x, mp1.y, mp2.x, mp2.y, background_color);
}


Point Graph::mapPoint(Point p, bool current){
  Point np;
  if (current){
    if (x_scale_type == SCALE_AUTO){
      xmin = x1 + border_width;
      xmax = x1 + dx - border_width;
    }
    if (y_scale_type == SCALE_AUTO){
      ymin = y1 + border_width;
      ymax = y1 + dy - border_width;
    }
    // np.x = map(p.x, x_range_min, x_range_max, x1 + dx - border_width, x1 + border_width);
    // np.y = map(p.y, y_range_min, y_range_max, y1 + border_width, y1 + dy - border_width);
    np.x = fmap(p.x, x_range_min, x_range_max, xmin, xmax);
    np.y = fmap(p.y, y_range_min, y_range_max, ymax, ymin);
  }
  else{
    np.x = fmap(p.x, x_range_min_ld, x_range_max_ld, xmin_ld, xmax_ld);
    np.y = fmap(p.y, y_range_min_ld, y_range_max_ld, ymax_ld, ymin_ld);
  }
  return np;
}

void Graph::calcRange(){  
  if (y_range_type == RANGE_AUTO){
    y_range_max = 0;
    y_range_min = 0;
  }
  if (x_range_type == RANGE_AUTO){
    x_range_max = 0;
    x_range_min = 0;
  }

  for (uint16_t i = 0; i < lines.size(); i++){
    if (lines[i].points_queue.size() > 0){
      std::sort(lines[i].points_queue.begin(), lines[i].points_queue.end(), [] (Point const& a, Point const& b) { return a.y < b.y; });
      if (y_range_type == RANGE_AUTO || y_range_min == y_range_max){
        if (x_range_type == RANGE_AUTO || (lines[i].points_queue.back().x >= x_range_min && lines[i].points_queue.back().x <= x_range_max)){
          if (lines[i].points_queue.back().y > y_range_max)  y_range_max = lines[i].points_queue.back().y;
        }
        if (x_range_type == RANGE_AUTO || (lines[i].points_queue.front().x >= x_range_min && lines[i].points_queue.front().x <= x_range_max)){
          if (lines[i].points_queue.front().y < y_range_min) y_range_min = lines[i].points_queue.front().y;
        }     
      }

      std::sort(lines[i].points_queue.begin(), lines[i].points_queue.end(), [] (Point const& a, Point const& b) { return a.x < b.x; });
      if (x_range_type == RANGE_AUTO || x_range_min == x_range_max){
        if (y_range_type == RANGE_AUTO || (lines[i].points_queue.back().y >= y_range_min && lines[i].points_queue.back().y <= y_range_max)){
          if (lines[i].points_queue.back().x > x_range_max)  x_range_max = lines[i].points_queue.back().x;
        }
        if (y_range_type == RANGE_AUTO || (lines[i].points_queue.front().y >= y_range_min && lines[i].points_queue.front().y <= y_range_max)){
          if (lines[i].points_queue.front().x < x_range_min) x_range_min = lines[i].points_queue.front().x;
        }
      }
    }
    if (lines[i].points_ld.size() > 0){
      std::sort(lines[i].points_ld.begin(), lines[i].points_ld.end(), [] (Point const& a, Point const& b) { return a.y < b.y; });
      if (y_range_type == RANGE_AUTO || y_range_min == y_range_max){
        if (x_range_type == RANGE_AUTO || (lines[i].points_ld.back().x >= x_range_min && lines[i].points_ld.back().x <= x_range_max)){
          if (lines[i].points_ld.back().y > y_range_max)  y_range_max = lines[i].points_ld.back().y;
        }
        if (x_range_type == RANGE_AUTO || (lines[i].points_ld.front().x >= x_range_min && lines[i].points_ld.front().x <= x_range_max)){
          if (lines[i].points_ld.front().y < y_range_min) y_range_min = lines[i].points_ld.front().y;
        }  
      }

      std::sort(lines[i].points_ld.begin(), lines[i].points_ld.end(), [] (Point const& a, Point const& b) { return a.x < b.x; });
      if (x_range_type == RANGE_AUTO || x_range_min == x_range_max){
        if (y_range_type == RANGE_AUTO || (lines[i].points_ld.back().y >= y_range_min && lines[i].points_ld.back().y <= y_range_max)){
          if (lines[i].points_ld.back().x > x_range_max)  x_range_max = lines[i].points_ld.back().x;
        }
        if (y_range_type == RANGE_AUTO || (lines[i].points_ld.front().y >= y_range_min && lines[i].points_ld.front().y <= y_range_max)){
          if (lines[i].points_ld.front().x < x_range_min) x_range_min = lines[i].points_ld.front().x;
        }     
      }
    }
  }
}

#endif