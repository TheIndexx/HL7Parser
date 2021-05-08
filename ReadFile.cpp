#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <experimental/filesystem>
using namespace std;
namespace fs = std::experimental::filesystem;
struct HL7rows
{
    std::string filename, rownumber, RecordType, MID, PID, Name, address, messagedate, rawrecord;
};

const std::string WHITESPACE = " \n\r\t\f\v";
 
std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}
std::vector<HL7rows> allrows;

std::vector<string> splitLine(string myString)
{
    std::stringstream iss(myString);

    std::vector<string> result;
    std::string strValue;
    while (std::getline(iss, strValue, '|'))
    {
        result.push_back(strValue);
    }
    return result;
}

HL7rows ParseMSHRecord(std::vector<string> parsedString)
{
    HL7rows MSHRecord;
    MSHRecord.RecordType = "MSH";
    MSHRecord.MID = parsedString[9];
    MSHRecord.messagedate = parsedString[6];

    return MSHRecord;
}

HL7rows ParsePIDRecord(std::vector<string> parsedString)
{
    HL7rows PIDRecord;
    PIDRecord.RecordType = "PID";
    PIDRecord.PID = parsedString[3];
    PIDRecord.Name = parsedString[5];
    PIDRecord.address = parsedString[11];

    return PIDRecord;
}


void ParseSingleFile(string path)
{
    std::ifstream infile(path);
    string messageDate;
    if (infile.is_open()) {
        std::string line;
        int rowNumber = 0;

        while (std::getline(infile, line)) {
            char chars[] = "^";
            line.erase(std::remove(line.begin(), line.end(), chars[0]), line.end());

            std::vector parsedString = splitLine(line.c_str());
            rowNumber++;

            HL7rows singleHL7row;
            std::string recType = ltrim(parsedString[0]);

            if (recType == "MSH") {
                singleHL7row = ParseMSHRecord(parsedString);
                singleHL7row.filename = path;
                singleHL7row.rownumber = rowNumber;
                messageDate = singleHL7row.messagedate;
                singleHL7row.RecordType = recType;
                singleHL7row.rawrecord = line.c_str();
            }
            else if (recType == "PID") {
                singleHL7row = ParsePIDRecord(parsedString);
                singleHL7row.filename = path;
                singleHL7row.rownumber = rowNumber;
                singleHL7row.messagedate = messageDate;
                singleHL7row.RecordType = recType;
                singleHL7row.rawrecord = line.c_str();
            }
            else {
                singleHL7row.filename = path;
                singleHL7row.rownumber = rowNumber;
                singleHL7row.RecordType = recType;
                singleHL7row.rawrecord = line.c_str();
            }

            allrows.push_back(singleHL7row);
        }
    }
}

void Option1()
{
    for(int i=0; i < allrows.size(); i++) {
        HL7rows rec = allrows[i];
        if (rec.RecordType == "PID") {
            cout << "File Name: " << rec.filename << endl;
            cout << "\nPID is: '" << rec.PID << "'\nPatient Name is: " << rec.Name << "\nMessage Date is: " << rec.messagedate << endl;
        }
    }
}

void Option2()
{
    string user_input1;
    cout << "Type the Patient ID: ";
    cin.ignore();
    getline(cin, user_input1);

    for(int i=0; i < allrows.size(); i++) {
        HL7rows rec = allrows[i];
        if (rec.PID == user_input1){
            cout << "\nPatient Name is: " << rec.Name << "\nAddress is: " << rec.address;
        }
    }
}

void Option3()
{
    string user_input2;
    cout << "Type HL7 Message ID: ";
    cin >> user_input2;
    string filePath = "";
    for(int i=0; i < allrows.size(); i++) {
        HL7rows rec = allrows[i];
        if (rec.RecordType == "MSH" && rec.MID == user_input2)
        {
            filePath = rec.filename;
        }
        if (rec.filename == filePath) {
            cout << rec.rawrecord;
        }
    }
            
}

void ParseFiles(string inputPath)
{
    const fs::path pathToShow = fs::path(inputPath);
    for (const auto & entry : fs::directory_iterator(pathToShow)) {
        ParseSingleFile(entry.path().string());
    }
}

std::string GetInputPath()
{
    std::string path;
    std::cout << "Enter directory: ";
    std::cin >> path;
    return path;
}

int main()
{
    string path = GetInputPath();
    ParseFiles(path);
    string FP = "FP";
    string PID = "PID";
    string MSH = "MSH";
    string step1;
    cout << "Enter FP, PID or MSH: ";
    cin >> step1;
    if (step1 == FP) {
        Option1();
    } else if (step1 == PID) {
        Option2();
    } else if (step1 == MSH) {
        Option3();
    } else {cout << "That is not one of the given options";}
    return 0;
}