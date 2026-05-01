#ifndef POKEPLATINUM_LOCKED_BOX_H
#define POKEPLATINUM_LOCKED_BOX_H

#include "pokemon.h"
#include "party.h"
#include "savedata.h"

#define MAX_LOCKED_MONS 50

typedef struct LockedBox {
    int count;
    BoxPokemon mons[MAX_LOCKED_MONS];
} LockedBox;

void LockedBox_Init(LockedBox *lb);
int LockedBox_SaveSize(void);
void LockedBox_TransferDeadMons(Party *party, SaveData *saveData);
LockedBox *SaveData_GetLockedBox(SaveData *saveData);

#endif // POKEPLATINUM_LOCKED_BOX_H
