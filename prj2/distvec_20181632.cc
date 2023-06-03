#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <stdlib.h>
#include <sstream>

using namespace std;
#define MAX 100
#define NON 999

// input output file
ifstream topo_input, mess_input, change_input;
ofstream output;

vector<pair<pair<int, int>, int>> saved;
typedef struct node
{
  int cost;
  int next;
  int is_connected;
} node;

// 주어진 i->j까지의 cost
int base_cost[MAX][MAX];

// i->j까지 가는 최적의 경로
node vec_table[MAX][MAX];
int node_cnt;

// functions

// initiallize distance-vector table and base-cost

void init_vec_table()
{
  string str;
  topo_input.clear();
  topo_input.seekg(0, std::ios::beg);
  getline(topo_input, str);
  node_cnt = stoi(str);

  for (int i = 0; i < MAX; i++)
  {
    for (int j = 0; j < MAX; j++)
    {
      if (i == j)
      {
        vec_table[i][j].cost = 0;
        vec_table[i][j].is_connected = 0;
        vec_table[i][j].next = i;
        base_cost[i][j] = 0;
      }
      else
      {
        vec_table[i][j].cost = NON;
        vec_table[i][j].is_connected = 0;
        vec_table[i][j].next = -1;
        base_cost[i][j] = NON;
      }
    }
  }

  // read base topology file and inset values
  while (getline(topo_input, str))
  {
    int from, to, cost;
    stringstream value(str);
    value >> from >> to >> cost;
    // cout << from << to << cost << endl;

    base_cost[from][to] = cost;
    base_cost[to][from] = cost;

    vec_table[from][to].cost = cost;
    vec_table[to][from].cost = cost;

    vec_table[to][from].is_connected = 1;
    vec_table[from][to].is_connected = 1;

    vec_table[to][from].next = from;
    vec_table[from][to].next = to;
  }
}

// calculate the minimum cost
void cal_dist()
{
  for (int c = 0; c < node_cnt; c++)
  {
    for (int i = 0; i < node_cnt; i++)
    {
      for (int j = 0; j < node_cnt; j++)
      {
        // 서로 연결되어 있다면
        if (vec_table[i][j].is_connected == 1)
        {
          // 연결되어 있는 node와 내 node의 cost +
          // 연결되어 있는 node에서 k까지의 거리
          // < 내 node에서 k까지의 거리
          // then update!
          for (int k = 0; k < node_cnt; k++)
          {
            if (base_cost[i][j] + vec_table[j][k].cost < vec_table[i][k].cost)
            {
              vec_table[i][k].cost = base_cost[i][j] + vec_table[j][k].cost;
              vec_table[i][k].next = j;
            }
            // cost가 같을 경우 더 작은 node 선택
            else if (base_cost[i][j] + vec_table[j][k].cost == vec_table[i][k].cost)
            {
              vec_table[i][k].cost = base_cost[i][j] + vec_table[j][k].cost;
              vec_table[i][k].next = min(j, vec_table[i][k].next);
            }
          }
        }
      }
    }
  }
}

// print distance vector table
void print_vec_table()
{

  for (int i = 0; i < node_cnt; i++)
  {
    for (int j = 0; j < node_cnt; j++)
    {
      if (vec_table[i][j].cost != NON)
      {
        output << j << " " << vec_table[i][j].next << " " << vec_table[i][j].cost << endl;
      }
    }
    output << endl;
  }
}

// handle messages
void handle_message()
{
  int from, to;
  string str;

  // get message NONormation

  // reset file input stream
  mess_input.clear();
  mess_input.seekg(0, std::ios::beg);
  while (getline(mess_input, str))
  {
    string msg = "";
    int cnt = 0;
    int index = 0;
    stringstream message(str);
    message >> from >> to;

    // get message
    while (cnt < 2)
    {
      if (str[index] == ' ')
      {
        cnt++;
      }
      index++;
    }
    for (int i = index; i < (int)str.size(); i++)
    {
      msg.append(str, index, 1);
      index++;
    }

    // no way to get destination
    if (vec_table[from][to].cost >= NON)
    {
      output << "from " << from << " to " << to << " cost infinite hops unreachable message " << msg << endl;
    }
    else
    {
      vector<int> hops;

      int cost = vec_table[from][to].cost;
      int cur_from = from;
      hops.push_back(cur_from);
      while (vec_table[cur_from][to].next != to)
      {
        cur_from = vec_table[cur_from][to].next;
        hops.push_back(cur_from);
      }
      output << "from " << from << " to " << to << " cost " << cost << " hops ";
      for (int i = 0; i < (int)hops.size(); i++)
      {

        output << hops[i] << " ";
      }
      output << "message " << msg << endl;
    }
  }
  output << endl;
}

// handle change
void handle_change()
{

  int from, to, cost;
  string str;
  while (getline(change_input, str))
  {

    init_vec_table();

    stringstream change(str);
    change >> from >> to >> cost;
    int new_connected = 1;

    for (int i = 0; i < (int)saved.size(); i++)
    {
      base_cost[saved[i].first.first][saved[i].first.second] = saved[i].second;
      base_cost[saved[i].first.second][saved[i].first.first] = saved[i].second;

      vec_table[saved[i].first.first][saved[i].first.second].cost = saved[i].second;
      vec_table[saved[i].first.second][saved[i].first.first].cost = saved[i].second;
    }
    if (cost == -999)
    {
      cost = NON;
      new_connected = 0;
    }
    base_cost[from][to] = cost;
    base_cost[to][from] = cost;

    vec_table[from][to].cost = cost;
    vec_table[to][from].cost = cost;

    vec_table[to][from].is_connected = new_connected;
    vec_table[from][to].is_connected = new_connected;
    // cout << vec_table[from][to].cost << endl;

    if (cost != NON)
    {

      vec_table[to][from].next = from;
      vec_table[from][to].next = to;
    }
    saved.push_back(make_pair(make_pair(from, to), cost));
    cal_dist();
    print_vec_table();
    handle_message();
  }
}
int main(int argc, char *argv[])
{

  ios_base::sync_with_stdio(false);

  cin.tie(NULL);
  cout.tie(NULL);

  if (argc != 4)
  {
    cout << "usage: distvec topologyfile messagesfile changesfile" << endl;
    exit(1);
  }

  // open input file

  topo_input.open(argv[1]);
  if (!topo_input.is_open())
  {
    cout << "Error: open input file." << endl;
    exit(1);
  }

  mess_input.open(argv[2]);
  if (!mess_input.is_open())
  {
    cout << "Error: open input file." << endl;
    exit(1);
  }
  change_input.open(argv[3]);
  if (!change_input.is_open())
  {
    cout << "Error: open input file." << endl;
    exit(1);
  }

  // open output
  output.open("output_dv.txt");
  if (!output.is_open())
  {
    cout << "Error: open output file." << endl;
    exit(1);
  }

  init_vec_table();
  cal_dist();
  print_vec_table();
  handle_message();
  handle_change();

  cout << "Complete. Output file written to output_dv.txt." << endl;
  // close files
  //   topo_input.close();
  //   mess_input.close();
  //   change_input.close();
}