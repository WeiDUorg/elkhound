// bb10.c
// "construction of efficient generalized lr parsers": i == 10

int main()
{
  int a, b;

  a = b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b    // 1
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b

      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b    // 2
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b

      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b    // 3
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b

      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b    // 4
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b

      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b    // 5
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b

      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b    // 6
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b

      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b    // 7
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b

      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b    // 8
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b

      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b    // 9
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b

      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b    // 10
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      +b+b+b+b+b+b+b+b+b+b
      ;

  return a;
}
