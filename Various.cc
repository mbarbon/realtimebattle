long
factorial(const int n)
{
  long fact = 1;
  for(int i=1; i<=n; i++)  
    fact *= i;

  return fact;
}

long
binomial(const int n, const int k)
{
  return factorial(n) / ( factorial(k) * factorial(n-k) );
}
