#include <iostream>
#include <fstream>
/*
 * Student1 : Waleed Kamal Elhamey
 * ID1 : 20180331
 * Student2 : Ziad Amr
 * ID2 : 20180379
 * Student3 : Ibrahim Hany Abdelghany
 * ID3 : 20180004
 */
using namespace std;
struct node {
    int key;
    int address;
};

struct Page {
    int leaf;
    node *nodes = new node[5];

    Page() {//Default constructor to set the values of the record with -1
        leaf = -1;
        for (int i = 0; i < 5; ++i) {
            nodes[i].key = nodes[i].address = -1;
        }
    }

    Page(int currentRow) {//Parameterize constructor to set the values of the record
        // with -1 and set the value of the second column with the index of the next
        // available index to be fill
        // currentRow is and integer to know what row am i filling right now
        leaf = -1;
        nodes[0].key = currentRow;
        nodes[0].address = -1;
        for (int i = 1; i < 5; ++i) {
            nodes[i].key = nodes[i].address = -1;
        }
    }
};

void CreateIndex(string fileName, int numRec) {//Create index file
    fstream file;
    file.open(fileName, ios::out | ios::binary);
    for (int i = 0; i < numRec; ++i) {
        if (i + 1 == numRec) {//If condition to check if we are in the last record or not
            // if true we set the first column with -1
            Page page(-1);
            //cout<<tree.pages[0].pageNumber<<endl;
            file.write((char *) &page, sizeof page);
        } else {
            Page page(i + 1);
            //cout<<tree.pages[0].pageNumber<<endl;
            file.write((char *) &page, sizeof page);
        }

    }
    file.close();
}

int hasAnEmptyNode(Page page) {
    for (int i = 0; i < 5; ++i) {
        if (page.nodes[i].key == -1) {
            return i;
            break;
        }
    }
    return -2;
}

void printRow(Page pages) {
    cout << pages.leaf << " | ";
    for (int i = 0; i < 5; ++i) {
        cout << pages.nodes[i].key << " | " << pages.nodes[i].address << " | ";
    }
    cout << endl;
}

void printFile(string fileName) {
    fstream file;
    file.open(fileName, ios::in | ios::binary);
    for (int i = 0; i < 10; ++i) {//to display the whole file by calling displayRow function
        Page page;
        file.read((char *) &page, sizeof page);
        printRow(page);
        cout << endl;

    }
    file.close();
}
void sortPage(Page page) {
    for (int i = 0; i < 4; ++i) {
        if (page.nodes[i].key > page.nodes[i+1].key) {
            int tempKey, tempAdd;
            tempKey = page.nodes[i].key;
            tempAdd = page.nodes[i].address;
            page.nodes[i ].key = page.nodes[i+1].key;
            page.nodes[i ].address = page.nodes[i+1].address;
            page.nodes[i+1].key = tempKey;
            page.nodes[i+1].address = tempAdd;
        }
    }
}
void copyPages(Page sourcePage,Page page1, Page page2){//Used to copy 1 page into 2 pages as we use it in the split function
    int max1,add1,max2,add2;
    for(int i=0;i<3;++i){//Copy the first 3 items in the first page
        page1.nodes[i].key=sourcePage.nodes[i].key;
        page1.nodes[i].address=sourcePage.nodes[i].address;
        max1=sourcePage.nodes[i].key;
        add1=sourcePage.nodes[i].address;
        sourcePage.nodes[i].key=-1;
        sourcePage.nodes[i].address=-1;

    }
    sourcePage.nodes[2].key=max1;
    sourcePage.nodes[2].address=add1;

    for(int i=3;i<5;++i){//Copy the last 2 items in the second page
        page2.nodes[i].key=sourcePage.nodes[i].key;
        page2.nodes[i].address=sourcePage.nodes[i].address;
        max2=sourcePage.nodes[i].key;
        add2=sourcePage.nodes[i].address;
        sourcePage.nodes[i].key=-1;
        sourcePage.nodes[i].address=-1;
    }
    sourcePage.nodes[4].key=max2;
    sourcePage.nodes[4].address=add2;
    sortPage(sourcePage);
    sortPage(page1);
    sortPage(page2);

}
void split(string fileName, int &currentPageIndex) {
    fstream file;
    file.open(fileName, ios::in | ios::out | ios::binary);
    Page currentPage;
    file.seekg(currentPageIndex * sizeof currentPage, ios::beg);
    file.read((char *) &currentPage, sizeof currentPage);
    Page nextPage, afterNextPage;
    copyPages(currentPage, nextPage, afterNextPage);
    currentPage.leaf = 1;
    nextPage.leaf = 0;
    afterNextPage.leaf = 0;
    file.seekp(currentPageIndex * sizeof(currentPage), ios::beg);
    file.write((char *) &currentPage, sizeof currentPage);
    currentPageIndex+=1;
    file.seekp(currentPageIndex * sizeof(nextPage), ios::beg);
    file.write((char *) &nextPage, sizeof nextPage);
    currentPageIndex+=1;
    file.seekp(currentPageIndex * sizeof(afterNextPage), ios::beg);
    file.write((char *) &afterNextPage, sizeof afterNextPage);
    return;
}
void updateNextEmptyRecord(string fileName,int newIndex){
    fstream file;
    file.open(fileName, ios::in | ios::out | ios::binary);
    Page currentPage;
    file.seekg(0, ios::beg);
    file.read((char *) &currentPage, sizeof currentPage);//Get first record
    currentPage.nodes[0].key=newIndex;//Get second column of the first record
    file.seekp(0, ios::beg);
    file.write((char *) &currentPage, sizeof currentPage);
    file.close();
}
int getMaxKeyInPage(string fileName,Page page){
    int max=page.nodes[0].key;
    for(int i=1;i<5;++i){
        if(max<page.nodes[i].key)
            max=page.nodes[i].key;
    }
    return max;
}
int getIndexOfKey(string fileName, int Key){//To get the index of the parent
    //Which means which page does the parent exist
    fstream file;
    file.open(fileName, ios::in | ios::out | ios::binary);
    Page currentPage;
    int i=2;
    file.seekg(i * sizeof currentPage ,ios::beg);
    file.read((char *) &currentPage, sizeof currentPage);
    while(i<10){
        if(getMaxKeyInPage(fileName,currentPage)==Key){
            return i;
        }else{
            i++;
            file.seekg(i * sizeof currentPage ,ios::beg);
            file.read((char *) &currentPage, sizeof currentPage);
        }
    }
    return i;
}
void splitFromLeaf(string fileName, Page currentPage){
    fstream file;
    Page temp;
    file.open(fileName, ios::in | ios::out | ios::binary);
    file.read((char *) &temp, sizeof temp);//Get first record
    int nextEmptyRecord = temp.nodes[0].key;//Get second column of the first record
    Page newPage;//Points to the new page(i.e. no.4)
    for(int i=2;i<5;++i){
        newPage.nodes[i].key=currentPage.nodes[i].key;
        newPage.nodes[i].address=currentPage.nodes[i].address;
        currentPage.nodes[i].key=-1;
        currentPage.nodes[i].address=-1;
    }
    updateNextEmptyRecord(fileName,nextEmptyRecord+1);
    newPage.leaf=0;
    file.seekp(nextEmptyRecord * sizeof(newPage), ios::beg);
    file.write((char *) &newPage, sizeof newPage);
    file.close();

}
void insertUsingTrace(string fileName, int Key, int Address){
    fstream file;
    file.open(fileName, ios::in | ios::out | ios::binary);
    Page currentPage;
    file.seekg(1 * sizeof currentPage ,ios::beg);
    file.read((char *) &currentPage, sizeof currentPage);
    int idx=-2;
    if(Key>currentPage.nodes[4].key){
        node nodo;//Relates to 24 in TA's example (pg.4)
        nodo.key=currentPage.nodes[4].key;
        nodo.address=currentPage.nodes[4].address;
        currentPage.nodes[4].key=Key;//Relates to adding 30 in TA's example
        currentPage.nodes[4].address=Address;
        idx =getIndexOfKey(fileName,nodo.key);//Get the index of 24
        file.seekg(idx * sizeof currentPage ,ios::beg);
        file.read((char *) &currentPage, sizeof currentPage);
        int index = hasAnEmptyNode(currentPage);
        currentPage.nodes[index].key = Key;
        currentPage.nodes[index].address = Address;
        sortPage(currentPage);
        file.seekp(idx * sizeof(currentPage), ios::beg);
        file.write((char *) &currentPage, sizeof currentPage);
    }
    else{
        for(int i=0;i<5;++i){
            if(Key<currentPage.nodes[i].key && currentPage.nodes[i].key !=-1){
                idx =getIndexOfKey(fileName,currentPage.nodes[i].key);
                file.seekg(idx * sizeof currentPage ,ios::beg);
                file.read((char *) &currentPage, sizeof currentPage);
                int index = hasAnEmptyNode(currentPage);
                if(index==-2){//Full
                    splitFromLeaf(fileName,currentPage);
                    index = hasAnEmptyNode(currentPage);
                    currentPage.nodes[index].key = Key;
                    currentPage.nodes[index].address = Address;
                    sortPage(currentPage);
                    file.seekp(idx * sizeof(currentPage), ios::beg);
                    file.write((char *) &currentPage, sizeof currentPage);
                    file.close();
                }
                else{
                    currentPage.nodes[index].key = Key;
                    currentPage.nodes[index].address = Address;
                    sortPage(currentPage);
                    file.seekp(idx * sizeof(currentPage), ios::beg);
                    file.write((char *) &currentPage, sizeof currentPage);
                    file.close();

                }
                break;
            }
        }
    }

}
void InsertNewRecordAtIndex(string fileName, int Key, int Address) {
    fstream file;
    file.open(fileName, ios::in | ios::out | ios::binary);
    Page currentPage;
    file.read((char *) &currentPage, sizeof currentPage);//Get first record
    int nextEmptyRecord = currentPage.nodes[0].key;//Get second column of the first record
    file.seekg(1 * sizeof currentPage ,ios::beg);
    file.read((char *) &currentPage, sizeof currentPage);
    if(currentPage.leaf == 1){//Has childs
        insertUsingTrace(fileName,Key,Address);
    }
    else if (currentPage.leaf == -1) {//This means that this is our first time to insert in this row
        currentPage.leaf = 0;
        currentPage.nodes[0].key = Key;
        currentPage.nodes[0].address = Address;
        sortPage(currentPage);
        file.seekp(nextEmptyRecord * sizeof(currentPage), ios::beg);
        file.write((char *) &currentPage, sizeof currentPage);
    }
    else if(currentPage.leaf == 0){//Has no childs
        int index = hasAnEmptyNode(currentPage);
        if (index != -2) {
            currentPage.nodes[index].key = Key;
            currentPage.nodes[index].address = Address;
            sortPage(currentPage);
            file.seekp(nextEmptyRecord * sizeof(currentPage), ios::beg);
            file.write((char *) &currentPage, sizeof currentPage);
        } else {//The record is full
            split(fileName, nextEmptyRecord);
            insertUsingTrace(fileName,Key,Address);
            cout<<"nextEmptyRecord: "<<nextEmptyRecord<<endl;
            updateNextEmptyRecord(fileName, nextEmptyRecord+1);
        }
    }
}
int searchRecord(string fileName, int key) {
    fstream file(fileName, ios::in | ios::binary);
    Page readPage;
    while (!file.eof()) {
        file.read((char *) &readPage, sizeof readPage);
        for (int i = 0; i < 5; i++) {
            if (readPage.nodes[i].key == key) {
                return readPage.nodes[i].address;
            }
        }
    }
    return -1;
}


int main() {
    string fileName = "btree.bin";
    CreateIndex(fileName, 10);

    while (true) {
        cout << "Choose your operation" << endl;
        cout << "1-Display" << endl;
        cout << "2-Insert" << endl;
        cout << "3-Search for a key" << endl;
        cout << "4-Exit" << endl;
        int choice;
        cin >> choice;
        if (choice == 1) {
            printFile(fileName);
        } else if (choice == 2) {
            int key, address;
            cout << "Enter key\n";
            cin >> key;
            cout << "Enter address\n";
            cin >> address;
            InsertNewRecordAtIndex(fileName, key, address);
        } else if (choice == 3) {
            int key;
            cout << "Enter key you want to search for \n";
            cin >> key;
            cout << searchRecord(fileName, key) << endl;
        } else if (choice == 4) {
            break;
        } else {
            cout << "Invalid Option\n";
        }

        cout << "-----------------------------------------------------------------------------\n";
    }
}