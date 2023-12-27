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
    vector<pair<short, short>> records;

public:


    int getM() const {
        return m;
    }

    void setM(int m) {
        BTreeNode::m = m;
    }


    void CreateIndexFile(const char* filename, int recordNum) {
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




    int SearchARecord(const char* filename, short RecordID) {
        int nodeSize = 2 * m + 1;

        fstream file(filename, ios::binary | ios::in);

        file.seekg(nodeSize, ios::beg);
        file.read(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));

        if (isLeaf == -1) {
            return -1; // Record not found in this branch
        }

        records.clear();
        for (int i = 0; i < m; ++i) {
            short value, offset;
            file.read(reinterpret_cast<char*>(&value), sizeof(value));
            file.read(reinterpret_cast<char*>(&offset), sizeof(offset));
            if (value == -1) {
                break;
            }
            records.emplace_back(value, offset);
        }

        file.close();

        // Search for the RecordID in the records vector
        auto it = find_if(records.begin(), records.end(),
                          [RecordID](const pair<short, short>& p) {
                              return p.first == RecordID;
                          });

        if (it != records.end()) {
            return it->second;
        } else {
            return -1;
        }
    }

};

int main() {
    const char* filename = "btree.txt";
    BTreeNode b;
    int n = 10; // number of records
    b.setM(5);
    int choice;
    cout << "Welcome to in our program, B-Tree.\n";
    do {
        cout << "1. Create Index File\n";
        cout << "2. Insert New Record\n";
        cout << "3. Delete Record\n";
        cout << "4. Display Index File Content\n";
        cout << "5. Search for a Record\n";
        cout << "0. Exit\n";
        cout << "Enter your choice:";
        cin >> choice;

        switch (choice) {
            case 1: {
                b.CreateIndexFile(filename, n);
                break;
            }
            case 2: {
                short RecordID, Reference;
                cout << "Enter RecordID and Reference:";
                cin >> RecordID >> Reference;
                b.InsertNewRecordAtIndex(filename, RecordID, Reference);
                break;
            }
            case 3: {

            }

            case 4: {

            }
            case 5: {
                short searchRecordID;
                cout << "Enter RecordID to search:";
                cin >> searchRecordID;
                int result = b.SearchARecord(filename, searchRecordID);
                if (result != -1) {
                    cout << "The record is found, the reference is:  " << result << endl;
                } else {
                    cout << "Record not found." << endl;
                }
                break;
            }

            case 0:
                cout << "Exiting program.\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);

    return 0;

}
