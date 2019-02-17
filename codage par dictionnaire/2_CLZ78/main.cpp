#include <iostream>
#include "../CTF/CTF.h"
#include "CLZ78.h"

int main() {
    const string file1 = "..\\ress\\test_s";
    const string file2 = "..\\ress\\test_c";
    const string file3 = "..\\ress\\USconstitution";

    cout << "###############  CODAGE CTF ############" << endl;
    /* Créatin de l'objet de codage à taille fixe */
    //CTF* ctf = new CTF(file);

    cout << "\n###############  CODAGE Digramme ############" << endl;
    /* Création de l'objet de codage Digramme des 3 variantes(1,2,3) */
    CLZ78* clz78_variante_1 = new CLZ78(file1, 1, -1); // new CLZ78(nom_fichier, variante, ecriture/lecture?)
    CLZ78* clz78_variante_2 = new CLZ78(file3, 2, -1);
    CLZ78* clz78_variante_3 = new CLZ78(file3, 3, -1);



    return 0;
}


