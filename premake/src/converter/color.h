#ifndef _COLOR_H_
#define _COLOR_H_


class Color3
{
public :
	unsigned char R;
	unsigned char G;
	unsigned char B;

	Color3(unsigned char R = 0, unsigned char G = 0, unsigned char B = 0) : R(R), G(G), B(B) {}
};

class Color4
{
public :
	unsigned char R;
	unsigned char G;
	unsigned char B;
	unsigned char A;

	Color4(unsigned char R = 0, unsigned char G = 0, unsigned char B = 0, unsigned char A = 255) : R(R), G(G), B(B), A(A) {}
};

#endif
