fn fib(a)
{
	if (a <= 1) return 1;
	return fib(a - 1) + fib(a - 2);
}
fn main()
{
	print fib(9);
}
