#ifndef PIXEL_H
#define PIXEL_H

#define _XTAL_FREQ 32000000 // Fosc�̎��g��

#define PIXEL_COUNTS 144 // �J�X�P�[�h�ڑ�����Ă���V���A��LED�̌����w��

// �o�b�t�@�ɐF���i�[����
void pixel_set(unsigned short pos, unsigned char r, unsigned char g, unsigned char b);
// �o�b�t�@����F��ǂݏo���Ĕ��f������
void pixel_show(void);

#endif	/* PIXEL_H */
