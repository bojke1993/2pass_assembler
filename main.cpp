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


enum allInstructions {PUSH, POP, LOAD, STORE,INT, JMP, CALL, RET, JZ, JNZ, JGZ, JGEZ, JLZ, JLEZ, ADD, SUB, MUL, DIV, MOD, AND, OR, XOR, NOT, ASL, ASR};

static map<string,int> instructionsMap = map<string,int>();

typedef enum{
    stackIns,
    lsIns,
    flowIns,
    arlogIns,
    unknown_type
}instructionType;

void initInstrucions(){
    instructionsMap.insert(pair<string,int>("PUSH", PUSH));
    instructionsMap.insert(pair<string,int>("POP", POP));

    instructionsMap["LOAD"] = LOAD;
    instructionsMap["STORE"] = STORE;

    instructionsMap["INT"] = INT;
    instructionsMap["JMP"] = JMP;
    instructionsMap["CALL"] = CALL;
    instructionsMap["RET"] = RET;
    instructionsMap["JZ"] = JZ;
    instructionsMap["JNZ"] = JNZ;
    instructionsMap["JGZ"] = JGZ;
    instructionsMap["JGEZ"] = JGEZ;
    instructionsMap["JLZ"] = JLZ;
    instructionsMap["JLEZ"] = JLEZ;

    instructionsMap["ADD"] = ADD;
    instructionsMap["SUB"] = SUB;
    instructionsMap["MUL"] = MUL;
    instructionsMap["DIV"] = DIV;
    instructionsMap["MOD"] = MOD;
    instructionsMap["AND"] = AND;
    instructionsMap["OR"] = OR;
    instructionsMap["XOR"] = XOR;
    instructionsMap["NOT"] = NOT;
    instructionsMap["ASL"] = ASL;
    instructionsMap["ASR"] = ASR;

}

int getInstructionsCode(string &ins){
    map<string, int>::iterator p;

    // find value given key
    p = instructionsMap.find(ins);
    if(p != instructionsMap.end())
        return p->second;
    else
        return -1;
}

instructionType getInstructionType(string ins){
    int value = getInstructionsCode(ins);
    if(value == 0 || value ==1) return stackIns;
    else if(value == 2 || value == 3) return lsIns;
    else if(value > 3 && value <=13) return flowIns;
    else if(value > 13) return arlogIns;
    else return unknown_type;
}



string removeComments(string &str){
    return str.substr(0, str.find(";",0));
}


regex randomText("[\\[\\]a-zA-Z0-9,#]+");
regex labelRgx("[a-zA-z0-9]+[:]");
regex regDirRgx("R[0-9]+,?");
regex regIndRgx("\\[R[0-9]+\\],?");
regex immedRgx("#-?[0-9]+");
regex memdirRgx("[a-zA-Z0-9]+");
regex sectionRgx("\\.[a-z]+");
regex trimmer("\\s*(.*\\S)\\s*");


string trim(string &line){
    line.erase(0, line.find_first_not_of(' '));
    line.erase(line.find_last_not_of(' ') + 1);

    return line;
}

bool isEmpty(string &str){
    if(regex_match(str, randomText)) return false;
    else return true;
}


int main() {

    ifstream ulaz("ulaz.txt");

    initInstrucions();

    string line = "";

    //pocetak obrade text sekcije
    bool ret = true;
    while(getline(ulaz, line)){

        line = removeComments(line);
        string line2 = trim(line);
        stringstream ss(line2);

        instructionLine il = instructionLine();

        string temp;

        ss >> temp;
        if(regex_match(temp, labelRgx)){

            il.label = temp;
            ss >> il.instruction;
            if(isEmpty(il.instruction)){
                cout << line2 <<endl;
                continue;
            }
        }else {
            il.instruction = temp;
        }

        if(regex_match(il.instruction, sectionRgx) || regex_match(temp, sectionRgx)){
            cout << "sekcija" << endl;
            continue;
        }

        instructionType tip = getInstructionType(il.instruction);

        switch(tip){
            case 0: ss >> il.op1 >> il.op2;
                    if(!isEmpty(il.op2) && isEmpty(il.op1)){
                        cout << "stek instrukcije imaju jedan operand!!!"<<endl;
                        ret = false;
                        break;
                    }
                    if(!regex_match(il.op1, regDirRgx)){
                        cout << "U stek instrukcijama dozvoljeno je samo registarsko direktno adresiranje!" <<endl;
                        ret = false;
                        break;
                    }
                    break;

            case 1: ss >> il.op1 >> il.op2 >> il.op3;
                    if(!isEmpty(il.op3)){
                        cout << "load/store instrukcije ne mogu imati vise od dva operanda!!!"<<endl;
                        ret = false;
                        break;
                    }
                    if(!(regex_match(il.op1, regDirRgx)) && !(regex_match(il.op1, regIndRgx))){
                        cout << "prvi operand mora biti regdir ili regind" <<endl;
                        ret = false;
                        break;
                    }
                    if(il.instruction == "STORE" && regex_match(il.op2, immedRgx)){
                        cout << "store instrukcija ne ide sa neposrednim adresiranjem!"<<endl;
                        ret = false;
                        break;
                    }
                    break;

            case 2: ss >> il.op1 >> il.op2 >> il.op3;
                    switch(getInstructionsCode(il.instruction)){
                        case 5:
                        case 6: cout << "jmp/call"<<endl;
                                if(!isEmpty(il.op2)){
                                    cout << "jmp/call podrzavaju samo 1 operand!!!"<<endl;
                                    ret = false;
                                    break;
                                }
                                if(!regex_match(il.op1, memdirRgx) && !regex_match(il.op1, regIndRgx) ) cout << "op1 moze biti regind ili memdir"<<endl;
                                break;
                        case 7:cout << "ret"<<endl;
                                if(!isEmpty(il.op1)){
                                    cout << "ret radi bez operanada!!!"<<endl;
                                    ret = false;
                                    break;
                                }
                        case 8:
                        case 9:
                        case 10:
                        case 11:
                        case 12:
                        case 13:cout << "jz-jlez"<<endl;
                                if(!isEmpty(il.op3)){
                                    cout << "jz-jlez podrzavaju 2 operanda!!!"<<endl;
                                    ret = false;
                                    break;
                                }
                                if( !regex_match(il.op1, regDirRgx)) {
                                    cout << "op1 mora biti regdir"<<endl;
                                    ret = false;
                                    break;
                                }
                                if( !regex_match(il.op2, regIndRgx) && !regex_match(il.op2, memdirRgx)){
                                    cout << "op2 mora biti regind ili memdir"<<endl;
                                    ret = false;
                                    break;
                                }
                    }
                    break;

            case 3: ss >> il.op1 >> il.op2 >> il.op3;
                    if(isEmpty(il.op1) || isEmpty(il.op2) || isEmpty(il.op3)){
                        cout << "arlog mora imati ima 3 operanda!!" << endl;
                        ret = false;
                        break;
                    }
                    if( !regex_match(il.op1, regDirRgx) || !regex_match(il.op2, regDirRgx) || !regex_match(il.op3, regDirRgx)){
                        cout << "U arlog instrukcijama dozvoljeno je samo regdir adresiranje!!" << endl;
                        ret = false;
                        break;
                    }
                    break;

            case 4: cout << "Nepoznata instrukcija!!" << endl;
                    ret = false;
                    break;
        }

        if(ret) cout << line2<<endl;
        else{
            cout << "###################################################"<<endl;
            cout << "GRESKA!!!"<< endl;
            break;
        }
    }

}
