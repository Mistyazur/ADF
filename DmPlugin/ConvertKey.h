#ifndef CONVERTKEY_H
#define CONVERTKEY_H

int StrToKeyNum(const QString &str,int &shiftup)
{
    int outcode;

    if(str == "0"){
        outcode = 0x30;
    }
    else if(str == "1"){
        outcode = 0x31;
    }
    else if(str == "2"){
        outcode = 0x32;
    }
    else if(str == "3"){
        outcode = 0x33;
    }
    else if(str == "4"){
        outcode = 0x34;
    }
    else if(str == "5"){
        outcode = 0x35;
    }
    else if(str == "6"){
        outcode = 0x36;
    }
    else if(str == "7"){
        outcode = 0x37;
    }
    else if(str == "8"){
        outcode = 0x38;
    }
    else if(str == "9"){
        outcode = 0x39;
    }
    else if(str == ")"){
        shiftup = 1;
        outcode = 0x30;
    }
    else if(str == "!"){
        shiftup = 1;
        outcode = 0x31;
    }
    else if(str == "@"){
        shiftup = 1;
        outcode = 0x32;
    }
    else if(str == "#"){
        shiftup = 1;
        outcode = 0x33;
    }
    else if(str == "$"){
        shiftup = 1;
        outcode = 0x34;
    }
    else if(str == "%"){
        shiftup = 1;
        outcode = 0x35;
    }
    else if(str == "^"){
        shiftup = 1;
        outcode = 0x36;
    }
    else if(str == "&"){
        shiftup = 1;
        outcode = 0x37;
    }
    else if(str == "*"){
        shiftup = 1;
        outcode = 0x38;
    }
    else if(str == "("){
        shiftup = 1;
        outcode = 0x39;
    }
    else if(str == "a"){
        outcode = 0x41;
    }
    else if(str == "b"){
        outcode = 0x42;
    }
    else if(str == "c"){
        outcode = 0x43;
    }
    else if(str == "d"){
        outcode = 0x44;
    }
    else if(str == "e"){
        outcode = 0x45;
    }
    else if(str == "f"){
        outcode = 0x46;
    }
    else if(str == "g"){
        outcode = 0x47;
    }
    else if(str == "h"){
        outcode = 0x48;
    }
    else if(str == "i"){
        outcode = 0x49;
    }
    else if(str == "j"){
        outcode = 0x4A;
    }
    else if(str == "k"){
        outcode = 0x4B;
    }
    else if(str == "l"){
        outcode = 0x4C;
    }
    else if(str == "m"){
        outcode = 0x4D;
    }
    else if(str == "n"){
        outcode = 0x4E;
    }
    else if(str == "o"){
        outcode = 0x4F;
    }
    else if(str == "p"){
        outcode = 0x50;
    }
    else if(str == "q"){
        outcode = 0x51;
    }
    else if(str == "r"){
        outcode = 0x52;
    }
    else if(str == "s"){
        outcode = 0x53;
    }
    else if(str == "t"){
        outcode = 0x54;
    }
    else if(str == "u"){
        outcode = 0x55;
    }
    else if(str == "v"){
        outcode = 0x56;
    }
    else if(str == "w"){
        outcode = 0x57;
    }
    else if(str == "x"){
        outcode = 0x58;
    }
    else if(str == "y"){
        outcode = 0x59;
    }
    else if(str == "z"){
        outcode = 0x5A;
    }
    else if(str == "A"){
        shiftup = 1;
        outcode = 0x41;
    }
    else if(str == "B"){
        shiftup = 1;
        outcode = 0x42;
    }
    else if(str == "C"){
        shiftup = 1;
        outcode = 0x43;
    }
    else if(str == "D"){
        shiftup = 1;
        outcode = 0x44;
    }
    else if(str == "E"){
        shiftup = 1;
        outcode = 0x45;
    }
    else if(str == "F"){
        shiftup = 1;
        outcode = 0x46;
    }
    else if(str == "G"){
        shiftup = 1;
        outcode = 0x47;
    }
    else if(str == "H"){
        shiftup = 1;
        outcode = 0x48;
    }
    else if(str == "I"){
        shiftup = 1;
        outcode = 0x49;
    }
    else if(str == "J"){
        shiftup = 1;
        outcode = 0x4A;
    }
    else if(str == "K"){
        shiftup = 1;
        outcode = 0x4B;
    }
    else if(str == "L"){
        shiftup = 1;
        outcode = 0x4C;
    }
    else if(str == "M"){
        shiftup = 1;
        outcode = 0x4D;
    }
    else if(str == "N"){
        shiftup = 1;
        outcode = 0x4E;
    }
    else if(str == "O"){
        shiftup = 1;
        outcode = 0x4F;
    }
    else if(str == "P"){
        shiftup = 1;
        outcode = 0x50;
    }
    else if(str == "Q"){
        shiftup = 1;
        outcode = 0x51;
    }
    else if(str == "R"){
        shiftup = 1;
        outcode = 0x52;
    }
    else if(str == "S"){
        shiftup = 1;
        outcode = 0x53;
    }
    else if(str == "T"){
        shiftup = 1;
        outcode = 0x54;
    }
    else if(str == "U"){
        shiftup = 1;
        outcode = 0x55;
    }
    else if(str == "V"){
        shiftup = 1;
        outcode = 0x56;
    }
    else if(str == "W"){
        shiftup = 1;
        outcode = 0x57;
    }
    else if(str == "X"){
        shiftup = 1;
        outcode = 0x58;
    }
    else if(str == "Y"){
        shiftup = 1;
        outcode = 0x59;
    }
    else if(str == "Z"){
        shiftup = 1;
        outcode = 0x5A;
    }
    else if(str == " "){
        outcode = 0x20;
    }
    else if(str == "`"){
        outcode = 0xc0;
    }
    else if(str == "~"){
        shiftup = 1;
        outcode = 0xc0;
    }
    else if(str == "-"){
        outcode = 189;
    }
    else if(str == "_"){
        shiftup = 1;
        outcode = 189;
    }
    else if(str == "="){
        outcode = 187;
    }
    else if(str == "+"){
        shiftup = 1;
        outcode = 187;
    }
    else if(str == "["){
        outcode = 219;
    }
    else if(str == "{"){
        shiftup = 1;
        outcode = 219;
    }
    else if(str == "]"){
        outcode = 221;
    }
    else if(str == "}"){
        shiftup = 1;
        outcode = 221;
    }
    else if(str == "\\"){
        outcode = 220;
    }
    else if(str == "|"){
        shiftup = 1;
        outcode = 220;
    }
    else if(str == ";"){
        outcode = 186;
    }
    else if(str == ":"){
        shiftup = 1;
        outcode = 186;
    }
    else if(str == "'"){
        outcode = 222;
    }
    else if(str == ","){
        outcode = 188;
    }
    else if(str == "<"){
        shiftup = 1;
        outcode = 188;
    }
    else if(str == "."){
        outcode = 190;
    }
    else if(str == ">"){
        shiftup = 1;
        outcode = 190;
    }
    else if(str == "/"){
        outcode = 191;
    }
    else if(str == "?"){
        shiftup = 1;
        outcode = 191;
    }
    else
        outcode = 0;


    return outcode;
}
#endif // CONVERTKEY_H

