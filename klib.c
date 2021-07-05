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
    int m = n;
    int length = 0;

    while (m > 0) {
        length++;
        m /= 10;
    }

    if (n == 0) {
        buf[0] = '0';
        length++;
    }
    for (int i = length; n > 0 && i > 0; i--) {
        buf[i - 1] = (n % 10) + '0';
        n /= 10;
    }
    buf[length] = '\0';
    return length;
}

/*
 * Set buf to string representation of number in uint.
 */
int utoa(char * buf, unsigned int n)
{
    unsigned int m = n;
    int length = 0;

    while (m > 0) {
        length++;
        m /= 10;
    }

    if (n == 0) {
        buf[0] = '0';
        length++;
    }
    for (int i = length; n > 0 && i > 0; i--) {
        buf[i - 1] = (n % 10) + '0';
        n /= 10;
    }
    buf[length] = '\0';
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
