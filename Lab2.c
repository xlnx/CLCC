fn strcmp(p1, p2)
{
	while (*p1 != 0 && *p1 == *p2) 
    {
    	++p1; ++p2;
    }
    return *p1 - *p2;
}
fn main()
{
	var pname = 0xF000;
	prints "Type a name and press Enter: ";
	scans pname;
	var count = 0;
	for (var p = 0x4000; p != 0; p = *p)
	{
    	var proom = p[1], pfirst = p[3], psecond = p[2];
    	if (!strcmp(pfirst, pname) || !strcmp(psecond, pname))
    	{
        	prints pfirst, " ", psecond, " ", proom, "\n";
        	count++;
        }
    }
    if (!count)
    {
    	prints "No Entry\n";
    }
}
