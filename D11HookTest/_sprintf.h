//x
#pragma once



// ??????

inline int _itoa(int num, char buf[32]);



int _i2a(int num, char buf[32], int radix = 10);



// ??????

int _ftoa(double val, char buf[32], int eps = 6);



// ??????????-1?1?

int __ftoa(double val, char buf[32], int eps = 6);



// ??sprintf

int __sprintf(char* dst, const char* format, ...);