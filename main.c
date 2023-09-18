#include <htc.h>
#include <stdio.h>
#include "pixel.h"

// �R���t�B�O���[�V�������[�h�̐ݒ�
__CONFIG(FOSC_INTOSC &WDTE_OFF &PWRTE_ON &MCLRE_OFF &CP_OFF &CPD_OFF &BOREN_OFF &CLKOUTEN_OFF &IESO_OFF &FCMEN_OFF);
// FOSC_INTOSC  �I�V���[�^�I��, �����I�V���[�^���g�p
// WDTE_OFF     �E�H�b�`�h�b�O�^�C�}�[�L���r�b�g, ����
// PWRTE_ON     �p���[�A�b�v�^�C�}�[�L���r�b�g, �L��
// MCLRE_OFF    RE3/MCLR/Vpp�s���@�\�I���r�b�g, RE3�I��
// CP_OFF       �R�[�h�v���e�N�V�����r�b�g, �ی얳��
// CPD_OFF      �f�[�^�R�[�h�v���e�N�V�����r�b�g, �ی얳��
// BOREN_OFF    �u���E���A�E�g�E���Z�b�g�L���r�b�g, ����
// CLKOUTEN_OFF �N���b�N�o�͗L���r�b�g, ����
// IESO_OFF     ��i�K�N���b�N�؂�ւ��r�b�g, ����
// FCMEN_OFF    �t�F�C���E�Z�[�t�E�N���b�N���j�^�[�L���r�b�g, ����
__CONFIG(WRT_OFF &VCAPEN_OFF &PLLEN_ON &STVREN_OFF &LVP_OFF);
// WRT_OFF      �t���b�V���E������ ���ȏ������ݕی�r�b�g, ����
// VCAPEN_OFF   �d�����M�����[�^ �L���p�V�^�L���r�b�g, ����
// PLLEN_ON     PLL�L���r�b�g, �L��
// STVREN_OFF   �X�^�b�N�E�I�[�o�[�t���[��A���_�[�t���[���Z�b�g�r�b�g, ����
// LVP_OFF      ��d���v���O���~���O�L���r�b�g, ����

#define _XTAL_FREQ 32000000
#define SWITCH_MODE RA1

// �v���g�^�C�v�錾
void init(void);
// �ʒu�ɉ������F���v�Z����
void wheel(unsigned short wheel, unsigned char *r, unsigned char *g, unsigned char *b);
// �������[�h
void rainbow();											 // ���F(�Œ�)
void rainbow_cycle();									 // ���F(�Œ�, �eLED�P�̔���)
void strobing();										 // ���F(�ړ�)
void mono(unsigned char r, unsigned char g, unsigned b); // �P�F

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
	// ���X�|���X����̂��ߤ��d���[�v�������
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

// �Q�l
// https://github.com/adafruit/Adafruit_NeoPixel/blob/9cf5e96e8f436cc3460f6e7a32b20aceab6e905c/examples/strandtest_wheel/strandtest_wheel.ino#L60

void strobing()
{
	static unsigned short i = 0;
	i = i < 256 ? i : 0;
	// ���X�|���X����̂��ߤ��d���[�v�������
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
	const char MODE_MAX = 5; // 0���܂߂����[�h���̍ő�

	while (1)
	{
		// ���[�h�I���X�C�b�`�������ꂽ�ꍇ
		if (SWITCH_MODE)
		{
			// �`���^�����O�h�~
			__delay_ms(20);
			if (SWITCH_MODE)
			{
				mode++;
				// ���[�h����葽�����0�ɖ߂�
				mode = mode > MODE_MAX ? 0 : mode;
				// ����
				for (unsigned short i = 0; i < PIXEL_COUNTS; i++)
				{
					pixel_set(i, 0, 0, 0);
				}
				pixel_show();
				while (SWITCH_MODE)
					; // �������܂ő҂�
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
			mono(255, 0, 0); // ��
			break;
		case 4:
			mono(0, 255, 0); // ��
			break;
		case 5:
			mono(0, 0, 255); // ��
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
	ANSELA = 0; // ����
}

// �Q�l
// https://github.com/adafruit/Adafruit_NeoPixel/blob/9cf5e96e8f436cc3460f6e7a32b20aceab6e905c/examples/strandtest_wheel/strandtest_wheel.ino#L123
// �ʒu�ɉ������F���v�Z��������̃|�C���^�ɐݒ肷��

void wheel(unsigned short wheel, unsigned char *r, unsigned char *g, unsigned char *b)
{
	unsigned char pos = 255 - wheel;
	if (pos < 85) // ��~��
	{
		*r = 255 - pos * 3;
		*g = 0;
		*b = pos * 3;
	}
	else if (pos < 170) // ��~��
	{
		pos -= 85; // 3�{���Ă����Ȃ��悤�Ɉ���
		*r = 0;
		*g = pos * 3;
		*b = 255 - pos * 3;
	}
	else // ��~��
	{
		pos -= 170; // 3�{���Ă����Ȃ��悤�Ɉ���
		*r = pos * 3;
		*g = 255 - pos * 3;
		*b = 0;
	}
}
