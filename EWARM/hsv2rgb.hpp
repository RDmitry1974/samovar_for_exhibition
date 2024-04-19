
#ifndef __HSV2RGB_HPP
#define __HSV2RGB_HPP

#ifdef __cplusplus
extern "C" {
#endif


typedef unsigned char uint8_t;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB_t;

typedef struct {
    uint8_t h;
    uint8_t s;
    uint8_t v;
} HSV_t;

RGB_t hsv2rgb(HSV_t hsv);

#ifdef __cplusplus
}
#endif

#endif

