#ifndef INPUT_OUTPUT_HPP
#define INPUT_OUTPUT_HPP

#include <cstdlib>
#include <cstdio>
#include <fstream>

void writeChar(char x, int& write_buf_pos);
void writeInt(int x, int& write_buf_pos);
bool readInt(int & out);

#endif
