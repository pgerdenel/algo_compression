#ifndef TP1_CTF_H
#define TP1_CTF_H

#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "../BitStream-1.2.12/BitStream.h"

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


class CTF {

    // attributes
    private:
    const std::string nom_file;
    long long int taille_fichier;
    const string nom_file_svg;
    string string_source;
    int size_source;
    int size_encoded;
    float taux_compression;
    float taux_expansion;
    float facteur_compression;
    float pourcentage_reduction;
    vector<string> symbol_list;
    map<string, Bits::varBlock> codage_table;
    Bits::Stream stream_encoded;
    string data_decoded;

    // constructors
    public:
    explicit CTF(const std::string &file) : nom_file(file+".txt"), nom_file_svg(file+ "_compressed_and_encoded.txt")  {
        fil_string_source();
        cout << "# source de donnee de taille " << getString_source().size() << endl;

        //cout << "\n# symbol list generated" << endl;
        generate_symbol_list();

        //cout << "\n\n# codage table generated" << endl;
        generate_codage_table();

        cout << "# encodage de la source, taille apres compression= " << encode() << endl;

        cout << "\n# decodage du stream encoded, taille apres decompression(taux d'expansion?)= " << decode() << endl;

        taux_compression = (float) getSize_encoded()/getString_source().size();
        taux_expansion = 0;
        facteur_compression = (float) getString_source().size()/getSize_encoded();
        pourcentage_reduction = getString_source().size() - getSize_encoded();
        pourcentage_reduction /= getString_source().size();

        cout << "# taux de compression " << taux_compression  << endl;
        cout << "# taux d'expansion " << taux_expansion << endl;
        cout << "# facteur de compresion " << facteur_compression << endl;
        cout << "# pourcentage reduction " << pourcentage_reduction*100 << "%" << endl;

    }
    CTF(const CTF &c) : nom_file(c.nom_file) {}
    CTF(CTF &&_c) : nom_file(_c.nom_file) {}
    ~CTF() = default;

    // Getters & Setters
    const string &getNom_file() const {
        return nom_file;
    }
    const string &getNom_file_svg() const {
        return nom_file_svg;
    }
    const string &getString_source() const {
        return string_source;
    }
    void setString_source(const string &string_source) {
        CTF::string_source = string_source;
    }
    int getSize_source() const {
        return size_source;
    }
    void setSize_source(int size_source) {
        CTF::size_source = size_source;
    }
    int getSize_encoded() const {
        return size_encoded;
    }
    void setSize_encoded(int size_encoded) {
        CTF::size_encoded = size_encoded;
    }
    const vector<string> &getSymbol_list() const {
        return symbol_list;
    }
    void setSymbol_list(const vector<string> &symbol_list) {
        CTF::symbol_list = symbol_list;
    }
    const map<string, Bits::varBlock> &getCodage_table() const {
        return codage_table;
    }
    void setCodage_table(const map<string, Bits::varBlock> &codage_table) {
        CTF::codage_table = codage_table;
    }
    const Bits::Stream &getStream_encoded() const {
        return stream_encoded;
    }
    void setStream_encoded(const Bits::Stream &stream_encoded) {
        CTF::stream_encoded = stream_encoded;
    }
    const string &getData_decoded() const {
        return data_decoded;
    }
    void setData_decoded(const string &data_decoded) {
        CTF::data_decoded = data_decoded;
    }

    // Methods et Fonctions
    /* Stocke les données du fichier dans un string */
    void fil_string_source() {
        // ouvrir le fichier en lecture
        ifstream ifs(nom_file, ios::in);
        if (ifs) {
            taille_fichier = getFileSize(nom_file+".txt");
            /* par caractère */
            char c;
            while (ifs.get(c)) {
                string_source.append(string(1, c));
                size_source++;
            }

            /* par ligne
            string ligne;
            while(getline(ifs, ligne)) {
                string_source += ligne;
            }*/
            size_source = (int) string_source.size();
            ifs.close();
        } else {
            cerr << "Erreur à l'ouverture !" << endl;
        }
    }
    /* lire les données dans un fichier
     * construire la table des caractères différents dans ce fichier
     * (alphabet == liste des symboles == table des caractères)
     */
    void generate_symbol_list() {
        for(int i =0;i<string_source.size();i++) {
            // vérifier si char non présent dans le vector
            if (!(find(symbol_list.begin(), symbol_list.end(), string(1, string_source[i])) != symbol_list.end())) {
                // si non présent le stocker
                symbol_list.emplace_back(string(1, string_source[i]));
            }
        }
    }
    /* creer la table de codage (association alphabet/codage)
     */
    void generate_codage_table() {
        int j = 0;
        for (auto const &value: symbol_list) { // pour chaque symbole du dictionnaire
            Bits::varBlock vblock(6, j);
            codage_table.emplace(symbol_list[j], vblock);
            j++;
        }
    }
    /*
     * Encode les données du fichier et les stocke dans le vector stream_encoded
     * retourne le taux de compression
     */
    int encode() {
        vector<Bits::varBlock> v;
        for(int i =0;i<string_source.size();i++) {
            stream_encoded << getValueInMap(std::string(1, string_source[i]));
        }
        size_encoded = stream_encoded.get_byte_size();
        return stream_encoded.get_byte_size();
    }
    /*
     * décode les données du fichier où les données compréssés ont été stockés
     * retourne le taux d'expansion
     */
    int decode() {
        Bits::varBlock varBlock;
        varBlock.set_valid(6);
        cout << "# string decode = | UNCOMMENT TO SEE";
        while(!stream_encoded.end_of_stream()) {
            stream_encoded >> varBlock;
            /* UNCOMMENT TO SEE
             * std::cout << getKeyInMap(varBlock);*/
            data_decoded.append(getKeyInMap(varBlock));
        }
        cout << "|";
        return size_source - size_encoded;
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
    Bits::varBlock getValueInMap(const std::string &key) {
        for (auto &it : codage_table)
            if (it.first == key)
                return it.second;
    }
    /* renvoie la clé de la map correspondant à la value passée en paramètre
     */
    std::string getKeyInMap(Bits::varBlock value) {
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
        cout << " > data_to_encode size= " << string_source.size() << endl;
        cout << " > data_decoded size= " << data_decoded.size() << endl;

        return ((string_source == data_decoded)?" > donnees original et donnees decompressees identiques":"\t> donnees original et donnees decompressees differentes");
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
    /* affiche la table de codage */
    void display_map(std::map<std::string, Bits::varBlock> map_p){
        for (auto &map : map_p) {
            cout << "key: " << map.first << " value: " << map.second << endl;
        }
    }
    /* extrait un string entre 2 délimiteurs de type string
     * */
    std::string extract_str_btwn_2_str(const std::string &s, const std::string &begin, const std::string &end) {
        auto first_delim_pos = static_cast<unsigned int>(s.find(begin));
        auto end_pos_of_first_delim = static_cast<unsigned int>(first_delim_pos + begin.length());
        auto last_delim_pos = static_cast<unsigned int>(s.find(end));

        return s.substr(end_pos_of_first_delim, last_delim_pos - end_pos_of_first_delim);
    }
    /* calcule la tailled d'un fichier */
    long GetFileSize(const string fichier) {
        ifstream file(fichier);
        // sauvegarder la position courante
        long pos = file.tellg();
        // se placer en fin de fichier
        file.seekg( 0 , std::ios_base::end );
        // récupérer la nouvelle position = la taille du fichier
        long size = file.tellg() ;
        // restaurer la position initiale du fichier
        file.seekg( pos,  std::ios_base::beg ) ;
        file.close();
        return size ;
    }

};


#endif //TP1_CTF_H
