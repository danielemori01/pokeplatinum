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

    // Iterate through the party in reverse to safely move and remove dead Pokémon.
    // Never remove the last Pokémon to prevent a game engine crash (empty party).
    for (i = partyCount - 1; i >= 0 && partyCount > 1; i--) {
        Pokemon *mon = Party_GetPokemonBySlotIndex(party, i);
        
        if (mon == NULL) {
            continue;
        }

        if (Pokemon_GetValue(mon, MON_DATA_IS_DEAD, NULL)) {
            // Try to store in Box 17 (the Locked Box).
            if (PCBoxes_TryStoreBoxMonInBox(pcBoxes, LOCKED_BOX_INDEX, &mon->box)) {
                // Successfully moved to Locked Box
            }
            
            // Remove from party
            Party_RemovePokemonBySlotIndex(party, i);
            partyCount--;
        }
    }
}
