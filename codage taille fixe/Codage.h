/* 
 * Class abstraite Codage regroupant les méthodes encode et decode virtuelle
 * - implémenter paramètre de type map<K,V> pour n'importe quel type
 * - implémenter std::vector<Bits::blocks<int>> et non 8
 * - voir pour implémenter type de retour générique
 */

#ifndef TP1_CODAGE_H
#define TP1_CODAGE_H
#include "BitStream-1.2.12/BitStream.h"

class Codage {

    virtual std::vector<Bits::Block<8>> encode() = 0; // permet d'encoder des données
    virtual std::vector<std::string> decode(std::vector<Bits::Block<8>>) = 0; // permet de décoder des données

};


#endif //TP1_CODAGE_H
