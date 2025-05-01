#define M_PI 3.14159265358979323846264338327950288
#define M_PI_2 1.57079632679489661923
#define M_PI_M_2 6.283185307179586476925286766559

int _fltused = 0;

// https://gist.github.com/giangnguyen2412/bcab883b5a53b437b980d7be9745beaf

int compare_float(double f1, double f2)
{
    double precision = 0.00000000000000000001;
    if ((f1 - precision) < f2)
    {
        return -1;
    }
    else if ((f1 + precision) > f2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

double cos(double x)
{
    if (x < 0.0f)
        x = -x;

    if (0 <= compare_float(x, M_PI_M_2))
    {
        do
        {
            x -= M_PI_M_2;
        } while (0 <= compare_float(x, M_PI_M_2));
    }

    if ((0 <= compare_float(x, M_PI)) && (-1 == compare_float(x, M_PI_M_2)))
    {
        x -= M_PI;
        return ((-1) * (1.0f - (x * x / 2.0f) * (1.0f - (x * x / 12.0f) * (1.0f - (x * x / 30.0f) * (1.0f - (x * x / 56.0f) * (1.0f - (x * x / 90.0f) * (1.0f - (x * x / 132.0f) * (1.0f - (x * x / 182.0f)))))))));
    }
    return 1.0f - (x * x / 2.0f) * (1.0f - (x * x / 12.0f) * (1.0f - (x * x / 30.0f) * (1.0f - (x * x / 56.0f) * (1.0f - (x * x / 90.0f) * (1.0f - (x * x / 132.0f) * (1.0f - (x * x / 182.0f)))))));
}

double sin(double x) 
{ 
    return cos(x - M_PI_2); 
}

double tan(double x)
{
    return (sin(x) / cos(x));
}

double sqrt(double x)
{
    // let initial guess to be 1
    double z = 1.0f;
    for (int i = 1; i <= 10; i++)
    {
        z -= (z * z - x) / (2 * z); // MAGIC LINE!!
    }
    return z;
}

double fabs(double f)
{
    return f >= 0.0 ? f : -f;
}
