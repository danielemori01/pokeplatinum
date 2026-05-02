#ifndef POKEPLATINUM_LOCKED_BOX_H
#define POKEPLATINUM_LOCKED_BOX_H

#include "pokemon.h"
#include "party.h"
#include "savedata.h"
#include "pc_boxes.h"

#define LOCKED_BOX_INDEX 17

void LockedBox_TransferDeadMons(Party *party, SaveData *saveData);

#endif // POKEPLATINUM_LOCKED_BOX_H
