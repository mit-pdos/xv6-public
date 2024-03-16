#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define MAX_NUMBERS 7

int abs(int x) {
    return x >= 0 ? x : -x;
}

double sqrt(double x) {
    if (x < 0) return -1;

    double epsilon = 0.1;
    double guess = x;

    while (abs(guess * guess - x) >= epsilon) {
        guess = (guess + x / guess) / 2.0;
    }

    return guess;
}

int calculate_sum(int arr[], int size) {
    int sum = 0;
    for(int i = 0; i < size; i++){
        sum += arr[i];
    }
    return sum;
}

double calculate_mean(int arr[], int size)
{
    return (double)calculate_sum(arr, size) / size;
}

double calculate_variance(int arr[], int n) {
    double sum_of_squared_differences = 0.0;
    double mean = calculate_mean(arr, n);

    for (int i = 0; i < n; i++) {
        double difference = (double)arr[i] - mean;
        sum_of_squared_differences += difference * difference;
    }

    return sum_of_squared_differences / n;
}

double calculate_standard_deviation(int arr[], int n) {
    return sqrt(calculate_variance(arr, n));
}

void initializeBiggerAndSmallerNumbers(int numbers[], int size , int mean, int* smallNumbers, int* bigNumbers){
    int j = 0;
    int k = 0;
    for (int i = 0; i < size; i++) {
        if (numbers[i] <= mean) {
            smallNumbers[j++] = numbers[i];
        } else {
            bigNumbers[k++] = numbers[i];
        }
    }
}

void reverse(char s[]) {
    int i, j;
    char c;
    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void itoa(int n, char s[]) {
    int i, sign;
    if ((sign = n) < 0) {
        n = -n; // Make n positive
    }
    i = 0;
    do {
        s[i++] = n % 10 + '0'; // Get next digit
    } while ((n /= 10) > 0); // Delete it
    if (sign < 0) {
        s[i++] = '-';
    }
    s[i] = '\0'; // Null-terminate the string
    reverse(s);
}


int main(int argc, char* argv[]) {

    if (argc > 8){
        printf(1, "Error! : size of array is not valid");
        exit();
    }

    int numbers[MAX_NUMBERS];
    int size = argc - 1; 

    for (int i = 0; i < size; i++){
        numbers[i] = atoi(argv[i + 1]);
    }

    int mean = (int)calculate_mean(numbers, size);
    

    int smallCount = 0;
    int bigCount = 0;
    for (int i = 0; i < size; i++) {
        if (numbers[i] <= mean) {
            smallCount++;
        } else {
            bigCount++;
        }
    }

    int* smallNumbers = (int*)malloc(smallCount * sizeof(int));
    int* bigNumbers = (int*)malloc(bigCount * sizeof(int));
    

    initializeBiggerAndSmallerNumbers(numbers, size, mean, smallNumbers, bigNumbers);
    
    int sd = (int)calculate_standard_deviation(smallNumbers, smallCount);
    int variance = (int)calculate_variance(bigNumbers, bigCount);

    free(bigNumbers);
    free(smallNumbers);

    
    unlink("strdiff_result.txt");
    int fd = open("sdvar_result.txt", O_CREATE | O_WRONLY);
    if (fd >= 0) {
        
        char mean_str[10];
        char sd_str[10];
        char variance_str[10];

        itoa(mean, mean_str);
        itoa(sd, sd_str);
        itoa(variance, variance_str);

        write(fd, mean_str, strlen(mean_str));
        write(fd, " ", 1); 
        write(fd, sd_str, strlen(sd_str));
        write(fd, " ", 1); 
        write(fd, variance_str, strlen(variance_str));
        write(fd, "\n", 1); 

        close(fd);
    } else {
        printf(1, "Error opening file for writing.\n");
    }

    exit();
}
