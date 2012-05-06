/**
 * @file pntrans.h
 * @author Jan Sedlak <xsedla85@stud.fit.vutbr.cz>
 * @author Lukas Brabec <xbrabe09@stud.fit.vutbr.cz>
 *
 * @section DESCRIPTION
 *
 * Trida pro reprezentaci prechodu simulatoru petriho siti.
 */

#ifndef PNTRANS_H
#define PNTRANS_H

#include <vector>
#include <QString>
#include <map>
#include "pnplace.h"
#include "constraint.h"

/**
  * Operace, ktere se mohou na hrane provadet.
  */
enum Operations {
    ADD, ///< pricteni
    SUB ///< odecteni
};

/**
 * Struktura popisujici jednu operaci s jednou promennou.
 */
struct Operation {
    Operations op; ///< operace
    QString var; ///< nazev promenne
    /**
      * Konstruktor struktury.
      */
    Operation(){}
    /**
      * Konstruktor struktury, naplni ji zadanymi daty.
      *
      * @param _op operace
      * @param _var typ promenne
      */
    Operation(Operations _op, QString _var){
        op = _op;
        var = _var;
    }
};

typedef std::vector<PNPlace*> PlaceVector;
typedef std::vector<Constraint*> ConstraintVector;
typedef std::map<QString,PNPlace*> StringToPnplaceMap;
typedef std::map<PNPlace*,QString> PnplaceToStringMap;
typedef std::vector<StringToPntypeMap> StrPntMapVector;
typedef std::vector<struct Operation> OperationVector;

/**
 * Struktura popisujici retezec operaci s jednim vystupem.
 */
struct OneOut {
    OperationVector operations; ///< vektor operaci
    QString output; ///< nazev vystupni promenne
    /**
      * Konstruktor struktury.
      */
    OneOut(){}
    /**
      * Konstruktor struktury, ktery ji naplni zadanymi daty.
      *
      * @param _operations vektor operaci
      * @param _output nazev vystupni promenne
      */
    OneOut(OperationVector _operations, QString _output){
        operations = _operations;
        output = _output;
    }
};

typedef std::vector<struct OneOut> OutputOperations;

/**
 * Trida popisujici prechod v petriho sitich. Poskytuje metody pro odpaleni sebe sama.
 */
class PNTrans
{
private:
    StrPntMapVector possible_choices; ///< vektor ruznych konfiguraci, se kterymi se da prechod odpalit

    /**
      * Zjisti, ktere z kombinaci vstupnich promennych vyhovuji podminkam, vysledek vrati a ulozi
      *
      * @param hash slovnik poli ve tvaru {"x":[1,2,3],"y":[4,5,6,...]}
      *
      * @return vektor slovniku moznych kombinaci odpaleni
      */
    StrPntMapVector chooseValues(StringToTokensMap hash);
public:
    ConstraintVector constraints; ///< vektor podminek
    StringToPnplaceMap in_names; ///< slovnik vstupnich hran (ukazatelu na pnplace)
    StringToPnplaceMap out_names; ///< slovnik vystupnich hran (ukazatelu na pnplace)
    OutputOperations operations; ///< vektor operaci
    QString id; ///< id prechodu
    QString x; ///< x souradnice prechodu
    QString y; ///< y souradnice prechodu

    /**
      * Konstruktor mista.
      */
    PNTrans();
    /**
      * Konstruktor mista, ktery naplni veskere zadane atributy.
      *
      * @param x souradnice x
      * @param y souradnice y
      * @param id id prechodu
      * @param constraints podminky prechodu
      * @param in_names slovnik vstupnich hran
      * @param out_names slovnik vystupnich hran
      * @param operations seznam operaci
      */
    PNTrans(QString x, QString y, QString id, ConstraintVector constraints,
            StringToPnplaceMap in_names, StringToPnplaceMap out_names, OutputOperations operations)
        :constraints(constraints),in_names(in_names),out_names(out_names),operations(operations),id(id),x(x),y(y) {}
    /**
      * Destruktor tridy, uvolni to, co je potreba.
      */
    ~PNTrans();
    /**
      * Zjisti, zda by prechod sel odpalit.
      *
      * @return true pokud odpalit jde, jinak false
      */
    bool fire();
    /**
      * Provede vybranou operaci podle vektoru moznych konfiguraci.
      *
      * @param choice vyber, ktera kombinace se ma provest
      *
      * @return true pokud se povedl prechod odpalit, jinak false
      */
    bool doOperations(unsigned int choice);
    /**
      * Vrati pocet moznych konfiguraci k odpaleni.
      *
      * @return pocet konfiguraci
      */
    int possibleChoicesCount();

    /**
      * Odstrani zadane napojene misto.
      *
      * @param place ukazatel na misto, ktere se ma odebrat
      * @param isIn jestli ma byt vstupni
      */
    void removeConnectedPlace(PNPlace * place, bool isIn);
};

#endif // PNTRANS_H
