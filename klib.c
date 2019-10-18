
/*
 * Get int representation of number in string.
 * String must be null terminated.
 */
int atoi(char * str)
{
    int res = 0;
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] < '0' || str[i] > '9')
            return -1;
        res = res * 10 + str[i] - '0';
    }
    return res;
}

/*
 * Set buf to string representation of number in int.
 */
int itoa(char * buf, int n)
{
    int i = n;
    int length = 0;

    while (i > 0) {
        length++;
        i /= 10;
    }

    char revbuf[length];

    if (n == 0) {
        *buf++ = '0';
    }
    for (i = 0; n > 0 && i < length; i++) {
        revbuf[i] = (n % 10) + '0';
        n /= 10;
    }
    while (--i >= 0) {
        *buf++ = revbuf[i];
    }
    *buf = '\0';
    return length;
}

/*
 * Set buf to string representation of number in uint.
 */
int utoa(char * buf, unsigned int n)
{
    int i = 0;
    unsigned int m = n;
    unsigned int j = n;
    unsigned int length = 0;

    while (m > 0) {
        length++;
        m /= 10;
    }

    char revbuf[length];

    for (i = 0; n > 0 && i < length; i++) {
        revbuf[i] = (n % 10) + '0';
        n /= 10;
    }
    while (--i >= 0) {
        *buf++ = revbuf[i];
    }

    if (j == 0) {
        *buf++ = '0';
        length++;
    }
    *buf = '\0';
    return length;
}

/*
 * Returns the number of digits in the integer.
 */
int intlen(int n)
{
    int len = 1;
    while(n/10 != 0){
        n /= 10;
        len++;
    }
    return len;
}
