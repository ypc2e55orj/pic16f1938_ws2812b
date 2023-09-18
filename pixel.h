#ifndef PIXEL_H
#define PIXEL_H

#define _XTAL_FREQ 32000000 // Foscの周波数

#define PIXEL_COUNTS 144 // カスケード接続されているシリアルLEDの個数を指定

// バッファに色を格納する
void pixel_set(unsigned short pos, unsigned char r, unsigned char g, unsigned char b);
// バッファから色を読み出して反映させる
void pixel_show(void);

#endif	/* PIXEL_H */
