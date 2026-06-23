#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <string>
#include "../types.h"
#include "BTree.h"
#include "traits.h"
using namespace std;

//const char * keys="CDAMPIWNBKEHOLJYQZFXVRTSGU";
const char * keys1 = "D1XJ2xTg8zKL9AhijOPQcEowRSp0NbW567BUfCqrs4FdtYZakHIuvGV3eMylmn";
const char * keys2 = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const char * keys3 = "DYZakHIUwxVJ203ejOP9Qc8AdtuEop1XvTRghSNbW567BfiCqrs4FGMyzKLlmn";

const T1 BTreeSize = 3; 
void ImprimirClave(tagObjectInfo< BTreeTrait<TypeBTree, Ref> >& info, T1 nivel) {
    cout<<info.key<<" ";
}

bool EsVocal(tagObjectInfo< BTreeTrait<TypeBTree, Ref> >& info, T1 nivel) {
    TypeBTree k=info.key;
    return (k=='A'||k=='E'||k=='I'||k=='O'||k=='U' ||k=='a'||k=='e'||k=='i'||k=='o'||k=='u');
}
void DemoBTree() 
{
    BTree < BTreeTrait<TypeBTree, Ref> > bt(BTreeSize); 
    
    //nsercion
    T1 i;
    for (i = 0; keys1[i]; i++)
    {
        //cout<<"Inserting "<<keys1[i]<<endl;
        bt.Insert((TypeBTree)keys1[i], (Ref)(i * i));
        //bt.Print(cout);
}
    bt.Print(cout);

    cout<<"\nForEach Variac"<<endl;
    bt.ForEach(ImprimirClave);
    cout<<endl;

    //FirstThat
    cout<<"\nFirstThat"<<endl;
    auto* encontrado = bt.FirstThat(EsVocal);
    if (encontrado) {
        cout<<"Vocal encontrada: "<<encontrado->key<<" (Ref: "<<encontrado->ObjID<<")"<<endl;
    } else {
        cout<<"No se encontraron vocales."<<endl;
    }

}