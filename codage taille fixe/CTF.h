#ifndef TP1_CTF_H
#define TP1_CTF_H

#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "Codage.h"
#include "BitStream-1.2.12/BitStream.h"

/*
 * A FAIRE
 * passer la taille du bitblock dans le constructeur
 * faire en sorte de passer la table de codage en paramètre des fonction encode et decode (comme tp) ==> supprimer l'attribut codage_table
 * voir pourquoi ce n'est pas possible d'utilisser find pour récupérer la valeur d'une clé de la map
 * implémenter surcharge [] pour accéder aux valeurs de la map
 * tostring en représentation binaire et pas décimal
 * passer tous en en ou tous en fr
 *
 */

using namespace std;


class CTF : public Codage {

    // attributes
    private:
    const std::string nom_file;
    const string nom_file_svg;
    vector<char> symbol_list;
    map<string, Bits::Block<8>> codage_table;
    vector<std::string> data_to_encode;
    vector<std::string> data_decoded;

    // constructors
    public:
    explicit CTF(const std::string &file) : nom_file(file+".txt"), nom_file_svg(file+ "_compressed_and_encoded.txt")  {
        generate_symbol_list();
        cout << "# symbol_list created" << endl;
        generate_codage_table();
        cout << "# codage_table created" << endl;
        if (check_double_coded_value() == 1) {
            cout
                    << "objet CTF crée mais le code de certaines valeurs est identiques, veuillez indiquer une taille de bit plus grande"
                    << endl;
        }
        backup_compressed_data(encode());  // svg les données encodées dans un fichier
        read_compressed_data();
    }
    CTF(const CTF &c) : nom_file(c.nom_file) {}
    CTF(CTF &&_c) : nom_file(_c.nom_file) {}
    ~CTF() = default;

    // Methods
    /* lire les données dans un fichier
     * construire la table des caractères différents dans ce fichier
     * (alphabet == liste des symboles == table des caractères)
     */
    void generate_symbol_list() {

        // ouvrir le fichier en lecture
        ifstream ifs(nom_file, ios::in);
        if (ifs) {
            //cout << "fichier ouvert" << endl;

            /* lire ligne par ligne
            string ligne;
            while(getline(ifs, ligne)) {
                cout << "ligne: " << ligne << endl;  // on l'affiche
            }*/

            // lire char par char
            char c;
            while (ifs.get(c)) {
                data_to_encode.emplace_back(string(1,c)); // on svg les caractères de base
                // vérifier si char non présent dans le vector
                if (!(find(symbol_list.begin(), symbol_list.end(), c) != symbol_list.end())) {
                    // si non présent le stocker
                    symbol_list.push_back(c);
                }
            }
            ifs.close();
        } else {
            cerr << "Erreur à l'ouverture !" << endl;
        }
    }
    /* creer la table de codage (association alphabet/codage)
     */
    void generate_codage_table() {
        Bits::Stream stream;
        int j = 0;
        //cout << "\n# Ecriture bit a bit des char de la symbole list dans un flux" << endl;
        // pour chaque symbole de la list
        for (auto const &value: symbol_list) {
            stream << value;  // on l'écrit dans le flux
            //cout << value << "\t";
        }
        //cout << "\nbits stockes dans le stream: " << stream.get_bit_size() << endl;

        //cout << "\n# Ecriture de Bits::Block de taille constante depuis un flux" << endl;
        // pour chaque symbole du fluxs
        while (!stream.end_of_stream()) {
            Bits::Block<8> bitblock;
            stream >> bitblock;
            //cout << "bitblock= " << bitblock << endl;
            //codage_table[string(1, symbol_list[j])] = bitblock; // on remplit la map avec le char casté en string et sa valeur codé sur 8 bits
            codage_table.emplace(string(1, symbol_list[j]),
                                 bitblock); // on remplit la map avec le char casté en string et sa valeur codé sur 8 bits
            j++;
        }
    }
    /* fonction permettant de sauvegarder les données compressés dans un fichier
     */
    void backup_compressed_data(vector<Bits::Block<8>> v_encoded) {
        cout << "# backing compressed data" << endl;
        cout << " > file \""<< nom_file_svg << "\" created" << std::endl;
        create_and_format_file(0);
        ofstream fichier(nom_file_svg, std::ios::app);  // ouverture en écriture avec effacement du fichier ouvert

        if(fichier) {
            // on se place à la fin du fichier
            // pour chaque Bits::Block  de v_encoded
            for (auto const &value: v_encoded) {
                // on l'enregistre dans le fichier
                fichier << Bits::Binary(value);
            }
            fichier << "\n</data>\n</root>";
            fichier.close();

        }
        else {
            cerr << "backup_compressed_data() Impossible d'ouvrir le fichier !" << endl;
        }
    }
    /* fonction permettant de lire les données compressés
     */
    vector<Bits::Block<8>> read_compressed_data() {
        cout << "# reading compressed data" << endl;
        // on ouvre le fichier compréssé
        ifstream fichier(nom_file_svg, ios::in);  // on ouvre en lecture
        vector<Bits::Block<8>> vb;

        if(fichier) {
            // on affiche la taille du fichier
            //cout << "taille du fichier compresse " << getFileSize(nom_file+"_encoded.txt") << endl;

            /* lecture du fichier en entier
            string contenu;
            while(getline(fichier, contenu)) {
                cout << contenu;
            }*/
            string contenu;
            string all;
            while(getline(fichier, contenu)) {
                all.append(contenu);
            }
            std::string start_delim = "<data>";
            std::string stop_delim = "</data>";
            string strNew = extract_str_btwn_2_str(all, start_delim, stop_delim);
            //cout << strNew;

            // on lit 8 par 8
            int a;
            Bits::Block<8> block;
            int indi_block = 7;
            for(char& c : strNew) {
                if(c=='0') {
                    block.set_bit(indi_block, 0);
                    indi_block--;
                }
                if(c=='1') {
                    block.set_bit(indi_block, 1);
                    indi_block--;
                }
                if(indi_block == -1) {
                    indi_block = 7;
                    vb.push_back(block);
                }
            }
            cout << "\n----------------------Data readed & decoded ----------------------------\n" << endl;
            cout << "// uncomment to see\n";
            for (auto const &value: vb) {
                //cout << getKeyInMap(value); UNCOMMENT TO SEE DATA
                data_decoded.emplace_back(getKeyInMap(value)); // on sauvegarde les caractères décodés
            }
            cout << "\n----------------------End data------------------------------------------\n" << endl;

            // on affiche un message pour savoir si l'intégrité des données est respectées
            cout << check_is_data_equal();

            fichier.close();
        }
        else {
            cerr << "read_compressed_data() Impossible d'ouvrir le fichier !" << endl;
        }
        return vb;
    }
    /* Renvoie les données encodés du fichier passée en paramètre
     */
    vector<Bits::Block<8>> encode() override {
        cout << "# encoding data started" << std::endl;
        vector<Bits::Block<8>> v;
        // ouvrir le fichier en lecture
        ifstream ifs (nom_file, ios::in);
        if(ifs) {
            //cout << "fichier ouvert" << endl;

            // lire char par char
            char c;
            while (ifs.get(c)) {
                // récupérer le code du symbole dans la table de codage
                // et l'insérer dans le vector
                v.push_back(getValueInMap(std::string(1, c)));
            }
            ifs.close();
        }
        else {
            cerr << "Erreur à l'ouverture !" << endl;
        }

        return v;
    }
    /* décode les données du fichier où les données compréssés ont été stockés
     */
    vector<std::string> decode(vector<Bits::Block<8>> v_encoded) override {
        cout << "# decoding data started" << std::endl;
        vector<std::string> v_decoded;
        // pour chaque Bits::Block du vector
        for (auto const &value: v_encoded) {
            // chercher sa clé dans la map
            // cout << getKeyInMap(value);
            data_decoded.push_back((getKeyInMap(value)));
            v_decoded.push_back(getKeyInMap(value));
        }
        return v_decoded;
    }

    // Support
    /* verifier si il existe des données codées de même taille
     * renvoie -1 = non, 1 = oui
     */
    int check_double_coded_value() {
        int i = 0;
        int haveDouble = -1; // -1 = non, 1 = oui

        for (auto &list_symbol : codage_table) {
            i = 0;

            for (auto &list_symbol2 : codage_table) {
                if (list_symbol.second == list_symbol2.second) {
                    i++;
                }
            }
            //cout << "i value= " << i <<endl;
            if (i > 1) {
                cout << "on break" << endl;
                haveDouble = 1;
                break;
            }
        }
        return haveDouble;
    }
    /* renvoie la valeur de la map correspondant à la clé  passée en paramètre
     */
    Bits::Block<8> getValueInMap(const std::string &key) {
        for (auto &it : codage_table)
            if (it.first == key)
                return it.second;
    }
    /* renvoie la clé de la map correspondant à la value passée en paramètre
     */
    std::string getKeyInMap(Bits::Block<8> value) {
        for (auto &it : codage_table)
            if (it.second == value)
                return it.first;
    }
    /* renvoie la taille du fichier
     */
    int getFileSize(const std::string &file_name) {
        std::ifstream in(file_name+".txt", std::ifstream::ate | std::ifstream::binary);
        return (int) in.tellg();
    }
    /* creer un fichier avec les en-têtes necessaires
     *  taille du fichier
     *  un magic number pour identifier le type de compression
     *  dans le cas du CTF, par exemple, le nombre de symboles à lire,
     *  la taille de la table des symboles/codes,
     *  puis la table des symboles/des codes
     */
    void create_and_format_file(int algo) {
        ofstream fichier(nom_file_svg, ios::out | ios::trunc);  // ouverture en écriture avec effacement du fichier ouvert

        if(fichier) {
            switch(algo) {
                case 0: { // CTF
                    //cout << "CTF format called" << endl;
                    std::string header =
                            "<?xml version=\"1.0\" encoding=\"UTF-16\"? >\n"
                            "<root"
                            " size_file=\"" + std::to_string(getFileSize(nom_file+".txt"))+"\"" +
                            " size_list_symbole=\"" + std::to_string(symbol_list.size())+"\"" +
                            " size_table_codage=\"" + std::to_string(codage_table.size())+"\"" +
                            " table_codage=\"" + codage_table_to_str()+"\"" +
                            ">"
                            "\n<data>";
                    fichier << header;
                    break;
                }
                default: {
                    cout << "algo non reconnu";
                    break;
                }
            }
            fichier.close();
        }
        else {
            cerr << "Impossible d'ouvrir le fichier !" << endl;
        }

    }
    /* fonction permettant de verifier si les données avant et après décodage sont identiques
     */
    std::string check_is_data_equal() {
        cout << "# check_is_data_equal started" << std::endl;
        cout << " > data_to_encode size= " << data_to_encode.size() << endl;
        cout << " > data_decoded size= " << data_decoded.size() << endl;

        return ((data_to_encode == data_decoded)?" > donnees original et donnees decompressees identiques":"\t> donnees original et donnees decompressees differentes");
    }
    /* afficher la table de codage

    void display_codage_table() {
        cout << "\n\n# Table de codage = \n";
        for (auto &list_symbol : codage_table) {
            cout << "symbole: |" << list_symbol.first << "| a pour codage en bits " << list_symbol.second << endl;
        }
    }*/
    /* afficher la liste des symboles
     */
    void display_symbol_list() {
        cout << "# Liste des symboles = " << endl;
        for (auto const &value: symbol_list) {
            cout << "|" << value << "|\t";
        }
    }
    /* convertit la table de codage en string
     */
    std::string codage_table_to_str() {
        std::string str ;
        for (auto &list_symbol : codage_table) {
            str += " symbole: |"+list_symbol.first+"| codage : "+std::to_string(list_symbol.second.get());
        }
        return str;
    }
    /* extrait un string entre 2 délimiteurs de type string
     * */
    std::string extract_str_btwn_2_str(const std::string &s, const std::string &begin, const std::string &end) {
        auto first_delim_pos = static_cast<unsigned int>(s.find(begin));
        auto end_pos_of_first_delim = static_cast<unsigned int>(first_delim_pos + begin.length());
        auto last_delim_pos = static_cast<unsigned int>(s.find(end));

        return s.substr(end_pos_of_first_delim, last_delim_pos - end_pos_of_first_delim);
    }
    /* affiche le contenu d'un fichier

    void display_file(const string &file) {
        // ouvrir le fichier en lecture
        ifstream ifs(file, ios::in);
        if (ifs) {
            //cout << "fichier ouvert" << endl;


            // lire char par char
            char c;
            while (ifs.get(c)) {
                cout << c;
            }
            ifs.close();
        } else {
            cerr << "Erreur à l'ouverture !" << endl;
        }
    }*/

};


#endif //TP1_CTF_H
