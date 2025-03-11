#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "donut.h"
#include "st77xx.h"

#define R(mul,shift,x,y) \
  _=x; \
  x -= mul*y>>shift; \
  y += mul*_>>shift; \
  _ = 3145728-x*x-y*y>>11; \
  x = x*_>>10; \
  y = y*_>>10;

int8_t b[1760], z[1760];

void print_donut(void) {
  int sA=1024,cA=0,sB=1024,cB=0,_;
  for (;;) {
    memset(b, 32, 1760);  // text buffer
    memset(z, 127, 1760);   // z buffer
    int sj=0, cj=1024;
    for (int j = 0; j < 90; j++) {
      int si = 0, ci = 1024;  // sine and cosine of angle i
      for (int i = 0; i < 324; i++) {
        int R1 = 1, R2 = 2048, K2 = 5120*1024;

        int x0 = R1*cj + R2,
            x1 = ci*x0 >> 10,
            x2 = cA*sj >> 10,
            x3 = si*x0 >> 10,
            x4 = R1*x2 - (sA*x3 >> 10),
            x5 = sA*sj >> 10,
            x6 = K2 + R1*1024*x5 + cA*x3,
            x7 = cj*si >> 10,
            x = 40 + 30*(cB*x1 - sB*x4)/x6,
            y = 12 + 15*(cB*x4 + sB*x1)/x6,
            N = (((-cA*x7 - cB*((-sA*x7>>10) + x2) - ci*(cj*sB >> 10)) >> 10) - x5) >> 7;

        int o = x + 80 * y;
        int8_t zz = (x6-K2)>>15;
        if (22 > y && y > 0 && x > 0 && 80 > x && zz < z[o]) {
          z[o] = zz;
          b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
        }
        _ = ci;
        ci -= 5 * si >> 8;
        si += 5 * _ >> 8;
        _ = (3145728 - ci * ci - si * si) >> 11;
        ci = ci * _ >> 10;
        si = si * _ >> 10; // rotate i
      }
      _ = cj;
      cj -= 9 * sj >> 7;
      sj += 9 * _ >> 7;
      _ = (3145728 - cj * cj - sj * sj) >> 11;
      cj = cj * _ >> 10;
      sj = sj * _ >> 10; // rotate j
    }
    uint16_t px_x = 0;
    uint16_t px_y = 0;

    int x_offset = 40;  // Adjust horizontal shift
    int y_offset = 50;   // Adjust vertical shift

    for (int k = 0; k < 1760; k++) {
        uint16_t x_pos = px_x + x_offset;
        uint16_t y_pos = px_y + y_offset;

        if (b[k] != ' ') {  // Draw donut shape
    uint16_t color = 0xFFFF;  // Default white

    // Use switch case to map shader characters to donut colors
            switch (b[k]) {
                case '@': color = 0x4A20; break; // Deep chocolate brown  
                case '#': color = 0x6B40; break; // Dark caramel brown  
                case '$': color = 0x8560; break; // Warm golden brown  
                case '*': color = 0xA760; break; // Lighter toasted brown  
                case '=': color = 0xC680; break; // Soft yellowish brown  
                case '!': color = 0xE720; break; // Light pink glaze  
                case ';': color = 0xF740; break; // Strawberry glaze  
                case ':': color = 0xFA60; break; // Cherry glaze  
                case '~': color = 0xFC80; break; // Neon pink highlight  
                case '-': color = 0xFFD0; break; // Golden shine  
                case ',': color = 0xFEE0; break; // Soft warm highlight  
                case '.': color = 0xFFFF; break; // Brightest white  
                default:  color = 0x0000; break; // Black background  
            }
            draw_pixel(x_pos, y_pos, color);
        } else {  // Background pixels
            draw_pixel(x_pos, y_pos, 0x0000);
        }

        px_x++;  // Move right

        if (px_x >= 80) {  // Wrap to new line
            px_x = 0;
            px_y++;
        }
    }
    _ = cA;
    cA -= 5 * sA >> 7;
    sA += 5 * _ >> 7;
    _ = (3145728 - cA * cA - sA * sA) >> 11;
    cA = cA * _ >> 10;
    sA = sA * _ >> 10;
    ;
    _ = cB;
    cB -= 5 * sB >> 8;
    sB += 5 * _ >> 8;
    _ = (3145728 - cB * cB - sB * sB) >> 11;
    cB = cB * _ >> 10;
    sB = sB * _ >> 10;
    ;
  }
}
