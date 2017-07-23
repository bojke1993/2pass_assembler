#include <iostream>
#include <string>
#include "include/Error.h"
#include <fstream>
#include <regex>


using namespace std;





struct instructionLine{
    string label;
    string instruction;
    string op1;
    string op2;
    string op3;
};


enum allInstructions {push, pop, load, store};
static map<string,int> stackInstructions = map<string,int>();
static map<string,int> LSInstructions = map<string,int>();

typedef enum{
    stackIns,
    lsIns,
    unknown_type
}instructionType;

void initInstrucions(){
    stackInstructions["push"] = push;
    stackInstructions["pop"] = pop;
    LSInstructions["load"] = load;
    LSInstructions["store"] = store;

}

instructionType getInstructionType(string ins){
    if(stackInstructions.count(ins)) return stackIns;
    else if(LSInstructions.count(ins)) return lsIns;
    else return unknown_type;

}



string removeComments(string &str){
    return str.substr(0, str.find(";",0));
}

regex labelRgx("[a-zA-z0-9]+[:]");
regex trimmer("\\s*(.*\\S)\\s*");

string trim(string &line){
    line.erase(0, line.find_first_not_of(' '));
    line.erase(line.find_last_not_of(' ') + 1);

    return line;
}




int main() throw (){

    ifstream ulaz("ulaz.txt");

    initInstrucions();

    string line = "";

    getline(ulaz, line);

    line = removeComments(line);
    string line2 = trim(line);
    stringstream ss(line2);

    instructionLine il = instructionLine();

    string temp;

    ss >> temp;
    if(regex_match(temp, labelRgx)){

        il.label = temp;
        ss >> il.instruction;
    }else {
        il.instruction = temp;
    }

    instructionType tip = getInstructionType(il.instruction);

    switch(tip){
        case 0: cout << "stek instrukcija" <<endl;
                break;
        case 1: cout << "Load/Store instrukcija" <<endl;
                break;
        case 2: cout << "nepoznata instrukcija" <<endl;
                break;
    }


    cout << line2;

}
