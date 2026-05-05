#include "pokemon.h"
#include "party.h"
#include "pc_boxes.h"
#include "savedata.h"
#include "heap.h"
#include "math_util.h"
#include "system_vars.h"
#include "unk_02054884.h"
#include "constants/items.h"
#include "generated/species.h"
#include "generated/battle_terrains.h"

#define ROLL_SIZE 6

// A very basic initial pool for testing the starter draft
static const u16 sInitialStarterPool[] = {
    SPECIES_BULBASAUR, SPECIES_CHARMANDER, SPECIES_SQUIRTLE,
    SPECIES_CHIKORITA, SPECIES_CYNDAQUIL, SPECIES_TOTODILE,
    SPECIES_TREECKO, SPECIES_TORCHIC, SPECIES_MUDKIP,
    SPECIES_TURTWIG, SPECIES_CHIMCHAR, SPECIES_PIPLUP,
    SPECIES_EEVEE, SPECIES_GIBLE, SPECIES_RIOLU,
    SPECIES_PICHU, SPECIES_MAGIKARP, SPECIES_GASTLY,
    SPECIES_ABRA, SPECIES_MACHOP, SPECIES_GEODUDE,
    SPECIES_RALTS, SPECIES_BELDUM, SPECIES_BAGON,
    SPECIES_STARLY, SPECIES_SHINX, SPECIES_BUIZEL
};

void StarterDraft_Execute(SaveData *saveData, int metLocation) {
    if (saveData == NULL) {
        return;
    }

    Party *party = SaveData_GetParty(saveData);
    if (party == NULL) {
        return;
    }

    PCBoxes *pcBoxes = SaveData_GetPCBoxes(saveData);
    if (pcBoxes == NULL) {
        return;
    }

    int i;
    int poolSize = sizeof(sInitialStarterPool) / sizeof(u16);
    TrainerInfo *trainerInfo = SaveData_GetTrainerInfo(saveData);
    
    u32 trainerID = TrainerInfo_ID(trainerInfo);
    
    for (i = 0; i < 7; i++) {
        u16 species = sInitialStarterPool[LCRNG_Next() % poolSize];
        
        // Ensure no repeats in the starting party or PC
        // (For simplicity, we just check if it's already in the party, 
        // as the PC is empty at this point, but we could add a PC check if needed)
        if (Party_HasSpecies(party, species)) {
            i--;
            continue;
        }

        if (i == 0) {
            VarsFlags *varsFlags = SaveData_GetVarsFlags(saveData);
            if (varsFlags != NULL) {
                SystemVars_SetPlayerStarter(varsFlags, species);
            }
        }

        Pokemon *mon = Pokemon_New(HEAP_ID_FIELD2);
        Pokemon_Init(mon);
        Pokemon_InitWith(mon, species, 5, INIT_IVS_RANDOM, FALSE, 0, OTID_SET, trainerID);
        Pokemon_SetCatchData(mon, trainerInfo, ITEM_POKE_BALL, metLocation, TERRAIN_MAX, HEAP_ID_FIELD2);
        
        if (i < 6) {
            Party_AddPokemon(party, mon);
        } else {
            PCBoxes_TryStoreBoxMon(pcBoxes, (BoxPokemon *)mon);
        }
        
        Heap_Free(mon);
    }
}
