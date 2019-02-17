#ifndef TP4_DICO_STATIQUE_H
#define TP4_DICO_STATIQUE_H

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "../BitStream-1.2.12/BitStream.h"

using namespace std;

class CLZ78 {

    // attributes
    private:
    const string nom_file;
    string string_source;                       // string des données à compréssés
    int size_source;
    int size_coded;
    int size_won;
    int size_expanded;
    int variante;                               // variante de LZ78
    float taux_compression;
    float facteur_compression;
    float taux_expansion;
    float pourcentage_reduction;
    Bits::Stream stream_encoded;                  // stream contenant les données encodés
    vector<Bits::varBlock> data_encoded;        // vector contenant les données encodés
    string data_readed_and_decoded;             // string des données décodés du fichier svg
    vector<string> list_symbole;                // list de tous les symboles uniques
    vector<string> list_xgramme;                // list de tous les digrammes uniques
    map<string, int> map_freq_symbole;          // map des symboles et de leur frequence
    vector<string> dico;                        // list de tous les symboles et xgramme
    map<string, Bits::varBlock> codage_table;   // map des digrammes et de leur codage binaire

    // constructors
    public:
    explicit CLZ78(const std::string &file, int variante, int want_io) : nom_file(file+".txt"), variante(variante)  {

        filled_source_file();
        cout << "\n\n###### LZ78 variante " << variante << endl;
        cout << "source de donnee" /*<< getString_source() << */" stockee de taille " << getString_source().size() << endl;

        fill_list_symbole();
        /*cout << "\n# list de symbole generee " << endl;
        cout << "\n affichage du vector list_symbole" << endl;
        display_vector(getList_symbole());*/

        /*cout << "\n affichage des xgramme les moins frequent " << endl;
        display_vector(getXGrammeFrequent());*/

        switch(variante) {
            case 1:
                compute_list_xgramme_and_freq_xgramme();
                break;
            case 2:
                compute_list_xgramme_and_freq_xgramme(256);
                break;
            case 3:
                compute_list_xgramme_and_freq_xgramme_and_flush(256);
                break;
            default:
                break;
        }
        cout << "\n# list_xgramme & map frequence xgramme generees, variante " << variante << endl;

        /*cout << "\n# affichage du vector list_Xgramme" << endl;
        display_vector(getList_Xgramme());*/

        /* map des digrammes et de leur fréquence
        cout << "\n\n# affichage de la map de digramme et de leur frequence" << endl;
        display_map(getMap_freq_xgramme());*/

        /* création du dico */
        fill_dico(256);
        // cout << "\n# dico de caractere et xgramme genere " << endl;
        /* affichage du dico
        cout << "\n # affichage du dico " << endl;
        display_vector(getDico());*/

        /* création de la table de codage */
        generate_codage_table();
        // cout << "\n# table de codage genere " << endl;
        /*cout << "\n# affichage de la map codage table" << endl;
        display_map(getCodage_table());*/
        cout << "# codage table a t elle des codes de symboles en double ? " << check_double_coded_value() << endl;

        /* encodage binaire */
        cout << "\n# encodage de la source, taille apres compression= " << encode() << endl;

        /* decodage binaire*/
        cout << "\n# decodage du stream codee, taille apres decompression(taux d'expansion?)= " <<  decode() << endl;

        taux_compression = (float) getSize_coded()/getString_source().size();
        taux_expansion = 0;
        facteur_compression = (float) getString_source().size()/getSize_coded();
        pourcentage_reduction = getString_source().size() - getSize_coded();
        pourcentage_reduction /= getString_source().size();

        cout << "# statistiques LZ78 variante " << variante << endl;
        cout << "# taux de compression " << taux_compression  << endl;
        cout << "# taux d'expansion " << taux_expansion << endl;
        cout << "# facteur de compresion " << facteur_compression << endl;
        cout << "# pourcentage reduction " << pourcentage_reduction*100 << "%" << endl;

        if(want_io == 0) { // 0:oui et -1: non
            // Ecriture de la compression dans un fichier
            backup_compressed_data(data_encoded);
            // Lecture de la compression dans un fichier
            read_compressed_data();
        }
    }
    CLZ78(const CLZ78 &c) : nom_file(c.nom_file) {}
    CLZ78(CLZ78 &&_c) : nom_file(_c.nom_file) {}
    ~CLZ78() = default;

    // getters & setters
    const string &getNom_file() const {
        return nom_file;
    }
    const string &getString_source() const {
        return string_source;
    }
    void setString_source(const string &string_source) {
        CLZ78::string_source = string_source;
    }
    float getTaux_compression() const {
        return taux_compression;
    }
    void setTaux_compression(float taux_compression) {
        CLZ78::taux_compression = taux_compression;
    }
    float getFacteur_compression() const {
        return facteur_compression;
    }
    void setFacteur_compression(float facteur_compression) {
        CLZ78::facteur_compression = facteur_compression;
    }
    float getTaux_expansion() const {
        return taux_expansion;
    }
    void setTaux_expansion(float taux_expansion) {
        CLZ78::taux_expansion = taux_expansion;
    }
    float getPourcentage_reduction() const {
        return pourcentage_reduction;
    }
    void setPourcentage_reduction(float pourcentage_reduction) {
        CLZ78::pourcentage_reduction = pourcentage_reduction;
    }
    const vector<string> &getList_symbole() const {
        return list_symbole;
    }
    void setList_symbole(const vector<string> &list_symbole) {
        CLZ78::list_symbole = list_symbole;
    }
    const vector<string> &getList_xgramme() const {
        return list_xgramme;
    }
    void setList_xgramme(const vector<string> &list_xgramme) {
        CLZ78::list_xgramme = list_xgramme;
    }
    const map<string, int> &getMap_freq_symbole() const {
        return map_freq_symbole;
    }
    void setMap_freq_symbole(const map<string, int> &map_freq_symbole) {
        CLZ78::map_freq_symbole = map_freq_symbole;
    }
    const vector<string> &getList_Xgramme() const {
        return list_xgramme;
    }
    void setList_Xgramme(const vector<string> &list_Xgramme) {
        CLZ78::list_xgramme = list_Xgramme;
    }
    const map<string, int> &getMap_freq_xgramme() const {
        return map_freq_symbole;
    }
    void setMap_freq_xgramme(const map<string, int> &map_freq_xgramme) {
        CLZ78::map_freq_symbole = map_freq_xgramme;
    }
    const map<string, Bits::varBlock> &getCodage_table() const {
        return codage_table;
    }
    void setCodage_table(const map<string, Bits::varBlock> &codage_table) {
        CLZ78::codage_table = codage_table;
    }
    const Bits::Stream &getStream_coded() const {
        return stream_encoded;
    }
    void setStream_coded(const Bits::Stream &stream_coded) {
        CLZ78::stream_encoded = stream_coded;
    }
    int getSize_source() const {
        return size_source;
    }
    void setSize_source(int size_source) {
        CLZ78::size_source = size_source;
    }
    int getSize_coded() const {
        return size_coded;
    }
    void setSize_coded(int size_coded) {
        CLZ78::size_coded = size_coded;
    }
    int getSize_won() const {
        return size_won;
    }
    void setSize_won(int size_won) {
        CLZ78::size_won = size_won;
    }
    int getSize_expanded() const {
        return size_expanded;
    }
    void setSize_expanded(int size_expanded) {
        CLZ78::size_expanded = size_expanded;
    }
    const vector<string> &getDico() const {
        return dico;
    }
    void setDico(const vector<string> &dico) {
        CLZ78::dico = dico;
    }
    int getVariante() const {
        return variante;
    }
    void setVariante(int variante) {
        CLZ78::variante = variante;
    }

    // fonctions et méhtodes

    /* INIT */
    /*
     * permet de sotcker les données du ficheir dans un string
     */
    void filled_source_file() {
        // ouvrir le fichier en lecture
        ifstream ifs(nom_file, ios::in);
        if (ifs) {
            /* par caractère */
            char c;
            while (ifs.get(c)) {
                string_source.append(string(1, c));
            }

            /* par ligne
            string ligne;
            while(getline(ifs, ligne)) {
                string_source += ligne;
            } */

            size_source = (int) string_source.size();

            ifs.close();
        } else {
            cerr << "Erreur à l'ouverture !" << endl;
        }
    }
    /*
     * permet de remplir le vector list_xgramme avec tous les caractères uniques
     */
    void fill_list_symbole() {
        for(int i=0;i<string_source.size();i++) {
            if(i==0) {
                // on insère le premiet caractère dans le vector de symbole
                list_symbole.emplace_back(string(1, string_source[i]));
            }
            else {
                if (!(find(list_symbole.begin(), list_symbole.end(), string(1, string_source[i])) != list_symbole.end())) { // si symbole non présent dans le vector list_xgramme
                    list_symbole.emplace_back(string(1, string_source[i])); // on l'ajoute
                }
            }
        }
    }
    /*
     * Fonction permettant de remplir le vector list_xgramme avec tous les Xgrammes uniques
     * permet de remplir la map_freq_xgramme avec les fréquences dex xgramme
     * pas de taille maximale
     */
    void compute_list_xgramme_and_freq_xgramme() {
        int g = 0;
        int h = 0;
        for(int i=0;i<string_source.size();i++) {
                int j = 1;
                if(i+j != string_source.size()) { // on n'ajoute pas le dernier caractère
                    string s = string(1, string_source[i]);
                    s.push_back(string_source[i + j]);
                    j++;
                    if (!(find(list_xgramme.begin(), list_xgramme.end(), s) != list_xgramme.end())) { // si le symbole n'est pas présent dans la liste des symboles
                        list_xgramme.emplace_back(s);
                        map_freq_symbole.emplace(s, 1);
                        //cout << "on ajoute la chaine  |" << s << "|" << endl;
                        i++; // on passe au caractère suivant
                    }
                    else {
                        map_freq_symbole[s] = map_freq_symbole[s] + 1;
                        while ((find(list_xgramme.begin(), list_xgramme.end(), s) != list_xgramme.end())) { // tant que le symbole est présent dans la liste

                            if (i + j != string_source.size()) {
                                // on construit le xgramme avec en rajoutant un caractère
                                s.push_back(string_source[i + j]);
                                i++;
                                j++;
                            } else {
                                break;
                            }
                        }
                        if (!(find(list_xgramme.begin(), list_xgramme.end(), s) != list_xgramme.end())) { // si le symbole n'est pas présent dans la liste des symboles
                            //cout << "on ajoute la chaine  " << s << endl;
                            list_xgramme.emplace_back(s); // on l'ajoute à la liste
                            map_freq_symbole.emplace(s, 1);
                        }
                    }
                }
        }
        cout << "taille list_xgramme " << list_xgramme.size() << endl;
        cout << "taille map " << map_freq_symbole.size() << endl;
    }
    /*
     * Fonction permettant de remplir le vector list_xgramme avec tous les Xgrammes uniques
     * permet de remplir la map_freq_xgramme avec les fréquences dex xgramme
     * taille maximale et utilisation comme dico statique
     */
    void compute_list_xgramme_and_freq_xgramme(int taille_max) {
        for(int i=0;i<string_source.size();i++) {
            int j = 1;
            if(i+j != string_source.size()) { // on n'ajoute pas le dernier caractère
                string s = string(1, string_source[i]);
                s.push_back(string_source[i + j]);
                j++;
                if (!(find(list_xgramme.begin(), list_xgramme.end(), s) !=
                      list_xgramme.end())) { // si le symbole n'est pas présent dans la liste des symboles
                    // on vérifie si la taille max n'est pas dépassé
                    if (list_xgramme.size() != taille_max) {
                        list_xgramme.emplace_back(s);
                        map_freq_symbole.emplace(s, 1);
                        i++; // on passe au caractère suivant
                    }
                } else {
                    map_freq_symbole[s] = map_freq_symbole[s] + 1;
                    while ((find(list_xgramme.begin(), list_xgramme.end(), s) !=
                            list_xgramme.end())) { // tant que le symbole est présent dans la liste

                        if (i + j != string_source.size()) {
                            // on construit le xgramme avec en rajoutant un caractère
                            s.push_back(string_source[i + j]);
                            i++;
                            j++;
                        } else {
                            break;
                        }
                    }
                    //cout << "on ajoute la chaine  " << s << endl;
                    // on vérifie si la taille max n'est pas dépassé
                    if (list_xgramme.size() != taille_max) {
                        if (!(find(list_xgramme.begin(), list_xgramme.end(), s) != list_xgramme.end())) { // si le symbole n'est pas présent dans la liste des symboles
                            list_xgramme.emplace_back(s); // on l'ajoute à la liste
                            map_freq_symbole.emplace(s, 1);
                        }
                    }
                }
            }
        }
        cout << "taille list_xgramme " << list_xgramme.size() << endl;
        cout << "taille map " << map_freq_symbole.size() << endl;
    }
    /*
     * Fonction permettant de remplir le vector list_xgramme avec tous les Xgrammes uniques
     * permet de remplir la map_freq_xgramme avec les fréquences dex xgramme
     * taille maximale et flush des 16 symboles les moins fréquents
     */
    void compute_list_xgramme_and_freq_xgramme_and_flush(int taille_max) {
        for(int i=0;i<string_source.size();i++) {
            int j = 1;
            if(i+j != string_source.size()) { // on n'ajoute pas le dernier caractère
                string s = string(1, string_source[i]);
                s.push_back(string_source[i + j]);
                j++;
                if (!(find(list_xgramme.begin(), list_xgramme.end(), s) != list_xgramme.end())) { // si le symbole n'est pas présent dans la liste des symboles
                    // on vérifie si la taille max n'est pas dépassé
                    if (list_xgramme.size() != taille_max) {
                        list_xgramme.emplace_back(s);
                        map_freq_symbole.emplace(s, 1);
                        i++; // on passe au caractère suivant
                    }
                    else {
                        delete_moins_frequent();
                    }
                } else {
                    map_freq_symbole[s] = map_freq_symbole[s] + 1;
                    while ((find(list_xgramme.begin(), list_xgramme.end(), s) != list_xgramme.end())) { // tant que le symbole est présent dans la liste

                        if (i + j != string_source.size()) {
                            // on construit le xgramme avec en rajoutant un caractère
                            s.push_back(string_source[i + j]);
                            i++;
                            j++;
                        } else {
                            break;
                        }
                    }
                    //cout << "on ajoute la chaine  " << s << endl;
                    // on vérifie si la taille max n'est pas dépassé
                    if (list_xgramme.size() != taille_max) {
                        if(!(find(list_xgramme.begin(), list_xgramme.end(), s) != list_xgramme.end())) {
                            list_xgramme.emplace_back(s); // on l'ajoute à la liste
                        }
                        map_freq_symbole.emplace(s, 1);
                    } else {
                        // on détermine les 16 symboles les moins fréquentes
                        delete_moins_frequent();
                    }
                }
            }
        }
        cout << "taille list_xgramme " << list_xgramme.size() << endl;
        cout << "taille map " << map_freq_symbole.size() << endl;
    }
    /*
     * génère le vector dico qui contient l'ensemble des caractères et des xgramme uniques
     * taille maximale
     */
    void fill_dico(int taille_max) {
        // on ajoute tous les symboles au dictionnaire
        for (auto const &value: list_xgramme) {
            dico.push_back(value);
        }
        for (auto const &value: list_symbole) {
            dico.push_back(value);
        }
    }

    /* CODAGE */
    /* creer la table de codage
     * association digramme -> codage
     * association caractère -> codage
     */
    void generate_codage_table() {
        int j = 0;
        for (auto const &value: dico) { // pour chaque symbole du dictionnaire
            Bits::varBlock vblock(6, j);
            codage_table.emplace(dico[j], vblock);
            j++;
        }
    }
    /*
     * encode le source string de la class
     * fonction de codage binaire par dictionnaire statique
     */
    int encode() {
        // on itère le string_source pour le code
        for(int i=0;i<string_source.size();i++) {
            // on vérifie si le dico contient le digramme du 1er et du 2eme caractère
            string s;
            s.push_back(string_source[i]);
            s.push_back(string_source[i+1]);

            if(check_if_key_exist(codage_table, s)) { // s'il le contient
                stream_encoded << getValueInMap(codage_table, s);
                data_encoded.push_back(getValueInMap(codage_table, s));
                //cout << "on encode le digramme " << s << endl;
                i++;
            }
            else { // sinon
                stream_encoded << getValueInMap(codage_table, string(1, string_source[i]));
                //cout << "on encode le caractere " << string(1, string_source[i]) << endl;
            }
        }
        size_coded = stream_encoded.get_byte_size();
        return stream_encoded.get_byte_size();
    }
    /* decode la table de codage ()
     * convertit chaque code binaire en son équivalent string
     */
    int decode() {
        Bits::varBlock vblock;
        vblock.set_valid(6);
        cout << "# string decode = | UNCOMMENT TO SEE";
        while(!stream_encoded.end_of_stream()) {
            stream_encoded >> vblock;
            /* UNCOMMENT TO SEE
            * std::cout << getKeyInMap(codage_table, vblock);
            */
        }
        cout << "|";
        size_won = size_source - size_coded;
        return size_won;
    }

    /* MAP */
    /* renvoie la valeur de la map correspondant à la clé  passée en paramètre
     */
    int getValueInMap(std::map<std::string, int> map, string key) {
        for (auto &it : map)
            if (it.first == key)
                return it.second;
    }
    /* renvoie la valeur de la map correspondant à la clé  passée en paramètre
     */
    Bits::varBlock getValueInMap(std::map<std::string, Bits::varBlock> map, string key) {
        for (auto &it : map)
            if (it.first == key)
                return it.second;
    }
    /* renvoie la clé de la map correspondant à la value passée en paramètre
    */
    std::string getKeyInMap(std::map<std::string, int> map, int value) {
        for (auto &it : map)
            if (it.second == value)
                return it.first;
    }
    /* renvoie la clé de la map correspondant à la value passée en paramètre
    */
    std::string getKeyInMap(std::map<std::string, Bits::varBlock> map, Bits::varBlock value) {
        for (auto &it : map)
            if (it.second == value)
                return it.first;
    }
    /*
    * verifie si une clé existe dans la map
    */
    bool check_if_key_exist(std::map<std::string, int> map, std::string key) {
        bool exist = false;
        for (auto &it : map) {
            if (it.first == key) {
                exist = true;
            }
        }
        return exist;
    }
    /*
    * verifie si une clé existe dans la map
    */
    bool check_if_key_exist(std::map<std::string, Bits::varBlock> map, std::string key) {
        bool exist = false;
        for (auto &it : map) {
            if (it.first == key) {
                exist = true;
            }
        }
        return exist;
    }
    /*
     * affiche la map
     * */
    void display_map(std::map<std::string, int> map_p){
        for (auto &map : map_p) {
            cout << map.first << " value: " << map.second << endl;
        }
    }
    /*
     * affiche le vector
     * */
    void display_vector(vector<string> v){
        for (auto const &value: v) {
            cout << value << endl;
        }
    }
    /*
     * affiche la table de codage
     * */
    void display_map(std::map<std::string, Bits::varBlock> map_p){
        for (auto &map : map_p) {
            cout << "key: " << map.first << " value: " << map.second << endl;
        }
    }
    /* verifier si il existe des données codées de même taille
     * renvoie -1 = non, 1 = oui
     */
    int check_double_coded_value() {
        int i = 0;
        int haveDouble = -1; // -1 = non, 1 = oui

        for (auto &ct : codage_table) {
            i = 0;

            for (auto &ct2 : codage_table) {
                if (ct.second == ct2.second) {
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
    /*
     * permet de renvoyer les 16 xgramme les moins fréquents
     */
    vector<string> getXGrammeFrequent() {
        vector<string> symbole_frequent;
        int i = 1;
        while(symbole_frequent.size() < 16) {
            for (auto &map : map_freq_symbole) {
                if(map.second == i && symbole_frequent.size() < 16) {
                    symbole_frequent.push_back(map.first);
                    //cout << "symbole " << map.first  << " avec frequence de " << map.second << " est retenu pour suppression " << endl;
                }
            }
            i++;
            //cout << "TAILLE DU VECTOR DES + FREQUENTS = " << symbole_frequent.size() << endl;
        }
        return symbole_frequent;
    }
    /*
     * supprime un élément d'un vector par sa clé
     */
    void delete_element_from_vector(string key) {
        for(int i=0;i<list_xgramme.size();i++){
            if(key == list_xgramme[i]) {
                if ( i < list_xgramme.size() ) {
                    //cout << "supppression vector de la cle " << list_xgramme[i] << endl;
                    list_xgramme.erase(list_xgramme.begin()+i);
                }
                else {
                    cout << "index non valide " << endl;
                }
            }
        }
    }
    /*
     * supprime un élément d'une map par sa clé
     */
    void delete_element_from_map(string key) {
        // on supprime les éléments ayant une fréquence inférieure au paramètre
        for(map<string, int>::iterator it = map_freq_symbole.begin(); it != map_freq_symbole.end();) {
            if(it->first < key) {
                //cout << "supppression map de la cle " << it->first << endl;
                it = map_freq_symbole.erase(it);
            }
            else {
                it++;
            }
        }
    }
    /*
     * supprime les éléments les moins fréquent de la list_xgramme et de la map_freq_symbole
     */
    void delete_moins_frequent() {
        vector<string> xgramme_frequent;
        // on détermine les 16 symboles les moins fréquentes
        xgramme_frequent = getXGrammeFrequent();
        for (int k = 0; k < xgramme_frequent.size(); k++) { // pour chaque symbole frequent
            // on le cherche dans la list_xgramme et on le suprpime
            delete_element_from_vector(xgramme_frequent[k]);
            // on le cherche dans la map_freq_xgramme et on le suprpime
            delete_element_from_map(xgramme_frequent[k]);
        }
    }
    /* convertit la table de codage en string
     */
    std::string codage_table_to_str() {
        std::string str ;
        for (auto &list_symbol : codage_table) {
            str += " symbole |"+list_symbol.first+"| codage |"+std::to_string(list_symbol.second.get())+"|";
        }
        return str;
    }
    /* IO */
    /* renvoie la taille du fichier
     */
    int getFileSize(const std::string &file_name) {
        std::ifstream in(file_name+".txt", std::ifstream::ate | std::ifstream::binary);
        return (int) in.tellg();
    }
    /* creer un fichier avec les en-têtes necessaires
     *  taille du fichier
     *  un magic number pour identifier le type de compression
     *  le nombre de symboles à lire,
     *  la taille de la table des symboles/codes,
     *  puis la table des symboles/des codes
     */
    void create_and_format_file() {
        ofstream fichier(nom_file+"_variante"+to_string(variante), ios::out | ios::trunc);  // ouverture en écriture avec effacement du fichier ouvert

        if(fichier) {
            std::string header =
                    "<?xml version=\"1.0\" encoding=\"UTF-16\"? >\n"
                    "<root"
                    " size_file=\"" + std::to_string(getFileSize(nom_file+".txt"))+"\"" +
                    " size_list_symbole=\"" + std::to_string(list_symbole.size())+"\"" +
                    " size_list_xgramme=\"" + std::to_string(list_xgramme.size())+"\"" +
                    " size_map_freq_symbole=\"" + std::to_string(map_freq_symbole.size())+"\"" +
                    " size_dico=\"" + std::to_string(dico.size())+"\"" +
                    " size_table_codage=\"" + std::to_string(codage_table.size())+"\"" +
                    " taux_compression=\"" + std::to_string(taux_compression)+"\"" +
                    " taux_expansion=\"" + std::to_string(taux_expansion)+"\"" +
                    " facteur_compression=\"" + std::to_string(facteur_compression)+"\"" +
                    " pourcentage_reduction=\"" + std::to_string(pourcentage_reduction)+"\"" +
                    " table_codage=\"" + codage_table_to_str()+"\"" +
                    ">"
                    "\n<data>";
            fichier << header;
            fichier.close();
        }
        else {
            cerr << "Impossible d'ouvrir le fichier !" << endl;
        }

    }
    /*
     * fonction permettant de sauvegarder les données compressés dans un fichier
     */
    void backup_compressed_data(vector<Bits::varBlock> v_encoded) {
        cout << "# backing compressed data" << endl;
        create_and_format_file();
        cout << " > file \""<< nom_file+"_variante"+to_string(variante) << "\" created" << std::endl;
        ofstream fichier(nom_file+"_variante"+to_string(variante), std::ios::app);  // ouverture en écriture avec effacement du fichier ouvert

        if(fichier) {
            // on se place à la fin du fichier
            // pour chaque Bits::varBlock  de v_encoded
            for (auto const &value: v_encoded) {
                // on l'enregistre dans le fichier
                fichier << Bits::Binary(value.get()); // A REVOIR !!!! marche avec Bits::Block mais pas avec varBlock ::( mais compile avec .get()
            }
            fichier << "\n</data>\n</root>";
            fichier.close();

        }
        else {
            cerr << "backup_compressed_data() Impossible d'ouvrir le fichier !" << endl;
        }
    }
    /*
     * fonction permettant de lire les données compressés
     */
    vector<Bits::varBlock> read_compressed_data() {
        cout << "# reading compressed data" << endl;
        // on ouvre le fichier compréssé
        ifstream fichier(nom_file+"_variante"+to_string(variante), ios::in);  // on ouvre en lecture
        vector<Bits::varBlock> vb;

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
            Bits::varBlock block;
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
                cout << getKeyInMap(codage_table, value); //UNCOMMENT TO SEE DATA
                data_readed_and_decoded.append(getKeyInMap(codage_table, value));
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
    /* extrait un string entre 2 délimiteurs de type string
     * */
    std::string extract_str_btwn_2_str(const std::string &s, const std::string &begin, const std::string &end) {
        auto first_delim_pos = static_cast<unsigned int>(s.find(begin));
        auto end_pos_of_first_delim = static_cast<unsigned int>(first_delim_pos + begin.length());
        auto last_delim_pos = static_cast<unsigned int>(s.find(end));

        return s.substr(end_pos_of_first_delim, last_delim_pos - end_pos_of_first_delim);
    }
    /* fonction permettant de verifier si les données avant et après décodage sont identiques
     */
    std::string check_is_data_equal() {
        cout << "# check_is_data_equal started" << std::endl;
        cout << " > data_to_encode size= " << string_source.size() << endl;
        cout << " > data_decoded size= " << data_readed_and_decoded.size() << endl;

        return ((string_source == data_readed_and_decoded)?" > donnees original et donnees decompressees identiques":"\t> donnees original et donnees decompressees differentes");
    }
};


#endif //TP4_DICO_STATIQUE_H
