#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>

using namespace std;


// Assuming RecordID and Reference are of type int
typedef map<short,short> RecordMap;

// Function to find the maximum instance in a map
pair<int, int> findMaxInstance(const RecordMap& recordMap) {
    pair<int, int> maxInstance = {INT_MIN, INT_MIN};
    for (const auto& p : recordMap) {
        if (p.first > maxInstance.first) {
            maxInstance = p;
        }
    }
    return maxInstance;
}


// m >> number of descendants
//2 * m + 1 >> size of each node (record)
class BTreeNode {
private:

    int m;
    short isLeaf;  // 0 for leaf, 1 for non-leaf
    short nodeSize = (2 * (2 * m + 1)) + 2;
//    vector<short>children; // record IDs or references to child nodes
    vector<pair<short, short>> records;
    // 0 for leaf, 1 for non-leaf
    //short children[nodeSize]; // record IDs or references to child nodes


public:


    int getM() const {
        return m;
    }

    void setM(int m) {
        BTreeNode::m = m;
    }

    RecordMap readRecord(short offset ,fstream &file){
        map<short, short> node;
        int temp = nodeSize - 1;

        while (temp) {
            short value;
            short offset;
            file.read(reinterpret_cast<char*>(&value), sizeof(value));
            file.read(reinterpret_cast<char*>(&offset), sizeof(offset));


            if (value == -1||value == 2573) {
                break;
            }

            node[value] = offset;
            temp = temp - 2;
        }
        return node;

    }
    void writeRecord(RecordMap R,fstream &file){
        for (const auto& entry : R) {
            file.write(reinterpret_cast<const char*>(&entry.first), sizeof(entry.first));
            file.write(reinterpret_cast<const char*>(&entry.second), sizeof(entry.second));
        }
        file.flush();


    }
    void split(RecordMap node ,short RecordID , short Reference , fstream &file, short position){

        node.insert({RecordID, Reference});

        // Sort the map by RecordID
        // Note: Maps are ordered by default based on the key
        size_t originalSize = node.size();
        size_t halfSize = originalSize / 2;

        // Create two child maps
        RecordMap child1(node.begin(), next(node.begin(), halfSize));
        RecordMap child2(next(node.begin(), halfSize), node.end());

        // Find the maximum instance in each half
        pair<int, int> maxInstanceChild1 = findMaxInstance(child1);
        pair<int, int> maxInstanceChild2 = findMaxInstance(child2);

        // Put the maximum instances back into the original map
        node.clear();
        node.insert(maxInstanceChild1);
        node.insert(maxInstanceChild2);


        // Assuming position is the position in the file where you want to write
        file.seekg(position - 2, ios::beg);
        file.read(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));

        short p1;
        for (short i = 1; i < 10; ++i) {
            // Your existing code for reading isLeaf goes here
            if (isLeaf == -1) {
                isLeaf = 0;

                file.seekg(-2,ios::cur);
                short r = file.tellg();
                p1 = i;

                file.write(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));

                writeRecord(child1,file);
                //p2 = i+1;
                file.seekg(r + nodeSize,ios::beg);

                file.write(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));

                writeRecord(child2,file);

                file.flush();

                break;
            }

            else{

                file.seekg(nodeSize - 2,ios::cur);
                file.read(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));
            }

        }

        file.seekg(position - 2, ios::beg);

        isLeaf = 1 ;

        file.write(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));

        for (const auto& entry : node) {

            if(entry.first != -1) {
                node[entry.first] = p1++;
            }

            file.write(reinterpret_cast<const char*>(&entry.first), sizeof(entry.first));
            file.write(reinterpret_cast<const char*>(&entry.second), sizeof(entry.second));
        }

        short value;
        file.read(reinterpret_cast<char*>(&value), sizeof(value));
        while(value != 2573){

            short x = -1 ;

            file.seekg(-2,ios::cur);
            file.write(reinterpret_cast<char*>(&x), sizeof(x));

            file.read(reinterpret_cast<char*>(&value), sizeof(value));
        }
    }




    int InsertNewRecordAtIndex (const char* filename, short RecordID, short Reference){
        fstream file(filename, std::ios::binary | std::ios::out | std::ios::in);
        short nodeSize = (2 * (2 * m + 1)) + 2;

        file.seekg(nodeSize, std::ios::beg);
        file.read(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));

        short position =file.tellg();

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

        map<short, short> node = readRecord(position,file);


        if (isLeaf == 0) {
            file.seekg(position, ios::beg);
            if (node.size() < m) {
                node[RecordID] = Reference;
                file.seekp(position, ios::beg);

                writeRecord(node,file);
                file.close();
                return 0;
            }
            else {

                // Insert elements into the map
                node.insert({RecordID, Reference});

                // Sort the map by RecordID
                // Note: Maps are ordered by default based on the key
                size_t originalSize = node.size();
                size_t halfSize = originalSize / 2;

                // Create two child maps
                RecordMap child1(node.begin(), next(node.begin(), halfSize));
                RecordMap child2(next(node.begin(), halfSize), node.end());

                // Find the maximum instance in each half
                pair<int, int> maxInstanceChild1 = findMaxInstance(child1);
                pair<int, int> maxInstanceChild2 = findMaxInstance(child2);

                // Put the maximum instances back into the original map
                node.clear();
                node.insert(maxInstanceChild1);
                node.insert(maxInstanceChild2);


                // Assuming position is the position in the file where you want to write
                file.seekg(position - 2, ios::beg);
                file.read(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));

                short p1, p2;
                for (short i = 1; i < 10; ++i) {
                    // Your existing code for reading isLeaf goes here
                    if (isLeaf == -1) {
                        isLeaf = 0;

                        file.seekg(-2,ios::cur);
                        short r = file.tellg();
                        p1 = i;

                        file.write(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));

                        writeRecord(child1,file);
                        //p2 = i+1;
                        file.seekg(r + nodeSize,ios::beg);

                        file.write(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));

                        writeRecord(child2,file);

                        file.flush();

                        break;
                    }

                    else{

                        file.seekg(nodeSize - 2,ios::cur);
                        file.read(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));
                    }

                }

                file.seekg(position - 2, ios::beg);

                isLeaf = 1 ;

                file.write(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));



                for (const auto& entry : node) {

                    if(entry.first != -1) {
                        node[entry.first] = p1++;
                    }

                    file.write(reinterpret_cast<const char*>(&entry.first), sizeof(entry.first));
                    file.write(reinterpret_cast<const char*>(&entry.second), sizeof(entry.second));
                }

                short value;
                file.read(reinterpret_cast<char*>(&value), sizeof(value));
                while(value != 2573){

                    short x = -1 ;

                    file.seekg(-2,ios::cur);
                    file.write(reinterpret_cast<char*>(&x), sizeof(x));

                    file.read(reinterpret_cast<char*>(&value), sizeof(value));
                }


            }
            if(isLeaf == 1){
                vector<pair<short,short>> storage;

                storage.push_back(make_pair(node.size(),position-2));

                short start = -2 ;
                while(isLeaf !=0){

                    short place = -2;
                    for (const auto &entry: node) {

                        if (entry.first > RecordID) {
                            place = entry.second;
                        }
                    }
                    if (place == -2) {
                        place = node.end()->second;
                    }
                    file.seekg(place * nodeSize, ios::beg);

                     start = file.tellg();

                    file.read(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));

                     node = readRecord(place, file);

                     storage.push_back(make_pair(node.size(),start));

                }

                storage.pop_back();

                if (node.size() < m) {
                    node[RecordID] = Reference;
                    file.seekp(start, ios::beg);
                    writeRecord(node, file);
                    file.close();
                    return 0;

                }
                else {

                    split(node, RecordID, Reference, file, start);

                    while (!storage.empty()) {

                        if (storage.end()->first < m) {
                            split(node, RecordID, Reference, file, start);
                        }
                        else {

                            file.seekg(storage.end()->first,ios::beg);
                            short p = file.tellg();
                            node = readRecord(p,file);
                            storage.pop_back();


                        }
                    }




                }







            }
        }

        return 0;



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

};
//
//void createFile(const char* filename, int n) {
//    ofstream file(filename, ios::binary);
//
//    for (int i = 0; i < n; ++i) {
//        BTreeNode node;
//        node.isLeaf = 0;
//        for (int j = 0; j < nodeSize; ++j) {
//            node.children[j] = -1;
//        }
//        file.write(reinterpret_cast<char*>(&node.children), sizeof(node.children));
//    }
//
//    file.close();
//}


//    int SearchARecord(const char* filename, short RecordID) {
//        int nodeSize = 2 * m + 1;
//
//        fstream file(filename, ios::binary | ios::in);
//
//        file.seekg(nodeSize, ios::beg);
//        file.read(reinterpret_cast<char*>(&isLeaf), sizeof(isLeaf));
//
//        if (isLeaf == -1) {
//            return -1; // Record not found in this branch
//        }
//
//        records.clear();
//        for (int i = 0; i < m; ++i) {
//            short value, offset;
//            file.read(reinterpret_cast<char*>(&value), sizeof(value));
//            file.read(reinterpret_cast<char*>(&offset), sizeof(offset));
//            if (value == -1) {
//                break;
//            }
//            records.emplace_back(value, offset);
//        }
//
//        file.close();
//
//        // Search for the RecordID in the records vector
//        auto it = find_if(records.begin(), records.end(),
//                          [RecordID](const pair<short, short>& p) {
//                              return p.first == RecordID;
//                          });
//
//        if (it != records.end()) {
//            return it->second;
//        } else {
//            return -1;
//        }
//    }



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
                b.creatFile(filename,5, 10);
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

                b.displayFile(filename);

            }
            case 5: {
                short searchRecordID;
                cout << "Enter RecordID to search:";
                cin >> searchRecordID;
//                int result = b.SearchARecord(filename, searchRecordID);
//                if (result != -1) {
//                    cout << "The record is found, the reference is:  " << result << endl;
//                } else {
//                    cout << "Record not found." << endl;
//                }
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
