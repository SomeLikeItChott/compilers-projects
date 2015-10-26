program lotsoferrors(input);
var x: integer;
begin
	read(x);
	x := x + 2.50;
	x := x - 12345678901 + 055;
	x := x + 2.0E123;
	x := x + 123456.3 - 4.123456;
	write(x)@
end.
