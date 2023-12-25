#include <iostream>
#include <fstream>
#include <vector>

using namespace std;


// m >> number of descendants
//2 * m + 1 >> size of each node (record)
struct BTreeNode {
    int isLeaf;  // 0 for leaf, 1 for non-leaf
    vector<int>children; // record IDs or references to child nodes

};

void createFile(const char* filename, int recordNum, int m) {
    BTreeNode node;
    int nodeSize = 2 * m + 1;
    fstream file(filename, ios::binary| ios::out| ios::app|ios::in);
    for (int i = 0; i < recordNum; ++i) {

        node.isLeaf = 0;

        for (int j = 0; j < nodeSize; ++j) {

            node.children.push_back(-1);

        }

        file.write(reinterpret_cast<char*>(&node), sizeof(BTreeNode));

        //file.read(reinterpret_cast<char*>(&node), sizeof(BTreeNode));
    }

    /*   for (int i = 0; i < recordNum; ++i) {

           cout << endl;
           for (int j = 0; j < nodeSize; j++) {
                    cout <<  node.children[j] << " ";

             }
       }*/

    file.close();
}



int main() {
    const char* filename = "btree.txt";
    int n = 10; // number of records
    int m = 5;
    createFile(filename, n, m);
    return 0;
}