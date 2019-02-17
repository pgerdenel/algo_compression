#include "CTF.h"

using namespace std;

int main() {
    const string file = "..\\ress\\test_s";
    const string file2 = "..\\ress\\test_c";
    const string file3 = "..\\ress\\USconstitution";

    /* Créatin de l'objet de codage à taille fixe */
    CTF* ctf = new CTF(file);

    /*cout << "# FILL String source de taille= " << endl;
    ctf->fil_string_source();
    cout << "# source de donnee stockee dans un string, taille " << ctf->getString_source().size() << endl;

    cout << "\n# symbol list generated" << endl;
    ctf->generate_symbol_list();
    ctf->display_symbol_list();

    cout << "\n\n# codage table generated" << endl;
    ctf->generate_codage_table();
    ctf->display_map(ctf->getCodage_table());

    cout << "\n# encodage de la source, taille encoded= " << ctf->encode()<< endl;

    cout << "\n# decodage du stream encoded= " << ctf->decode() << endl;*/




    return 0;
}

