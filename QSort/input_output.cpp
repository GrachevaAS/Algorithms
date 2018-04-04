#include "input_output.hpp"

const int buf_size = 4096;

void writeChar(char x, int& write_buf_pos) {
    static char write_buf[buf_size];
	if (write_buf_pos == buf_size) {
		fwrite(write_buf, 1, buf_size, stdout), write_buf_pos = 0;
	}
	write_buf[write_buf_pos++] = x;
    if (x == '\n') {
        fwrite(write_buf, 1, write_buf_pos, stdout);
    }
}

void writeInt(int x, int& write_buf_pos) {
	char s[10];
	int n = 0;
	while (x || !n)
		s[n++] = '0' + x % 10, x /= 10;
	while (n--)
		writeChar(s[n], write_buf_pos);
	writeChar(' ', write_buf_pos);
}

bool readInt(int & out)
{
    int c = getchar_unlocked();
    int x = 0;
    for (; !('0'<=c && c<='9'); c = getchar_unlocked())
    {
        if (c == EOF)
            return false;
    }
    if (c == EOF)
        return false;
    for (; '0' <= c && c <= '9' ; c = getchar_unlocked())
    {
        x = x*10 + c - '0';
    }
    out = x;
    return true;
}

