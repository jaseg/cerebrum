#!/usr/bin/python

gamma = 1.7

values = []
for i_in in range(0x0, 0xff):
	i_out = ((i_in / 255.0)**gamma) * 0xffff
	values.append('%#x' % i_out)
	
print 'const uint16_t gammatable[] = {', ', '.join(values), '};'
	