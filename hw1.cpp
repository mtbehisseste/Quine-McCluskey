#include <iostream>
#include <map>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

map<int, int> one_bit_count;
int number_of_bits = 0;
vector<vector<int> > primary_implicants;

// create map of number of bit 1 in each number
// e.g. 0->0, 5->2
void create_one_bit_count()
{
    int cnt;
    for (int i = 0; i < (int)pow(2, number_of_bits); ++i) {
        cnt = 0;
        int num = i;
        while (num) {
            if (num % 2)
                cnt ++;
            num = num >> 1;
        }
        one_bit_count.insert(make_pair(i, cnt));
    }
}

vector<int> dec_to_bits(int a)
{
    vector<int> ret;
    for (int i = 0; i < number_of_bits; ++i)
        ret.push_back(0);
    int cnt = 0;
    while (a) {
        ret[number_of_bits - cnt++ - 1] = (a & 1);
        a >>= 1;
    }
    return ret;
}

// determine whether only one bit is different in two vector
// if so, return the index of the different bit
int one_bit_diff(vector<int> a, vector<int> b)
{
    int diff = -1;
    for (int i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) {
            if (diff != -1)  // second different appears
                return -1;
            diff = i;
        }
    }
    return diff;
}

void grouping(vector<int> &input_set, vector<vector<vector<int> > > &group)
{
    int group_index;
    for (int i = 0; i < input_set.size(); ++i) {
        group_index = one_bit_count[input_set[i]];
        group[group_index].push_back(dec_to_bits(input_set[i]));
    }
}

vector<vector<vector<int> > > merge_neighbor_groups(vector<vector<vector<int> > > &group)
{
    cout << group.size() << endl;
    int diff_bit;
    vector<vector<vector<int> > > new_group;
    int pi_matrix[group.size()][1024] = {0};  // matrix that record if merge happends
    for (int i = 0; i < group.size() - 1; ++i) {
        new_group.push_back({});
        // compare each pair of number in two group
        for (int j = 0; j < group[i].size(); ++j) {
            for (int k = 0; k < group[i+1].size(); ++k) {
                diff_bit = one_bit_diff(group[i][j], group[i+1][k]);
                if (diff_bit != -1) {  // there's only 1 bit different
                    pi_matrix[i][j] = 1;
                    pi_matrix[i+1][k] = 1;

                    // new_group[i] means the merge of group[i] and group[i+1]
                    vector<int> tmp = group[i][j];  // 2 is don't care bit
                    tmp[diff_bit] = 2;
                    new_group[i].push_back(tmp);
                }
            }
        }
    }

    // record primary implicants
    for (int i = 0; i < group.size(); ++i) {
        for (int j = 0; j < group[i].size(); ++j) {
            if (pi_matrix[i][j] == 0)  // for j-th index in group[i], no one-diff in group[i+1]
                primary_implicants.push_back(group[i][j]);
        }
    }
    
    return new_group;
}

int main()
{
    // TODO read input from file
    number_of_bits = 4;
    // vector<int> on_set = {4, 5, 6, 7, 8, 9, 10, 13};
    // vector<int> dontcare_set = {0, 7, 15};
    vector<int> on_set = {4, 5, 6, 8, 9, 10, 13};
    vector<int> dontcare_set = {0, 7, 15};

    create_one_bit_count();

    vector<vector<vector<int> > > group;  // group of one 1, two 1s, three 1s, etc
    for (int i = 0; i <= number_of_bits; ++i)
        group.push_back({});


    // group each number with the number of bit 1 of it
    grouping(on_set, group); 
    grouping(dontcare_set, group); 

    while (group.size())
        group = merge_neighbor_groups(group);

    cout << ".pi " << primary_implicants.size() << endl;
    int pi_size = primary_implicants.size();
    for (int i = 0; i < (pi_size > 20 ? 20 : pi_size); ++i) {
        for (int j = 0; j < primary_implicants[i].size(); ++j) {
            cout << primary_implicants[i][j];
        }
        cout << endl;
    }

    return 0;
}
