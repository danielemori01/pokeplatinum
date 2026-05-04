#include "pokemon.h"
#include "party.h"
#include "savedata.h"
#include "heap.h"
#include "math_util.h"
#include "generated/species.h"

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

void StarterDraft_Execute(SaveData *saveData) {
    Party *party = SaveData_GetParty(saveData);
    int i, j;
    
    // For now, we'll just give 6 random mons from the initial pool at level 5
    // In the future, this will be an interactive draft.
    for (i = 0; i < 6; i++) {
        u16 species = sInitialStarterPool[MTRNG_Next() % (sizeof(sInitialStarterPool) / sizeof(u16))];
        
        // Ensure no repeats in the starting party
        BOOL repeat = FALSE;
        for (j = 0; j < i; j++) {
            if (Pokemon_GetValue(Party_GetPokemonBySlotIndex(party, j), MON_DATA_SPECIES, NULL) == species) {
                repeat = TRUE;
                break;
            }
        }
        
        if (repeat) {
            i--;
            continue;
        }

        Pokemon *mon = Party_GetPokemonBySlotIndex(party, i);
        Pokemon_Init(mon);
        Pokemon_InitWithLevel(mon, species, 5, 31, FALSE, 0, 0, 0);
    }
}
