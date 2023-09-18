#include <htc.h>
#include <stdio.h>
#include "pixel.h"

// コンフィグレーションワードの設定
__CONFIG(FOSC_INTOSC &WDTE_OFF &PWRTE_ON &MCLRE_OFF &CP_OFF &CPD_OFF &BOREN_OFF &CLKOUTEN_OFF &IESO_OFF &FCMEN_OFF);
// FOSC_INTOSC  オシレータ選択, 内蔵オシレータを使用
// WDTE_OFF     ウォッチドッグタイマー有効ビット, 無効
// PWRTE_ON     パワーアップタイマー有効ビット, 有効
// MCLRE_OFF    RE3/MCLR/Vppピン機能選択ビット, RE3選択
// CP_OFF       コードプロテクションビット, 保護無効
// CPD_OFF      データコードプロテクションビット, 保護無効
// BOREN_OFF    ブラウンアウト・リセット有効ビット, 無効
// CLKOUTEN_OFF クロック出力有効ビット, 無効
// IESO_OFF     二段階クロック切り替えビット, 無効
// FCMEN_OFF    フェイル・セーフ・クロックモニター有効ビット, 無効
__CONFIG(WRT_OFF &VCAPEN_OFF &PLLEN_ON &STVREN_OFF &LVP_OFF);
// WRT_OFF      フラッシュ・メモリ 自己書き込み保護ビット, 無効
// VCAPEN_OFF   電圧レギュレータ キャパシタ有効ビット, 無効
// PLLEN_ON     PLL有効ビット, 有効
// STVREN_OFF   スタック・オーバーフロー､アンダーフローリセットビット, 無効
// LVP_OFF      低電圧プログラミング有効ビット, 無効

#define _XTAL_FREQ 32000000
#define SWITCH_MODE RA1

// プロトタイプ宣言
void init(void);
// 位置に応じた色を計算する
void wheel(unsigned short wheel, unsigned char *r, unsigned char *g, unsigned char *b);
// 発光モード
void rainbow();											 // 虹色(固定)
void rainbow_cycle();									 // 虹色(固定, 各LED単体発光)
void strobing();										 // 虹色(移動)
void mono(unsigned char r, unsigned char g, unsigned b); // 単色

void rainbow()
{
	for (unsigned char i = 0; i < PIXEL_COUNTS; i++)
	{
		unsigned char r, g, b;
		wheel((i * 2) & 0xFF, &r, &g, &b);
		pixel_set(i, r, g, b);
	}
	pixel_show();
}

void rainbow_cycle()
{
	static unsigned char i = 0;
	i = i < PIXEL_COUNTS ? i : 0;
	// レスポンス向上のため､二重ループを避ける
	// for (unsigned char i = 0; i < PIXEL_COUNTS; i++)
	//{
	for (unsigned char j = 0; j < PIXEL_COUNTS; j++)
	{
		if (i == j)
		{
			unsigned char r, g, b;
			wheel((j * 2) & 0xFF, &r, &g, &b);
			pixel_set(j, r, g, b);
		}
		else
		{
			pixel_set(j, 0, 0, 0);
		}
	}
	pixel_show();
	//}
	i++;
}

// 参考
// https://github.com/adafruit/Adafruit_NeoPixel/blob/9cf5e96e8f436cc3460f6e7a32b20aceab6e905c/examples/strandtest_wheel/strandtest_wheel.ino#L60

void strobing()
{
	static unsigned short i = 0;
	i = i < 256 ? i : 0;
	// レスポンス向上のため､二重ループを避ける
	// for (unsigned short i = 0; i < 256; i++)
	//{
	for (unsigned short j = 0; j < PIXEL_COUNTS; j++)
	{
		unsigned char r, g, b;
		wheel((i + j) & 0xFF, &r, &g, &b);
		pixel_set(j, r, g, b);
	}
	pixel_show();
	//}
	i++;
}

void mono(unsigned char r, unsigned char g, unsigned b)
{
	for (unsigned short i = 0; i < PIXEL_COUNTS; i++)
	{
		pixel_set(i, r, g, b);
	}
	pixel_show();
}

void main(void)
{
	init();
	char mode = 0;
	const char MODE_MAX = 5; // 0を含めたモード数の最大

	while (1)
	{
		// モード選択スイッチが押された場合
		if (SWITCH_MODE)
		{
			// チャタリング防止
			__delay_ms(20);
			if (SWITCH_MODE)
			{
				mode++;
				// モード数より多ければ0に戻る
				mode = mode > MODE_MAX ? 0 : mode;
				// 消灯
				for (unsigned short i = 0; i < PIXEL_COUNTS; i++)
				{
					pixel_set(i, 0, 0, 0);
				}
				pixel_show();
				while (SWITCH_MODE)
					; // 離されるまで待つ
			}
		}

		switch (mode)
		{
		case 0:
			rainbow_cycle();
			break;
		case 1:
			strobing();
			break;
		case 2:
			rainbow();
			break;
		case 3:
			mono(255, 0, 0); // 赤
			break;
		case 4:
			mono(0, 255, 0); // 緑
			break;
		case 5:
			mono(0, 0, 255); // 青
			break;
		default:
			break;
			// do nothing
		}
	}
}

void init(void)
{
	// OSC
	OSCCON = 0b11110000; // 32MHz
	// Tri-state
	TRISA = 0b11111110; // RA0 Output
	// A/D
	ANSELA = 0; // 無効
}

// 参考
// https://github.com/adafruit/Adafruit_NeoPixel/blob/9cf5e96e8f436cc3460f6e7a32b20aceab6e905c/examples/strandtest_wheel/strandtest_wheel.ino#L123
// 位置に応じた色を計算し､引数のポインタに設定する

void wheel(unsigned short wheel, unsigned char *r, unsigned char *g, unsigned char *b)
{
	unsigned char pos = 255 - wheel;
	if (pos < 85) // 青~赤
	{
		*r = 255 - pos * 3;
		*g = 0;
		*b = pos * 3;
	}
	else if (pos < 170) // 緑~青
	{
		pos -= 85; // 3倍しても溢れないように引く
		*r = 0;
		*g = pos * 3;
		*b = 255 - pos * 3;
	}
	else // 赤~緑
	{
		pos -= 170; // 3倍しても溢れないように引く
		*r = pos * 3;
		*g = 255 - pos * 3;
		*b = 0;
	}
}
