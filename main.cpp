//This is a program that reads a borg file that is in txt format and reads it to execute the program. It can do simple math caculations as well as variable declaration and scope detection.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <cmath>

using namespace std;

#define HASHTABLESIZE 15


//for chaining collision resolution
class SingleLinkedListNode {
private:
    int key, value;
public:
    class SingleLinkedListNode *next;
    
    SingleLinkedListNode(int k, int v, SingleLinkedListNode *ptr = NULL) {
        next = ptr;
        key = k;
        value = v;
    }
    
    //Getters
    int getKey() { return key; }
    int getValue() { return value; }
    
    //Setter
    void changeValue(int num) { value = num; }
};

//HASHTABLE
class Hashtable {
public:
    //Variables
    SingleLinkedListNode *arr[HASHTABLESIZE];   //List to store elements in the hash table
    SingleLinkedListNode* pos;                //Check current position
    
    //initializer
    Hashtable() {
        for (int i = 0; i<HASHTABLESIZE; i++)
            arr[i] = NULL;    //Set all values to null
    }
    
    //functions
    int computeKeyValue(string name);
    int Hash(string name);
    void add(string variableName, int value);
    int search(string name);
    int operate(string name, char op, int num = 0);
    void operate(string name, string op, int num = 0);
    
private:
    int value;
    
};

int Hashtable::computeKeyValue(string name) {
    int key = 0;
    for (int i = 0; i< name.length(); i++) {
        key = static_cast<int>(name.at(i))*(i + 1);
    }
    return key;
}

//Hash function
int Hashtable::Hash(string name) {
    int hash = computeKeyValue(name);
    hash = hash % HASHTABLESIZE;
    return hash;
}

//Add an element to the hash table
void Hashtable::add(string variableName, int value) {
    int key = computeKeyValue(variableName);
    int hash = Hash(variableName);
    bool foundPosition = false;
    
    //if it is the first element
    if (arr[hash] == NULL) {
        arr[hash] = new SingleLinkedListNode(key, value);
    }
    
    //add to sorted list
    else {
        SingleLinkedListNode* temp = new SingleLinkedListNode(key, value);
        //identify the root
        pos = arr[hash];
        
        //if new key is smaller than first element
        //make new key root
        while (!foundPosition) {
            if (pos->getKey() == key) {
                foundPosition = true;
                break;
            }
            else if (pos->getKey() > key) {
                temp->next = pos;
                arr[hash] = temp;
                break;
            }
            
            //if new key is bigger than first element
            //find its place and insert into list
            else {
                while (pos->next != NULL) {
                    if (pos->getKey() == key) {
                        foundPosition = true;
                        break;
                    }
                    else if (pos->next->getKey() < key) {
                        pos = pos->next;
                        break;
                    }
                }
                
                //insert key
                temp->next = pos->next;
                pos->next = temp;
            }
        }
    }
    if (foundPosition) {
        pos->changeValue(value);
    }
}

//lookup return the hash value
int Hashtable::search(string name) {
    int key = computeKeyValue(name);
    int hash = Hash(name);
    
    for (pos = arr[hash]; pos != NULL; pos = pos->next) {
        if (pos->getKey() == key)
            return pos->getValue();
    }
    return -1;
}

//Take care of the operations that are done with single characters
int Hashtable::operate(string name, char op, int num) {
    int value = search(name);
    
    if (value != -1) {
        if (op == '+')
            value = value + num;
        else if (op == '-')
            value = value - num;
        else if (op == '*')
            value = value * num;
        else if (op == '/')
            value = value / num;
        else if (op == '%')
            value = value % num;
        else if (op == '^')
            value = pow(value, num);
        return value;
    }
    return -1;
}

//Take care of the operations consisted with more than a character such as ++ or --
void Hashtable::operate(string name, string op, int num) {
    //Variables
    int key = computeKeyValue(name);
    int hash = Hash(name);
    int value = search(name);
    
    if (value != -1) {
        //update value
        if (op == "++")
            value++;
        else if (op == "--")
            value--;
        for (pos = arr[hash]; pos != NULL; pos = pos->next) {
            if (pos->getKey() == key)
                pos->changeValue(value);
        }
    }
    
    else
        cout << "VARIABLE NOT FOUND" << endl;
}

class BorgProcessing {
public:
    //Variables
    ifstream infile;
    bool scope;
    int count = -1;
    vector <Hashtable> hash_array;
    
    //Functions
    BorgProcessing(string address);
    ~BorgProcessing();
    void readFile();
    void processVar(vector <string>token);
    
private:
    enum BORG {
        START,
        FINISH,
        COM,
        VAR,
        PRINT,
        IF,
        ENDMARKER
    };
};

//Constructor with address
BorgProcessing::BorgProcessing(string address) {
    //read file address
    infile.open(address.c_str());
    scope = true;
    readFile();
}

//Dispose
BorgProcessing::~BorgProcessing() {
    infile.close();
}

//Read file with the address
void BorgProcessing::readFile() {
    string line, temp;
    vector <string> token;
    
    //read the line
    while (!infile.eof()) {
        token.clear();
        infile >> ws;
        getline(infile, line);
        stringstream stream(line);
        
        //split line into tokens and process
        while (getline(stream, temp, ' '))
        {
            temp.erase(temp.find_last_not_of('\t') + 1);
            token.push_back(temp);
        }
        
        processVar(token);
    }
}

//Process varaibles
void BorgProcessing::processVar(vector <string>elements) {
    map<string, BORG> keywords;
    
    keywords["START"] = BORG::START;
    keywords["FINISH"] = BORG::FINISH;
    keywords["COM"] = BORG::COM;
    keywords["VAR"] = BORG::VAR;
    keywords["PRINT"] = BORG::PRINT;
    
    //Print The elements
    for (int i = 0; i < elements.size(); i++) {
        cout << elements[i] << " ";
    }
    cout << "\n";
    
    //get pointer to the keyword
    map<string, BORG>::iterator it = keywords.find(elements[0]);
    
    if (it != keywords.end()) {
        switch (it->second) {
            case START: {
                Hashtable h;
                hash_array.push_back(h);
                count++;
                readFile();
                break;
            }
            case COM:
                break;
            case VAR:
                try {
                    hash_array[count].add(elements[1], stoi(elements[3]));
                }
                catch (...) {
                    cout << "Invalid snytax" << endl;
                }
                break;
            case PRINT:
                try {
                    //if there is operator following variable name
                    if (elements.size()>2) {
                        int new_value = hash_array[count].operate(elements[1], *elements[2].c_str(), stoi(elements[3]));
                        if (new_value != -1)
                            if (elements.size() == 4) {
                                cout << elements[1] << " " << elements[2] << " " << elements[3] << " IS " << new_value << endl;
                            }
                            else if (elements.size() == 3) {
                                cout << elements[1] << " " << elements[2] << " IS " << new_value << endl;
                            }
                    }
                    
                    
                    else {
                        int tempInt;
                        int i = count;
                        
                        do {
                            tempInt = hash_array[i].search(elements[1]);
                        } while (tempInt == -1 && --i >= 0);
                        if (tempInt != -1)
                            cout << elements[1] << " IS " << tempInt << endl;
                        else
                            cout << elements[1] << " IS UNDEFINED" << endl;
                    }
                }
                catch (...) {
                    cout << "PRINTING ERROR" << endl;
                }
                break;
            case FINISH:
                scope = false;
                hash_array[count].~Hashtable();
                count--;
                break;
        }
    }
    
    //declared variables
    else {
        int value = hash_array[count].search(elements[0]);
        int track;
        
        //found in scope
        if (value != -1) {
            if (elements[1] == "++" || elements[1] == "--") {
                hash_array[count].operate(elements[0], elements[1]);
            }
            else if (elements[1] == "=") {
                hash_array[count].add(elements[0], stoi(elements[2]));
            }
        }
        
        //not found in the scope then execute
        else if (value == -1) {
            int i = hash_array.size() - 1;
            value = hash_array[i].search(elements[0]);
            while (value == -1 && i >= 0) {
                value = hash_array[--i].search(elements[0]);
            }
            if (value == -1)
                cout << "No variable of name " << elements[0] << endl;
            else {
                track = i++;
                if (elements[1] == "=") {
                    hash_array[track].add(elements[0], stoi(elements[2]));
                }
            }
        }
    }
}

int main() {
    
    string inputFileName;
    cout << "Enter the file full path/address (i.e. C:\Project\input.txt): " << endl;
    getline(cin, inputFileName);
    BorgProcessing bg(inputFileName);
    
    system("PAUSE");
    return 0;
}

/*
 
 RESULT OF THE SAMPLE BORG PROGRAM (BORG.txt)
 
 Enter the file full path/address (i.e. C:Projectinput.txt):
 C:\Users\Christian Miller\Desktop\Program 4\New folder\BORG.txt
 COM HERE IS OUR FIRST BORG PROGRAM
 COM WHAT A ROBUST LANGUAGE IT IS
 START
 VAR BORAMIR = 25
 VAR LEGOLAS = 101
 PRINT BORAMIR
 BORAMIR IS 25
 BORAMIR ++
 PRINT LEGOLAS
 LEGOLAS IS 101
 PRINT GANDALF
 GANDALF IS UNDEFINED
 PRINT BORAMIR * 2
 BORAMIR * 2 IS 52
 COM
 COM NESTED BLOCK
 COM
 START
 VAR GANDALF = 49
 PRINT GANDALF
 GANDALF IS 49
 PRINT BORAMIR
 BORAMIR IS 26
 FINISH
 PRINT GANDALF
 GANDALF IS UNDEFINED
 START
 LEGOLAS = 1000
 PRINT LEGOLAS
 LEGOLAS IS 1000
 FINISH
 PRINT LEGOLAS
 LEGOLAS IS 1000
 LEGOLAS --
 PRINT LEGOLAS
 LEGOLAS IS 999
 FINISH
 Press any key to continue . . .
 
 
 
*/ 
