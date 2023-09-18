#include <htc.h>
#include "pixel.h"

static unsigned char buffer[PIXEL_COUNTS * 3] = { 0 };
static unsigned char buffer_index; // バッファの位置を保持する
static unsigned char color; // 出力する色の作業領域
static unsigned char color_index; // 出力している色の残りビットを保持する

void pixel_set(unsigned short pos, unsigned char r, unsigned char g, unsigned char b) {
	if (pos >= PIXEL_COUNTS) return;
	// g, r, bの順に送信する
	buffer[pos * 3] = g;
	buffer[pos * 3 + 1] = r;
	buffer[pos * 3 + 2] = b;
}

// 先行事例(AVR):
// https://github.com/adafruit/Adafruit_NeoPixel/blob/56e662c32efcf7e4ce343c34f921848dac316025/Adafruit_NeoPixel.cpp#L291
void pixel_show(void) {
	buffer_index = PIXEL_COUNTS;
#asm
#include <aspic.h>
	bcf GIE					; GIE無効
	movlw low _buffer		; バッファのアドレス下位8ビットを読み出し
	movwf FSR0L				; FSR0Lに反映
	movlw high _buffer		; バッファのアドレス上位8ビットを読み出し
	movwf FSR0H				; FSR0Hに反映
	BANKSEL (PORTA)			; PORTAバンクを選択
pixel_loop:
	moviw FSR0++			; バッファに格納されている値を読み出して､アドレスをインクリメント
	movwf _color			; 表示変数に設定
	call serial_output		; 1色表示

	moviw FSR0++
	movwf _color
	call serial_output

	moviw FSR0++
	movwf _color
	call serial_output

	decfsz _buffer_index,f	; シリアルLEDの総数
	goto pixel_loop			; 0ではなければ繰り返して表示
	goto exit_inline		; 0ならインラインアセンブリを抜ける

serial_output:
	movlw 8					; 8ビット
	movwf _color_index		; Wレジスタに設定
serial_output_loop:
	rlf _color,f			; 左シフト(MSB)
	btfsc CARRY				; キャリーを確認
	goto serial_high		; キャリーが1ならhigh

; 0codeを出力
	bsf RA0					; 出力ポートhigh
	nop						; タイミング合わせ(400ns±150ns)
	nop
	bcf RA0					; 出力ポートlow
	nop						; タイミング合わせ(850ns±150ns)
	nop
	nop
	nop
	decfsz _color_index,f
	goto serial_output_loop	; 0ではなければ残りを表示
	return

serial_high:				; 1codeを出力
	bsf RA0
	nop						; タイミング合わせ(800ns±150ns)
	nop
	nop
	nop
	bcf RA0
	nop						; タイミング合わせ(450ns±150ns)
	nop
	decfsz _color_index,f
	goto serial_output_loop
	return

exit_inline:
	bsf GIE					; GIE有効
#endasm
	__delay_us(50); // リセット・コード
}
