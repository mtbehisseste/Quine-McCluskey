#include <iostream>
#include <map>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

map<int, int> one_bit_count;
int number_of_bits = 0;

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

void merge_neighbor_groups(vector<vector<vector<int> > > &group,
        vector<vector<vector<int> > > &candidate)
{
    int diff_bit;
    for (int i = 0; i < group.size() - 1; ++i) {
        candidate.push_back({});
        // compare each pair of number in two group
        for (int j = 0; j < group[i].size(); ++j) {
            for (int k = 0; k < group[i+1].size(); ++k) {
                diff_bit = one_bit_diff(group[i][j], group[i+1][k]);
                if (diff_bit != -1) {  // there's only 1 bit different
                    for (int m = 0; m < group[i][j].size(); ++m) {
                        cout << group[i][j][m];
                    }
                    cout << endl;
                    for (int m = 0; m < group[i+1][k].size(); ++m) {
                        cout << group[i+1][k][m];
                    }
                    cout << endl;
                    cout << diff_bit << endl;

                    // candidate[i] means the merge of group[i] and group[i+1]
                    vector<int> tmp = group[i][j];  // 2 is don't care bit
                    tmp[diff_bit] = 2;
                    candidate[i].push_back(tmp);
                }
            }
        }
    }
    cout << candidate.size() << endl;
    for (int i = 0; i < candidate.size(); ++i) {
        for (int j = 0; j < candidate[i].size(); ++j) {
            for (int k = 0; k < 4; ++k) {
                cout << candidate[i][j][k];
            }
            cout << ' ';
        }
        cout << endl;
    }
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

    vector<vector<vector<int> > > candidate;

    // group each number with the number of bit 1 of it
    grouping(on_set, group); 
    grouping(dontcare_set, group); 

    merge_neighbor_groups(group, candidate);

    return 0;
}
