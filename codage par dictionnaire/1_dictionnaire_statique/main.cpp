#include <iostream>
#include "Digramme.h"
#include "../CTF/CTF.h"

int main() {
    const string file = "..\\ress\\test_s";
    const string file2 = "..\\ress\\test_c";
    const string file3 = "..\\ress\\USconstitution";

    cout << "###############  CODAGE CTF ############" << endl;
    /* Créatin de l'objet de codage à taille fixe */
    CTF* ctf = new CTF(file);


    cout << "\n###############  CODAGE Digramme ############" << endl;
    /* Création de l'objet de codage Digramme */
    Digramme* dig = new Digramme(file);




    return 0;
}


