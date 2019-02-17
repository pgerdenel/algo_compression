#ifndef TP4_DICO_STATIQUE_H
#define TP4_DICO_STATIQUE_H

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "..\BitStream-1.2.12/BitStream.h"

using namespace std;

class Digramme {

    // attributes
    private:
    const string nom_file;
    string string_source;
    int size_source;
    int size_coded;
    int size_won;
    int size_expanded;
    float taux_compression;
    float facteur_compression;
    float taux_expansion;
    float pourcentage_reduction;
    Bits::Stream stream_coded;
    vector<string> list_symbole;    // list de tous les caractères uniques
    vector<string> list_digramme;   // list de tous les digrammes (y compris ceux en double)
    vector<string> dico; // list des digrammes uniques
    map<string, int> map_freq_digramme; // map des digrammes et de leur frequence
    map<string, Bits::varBlock> codage_table; // map des digrammes et de leur codage binaire

    // constructors
    public:
    explicit Digramme(const std::string &file) : nom_file(file+".txt")  {

       filled_source_file();
        cout << "# source de donnee stockee de taille " << getString_source().size() << endl;

        /* vector list_xgramme */
        fill_list_symbole();
        /*cout << "\n# affichage du vector list_xgramme" << endl;
        c->display_vector(c->getList_symbole());*/

        /* vector des digrammes */
        fill_list_digrammes();
        /*cout << "\n# affichage du vector list_Xgramme" << endl;
        c->display_vector(c->getList_digramme());*/

        /* vector dico */
        fill_dico();
        /*cout << "\n# affichage de l'aphabet" << endl;
        c->display_vector(c->getDico());*/

        /* map des digrammes et de leur fréquence */
        fill_map_freq_digramme(2);
        /*cout << "\n\n# affichage de la map de Digramme et de leur frequence" << endl;
        c->display_map(c->getMap_freq_digramme());*/

        /* table de codage des digrammes*/
        generate_codage_table();
        /*cout << "\n# affichage de la map codage table" << endl;
        c->display_map(c->getCodage_table());
        cout << "\n# codage table a t elle des codes de symboles en double ? " << c->check_double_coded_value() << endl;*/

        /* encodage binaire */
        cout << "# encodage de la source, taille apres compression= " << encode() << endl;

        /* decodage binaire */
        cout << "\n# decodage du stream codee, taille apres decompression(taux d'expansion?)= " <<  decode() << endl;

        taux_compression = (float) getSize_coded()/getString_source().size();
        taux_expansion = 0;
        facteur_compression = (float) getString_source().size()/getSize_coded();
        pourcentage_reduction = getString_source().size() - getSize_coded();
        pourcentage_reduction /= getString_source().size();

        cout << "# taux de compression " << taux_compression  << endl;
        cout << "# taux d'expansion " << taux_expansion << endl;
        cout << "# facteur de compresion " << facteur_compression << endl;
        cout << "# pourcentage reduction " << pourcentage_reduction*100 << "%" << endl;
    }
    Digramme(const Digramme &c) : nom_file(c.nom_file) {}
    Digramme(Digramme &&_c) : nom_file(_c.nom_file) {}
    ~Digramme() = default;

    // getters & setters
    const string &getNom_file() const {
        return nom_file;
    }
    const string &getString_source() const {
        return string_source;
    }
    void setString_source(const string &string_source) {
        Digramme::string_source = string_source;
    }
    const vector<string> &getVector_digramme() const {
        return dico;
    }
    void setVector_digramme(const vector<string> &vector_digramme) {
        Digramme::dico = vector_digramme;
    }
    const map<string, int> &getMap_freq_digramme() const {
        return map_freq_digramme;
    }
    void setMap_freq_digramme(const map<string, int> &map_freq_digramme) {
        Digramme::map_freq_digramme = map_freq_digramme;
    }
    const map<string, Bits::varBlock> &getCodage_table() const {
        return codage_table;
    }
    void setCodage_table(const map<string, Bits::varBlock> &codage_table) {
        Digramme::codage_table = codage_table;
    }
    const vector<string> &getList_symbole() const {
        return list_symbole;
    }
    void setList_symbole(const vector<string> &list_symbole) {
        Digramme::list_symbole = list_symbole;
    }
    const vector<string> &getList_digramme() const {
        return list_digramme;
    }
    void setList_digramme(const vector<string> &list_digramme) {
        Digramme::list_digramme = list_digramme;
    }
    const vector<string> &getVector_alphabet() const {
        return dico;
    }
    void setVector_alphabet(const vector<string> &vector_alphabet) {
        Digramme::dico = vector_alphabet;
    }
    const vector<string> &getDico() const {
        return dico;
    }
    void setDico(const vector<string> &dico) {
        Digramme::dico = dico;
    }
    const Bits::Stream &getStream_coded() const {
        return stream_coded;
    }
    void setStream_coded(const Bits::Stream &stream_coded) {
        Digramme::stream_coded = stream_coded;
    }
    int getSize_source() const {
        return size_source;
    }
    void setSize_source(int size_source) {
        Digramme::size_source = size_source;
    }
    int getSize_coded() const {
        return size_coded;
    }
    void setSize_coded(int size_coded) {
        Digramme::size_coded = size_coded;
    }
    int getSize_won() const {
        return size_won;
    }
    void setSize_won(int size_won) {
        Digramme::size_won = size_won;
    }
    int getSize_expanded() const {
        return size_expanded;
    }
    void setSize_expanded(int size_expanded) {
        Digramme::size_expanded = size_expanded;
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
     * Fonction permettant de remplir le vector list_Xgramme avec tous les digrammes
     */
    void fill_list_digrammes() {
        // on itère le le source_file pour remplir le vector de Digramme
        for(int i=0;i<string_source.size();i++) {
            if(i+1 == string_source.size()) { // si c'est le dernier caractère, on ne peut former un Digramme (car nombre de caractère impair)
               // on ne fait rien
            }
            else {
                // on construit le Digramme avec le 1er et second char
                string s;
                s.push_back(string_source[i]);
                s.push_back(string_source[i+1]);

                // on insère le Digramme du 1er et second char dans le vector list_Xgramme
                list_digramme.emplace_back(s);
            }
        }
    }
    /*
     * permet de remplir la map_freq_xgramme avec les digrammes uniques et leur fréquence
     * le paramètre int frequence permet de ne retenir que les digrammes avec une fréquence minimum
     */
    void fill_map_freq_digramme(int frequence) {
        // on parcout le vector
        for (auto const &value: list_digramme) {
                // si la clé existe
                if (check_if_key_exist(map_freq_digramme, value)) {
                    // on incrémente sa fréquence de 1
                    map_freq_digramme[value] = map_freq_digramme[value] + 1;
                }
                else {
                    // sinon on insère le Digramme avec la fréquence 1
                    map_freq_digramme.emplace(value, 1);
                }
        }
        // on supprime les éléments ayant une fréquence inférieure au paramètre
        for(map<string, int>::iterator it = map_freq_digramme.begin(); it != map_freq_digramme.end();) {
           if(it->second < frequence) {
               //cout << "est ce que " << it->second << " est < a " << frequence << endl;
               it = map_freq_digramme.erase(it);
           }
           else {
               //cout << "la frequence " << it->second << " est accepte " << endl;
               it++;
           }
        }

    }
    /*
     * génère le vector alphabet qui contient l'ensemble des caractères et des digrammes uniques
     * pas de taille maximale
     */
    void fill_dico() {
        for (auto const &value: list_symbole) {
            dico.push_back(value);
        }
        for (auto const &value: list_digramme) {
            dico.push_back(value);
        }
    }
    /*
     * génère le vector alphabet qui contient l'ensemble des caractères et des digrammes uniques
     * taille maximale
     */
    void fill_dico(int taille_max) {
        // on ajoute tous les symboles au dictionnaire
        for (auto const &value: list_symbole) {
            dico.push_back(value);
        }
        for (auto const &value: list_digramme) {
            dico.push_back(value);
        }
    }

    /* CODAGE */
    /* creer la table de codage
     * association Digramme -> codage
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
            // on vérifie si le dico contient le Digramme du 1er et du 2eme caractère
            string s;
            s.push_back(string_source[i]);
            s.push_back(string_source[i+1]);

            if(check_if_key_exist(codage_table, s)) { // s'il le contient
                stream_coded << getValueInMap(codage_table, s);
                //cout << "on encode le Digramme " << s << endl;
                i++;
            }
            else { // sinon
                stream_coded << getValueInMap(codage_table, string(1, string_source[i]));
                //cout << "on encode le caractere " << string(1, string_source[i]) << endl;
            }
        }
        size_coded = stream_coded.get_byte_size();
        return stream_coded.get_byte_size();
    }
    /* decode la table de codage ()
     * convertit chaque code binaire en son équivalent string
     */
    int decode() {
        Bits::varBlock vblock;
        vblock.set_valid(6);
        cout << "# string decode = | UNCOMMENT TO SEE";
        while(!stream_coded.end_of_stream()) {
            stream_coded >> vblock;
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
    /* affiche la map */
    void display_map(std::map<std::string, int> map_p){
        for (auto &map : map_p) {
            cout << "key: " << map.first << " value: " << map.second << endl;
        }
    }
    /* affiche le vector */
    void display_vector(vector<string> v){
        for (auto const &value: v) {
            cout << value << endl;
        }
    }
    /* affiche la table de codage */
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
};


#endif //TP4_DICO_STATIQUE_H
