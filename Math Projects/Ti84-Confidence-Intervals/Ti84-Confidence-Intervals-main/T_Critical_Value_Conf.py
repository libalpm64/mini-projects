from math import sqrt

def t_interval(n, mean, std, conf):
    df = n - 1
    print("Degrees of freedom = " + str(n) + " - 1 = " + str(df))
    print("For " + str(conf*100) + "% confidence level with df=" + str(df))
    print("Look up t-critical value in t-table")
    t = float(input("Enter t-critical value from table: "))
    
    margin = t * (std/sqrt(n))
    lower = mean - margin
    upper = mean + margin
    
    print("Calculation:")
    print("Margin of Error = " + str(t) + " * (" + str(std) + "/sqrt(" + str(n) + "))")
    print("               = " + str(t) + " * (" + str(std) + "/" + str(round(sqrt(n),4)) + ")")
    print("               = " + str(round(margin,4)))
    print(str(mean) + " +/- " + str(round(margin,4)))
    print("Interval = (" + str(round(lower,4)) + ", " + str(round(upper,4)) + ")")
    
    return lower, upper

while True:
    print("T-INTERVAL CALCULATOR")
    print("1: Calculate T-Interval") 
    print("2: Exit")
    
    choice = input("Choice: ")
    
    if choice == "1":
        n = float(input("N="))
        mean = float(input("MEAN="))
        std = float(input("STD="))
        conf = float(input("CONF="))
        
        lower, upper = t_interval(n, mean, std, conf)
        
    elif choice == "2":
        print("EXIT")
        break
        
    else:
        print("INVALID")
