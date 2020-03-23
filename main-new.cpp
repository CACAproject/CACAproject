//
//  main.cpp
//  MM
//
//  Created by shanxigy on 2018/1/18.
//  Copyright © 2018年 shanxigy. All rights reserved.
//

#include <cstring>
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cmath>
using namespace std;

struct world0 {
    int num_of_nodes[4] , expand_pop , learnfreq , allownumber[4] , f1 = 8 , f2 = 6;
    void newworld()
    {
        num_of_nodes[0] = 7;
        num_of_nodes[1] = 2;
        num_of_nodes[2] = 1;
        expand_pop = 1000;
        learnfreq = 24;
    }
};

world0 world;
int flag1 , tmpport , tmpup;

struct tree {
    string name;
    int popularity, hit;
    bool leaf , allow;
    tree *son[1000];
    int sum_of_sons , chosen_times , tier;
    
    void refresh(string name0)
    {
        name = name0;
        popularity = hit = 0;
        leaf = 1 , allow = 0;
        sum_of_sons = chosen_times = tier = 0;
    }
};

bool cmp(tree *a , tree *b)
{
    double sa = log((double)a -> hit + 8) / log((double)a -> chosen_times + 6) * log((double)a -> tier + 9);
    double sb = log((double)b -> hit + 8) / log((double)b -> chosen_times + 6) * log((double)b -> tier + 9);
    return sa > sb;
}

struct node0 {
    tree root;
    int cache_size , hit , miss , tier , num , spacesize , chosensize , allownumber , expand_pop , delete_pop;
    string cache[1000] , chosen[1000];
    tree *space[100000];
    double expand_hit;
    
    string segment(string name , unsigned long l)
    {
        unsigned long len = name.size();
        for (unsigned long i = l; i < name.size(); ++i)
            if (name[i] == '/') {
                len = i + 1;
                break;
            }
        return name.substr(0 , len);
    }
    
    bool allow(string name)
    {
        for (int i = 0; i < chosensize; ++i)
            if (chosen[i] == name.substr(0 , chosen[i].size())) return 1;
        return 0;
    }
    
    void count(string name , int hit)
    {
        tree *p = &root;
        int flag;
        while (1) {
            p -> popularity++;
            p -> hit += hit;
            if (p -> leaf) break;
            flag = 0;
            for (int i = 0; i < p -> sum_of_sons; i++)
                if (p -> son[i] -> name == name.substr(0 , p -> son[i] -> name.size())) {
                    p = p -> son[i];
                    flag = 1;
                    break;
                }
            if (!flag && p -> sum_of_sons < 1000) {
                tree *q = new tree;
                q -> refresh(segment(name , p -> name.size()));
                q -> tier = p -> tier + 1;
                p -> son[p -> sum_of_sons++] = q;
                p = q;
            }
        }
    }
    
    void leaf_collection(tree *p)
    {
        if (p -> leaf) space[spacesize++] = p;
        for (int i = 0; i < p -> sum_of_sons; ++i)
            leaf_collection(p -> son[i]);
    }
    
    void admission()
    {
        spacesize = chosensize = 0;
        leaf_collection(&root);
        sort(space , space + spacesize , cmp);
        for (int i = 0; i < allownumber; ++i) {
            if (i >= spacesize) break;
            space[i] -> chosen_times++;
            chosen[chosensize++] = space[i] -> name;
        }
        if (tier != 2 && num == 0)
            cout << "TreeSize: " << spacesize << endl;
    }
    
    void division(tree *p)
    {
        int j = 0;
        for (int i = 0; i < p -> sum_of_sons; ++i) {
            p -> son[j] = p -> son[i];
            if (p -> son[i] -> popularity == 0) {
                delete p -> son[i];
                continue;
            }
            division(p -> son[j++]);
        }
        p -> sum_of_sons = j;
        if (p -> leaf) {
            if ((double)p -> hit / (double)p -> popularity < expand_hit && p -> popularity > expand_pop) p -> leaf = 0;
        } else {
            if (p -> popularity < delete_pop) {
                for (int i = 0; i < p -> sum_of_sons; ++i)
                    delete p -> son[i];
                p -> refresh(p -> name);
            }
        }
        p -> hit = p -> hit * 1 / 2 , p -> popularity = p -> popularity * 1 / 2;
    }
    
    void printtree(tree *p , int layer)
    {
        if (tier != 0 || num != 0) return;
        for (int i = 0; i < layer; ++i)
            cout << "     ";
        cout << p -> name << " " << p -> hit << " " << p -> popularity << " " << p -> sum_of_sons << endl;
        for (int i = 0; i < p -> sum_of_sons; ++i)
            printtree(p -> son[i] , layer + 1);
    }
    
    int forward(string name)
    {
        if (tier == 1) return 0;
        tmpup = (tmpup + 1) % 2;
        return tmpup;
        /*int x = 0;
        for (int i = 0; i < name.size(); ++i)
            x = ((x * 991 + name[i]) % 997 + 997) % 997;
        x =  x / (997 / world.num_of_nodes[tier + 1]);
        if (x >= world.num_of_nodes[tier + 1]) x = world.num_of_nodes[tier + 1] - 1;
        return x;*/
    }
    
    void replace(string name, int p)
    {
        for (int i = p - 1; i >= 0; i--)
            cache[i + 1] = cache[i];
        cache[0] = name;
    }
    
    int check(string name)
    {
        if (tier == 2) return 0;
        for (int i = 0; i < cache_size; ++i)
            if (cache[i] == name) return i;
        return -1;
    }
};

node0 node[3][7];

void work(string name , int x , int y)
{
    int isallowed = node[x][y].allow(name);
    if (x <= 1 && !isallowed) {
        ++node[x][y].miss;
        node[x][y].count(name , 0);
        work(name , x + 1 , node[x][y].forward(name));
        return;
    }
    int tmp = node[x][y].check(name);
    if (tmp != -1) {
        ++node[x][y].hit;
        node[x][y].replace(name , tmp);
        node[x][y].count(name , 1);
    } else {
        ++node[x][y].miss;
        node[x][y].count(name , 0);
        node[x][y].replace(name , node[x][y].cache_size);
        work(name , x + 1 , node[x][y].forward(name));
    }
}

int M , port[2000000];
double time0;
string name[2000000];

int main(int argc, const char * argv[]) {
    world.newworld();
    freopen("input_b-new.txt" , "r" , stdin);
    freopen("result.txt" , "w" , stdout);
    cin >> M;
    for (int i = 0; i < M; ++i) {
        cin >> name[i] >> time0 >> port[i];
        port[i]--;
    }
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < world.num_of_nodes[i]; ++j)
            node[i][j].cache_size = (i == 0 ? 400 : 1000);
    for (int x = 3; x <= 3; x = x + 1)
        for (int y = 500; y <= 500; y = y + 500)
            for (int z = 500; z <= 500; z *= 2) {
                printf("Test:  x %d y %d\n" , x , y);
                for (int i = 0; i < 3; ++i)
                    for (int j = 0; j < world.num_of_nodes[i]; ++j) {
                        node[i][j].hit = node[i][j].miss = 0;
                        for (int k = 0; k < node[i][j].cache_size; ++k)
                            node[i][j].cache[k] = "";
                        node[i][j].root.refresh("");
                        node[i][j].root.leaf = 1;
                        node[i][j].tier = i;
                        node[i][j].num = j;
                        node[i][j].allownumber = (i == 0 ? 3 : 160);
                        node[i][j].expand_pop = (i == 0 ? 1000 : 2000);
                        node[i][j].delete_pop = node[i][j].expand_pop / 2;
                        node[i][j].expand_hit = (i == 0 ? 0.35 : 0.35);
                    }
                for (int i = 0; i < M; ++i) {
                    if ((i % (M / world.learnfreq)) == 0) {
                        //cout << "time: " << i << endl;
                        for (int j = 0; j < 3; ++j)
                            for (int k = 0; k < world.num_of_nodes[j]; ++k) {
                                //node[j][k].printtree(&node[j][k].root , 0);
                                node[j][k].admission();
                                node[j][k].division(&node[j][k].root);
                            }
                    }
                    work(name[i] , 0 , tmpport);
                    tmpport = (tmpport + 1) % 7;
                }
                for (int i = 0; i < 3; ++i)
                    for (int j = 0; j < world.num_of_nodes[i]; ++j)
                        cout << i << " " << j << " " << node[i][j].hit << " " << node[i][j].miss << " " << 1.0 * node[i][j].hit / (1.0 * (node[i][j].hit + node[i][j].miss)) << endl;
            }
    return 0;
}
