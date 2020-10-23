#include <iostream>
#include <cstdio>
#include <map>
#include <vector>
#include <set>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <cstring>

using namespace std;

map<int, int> one_bit_count;
int number_of_bits = 0;
set<vector<int> > primary_implicants;
set<vector<int> > minimum_covering;

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

bool check_exist(vector<vector<vector<int> > > new_group, vector<int> tmp)
{
    for (int i = 0; i < new_group.size(); ++i) {
        for (int j = 0; j < new_group[i].size(); ++j) {
            if (new_group[i][j] == tmp)
                return true;
        }
    }
    return false;
}

// given a primary implicant, return all numbers covered by this PI.
set<int> pi_covered(vector<int> pi) {
    set<int> tmp;
    for (int i = 0; i < pi.size(); ++i) {
        if (pi[i] == 2) {
            // recursively strip don't care bits and get covered numbers
            pi[i] = 0;
            set<int> zero = pi_covered(pi);
            pi[i] = 1;
            set<int> one = pi_covered(pi);
            for (int j = 0; j < zero.size(); ++j)
                tmp.insert(*next(zero.begin(), j));
            for (int j = 0; j < one.size(); ++j)
                tmp.insert(*next(one.begin(), j));
        }
    }

    int ans = 0;
    for (int i = pi.size() - 1; i >= 0; --i)
        ans += (int)(pow(2, pi.size() - 1 - i)) * pi[i];
    tmp.insert(ans);
    return tmp;
}

vector<vector<vector<int> > > find_primary_implcants(vector<vector<vector<int> > > &group)
{
    int diff_bit;
    vector<vector<vector<int> > > new_group;
    int pi_matrix[group.size()][1024] = {};  // matrix that record if merge happends
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
                    if (!check_exist(new_group, tmp))
                        new_group[i].push_back(tmp);
                }
            }
        }
    }

    // record primary implicants
    for (int i = 0; i < group.size(); ++i) {
        for (int j = 0; j < group[i].size(); ++j) {
            if (pi_matrix[i][j] == 0)  // for j-th index in group[i], no one-diff in group[i+1]
                primary_implicants.insert(group[i][j]);
        }
    }
    
    return new_group;
}

void find_min_SOP(vector<int> on_set)
{
    // the map stores the value and its corresponding index in on_set
    map<int, int> on_set_index;
    map<int, int> on_set_index_rev;
    for (int i = 0; i < on_set.size(); ++i) {
        on_set_index.insert(make_pair(on_set[i], i));
        on_set_index_rev.insert(make_pair(i, on_set[i]));
    }

    int count_column[on_set.size()] = {};  // count the number of primary implicants
                                           // that covers the number in on_set
    int count_row[primary_implicants.size()] = {};  // count the number of number in
                                                    // on_set that is covered by each PI
    int table[primary_implicants.size()][on_set.size()] = {};

    // build table
    vector<set<int> > pic_group;
    for (int i = 0; i < primary_implicants.size(); ++i) {
        set<int> pic = pi_covered(*next(primary_implicants.begin(), i));
        for (int k = 0; k < pic.size(); ++k) {
            int n = *next(pic.begin(), k);
            // if the covered number is in on_set
            if (on_set_index.find(n) != on_set_index.end()) {
                count_column[on_set_index[n]]++;
                count_row[i]++;
                table[i][on_set_index[n]] = 1;
            }
        }
        pic_group.push_back(pic);
    }

    // find essential implicant
    for (int i = 0; i < on_set.size(); ++i) {
        if (count_column[i] == 1) {
            // for all numbers that are covered with this PI
            for (int j = 0; j < pic_group.size(); ++j) {
                // if the covered number is in on_set
                if (pic_group[j].find(on_set_index_rev[i]) != pic_group[j].end()) {
                    // for each number covered by the primary implicant
                    for (auto k: pic_group[j]) {
                        // for the whole column of the number
                        for (int m = 0; m < pic_group.size(); ++m) {
                            if (table[m][on_set_index[k]] == 1) {
                                count_row[m]--;
                            }
                            table[m][on_set_index[k]] = 0;
                        }
                        minimum_covering.insert(*next(primary_implicants.begin(), j));
                        count_column[i] = 0;
                    }
                }
            }
        }
    }
    
    while (*max_element(count_row,
                count_row + primary_implicants.size()) > 0) {
        int max_index = distance(count_row,
                max_element(count_row, count_row + primary_implicants.size()));
        // for each number covered by the primary implicant
        for (auto k: pic_group[max_index]) {
            // for the whole column of the number
            for (int m = 0; m < pic_group.size(); ++m) {
                if (table[m][on_set_index[k]] == 1)
                    count_row[m]--;
                table[m][on_set_index[k]] = 0;
            }
            minimum_covering.insert(*next(primary_implicants.begin(), max_index));
            count_column[k] = 0;
        }
    }
}

int calc_cost()
{
    int cost = 0;
    for (int i = 0; i < minimum_covering.size(); ++i) {
        vector<int> tmp = *next(minimum_covering.begin(), i);
        int sz = tmp.size();
        cost += count(tmp.begin(), tmp.begin() + sz, 1);
        cost += count(tmp.begin(), tmp.begin() + sz, 0);
    }
    return cost;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        cout << "Usage: ./hw1 [input file name] [output file name]\n";
        exit(-1);
    }
        
    fstream finput, foutput;
    finput.open(argv[1], ios::in);
    foutput.open(argv[2], ios::out);
    if (!finput || !foutput) {
        cout << "Error reading input/output file\n";
        exit(-1);
    }

    vector<int> on_set, dontcare_set;

    // read input from file
    char line[1000];
    char *token;
    finput.getline(line, 1000);
    token = strtok(line, " ");  // ".i"
    token = strtok(NULL, " ");  // number_of_bits
    number_of_bits = atoi(token);
    
    // read on set
    finput.getline(line, 1000);  // ".m"
    finput.getline(line, 1000);  // on set number
    token = strtok(line, " ");
    while (token) {
        on_set.push_back(atoi(token));
        token = strtok(NULL, " ");
    }
    
    // read don't care set
    finput.getline(line, 1000);  // ".m"
    finput.getline(line, 1000);  // on set number
    token = strtok(line, " ");
    while (token) {
        dontcare_set.push_back(atoi(token));
        token = strtok(NULL, " ");
    }
    
    // number_of_bits = 5;
    // vector<int> on_set = {4, 5, 6, 8, 9, 10, 13};
    // vector<int> dontcare_set = {0, 7, 15};
    // vector<int> on_set = {2,6,8,9,10,11,14,15};
    // vector<int> dontcare_set = {};
    // vector<int> on_set = {4, 8, 10, 11, 12, 15};
    // vector<int> dontcare_set = {9, 14};

    // count number of bit 1 for all number below 2^(number_of_bit)
    create_one_bit_count();

    vector<vector<vector<int> > > group;  // group of one 1, two 1s, three 1s, etc
    for (int i = 0; i <= number_of_bits; ++i)
        group.push_back({});


    // group each number with the number of bit 1 of it
    grouping(on_set, group); 
    grouping(dontcare_set, group); 

    // find primary implicants
    while (group.size())
        group = find_primary_implcants(group);

    // output primary implicants to file
    foutput << ".pi " << primary_implicants.size() << endl;
    int pi_size = primary_implicants.size();
    for (int i = 0; i < (pi_size > 20 ? 20 : pi_size); ++i) {
        for (int j = 0; j < (*next(primary_implicants.begin(), i)).size(); ++j) {
            if ((*next(primary_implicants.begin(), i))[j] == 2)
                foutput << '-';
            else
                foutput << (*next(primary_implicants.begin(), i))[j];
        }
        foutput << endl;
    }

    // find minimum cover
    find_min_SOP(on_set);

    // output minimum cover to file
    foutput << endl;
    foutput << ".mc " << minimum_covering.size() << endl;
    for (int i = 0; i < minimum_covering.size(); ++i) {
        for (int j = 0; j < (*next(minimum_covering.begin(), i)).size(); ++j) {
            if ((*next(minimum_covering.begin(), i))[j] == 2)
                foutput << '-';
            else
                foutput << (*next(minimum_covering.begin(), i))[j];
        }
        foutput << endl;
    }

    foutput << "cost=" << calc_cost() << endl;

    finput.close();
    foutput.close();
    return 0;
}
