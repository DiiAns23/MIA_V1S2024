#include "../lib/disco.h"
#include "../lib/scanner.h"

#include <iostream>
#include <stdlib.h>
#include <locale>
#include <fstream>
#include <vector>
#include <cstdlib>

using namespace std;

Disk disco;
scanner::scanner(){}

void Clear(){
    cout << "\x1B[2J\x1B[H";
}

void scanner::start(){
    system("clear");
    cout << "------------------------------INGRESE UN COMANDO------------------------------\n" << endl;
    cout << "--------------------------------exit para salir-------------------------------\n" << endl;
    cout << ">>";
    while (true)
    {
        string texto;
        getline(cin, texto);
        Clear();
        if (compare(texto, "exit"))
        {
            break;
        }
        // mkdisk -s=10 -u=m -path=/home/hola.dsk
        string tk = token(texto); // mkdisk
        texto.erase(0,tk.length()+1);
        vector<string> tks = split_tokens(texto); //[-s=10, -u=m, -path=/home/hola.dsk]
        functions(tk, tks);
        cout << "\nPresione Enter para continuar...." << endl;
        getline(cin,texto);
        Clear();
        cout << "------------------------------INGRESE UN COMANDO------------------------------\n" << endl;
        cout << "--------------------------------exit para salir-------------------------------\n" << endl;
        cout << ">>";
    }
}

void scanner::errores(string operacion, string mensaje){
    
    cout << "\033[1;41m Error\033"<< "\033[0;31m(" + operacion + ")~~> \033[0m"<< mensaje << endl;
}

void scanner::respuesta(string operacion, string mensaje){
    
    cout << "\033[0;42m(" + operacion + ")~~> \033[0m"<< mensaje << endl;
}

bool scanner::confirmar(string mensaje){
    cout << mensaje << "Confirmar(S), cualquier otra letra para cancelar" << endl;
    string respuesta;
    getline(cin,respuesta);
    if (compare(respuesta, "s"))
    {
        return true;
    }
    return false;
    
}

void scanner::functions(string token, vector<string> tks)
{
    if (compare(token, "MKDISK"))
    {
        cout << "FUNCION MKDISK" << endl;
        disco.mkdisk(tks); // [-size=10, -u=m, -path=/home/hola.dsk]
    }else if(compare(token, "RMDISK")){
        cout << "FUNCION RMDISK" << endl;
        disco.rmdisk(tks); // [-path=/home/hola.dsk]
    }else if(compare(token, "FDISK")){
        cout << "FUNCION FDISK" << endl;
        disco.fdisk(tks); // [-size=10, -u=m, -path=/home/hola.dsk]
    }else if(compare(token, "EXEC")){
        cout << "FUNCION EXEC" << endl;
        funcion_excec(tks); // [-path=/home/comandos.txt]
    }
    else{
        cout << "ERROR" << endl;
        //errores("SYSTEM","El comando ingresado no se reconoce en el sistema \""+token+"\"");
    }
}

string scanner::token(string text)
{
    string tkn = "";
    bool terminar = false;
    for (char &c : text){
        if (terminar)
        {
            if (c == ' ' || c == '-'){
                break;
            }
            tkn += c;
        }
        else if ((c != ' ' && !terminar))
        {
            if (c=='#'){
                tkn=text;
                break;
            }else{
                tkn += c;
                terminar = true;
            }
        }
    }
    return tkn;
}

vector<string> scanner::split_tokens(string text){
    vector<string> tokens;
    if (text.empty())
    {
        return tokens;
    }
    text.push_back(' ');
    string token = "";
    int estado = 0;
    for(char &c: text){
        if (estado ==0 && c=='-')
        {
            estado = 1;

        }else if(estado==0 && c=='#'){
            continue;
        }else if(estado!=0){
            if (estado == 1)
            {
                if(c=='='){
                    estado = 2;
                }else if(c == ' '){
                    continue;
                }
            }else if(estado == 2){
                if (c=='\"')
                {
                    estado = 3;
                }else{
                    estado = 4;
                }
                
            }else if(estado == 3){
                if (c=='\"')
                {
                    estado = 4;
                }
            }else if (estado==4 && c=='\"')
            {
                tokens.clear();
                continue;
            }else if (estado ==4 && c==' ')
            {
                estado = 0;
                tokens.push_back(token);
                token = "";
                continue;
            }
            token+=c;
        }
    }
    return tokens;
}

string scanner::upper(string a){
    string up="";
    for(char &a: a){
        up+=toupper(a);
    }
    return up;  
}

bool scanner::compare(string a, string b){
    if (upper(a)==upper(b))
    {
        return true;
    }
    return false;
}


void scanner::funcion_excec(vector<string> tokens){ // excec -path=/home/comandos.txt
    
    string path = "";
    for (string token:tokens){
        string tk = token.substr(0,token.find("="));
        token.erase(0,tk.length()+1);
        if(compare(tk, "path")){
            path = token;
        }
    }
    if (path.empty()){
        errores("EXCEC","No se encontro el path del archivo a ejecutar");
        return;
    }
    excec(path);
}

void scanner::excec(string path){
    cout << "Ejecutando archivo: " << path << endl;
    if (path.substr(0, 1) == "\""){
        path = path.substr(1, path.length() - 2);
    }
    string filename(path);
    vector <string> comandos;
    string comando;
    ifstream input_file(filename);
    if(!input_file.is_open()){
        errores("EXCEC","No se pudo abrir el archivo \""+filename+"\"");
        return;
    }
    
    while(getline(input_file, comando)){
        comandos.push_back(comando);
    }

    input_file.close();

    for(string &comando:comandos){
        string texto = comando;
        string tk = token(texto);
        if(texto!=""){
            if(compare(texto,"PAUSE")){
                string pause;
                respuesta("PAUSE","Presione Enter para continuar....");
                getline(cin,pause);
                continue;
            }
            texto.erase(0,tk.length()+1);
            vector<string> tks = split_tokens(texto);
            functions(tk, tks);
        }
    }
    return;
}

