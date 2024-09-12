#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <unordered_set>
#include <sstream>
#include <set>

using namespace std;

void appendFile(const std::string& sourceFile, const std::string& destFile) {
    // Abre o arquivo de origem em modo leitura
    std::ifstream source(sourceFile, std::ios::binary);
    if (!source) {
        std::cerr << "Não foi possível abrir o arquivo de origem: " << sourceFile << std::endl;
        return;
    }

    // Abre o arquivo de destino em modo append (acrescentar)
    std::ofstream dest(destFile, std::ios::binary | std::ios::app);
    if (!dest) {
        std::cerr << "Não foi possível abrir o arquivo de destino: " << destFile << std::endl;
        return;
    }

    // Copia o conteúdo do arquivo de origem para o arquivo de destino
    dest << source.rdbuf();
}


void traduz(const vector<int>& obj, ofstream& arquivosaida) {
    unordered_map<int, string> nomes;
    vector<int> lista_args;
    unordered_map<int, string> labels;
    vector<string> lista_nomes;
    stringstream bss, data, text;
    bool in_data = false;
    int i = 0;

    text << "section .text\n";
    text << "global _start\n\n";
    text << "_start:\n";

    for (i = 0; i < obj.size(); i++) {
        if (!in_data){
            int opcode = obj[i];
            int argumento = obj[i+1];

            if (opcode == 14) {
                in_data = true;
                i++; 
                continue;
            }

            if (opcode == 9){
                i++;
            }

            string label;
            if (opcode == 5 || opcode == 6 || opcode == 7 || opcode == 8){
                if (labels.find(argumento) == labels.end()){
                    label = "label" + to_string(argumento);
                    labels[argumento] = label;
                }
            }
            i++;
        }
    }


    in_data = false;
    for (i = 0; i < obj.size(); i++) {
        lista_args.clear();
        lista_nomes.clear();
        if (!in_data) {
            if (i + 1 >= obj.size()){
                break;
            } 
            int opcode = obj[i];

            int argumento;
            string nome; 
            string curr_label;
            lista_args.push_back(obj[i+1]);

            if (opcode == 5 || opcode == 6 || opcode == 7 || opcode == 8){
                curr_label = labels[lista_args[0]];
                goto switch_linha;
            }
            
            if(opcode == 9){
                lista_args.push_back(obj[i+2]);
            }

            if (opcode == 14) {
                in_data = true;
                i++; 
                continue;
            }

            for (size_t i = 0; i < lista_args.size(); i++) {
                int valor = obj[lista_args[i]-1];
                argumento = lista_args[i];
                if (nomes.find(argumento) == nomes.end()) {
                    nome = "dado" + to_string(nomes.size());
                    lista_nomes.push_back(nome);
                    nomes[argumento] = nome;

                    if (valor == 0) {
                        bss <<  nome << " resb 4\n";
                    } else {
                        data << nome << " dd " << valor << "\n";
                    }
                }else {
                    nome = nomes[argumento];
                    lista_nomes.push_back(nome);
                }
            }

            nome = lista_nomes[0];

    switch_linha:
            switch (opcode) {
                case 1:
                    text << "    add ebx, [" << nome << "]\n";
                    break;
                case 2:
                    text << "    sub ebx, [" << nome << "]\n";
                    break;
                case 3:
                    text << "    mov eax, ebx\n"; 
                    text << "    imul dword [" << nome << "]\n";
                    text << "    jo overflow\n";
                    text << "    mov ebx, eax\n";
                    break;
                case 4:
                    text << "    mov eax, ebx\n";  // descobrimos que idiv só pode ser utilizada com eax de acumulador, estamos usando ebx
                    text << "    mov edx, 0\n";
                    text << "    cdq\n";
                    text << "    mov ecx, [" << nome << "]\n";
                    text << "    idiv ecx\n";
                    text << "    mov ebx, eax\n";
                    break;
                    // div  
                case 5:
                    text << "    jmp " << curr_label << "\n";
                    // jmp
                    break;
                case 6:
                    text << "    cmp ebx, 0\n";
                    text << "    jl " << curr_label << "\n";
                    // jmpn
                    break;
                case 7:
                    text << "    cmp ebx, 0\n";
                    text << "    jg " << curr_label << "\n";
                    // jmpp
                    break;
                case 8:
                    text << "    cmp ebx, 0 ;bluiggi\n";
                    text << "    jz " << curr_label << "\n";
                    // jmpz
                    break;
                case 9:
                    text << "    mov eax, [" << nome << "]\n";
                    text << "    mov [" << lista_nomes[1] << "], eax\n";
                    i++;
                    // copy
                    break;
                case 10:
                    text << "    mov ebx, [" << nome << "]\n";
                    // load
                    break;
                case 11:
                    text << "    mov [" << nome << "], ebx\n";
                    // store 
                    break;
                case 12:
                    text << "    mov eax, " << nome << "\n";
                    text << "    call input\n";
                    // input func
                    break;
                case 13:
                    text << "    mov eax, [" << nome << "]\n";
                    text << "    call output\n";
                    // outpunt func
                    break;
                default:
                    break;
            }
            
            i++; 

            if (labels.find(i+1) != labels.end()){
                text <<  labels[i+1] << ":\n"; 
            }

        } 
        // else {
        //     // if (obj[i] != 0 && nomes.find(obj[i]) == nomes.end()) {
        //     //     string nome = "dado" + to_string(nomes.size());
        //     //     nomes[obj[i]] = nome;
        //     //     data << nome << " dd " << obj[i] << "\n";
        //     // } else if (obj[i] == 0) {
        //     //     string nome = "dado" + to_string(nomes.size());
        //     //     nomes[obj[i]] = nome;
        //     //     bss << nome << " resb 4\n";
        //     // }
        // }
    }

    if (!bss.str().empty()) {
        arquivosaida << "section .bss\n" << bss.str() << "\n";
    }
    if (!data.str().empty()) {
        arquivosaida << "section .data\n" << data.str() << "\n";
    }
    arquivosaida << text.str();
    arquivosaida << "\n    ; sair do código\n";
    arquivosaida << "    mov eax, 1\n";
    arquivosaida << "    sub ebx, ebx\n";
    arquivosaida << "    int 80h\n";
}

int main(int argc, char* argv[]) {
    string arquivo = argv[1];
    string file_name = arquivo.substr(0, arquivo.find_last_of('.'));
    string saida = file_name + ".s";
    ifstream arquivoentrada(arquivo);

    vector<int> obj;
    string linha;
    while (getline(arquivoentrada, linha)) {
        istringstream iss(linha);
        int token;
        while (iss >> token) {
            obj.push_back(token);
        }
    }

    arquivoentrada.close();
    ofstream arquivosaida(saida);
    traduz(obj, arquivosaida);
    arquivosaida.close();

    string sourceFile = "inputoutputfunc.asm"; // Nome do arquivo .s de origem
    string destFile = saida;   // Nome do arquivo .asm de destino

    appendFile(sourceFile, destFile);

    return 0;
}