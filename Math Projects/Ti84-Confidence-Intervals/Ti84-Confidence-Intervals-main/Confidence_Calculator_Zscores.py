from ti_system import *
from math import *

def z_crit(c):
  if c==0.80: return 1.282
  if c==0.90: return 1.645
  if c==0.95: return 1.960
  if c==0.98: return 2.326
  if c==0.99: return 2.576
  return 0

while 1:
  print("CONF CALC")
  print("1:MEAN")
  print("2:PROPORTION") 
  print("3:SAMPLE SIZE")
  print("4:POINT EST")
  
  c=input("CHOICE:")
  
  if c=="1":
    print("1:MARGIN 2:INTERVAL")
    ch=input("CHOICE:")
    if ch=="1":
      n=float(input("N="))
      s=float(input("STD="))
      cl=float(input("CONF="))
      z=z_crit(cl)
      m=z*(s/sqrt(n))
      print(str(z)+"*"+str(s)+"/sqrt("+str(n)+") = "+str(round(m,4)))
    elif ch=="2":
      m=float(input("MEAN="))
      n=float(input("N="))
      s=float(input("STD="))
      cl=float(input("CONF="))
      z=z_crit(cl)
      mg=z*(s/sqrt(n))
      print(str(m)+" +/- "+str(round(mg,4)))
      print("("+str(round(m-mg,4))+", "+str(round(m+mg,4))+")")
  
  elif c=="2":
    print("1:MARGIN 2:INTERVAL") 
    ch=input("CHOICE:")
    if ch=="1":
      n=float(input("N="))
      x=float(input("SUCCESS="))
      cl=float(input("CONF="))
      p=x/n
      z=z_crit(cl)
      m=z*sqrt((p*(1-p))/n)
      print(str(z)+"*sqrt("+str(round(p,4))+"*"+str(round(1-p,4))+"/"+str(n)+") = "+str(round(m,4)))
    elif ch=="2":
      n=float(input("N="))
      x=float(input("SUCCESS="))
      cl=float(input("CONF="))
      p=x/n
      z=z_crit(cl)
      m=z*sqrt((p*(1-p))/n)
      print(str(round(p,4))+" +/- "+str(round(m,4)))
      print("("+str(round(p-m,4))+", "+str(round(p+m,4))+")")
  
  elif c=="3":
    print("1:MEAN 2:PROPORTION")
    ch=input("CHOICE:")
    if ch=="1":
      s=float(input("STD="))
      m=float(input("MARGIN="))
      cl=float(input("CONF="))
      z=z_crit(cl)
      n=ceil((z*s/m)**2)
      print("("+str(z)+"*"+str(s)+"/"+str(m)+")^2 = "+str(n))
    elif ch=="2":
      m=float(input("MARGIN="))
      cl=float(input("CONF="))
      p=input("PROP(.5):")
      p=0.5 if p=="" else float(p)
      z=z_crit(cl)
      n=ceil((z**2*p*(1-p))/(m**2))
      print("("+str(z)+"^2*"+str(p)+"*"+str(1-p)+")/"+str(m)+"^2 = "+str(n))

  elif c=="4":
    print("1:MEAN 2:PROP")
    ch=input("CHOICE:")
    if ch=="1":
      n=int(input("N="))
      t=0
      for i in range(n):
        x=float(input("x"+str(i+1)+"="))
        t+=x
        print(str(round(t,4)))
      print(str(round(t,4))+"/"+str(n)+" = "+str(round(t/n,4)))
    elif ch=="2":
      n=float(input("N="))
      x=float(input("SUCCESS="))
      print(str(x)+"/"+str(n)+" = "+str(round(x/n,4)))
  
  else:
    print("INVALID")