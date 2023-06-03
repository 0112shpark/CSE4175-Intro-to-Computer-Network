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
  int parent;
} node;

// 주어진 i->j까지의 cost
int base_cost[MAX][MAX];

// 방문기록
int visited[MAX];

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
        vec_table[i][j].parent = -1;
      }
      else
      {
        vec_table[i][j].cost = NON;
        vec_table[i][j].is_connected = 0;
        vec_table[i][j].next = -1;
        base_cost[i][j] = NON;
        vec_table[i][j].parent = -1;
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

    vec_table[from][to].parent = from;
    vec_table[to][from].parent = to;
  }
}

// calculate the minimum cost
void cal_dist()
{
  int cur_node;

  for (int i = 0; i < node_cnt; i++)
  {
    int n_node = 1; // 방문한 노드 갯수
    cur_node = i;
    // 새로운 노드연산시 모든 방문기록 초기화
    for (int c = 0; c < node_cnt; c++)
    {
      visited[c] = 0;
    }
    visited[cur_node] = 1;
    while (n_node != node_cnt)
    {
      int min_cost = NON;
      int near_node;

      // 방문하지 않은 node중 가장 가까운 node찾기
      for (int k = 0; k < node_cnt; k++)
      {
        if (visited[k] == 0)
        {
          if (vec_table[i][k].cost < min_cost)
          {
            min_cost = base_cost[i][k];
            near_node = k;
          }
        }
      }
      cur_node = near_node;
      visited[cur_node] = 1;
      n_node++;

      for (int j = 0; j < node_cnt; j++)
      {

        // j node로 가기위한 더 가까운 node가 존재할 시
        if (vec_table[i][cur_node].cost + vec_table[cur_node][j].cost < vec_table[i][j].cost)
        {
          if (cur_node != j)
          {
            vec_table[i][j].next = vec_table[i][cur_node].next;
            vec_table[i][j].parent = cur_node;
            vec_table[i][j].cost = vec_table[i][cur_node].cost + vec_table[cur_node][j].cost;
          }
        }
        else if (vec_table[i][cur_node].cost + vec_table[cur_node][j].cost == vec_table[i][j].cost)
        {
          if (cur_node != j)
          {
            // 적은 ID를 가진 parents
            if (min(vec_table[i][j].parent, cur_node) == cur_node)
            {
              vec_table[i][j].next = vec_table[i][cur_node].next;
              vec_table[i][j].parent = cur_node;
            }
            vec_table[i][j].cost = vec_table[i][cur_node].cost + vec_table[cur_node][j].cost;
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
  output.open("output_ls.txt");
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

  cout << "Complete. Output file written to output_ls.txt." << endl;
  // close files
  //   topo_input.close();
  //   mess_input.close();
  //   change_input.close();
}