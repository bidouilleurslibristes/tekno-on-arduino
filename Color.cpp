// from :
//  * https://github.com/ratkins/RGBConverter
//  * https://github.com/lucidtronix/ColorArduino

#include "Color.h"

Color :: Color(){
	red = 0;
	green = 0;
	blue = 0;
}

Color :: Color(byte r, byte g, byte b) :
red(r), green(g), blue(b) { }

void Color::set_color(byte r, byte g, byte b){
  red = r;
  green = g;
  blue = b;
}


void Color::rgbToHsl( double hsl[]) {
  double rd = (double) red/255;
  double gd = (double) green/255;
  double bd = (double) blue/255;
  double max = threeway_max(rd, gd, bd);
  double min = threeway_min(rd, gd, bd);
  double h, s, l = (max + min) / 2;
  if (max == min)
  {
    h = s = 0; // achromatic
  }
  else
  {
    double d = max - min;
    s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
    if (max == rd)
      h = (gd - bd) / d + (gd < bd ? 6 : 0);
    else if (max == gd)
      h = (bd - rd) / d + 2;
    else if (max == bd)
      h = (rd - gd) / d + 4;
    h /= 6;
  }
  hsl[0] = h;
  hsl[1] = s;
  hsl[2] = l;
}


/**
* Converts an HSL color value to RGB. Conversion formula
* adapted from http://en.wikipedia.org/wiki/HSL_color_space.
* Assumes h, s, and l are contained in the set [0, 1] and
* returns r, g, and b in the set [0, 255].
*
* @param Number h The hue
* @param Number s The saturation
* @param Number l The lightness
* @return A color object
*/
Color Color::hslToRgb(double h, double s, double l)
{
  double r, g, b;

  if (s == 0)
    r = g = b = l; // achromatic
  else
  {
    double q = l < 0.5 ? l * (1 + s) : l + s - l * s;
    double p = 2 * l - q;
    r = Color::hue2rgb(p, q, h + 1.0/3.0);
    g = Color::hue2rgb(p, q, h);
    b = Color::hue2rgb(p, q, h - 1.0/3.0);
  }

  byte res_r = (byte)floor(r * 255);
  byte res_g = (byte)floor(g * 255);
  byte res_b = (byte)floor(b * 255);

  return Color(res_r, res_g, res_b);
}

double Color::hue2rgb(double p, double q, double t)
{
  // Serial.print("p : "); // Serial.println(p);
  // Serial.print("q : "); // Serial.println(q);
  // Serial.print("t : "); // Serial.println(t);
  if(t < 0) t += 1;
  if(t > 1) t -= 1;
  if(t < 1.0/6.0) return p + (q - p) * 6 * t;
  if(t < 1.0/2.0) return q;
  if(t < 2.0/3.0) return p + (q - p) * (2.0/3.0 - t) * 6;
  return p;
}

double Color::threeway_max(double a, double b, double c)
{
  return max(a, max(b, c));
}

double Color::threeway_min(double a, double b, double c)
{
  return min(a, min(b, c));
}


Color Color::valInterm(Color col1, Color col2, float pos)
{
  /*
  * 0 <= pos <= 1
  */
    // res_h = (hsl2[0]-hsl1[0])*pos + hsl1[0]
    // hue value transition can be tricky to compute
    // we have to decide if we go clock wise or counter clowk wise.
    // inspired from here : http://stackoverflow.com/questions/2593832
    double hsl1 [3];
    double hsl2 [3];
    double distCCW, distCW;
    col1.rgbToHsl(hsl1);
    col2.rgbToHsl(hsl2);

    if (hsl1[0] >= hsl2[0])
        distCCW = hsl1[0] - hsl2[0];
    else
       distCCW = 1 + hsl1[0] - hsl2[0];

    if (hsl1[0] >= hsl2[0])
    distCW = 1 + hsl2[0] - hsl1[0];
    else
      distCW = hsl2[0] - hsl1[0];

    //ensure we get the right number of steps by adding remainder to final part
    double res_h, res_l, res_s;
    if (distCW <= distCCW)
        res_h = hsl1[0] + (distCW * pos);
    else
        res_h = hsl1[0] - (distCCW * pos);

    if (res_h < 0)
      res_h = 1 + res_h ;
    if (res_h > 1)
      res_h = res_h - 1;

    if (hsl2[2] >= hsl1[2])
      res_l = (hsl2[2]-hsl1[2])*pos + hsl1[2];
    else
      res_l = hsl1[2] - (hsl1[2]-hsl2[2])*pos;

    if (hsl2[1] >= hsl1[1])
      res_s = (hsl2[1]-hsl1[1])*pos + hsl1[1];
    else
      res_s = hsl1[1] - (hsl1[1]-hsl2[1])*pos;

    Color col = Color::hslToRgb(res_h, res_s, res_l);

    return col;
}