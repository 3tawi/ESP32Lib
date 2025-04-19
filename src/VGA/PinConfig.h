/*
	Author: bitluni 2019
	License: 
	Creative Commons Attribution ShareAlike 4.0
	https://creativecommons.org/licenses/by-sa/4.0/
	
	For further details check out: 
		https://youtube.com/bitlunislab
		https://github.com/bitluni
		http://bitluni.net
*/
#pragma once

class PinConfig
{
  public:
	int r0, r1, r2, r3, r4;
	int g0, g1, g2, g3, g4;
	int b0, b1, b2, b3;
	int hSync, vSync, clock;
	PinConfig(
		const int r0 = -1,
		const int r1 = -1,
		const int r2 = -1,
		const int r3 = -1,
		const int r4 = -1,
		const int g0 = -1,
		const int g1 = -1,
		const int g2 = -1,
		const int g3 = -1,
		const int g4 = -1,
		const int b0 = -1,
		const int b1 = -1,
		const int b2 = -1,
		const int b3 = -1,
		const int hSync = -1,
		const int vSync = -1,
		const int clock = -1)
		: r0(r0),
		  r1(r1),
		  r2(r2),
		  r3(r3),
		  r4(r4),
		  g0(g0),
		  g1(g1),
		  g2(g2),
		  g3(g3),
		  g4(g4),
		  b0(b0),
		  b1(b1),
		  b2(b2),
		  b3(b3),
		  hSync(hSync),
		  vSync(vSync),
		  clock(clock)
	{
	}

	void fill3Bit(int *pins) const
	{
		pins[0] = r4;
		pins[1] = g4;
		pins[2] = b3;
		pins[3] = -1;
		pins[4] = -1;
		pins[5] = -1;
		pins[6] = hSync;
		pins[7] = vSync;
	}
};