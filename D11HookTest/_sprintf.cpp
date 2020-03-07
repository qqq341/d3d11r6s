#include "_sprintf.h"

#include <string>

#include <assert.h>

#include <stdarg.h>

#include <stdlib.h>

#include <Windows.h>

#ifndef va_copy

#define va_copy(dst, src) memcpy(&(dst), &(src), sizeof(va_list))

#endif



/************************************************************************

* @brief ??????

* @param[in] num ??

* @param[out] buf ???

* @return ???????

************************************************************************/

int _itoa(int num, char buf[32])

{

	return _i2a(num, buf, 10);

}



/************************************************************************

* @brief ??????

* @param[in] num ??

* @param[out] buf ???

* @param[in] radix ?????

* @return ???????

************************************************************************/

int _i2a(int num, char buf[32], int radix)

{

	static const char s[] = "0123456789abcdef";

	int n = num, R = radix;

	char* dst = buf;

	if (n < 0) { *dst++ = '-'; n = -n; }

	if (n < 10)

	{

		*dst++ = s[n]; *dst = 0;

	}
	else

	{

		char tmp[32], * p = tmp;

		while (n) { *p++ = s[n % R]; n /= R; }

		while (--p != tmp) * dst++ = *p;

		*dst++ = *tmp; *dst = 0;

	}

	return dst - buf;

}


/************************************************************************

* @brief ???????

* @param[in] val ???

* @param[out] buf ???

* @param[in] eps ??(???)

* @return ???????

************************************************************************/

int _ftoa(double val, char buf[32], int eps)

{

	double f = val;

	char* p = buf;

	if (val < 0) { *p++ = '-'; f = -f; }

	int n = f;

	int len = _itoa(n, p);

	return len + __ftoa(f - n, p + len, eps);

}



/************************************************************************

* @brief ??????????(-1, 1)

* @param[in] val ???

* @param[out] buf ???

* @param[in] eps ??(???)

* @return ???????

************************************************************************/

int __ftoa(double val, char buf[32], int eps)

{

	double f = val;

	char* p = buf;

	assert(-1.0 < f && f < 1.0 && eps > 0);

	static const char s[] = "0123456789";

	if (f < 0) { *p++ = '-'; f = -f; }*p++ = '.';

	for (int i = eps + 1, n; --i; ++p, f -= n)

		* p = s[n = f *= 10.0];

	*p = 0;

	return p - buf;

}



/************************************************************************

* @brief ??sprintf

* @ref ????????????——va_arg???????

* http://www.cppblog.com/ownwaterloo/archive/2009/04/21/80655.aspx

************************************************************************/

int __sprintf(char* dst, const char* format, ...)

{

	char* s = dst;

	const char* f = format;

	va_list ap, another;

	va_start(ap, format);

	va_copy(another, ap);

	while (*f)

	{

		int n = 1;

		if ('%' != *f)

		{

			*s = *f;

		}
		else {

			++f;

			switch (*f)

			{

			case 's':// ???

			{

				const char* p = va_arg(ap, char*);

				n = strlen(p);

				memcpy(s, p, n);

			}

			break;



			case 'd': case 'u':// ??

			{

				char buf[32];

				n = _itoa(va_arg(ap, int), buf);

				memcpy(s, buf, n);

			}

			break;



			case 'f':// ???

			{

				char buf[32];

				n = _ftoa(va_arg(ap, double), buf, 5);

				memcpy(s, buf, n);

			}

			break;



			case 'x':// 16???

			{

				char buf[32];

				n = _i2a(va_arg(ap, int), buf, 16);

				memcpy(s, buf, n);

			}

			break;



			case 'c':// ??

			{

				*s = va_arg(ap, int);

			}

			break;



			case '%':// ???

			{

				*s = '%';

			}

			break;
			case 'l':// 16???
			{
				char buf[32];
				_i64toa(va_arg(ap, DWORD64), buf, 16);
				n = sizeof(buf);
				memcpy(s, buf, n);
			}
			break;


			default:

			{

				//	assert(0);

					//va_end(ap);

					//int x = vsprintf(dst, format, another);

					//va_end(another);

					//return x;

			}

			break;

			}

		}

		++f;

		s += n;

	}

	*s = 0;

	va_end(ap);

	return s - dst;

}