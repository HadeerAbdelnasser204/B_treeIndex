#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

const int leafNodeMarker = 0;
const int nonLeafNodeMarker = 1;

struct Record {
    int data;

    Record() {
        data = 0;
    }
};

struct BTreeNode {
     int nodeMarker; // 0 for leaf, 1 for non-leaf
     vector<int> childIds;
     vector<Record> records;

    BTreeNode(){
        nodeMarker = leafNodeMarker;
    }
};

void insertNode (fstream& file, const BTreeNode& node) {
    file.seekp(0, ios::end);
    file.write(reinterpret_cast<const char*>(&node), sizeof(BTreeNode));
}

void CreateIndexFile(const char* filename, int numberOfRecords, int m) {
    fstream file(filename, std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);

    // Create an empty root node
    BTreeNode rootNode;
  insertNode(file, rootNode);

    // Create and write additional nodes as needed
    for (int i = 1; i < numberOfRecords; ++i) {
        BTreeNode node;
        insertNode(file, node);
    }

    file.close();
}
int main() {
    const char* filename = "btree_index.txt";
    int numberOfRecords = 10;
    int m = 5;

    CreateIndexFile(filename, numberOfRecords, m);

}
