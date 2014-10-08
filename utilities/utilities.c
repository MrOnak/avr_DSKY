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

/**
 * returns the position of value in array if found, or -1 otherwise
 *
 * note that both array and value must contain uint16_t compatible data
 * and that array must not contain more than 66536 values total
 *
 * @param	uint16_t arr
 * @param	uint16_t value
 * @return	int16_t
 */
int16_t arrayPos16(uint16_t arr[], uint16_t value) {
	uint16_t i;
	uint16_t len = sizeof(arr);
	
	for (i = 0; i < len; i++) {
		if (arr[i] == value) {
			return i;
		}
	}
	
	return -1;
}
/**
 * returns the position of value in array if found, or -1 otherwise
 *
 * note that both array and value must contain uint8_t compatible data
 * and that array must not contain more than 255 values total
 *
 * @param	uint8_t arr
 * @param	uint8_t value
 * @return	int8_t
 */
int8_t arrayPos8(uint8_t arr[], uint8_t value) {
	uint8_t i;
	uint8_t len = sizeof(arr);
	
	for (i = 0; i < len; i++) {
		if (arr[i] == value) {
			return i;
		}
	}
	
	return -1;
}
