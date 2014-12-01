#include <avr/io.h>

/**
 * pow() function that requires integers for both base and exponent
 * AND that will return an unsigned long in turn. so DON'T use it for negative
 * exponents.
 *
 * @param int base
 * @param int exponent
 * @return unsigned long
 */
unsigned long ulpow(int base, int exponent) {
    unsigned long result = 1;

    while (exponent) {
        if (exponent & 1) {
            result *= base;
        }

        exponent >>= 1;
        base *= base;
    }

    return result;
}