#!env python3

import sys

hex = sys.argv[1].upper()
split_size = 2
split_hex = [hex[i:i+split_size] for i in range(0, len(hex), split_size)]
print('{', end="")
for i in split_hex[:-1]:
    print('0x' + i + ',', end=" ")

print('0x' + split_hex[-1] + '}')

