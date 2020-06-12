//
// C. Jesús Valls 
// cjesus@ifae.es
//
// M. Granado-González
// MMG998@student.bham.ac.uk
//

#include "pCTXML.hh"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

pCTXML::pCTXML(const G4String inxml){
    fXMLFile = inxml;
    SetInputs();
}

pCTXML::~pCTXML(){
    fXMLFile = "";
}

void pCTXML::SetInputs(){
  
    G4cout << G4endl;
    G4cout << "Reading inputs from XML file: " 
     << GetXMLFileName() 
     << G4endl;
    G4cout << G4endl;
    
    string line;
    ifstream myfile (fXMLFile);
    if(myfile.is_open()){    
      
        XML2String  ("Example",fExample);
        XML2Int     ("SciDetNBars",fSciDetNBars);
        XML2Int     ("SciDetNLayers",fSciDetNLayers);
        XML2Double  ("SciDetBarX",fSciDetBarX);  
        XML2Double  ("SciDetBarY",fSciDetBarY);
        XML2Double  ("SciDetBarZ",fSciDetBarZ);
        XML2Double  ("SciDetX",fSciDetX);
        XML2Double  ("SciDetY",fSciDetY);
        XML2Double  ("SciDetZ",fSciDetZ);
        XML2Bool    ("UseSciDet",fUseSciDet);
        XML2Bool    ("UseCMOS",fUseCMOS);
        XML2Bool    ("UsePhantom",fUsePhantom);

    }
    else cout << "Unable to open file"; 
    myfile.close();
}


void pCTXML::XML2String(string tmp_app,string &dest){
    string line;
    ifstream in(fXMLFile);
    
    bool begin_tag = false;
    while (getline(in,line)){
        std::string tmp; // strip whitespaces from the beginning
        for (uint i = 0; i < line.length(); i++) if (!(line[i] == ' ' && tmp.size() == 0)) tmp += line[i];
    
        tmp = TrimSpaces(tmp); // remove spaces
        string First = Form("<%s>", tmp_app.c_str());
        string Last  = Form("</%s>",tmp_app.c_str());
     
        if ( tmp == First ){
            begin_tag = true;
            continue;
        }    
        else if ( tmp == Last ){
            begin_tag = false;
        }
        
        if (begin_tag){
            dest = tmp;
        }
    }

    if(dest==""){
        std::cout << "Path is : " << tmp_app << std::endl;
        std::cout << "Path not found" << std::endl;
        const char *msg = "Path to the GENIE file is empty";
        const char *origin = "pCTXML::XML2String";
        const char *code = "if(dest==""){";
        G4Exception(origin,code,FatalException,msg);
    }
  
  return;
}

void pCTXML::XML2Int(string tmp_app,int &dest){
    string dest_string;
    XML2String(tmp_app,dest_string);
    dest = atoi(dest_string.c_str());
    return;
}

void pCTXML::XML2Double(string tmp_app,double &dest){
    string dest_string;
    XML2String(tmp_app,dest_string);
    dest = atof(dest_string.c_str());
    return;
}

void pCTXML::XML2Bool(string tmp_app,bool &dest){
    string dest_string;
    XML2String(tmp_app,dest_string);

    if(         dest_string == "true" || 
                dest_string == "TRUE" || 
                dest_string == "True" || 
                dest_string == "1"    || 
                dest_string == "I")         dest=true;

    else if(    dest_string == "false" || 
                dest_string == "FALSE" || 
                dest_string == "False" || 
                dest_string == "0"     || 
                dest_string == "O")         dest=false;

    else{
                G4ExceptionDescription msg;
                msg << "Use either True or False" << G4endl;
                G4Exception("pCTXML::XML2Bool(",
                "MyCode0002",FatalException, msg);
    }

  return;
}


string pCTXML::TrimSpaces(string input)
{
    if( input.find_first_not_of(" \n") != 0)
        input.erase( 0,  input.find_first_not_of(" \n")  );

    if( input.find_last_not_of(" \n") != input.length() )
        input.erase( input.find_last_not_of(" \n")+1, input.length() );

    return input;
}
