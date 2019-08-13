
/*
 * Get int representation of number in string.
 * String must be null terminated.
 */
int atoi(char * str)
{
    int res = 0;
    for (int i = 0; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';

    return res;
}