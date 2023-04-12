#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <cstring>
#include <stdlib.h>

using namespace std;

// functions

char xor_op(char a, char b);
void mod_2_div(vector<string> divide_by);
int check_error();
void convert_to_string();

// input output file
ifstream input_file;
ofstream output_file;
ofstream result_file;

int dataword;
int codeword_size;
string generator;
vector<char> encoded_binary;
string encoded_data;

vector<string> remain;
vector<string> divide_to;
string decoded_str;
string final_str;

int main(int argc, char *argv[])
{

  ios_base::sync_with_stdio(false);

  cin.tie(NULL);
  cout.tie(NULL);

  // handle error
  if (argc != 6)
  {
    // 인자의 갯수가 5개가 아닐 때
    cout << "usage: ./crc_decoder input_file output_file result_file generator dataword_size\n";
    exit(1);
  }

  // input file open error
  input_file.open(argv[1], ios::binary);
  if (!input_file.is_open())
  {
    cout << "input file open error.\n";
    exit(1);
  }

  // output file open error
  output_file.open(argv[2], ios::binary);
  if (!output_file.is_open())
  {
    cout << "output file open error.\n";
    exit(1);
  }

  // result file open error
  result_file.open(argv[3], ios::binary);
  if (!result_file.is_open())
  {
    cout << "result file open error.\n";
    exit(1);
  }

  // get generator
  generator = argv[4];

  // dataword size error
  dataword = atoi(argv[5]);
  if (dataword != 4 && dataword != 8)
  {
    cout << "dataword size must be 4 or 8.\n";
    exit(1);
  }

  // get data
  char c;
  while (input_file)
  {

    input_file.get(c);
    // if (c == ' ')
    // {
    //   input_file.get(c);
    // }
    if (input_file.eof())
    {
      break;
    }
    encoded_binary.push_back(c);
    // cout << int(c) << endl;
  }
  // cout << encoded_binary.size() << endl;

  string temp;
  int val;

  for (int i = 0; i < encoded_binary.size(); i++)
  {

    temp = bitset<8>(encoded_binary[i]).to_string();
    // cout << "temp:" << temp << endl;
    // val = strtol(temp.c_str(), 0, 2);
    // cout << "val:" << val << endl;
    encoded_data += temp;
  }

  // cout << encoded_data << endl;
  // get padding count
  int padding_num = strtol(encoded_data.substr(0, 8).c_str(), 0, 2);
  // cout << padding_num << endl;
  encoded_data = encoded_data.substr(8 + padding_num);

  // codeword의 길이
  codeword_size = generator.size() + dataword - 1;
  // cout << "codeword count: " << codeword_size << endl;
  // encode된 data codeword로 자르기
  int index = 0;
  for (int i = 0; i < encoded_data.size() / codeword_size; i++)
  {
    string divide;

    for (int j = index; j < index + codeword_size; j++)
    {
      divide.push_back(encoded_data[j]);
    }
    index += codeword_size;
    divide_to.push_back(divide);
  }
  // cout << "cnt: " << divide_to.size() << endl;
  mod_2_div(divide_to);
  int error_cnt = check_error();
  // cout << "#of codewords: " << divide_to.size() << endl;
  // cout << "error: " << error_cnt << endl;
  result_file << divide_to.size() << " " << error_cnt;
  // string 으로 변환
  convert_to_string();
}

void convert_to_string()
{
  for (int i = 0; i < divide_to.size(); i++)
  {
    // cout << divide_to[i] << endl;
    decoded_str += divide_to[i].substr(0, dataword);
  }
  // cout << decoded_str << endl;

  char temp;

  int index = 0;
  for (int i = 0; i < decoded_str.size() / 8; i++)
  {
    string temp_string;
    for (int j = index; j < index + 8; j++)
    {
      temp_string += decoded_str[j];
    }
    index += 8;
    temp = strtol(temp_string.c_str(), 0, 2);
    output_file << temp;
  }
}

int check_error()
{
  // error check
  int cnt = 0;
  for (int i = 0; i < remain.size(); i++)
  {
    if (atoi(remain[i].c_str()) > 0)
    {
      cnt++;
    }
  }
  return cnt;
}

char xor_op(char a, char b)
{
  if (a == b)
    return '0';
  else
    return '1';
}
void mod_2_div(vector<string> divide_by)
{

  char encoded_data[100];
  char temp[100];
  for (int i = 0; i < divide_by.size(); i++)
  {
    for (int j = 0; j < generator.size(); j++)
    {
      encoded_data[j] = divide_by[i][j];
    }
    for (int k = generator.size(); k <= divide_by[i].size(); k++)
    {

      for (int j = 0; j < generator.size(); j++)
      {
        temp[j] = encoded_data[j];
      }
      if (encoded_data[0] == '0')
      {
        for (int j = 0; j < generator.size() - 1; j++)
        {
          encoded_data[j] = temp[j + 1];
        }
      }
      else
      {
        for (int j = 0; j < generator.size() - 1; j++)
        {
          encoded_data[j] = xor_op(temp[j + 1], generator[j + 1]);
        }
      }
      encoded_data[generator.size() - 1] = divide_by[i][k];
    }
    // cout << "decoded:" << encoded_data << endl;
    remain.push_back(encoded_data);
  }
}
