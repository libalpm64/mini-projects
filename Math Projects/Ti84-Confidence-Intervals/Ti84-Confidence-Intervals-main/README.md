# Ti84-Confidence-Intervals
A little math side project for the Ti-84 Python CE calculator.

# Some roadblocks
### RB 1
When I was coding this, the Ti-84 Python Ce uses something called Circuit Python [https://circuitpython.org/]. It is an open-source Python fork. However, Texas Instruments doesn't have support for PiP or any modules such as Scipy. The only thing it has support for is randomness and math. For statisticians, this means we are out of luck when it comes down to it; it cannot be fully automated. ( They have a T-critical calculator with the area and stuff in there, but honestly, it's not great as it basically hardcodes T-critical values by Texas instruments. You can, for example, get the 
For the critical value on a Ti-84, you do the following: 2nd, VARS, invT (Option 4) Enter the area to the left of the critical value; for example, if we are finding 14 DF and a confidence of 0.9 (90%)
We would do the following: (1-0.9), which leaves 10%, then we need to find both sides unless explicitly stated that we are only finding one tail, so we divide it by two.
(1-0.9)/2 = 0.05 
Now subtract 0.05 from 1:
1 - 0.05 = 0.09
Then input invT(0.95, 14)
which will give you the T critical value. This seems very impratical. I would rather use a table than do this, and sometimes if it's not hard coded, it will calculate the wrong value.
### RB 2
The fact that you can only use 2049 bytes for calculation makes it really difficult to code things with modern languages such as Python or C. It's so constrained that I had to split the two files as we would run out of memory.
and would throw an overflow error. 

### Rb 3
Ti-84 doesn't have a direct implementation of the Vars values, so you can't even make use of their invT function on the Python CE calculator, meaning you can't even automate the process. (Please add this capability, Texas instruments.) 
 
## Attempt 1:
I attempted to use Student's T-Distrubition to calculate the T critical. Is it calculated using the cumulative distrubtion function (CDF) of the t-distrubition. Here is the equation:
t = (x_bar - mew) / (standard_dev / sqr(population))
X_bar = Sample mean
mew = population mean
standard_dev = sample standard deviation
population = sample size

Okay, so we follow the t-statistics with the distribution with the degree of freedom, which is calculated using
df = n - 1

Now, the T critical value (tₛₜ) is determined based on the desired significance level (α) and degrees of freedom (df). It is the value that separates the rejection region from the non-rejection region in hypothesis testing. You will reject the null hypothesis if the absolute value of the t-statistic exceeds the T critical value.

T Distribution and Critical Values
For a one-tailed test (left- or right-tailed), you calculate the critical value from the cumulative probability corresponding to the significance level α.
For a two-tailed test, the critical values are determined by dividing the significance level by 2 (α/2) for each tail.

The T critical value is the point at which the cumulative probability from the left of the distribution equals the significance level. This means:
P(T≤t_α)=1−α for right-tailed
P(T≥t_a)=α for left-tailed
P(∣T∣≥t_α/2)=α for two-tailed
_ = The subscript of a or a/2 for two-tailed 

This did not work, however, as our calculator cannot compute accurately as it doesn't have enough computation to go through many trials like traditionally something like Scipy would do in a millisecond on modern-day CPUs.

## Attempt 2 
Gamma function (Lancoz approximation)
The gamma function is an extension of a factorial function. When you input N, the gamma function will return n-1. It can also handle non-interger values. This should've produced a good result, right? Yes, it did; however, the calculator was stuck on calculating for well over 20 minutes. The reason for this is we have to use the reflection formula for negative alues and do the Lanczos series and get an approximation for the range using a series function, then pass it over for beta. Here's how it follows in code format:

```
def gamma(x):
    g = 7
    p = [0.99999999999980993, 676.5203681218851, -1259.1392167224028, ...]
    
    if x < 0.5:
        return pi / (sin(pi * x) * gamma(1 - x))  # Reflection formula (I have no idea what I'm doing here)
    x -= 1  # Adjusting for the Lanczos series
    a = p[0]
    t = x + g + 0.5
    
    for i in range(1, 9):  # Approximation using series
        a += p[i] / (x + i)
    
    return sqrt(2 * pi) * pow(t, x + 0.5) * exp(-t) * a
```
Now Defining Beta;
```
def beta(a, b):
    return gamma(a) * gamma(b) / gamma(a + b)
```

Now we have to iterate this using the T-CDF, which is computed using number integration. The purpose is to compute the cumulative probality of the t-distrubition at a given value t and for a given degree of freedom (DF). This is calculated by integrating the probability density function (PDF) of the t-distrubution. 

```
def t_cdf(t, df):
    # Compute the cumulative probability of t-distribution using numerical integration
    def integrand(u):
        return (1 + u**2 / df) ** (-(df + 1) / 2)

    # Use numerical integration (trapezoidal rule) to compute CDF
    steps = 10000
    a, b = 0, t
    step_size = (b - a) / steps
    area = 0.5 * integrand(a) + 0.5 * integrand(b)

    for i in range(1, steps):
        area += integrand(a + i * step_size)

    cdf = 0.5 + area * step_size / (sqrt(df * pi) * beta(0.5, df / 2))
    return cdf
```

## Attempt 3
Binary based calculation 
Start by defining a range of t-values. The t-distribution's values range from negative infinity to positive infinity, so we can set an upper and lower bound, say -1000 to 1000.
Narrow down the range based on whether the CDF of the current middle point of the range is above or below the target value.
Well, it is in fact a lot faster; however, the calculation turns out completely wrong, and I'm unsure as to why I was probably not given enough information. As I'm not a statistician, feel free to correct me.
```
def t_cdf(x, df):
    # Approximate CDF for t-distribution using the incomplete beta function approximation
    a = df / 2.0
    b = 0.5
    t = x / math.sqrt(df)
    return 0.5 + 0.5 * math.erf(t / math.sqrt(2))

def binary_search_t_critical(df, target_area, lower_bound=-10, upper_bound=10, epsilon=1e-6):
    while upper_bound - lower_bound > epsilon:
        mid = (lower_bound + upper_bound) / 2
        cdf_value = t_cdf(mid, df)
        
        if cdf_value < target_area:
            lower_bound = mid  # We need a higher t-value
        else:
            upper_bound = mid  # We need a lower t-value
            
    return (lower_bound + upper_bound) / 2

t_critical = binary_search_t_critical(df, area_left)
```
When given a value here it's way off.

### Attempt 4
Using a table to calculate it This is by far the best option and quite easy to do; it doesn't take that much time compared to other options, and it is correct every time. 
I made it quite dummy proof for the TCritical values by telling you exactly what you need to do so you don't mess it up and forgetting to subtract 1. 
![image](https://github.com/user-attachments/assets/6cb83f45-3311-45ce-875c-11b1de5b53df)
