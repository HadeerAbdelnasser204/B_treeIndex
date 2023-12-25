#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;


// m >> number of descendants
//2 * m + 1 >> size of each node (record)
class BTreeNode {
private:

    int m;
    short isLeaf;  // 0 for leaf, 1 for non-leaf
    vector<short>children; // record IDs or references to child nodes

public:


    int getM() const {
        return m;
    }

    void setM(int m) {
        BTreeNode::m = m;
    }


    void createFile(const char* filename, int recordNum) {
        //BTreeNode node;
        int nodeSize = 2 * m + 1;
        fstream file(filename, ios::binary| ios::out| ios::app|ios::in);
        for (int i = 0; i < recordNum; ++i) {

            for (int j = 0; j < nodeSize; ++j) {

                children.push_back(-1);
                short num = -1;
                file.write(reinterpret_cast<char*>(&num), sizeof(num));

            }

            //file.read(reinterpret_cast<char*>(&node), sizeof(BTreeNode));
        }

//           for (int i = 0; i < recordNum; ++i) {
//
//               cout << endl;
//               for (int j = 0; j < nodeSize; j++) {
//                        cout <<  node.children[j] << " ";
//
//                 }
//           }

        file.close();
    }

    int InsertNewRecordAtIndex (const char* filename, short RecordID, short Reference){

        int nodeSize = 2 * m + 1;

        fstream file(filename, ios::binary| ios::out|ios::in);

        file.seekg(nodeSize,ios::beg);

        file.read(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));

        if(isLeaf == -1){
            isLeaf = 0;
            short leaf = 0;
            file.seekg(nodeSize, ios::beg);
            file.write(reinterpret_cast<char*>(&leaf), sizeof(leaf));
            file.write(reinterpret_cast<char*>(&RecordID), sizeof(RecordID));
            file.write(reinterpret_cast<char*>(&Reference), sizeof(Reference));
            file.flush();

            file.seekg(nodeSize,ios::beg);
            file.read(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));
            file.read(reinterpret_cast<char*>(&RecordID), sizeof(RecordID));
            file.read(reinterpret_cast<char*>(&Reference), sizeof(Reference));
            file.close();


            return 0;

        }

        vector<pair<short,short>> node;

        int temp = nodeSize-1;
        int position = file.tellg();

        while(temp){
            short value;
            short offset;
            file.read(reinterpret_cast<char*>(&value), sizeof(value));
            file.read(reinterpret_cast<char*>(&offset), sizeof(offset));
            if(value == -1){
                break;
            }
            node.push_back(make_pair(value,offset));
            temp = temp - 2;


        }


        if(isLeaf == 0){

            file.seekg(1,ios::cur);
            file.seekp(position,ios::beg);


            if(node.size() < 5){
                node.push_back(make_pair(RecordID,Reference));
                sort(node.begin(),node.end());

                for (int i = 0; i < node.size() ; ++i) {
                    cout << node[i].first << " "<< node[i].second << "\n";
                    file.write(reinterpret_cast<char*>(&node[i].first), sizeof(&node[i].first));
                    file.write(reinterpret_cast<char*>(&node[i].second), sizeof(&node[i].second));

                }

                file.seekg(position,ios::beg);

                while(temp){
                    short value;
                    short offset;
                    file.read(reinterpret_cast<char*>(&value), sizeof(value));
                    file.read(reinterpret_cast<char*>(&offset), sizeof(offset));
                    if(value == -1){
                        break;
                    }
                    temp = temp - 2;
                }

                file.close();

                return 0;

            }
            else{
                //split
            }

        }

    }

};




int main() {
    const char* filename = "btree.txt";
    BTreeNode b;
    int n = 10; // number of records
    b.setM(5);
    b.createFile(filename, n);
    b.InsertNewRecordAtIndex(filename, 3, 12);
    b.InsertNewRecordAtIndex(filename, 4, 15);
   // b.InsertNewRecordAtIndex(filename, 2, 18);
    return 0;

}