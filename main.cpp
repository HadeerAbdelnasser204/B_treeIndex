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
//    vector<short>children; // record IDs or references to child nodes
    vector<pair<short, short>> records;
    // 0 for leaf, 1 for non-leaf
    short children[nodeSize]; // record IDs or references to child nodes


public:


    int getM() const {
        return m;
    }

    void setM(int m) {
        BTreeNode::m = m;
    }


   int InsertNewRecordAtIndex (const char* filename, short RecordID, short Reference){
    fstream file(filename, std::ios::binary | std::ios::out | std::ios::in);
    short nodeSize = (2 * (2 * m + 1)) + 2;

    file.seekg(nodeSize, std::ios::beg);
    file.read(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));

   short first=file.tellg();

    if (isLeaf == -1) {
        short leaf = 0;
        file.seekg(nodeSize, ios::beg);
        file.write(reinterpret_cast<char*>(&leaf), sizeof(leaf));
        file.write(reinterpret_cast<char*>(&RecordID), sizeof(RecordID));
        file.write(reinterpret_cast<char*>(&Reference), sizeof(Reference));
        file.flush();
        file.close();
        return 0;
    }

     map<short, short> node;
    int temp = nodeSize - 1;
    int position = file.tellg();

    while (temp) {
        short value;
        short offset;
        file.read(reinterpret_cast<char*>(&value), sizeof(value));
        file.read(reinterpret_cast<char*>(&offset), sizeof(offset));

        if (value == -1) {
            break;
        }

        node[value] = offset;
        temp = temp - 2;
    }

    if (isLeaf == 0) {
        file.seekg(position, ios::beg);
        if (node.size() < 5) {
            node[RecordID] = Reference;
            file.seekp(position, ios::beg);
            for (const auto& entry : node) {
                file.write(reinterpret_cast<const char*>(&entry.first), sizeof(entry.first));
                file.write(reinterpret_cast<const char*>(&entry.second), sizeof(entry.second));
            }
            file.close();
            return 0;
        } else {
           short sizeNode=floor((node.size())/4);
           vector<int>node1;vector<int>node2;
            //split



        }
    }

    return 0;
}

};

void createFile(const char* filename, int n) {
    ofstream file(filename, ios::binary);

    for (int i = 0; i < n; ++i) {
        BTreeNode node;
        node.isLeaf = 0;
        for (int j = 0; j < nodeSize; ++j) {
            node.children[j] = -1;
        }
        file.write(reinterpret_cast<char*>(&node.children), sizeof(node.children));
    }

    file.close();
}

void displayFile(const char* filename) {
     ifstream inputFile(filename, std::ios::binary);

    if (inputFile.is_open()) {
        // Read the content as binary data
         short value;
        while (inputFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
            if (value != 2573) cout << value << ' ';
            else cout <<'\n';
            }}
        // Close the file
        inputFile.close();


}

void creatFile(const char* filename, int n,int record){
    fstream file(filename,ios::out);
    if(file.is_open()){
        cout<<"file open successfully\n";
        n=(n*2)+1;
        short num=-1,orderR=1;
        for(short i=0;i<record;i++){
            for(int j=0;j<n;j++){
                file.write((char*)&num,sizeof(num));
            }
            file<<'\n';
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
