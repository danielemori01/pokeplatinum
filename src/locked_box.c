#include "locked_box.h"
#include "heap.h"
#include "savedata.h"
#include "pc_boxes.h"
#include "savedata/save_table.h"
#include <nitro.h>
#include <string.h>

void LockedBox_TransferDeadMons(Party *party, SaveData *saveData) {
    if (saveData == NULL || party == NULL) {
        return;
    }

    PCBoxes *pcBoxes = SaveData_GetPCBoxes(saveData);
    if (pcBoxes == NULL) {
        return;
    }

    int i;
    int partyCount = Party_GetCurrentCount(party);

    // Never remove the last pokemon from the party to avoid a game engine crash
    for (i = 0; i < partyCount && partyCount > 1; i++) {
        Pokemon *mon = Party_GetPokemonBySlotIndex(party, i);
        if (mon->box.isDead) {
            // Try to store in Box 17 (Locked Box)
            if (PCBoxes_TryStoreBoxMonInBox(pcBoxes, LOCKED_BOX_INDEX, &mon->box)) {
                // Successfully moved
            }
            // Remove from party
            Party_RemovePokemonBySlotIndex(party, i);
            i--; 
            partyCount--;
        }
    }
}
