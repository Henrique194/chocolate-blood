extern char CoolTable[];

void CellularFrame(char* pFrame, int sizeX, int sizeY)
{
	char* p = pFrame;
	int c = sizeX * sizeY;
	do
	{
		char* p2 = p + sizeX;
		unsigned int s = p2[-1] + p2[0] + p2[1] + p2[sizeX];
		if (p2[sizeX] > 96)
		{
			p2 += sizeX;
			s += p2[-1] + p2[0] + p2[1] + p2[sizeX];
			s >>= 1;
		}
		*p++ = CoolTable[s];
	} while (--c);
}
