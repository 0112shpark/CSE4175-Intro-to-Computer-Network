#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>

using namespace std;

// functions

void to_binary();
void handle_binary();
char xor_op(char a, char b);
void mod_2_div(vector<string> divide_by);

// input output file
ifstream input_file;
ofstream output_file;

int dataword;
string generator;
vector<char> origin_data;
string binary_data;
string codeword;
vector<string> remain;
vector<string> datawords;

int main(int argc, char *argv[])
{

  ios_base::sync_with_stdio(false);

  cin.tie(NULL);
  cout.tie(NULL);

  // handle error
  if (argc != 5)
  {
    // 인자의 갯수가 4개가 아닐 때
    cout << "usage: ./crc_encoder input_file output_file generator dataword_size\n";
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

  // get generator
  generator = argv[3];

  // dataword size error
  dataword = atoi(argv[4]);
  if (dataword != 4 && dataword != 8)
  {
    cout << "dataword size must be 4 or 8.\n";
    exit(1);
  }
  // cout << "generator: " << generator << "\n";

  // get data
  char c;
  while (input_file)
  {

    input_file.get(c);

    if (input_file.eof())
    {
      break;
    }

    origin_data.push_back(c);
  }
  // cout << "dd";
  to_binary();

  handle_binary();
}
// change to binary
void to_binary()
{
  for (int i = 0; i < origin_data.size(); i++)
  {
    bitset<8> bits(origin_data[i]);
    binary_data += bits.to_string();
  }
}

// handle division by adding 0s and divide by dataword size

void handle_binary()
{
  vector<string> divide_by;
  string zeros_to_add;

  // calculate #of zeros to add

  for (int i = 0; i < generator.size() - 1; i++)
  {
    zeros_to_add.push_back('0');
  }

  // split binary data by dataword size
  for (int i = 0; i < binary_data.size();)
  {
    string str;
    for (int j = i; j < i + dataword; j++)
    {
      str.push_back(binary_data[j]);
    }
    datawords.push_back(str);
    str += zeros_to_add;
    divide_by.push_back(str);
    i = i + dataword;
  }
  for (int i = 0; i < divide_by.size(); i++)
  {
    cout << "splited: " << divide_by[i] << endl;
  }

  mod_2_div(divide_by);
  // datawords에 나머지 붙이기
  for (int i = 0; i < datawords.size(); i++)
  {
    datawords[i] += remain[i];
    codeword += datawords[i];
  }

  // padding 계산
  int padding = 8 - (codeword.length() % 8);
  if (padding != 0)
  {
    for (int i = 0; i < padding; i++)
    {
      codeword.insert(0, 1, '0');
    }
  }
  // cout << codeword << endl;
  bitset<8> padding_cnt(padding);
  codeword.insert(0, padding_cnt.to_string());
  cout << codeword << endl;
  output_file << codeword << endl;
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

  for (int i = 0; i < divide_by.size(); i++)
  {
    char encoded_data[100];
    char temp[100];
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
    cout << "encoded:" << encoded_data << endl;
    remain.push_back(encoded_data);
  }
}
