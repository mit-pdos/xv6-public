
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

