#include <htc.h>
#include "pixel.h"

static unsigned char buffer[PIXEL_COUNTS * 3] = { 0 };
static unsigned char buffer_index; // �o�b�t�@�̈ʒu��ێ�����
static unsigned char color; // �o�͂���F�̍�Ɨ̈�
static unsigned char color_index; // �o�͂��Ă���F�̎c��r�b�g��ێ�����

void pixel_set(unsigned short pos, unsigned char r, unsigned char g, unsigned char b) {
	if (pos >= PIXEL_COUNTS) return;
	// g, r, b�̏��ɑ��M����
	buffer[pos * 3] = g;
	buffer[pos * 3 + 1] = r;
	buffer[pos * 3 + 2] = b;
}

// ��s����(AVR):
// https://github.com/adafruit/Adafruit_NeoPixel/blob/56e662c32efcf7e4ce343c34f921848dac316025/Adafruit_NeoPixel.cpp#L291
void pixel_show(void) {
	buffer_index = PIXEL_COUNTS;
#asm
#include <aspic.h>
	bcf GIE					; GIE����
	movlw low _buffer		; �o�b�t�@�̃A�h���X����8�r�b�g��ǂݏo��
	movwf FSR0L				; FSR0L�ɔ��f
	movlw high _buffer		; �o�b�t�@�̃A�h���X���8�r�b�g��ǂݏo��
	movwf FSR0H				; FSR0H�ɔ��f
	BANKSEL (PORTA)			; PORTA�o���N��I��
pixel_loop:
	moviw FSR0++			; �o�b�t�@�Ɋi�[����Ă���l��ǂݏo���Ĥ�A�h���X���C���N�������g
	movwf _color			; �\���ϐ��ɐݒ�
	call serial_output		; 1�F�\��

	moviw FSR0++
	movwf _color
	call serial_output

	moviw FSR0++
	movwf _color
	call serial_output

	decfsz _buffer_index,f	; �V���A��LED�̑���
	goto pixel_loop			; 0�ł͂Ȃ���ΌJ��Ԃ��ĕ\��
	goto exit_inline		; 0�Ȃ�C�����C���A�Z���u���𔲂���

serial_output:
	movlw 8					; 8�r�b�g
	movwf _color_index		; W���W�X�^�ɐݒ�
serial_output_loop:
	rlf _color,f			; ���V�t�g(MSB)
	btfsc CARRY				; �L�����[���m�F
	goto serial_high		; �L�����[��1�Ȃ�high

; 0code���o��
	bsf RA0					; �o�̓|�[�ghigh
	nop						; �^�C�~���O���킹(400ns�}150ns)
	nop
	bcf RA0					; �o�̓|�[�glow
	nop						; �^�C�~���O���킹(850ns�}150ns)
	nop
	nop
	nop
	decfsz _color_index,f
	goto serial_output_loop	; 0�ł͂Ȃ���Ύc���\��
	return

serial_high:				; 1code���o��
	bsf RA0
	nop						; �^�C�~���O���킹(800ns�}150ns)
	nop
	nop
	nop
	bcf RA0
	nop						; �^�C�~���O���킹(450ns�}150ns)
	nop
	decfsz _color_index,f
	goto serial_output_loop
	return

exit_inline:
	bsf GIE					; GIE�L��
#endasm
	__delay_us(50); // ���Z�b�g�E�R�[�h
}
