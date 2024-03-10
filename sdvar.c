#define MAX_NUMBERS 7

float calculateSD(double data[],int n) {
    float sum = 0.0, mean, SD = 0.0;
    int i;
    for (i = 0; i < n; ++i) {
        sum += data[i];
    }
    mean = sum / n;
    for (i = 0; i < n; ++i) {
        SD += pow(data[i] - mean, 2);
    }
    return sqrt(SD / n);
}
float Variance(double a[], int n)
{
    int sum = 0;
    for (int i = 0; i < n; i++)
        sum += a[i];
    double mean = (double)sum / (double)n;
 
    double sqDiff = 0;
    for (int i = 0; i < n; i++) 
        sqDiff += (a[i] - mean) * 
                  (a[i] - mean);
    return sqDiff / n;
}
int main() {
    double numbers[MAX_NUMBERS];
    int count = 0;
    double sum = 0.0;

    // Read input numbers
    printf("Enter up to %d numbers (separated by spaces):\n", MAX_NUMBERS);
    while (count < MAX_NUMBERS && scanf("%lf", &numbers[count]) == 1) {
        sum += numbers[count];
        count++;
    }

    // Calculate average
    int average = sum / count;
    
    // Calculate standard deviation and variance
    double standard_deviation_array[MAX_NUMBERS];
    double variance_array[MAX_NUMBERS];
    int sda_temp = 0;
    int va_temp = 0;
    for(int i=0; i<count; i++)
    {
        if(numbers[i] <= average)
        {
            
            standard_deviation_array[sda_temp] = numbers[i];
            sda_temp++;
        }
        else
        {
            variance_array[va_temp] = numbers[i];
            va_temp++;
        }
    }
    int standard_deviation = calculateSD(standard_deviation_array, sda_temp);
    int variance = Variance(variance_array, va_temp);
    // Save results to a file
    FILE *file = fopen("txt.result_sdvar", "w");
    if (file != NULL) {
        fprintf(file, "%d\n", average);
        fprintf(file, "%d\n", standard_deviation);
        fprintf(file, "%d\n", variance);
        fclose(file);
    } else {
        printf("Error opening file for writing.\n");
    }

    return 0;
}
