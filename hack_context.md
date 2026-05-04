# Hack Context — Pokémon Platinum Nuzlocke ROM Hack

Reference document for all intentional modifications to the vanilla decomp. Use this when you need to find where something was changed, what flags control a feature, or which files define a custom system.

### Changelog
- **Permadeath Engine:**
    - Added `isDead` bitfield to `BoxPokemon` struct in `include/struct_defs/pokemon.h`.
    - Integrated `MON_DATA_IS_DEAD` into `src/pokemon.c` getters and setters.
    - Updated `src/battle/battle_script.c` (`BtlCmd_TryFaintMon`) to mark player Pokémon as dead when they faint.
    - Updated `src/item_use_pokemon.c` (`Party_HealAllMembers`) to skip dead Pokémon.
    - Implemented `TransferDeadMonsToLockedBox` in `src/encounter.c` (moved from `locked_box.c` to fix a post-battle crash).
    - Integrated transfer call into `UpdateFieldSystemFromDTO` in `src/encounter.c`.
    - Dead Pokémon are moved to PC Box 18 (index 17), which is hidden from normal PC navigation.
- **Fast Forward / Skip Tutorial:**
    - Skipped Professor Rowan's intro speech (Lucas/Barry names forced).
    - Player spawns in Sandgem Lab.
    - Added extra starters (Rattata, Pidgey, Caterpie) in `res/field/scripts/scripts_sandgem_town_pokemon_research_lab.s`.
    - Skipped Route 202 catching tutorial in `res/field/scripts/scripts_route_202.s`.
    - Updated Route 202 NPC text in `res/text/route_202.json` to give Potions instead of Poké Balls.
- **Fixes:**
    - Resolved `SAVE_TABLE_ENTRY_MAX` mismatch crash in `include/constants/savedata/save_table.h`.

## Subsystem Details

### Permadeath System

| File | Change |
| --- | --- |
| `include/struct_defs/pokemon.h` | Added `u16 isDead : 1` to `BoxPokemon` (bit 3 of word at 0x04). |
| `src/pokemon.c` | Added `MON_DATA_IS_DEAD` case to `BoxPokemon_GetDataInternal` and `BoxPokemon_SetDataInternal`. |
| `src/battle/battle_script.c` | In `BtlCmd_TryFaintMon`, if side is player, set `MON_DATA_IS_DEAD = 1`. |
| `src/item_use_pokemon.c` | `Party_HealAllMembers` checks `MON_DATA_IS_DEAD` and continues if true. |
| `src/locked_box.c` | Defines `LockedBox_TransferDeadMons(Party *party, SaveData *saveData)`. |
| `src/encounter.c` | `UpdateFieldSystemFromDTO` calls `LockedBox_TransferDeadMons`. |

### Tutorial Skip / Starters

| File | Change |
| --- | --- |
| `res/field/scripts/scripts_sandgem_town_pokemon_research_lab.s` | Added `GivePokemon` for SPECIES_RATTATA, PIDGEY, and CATERPIE (Level 1). |
| `res/field/scripts/scripts_route_202.s` | Removed `StartCatchingTutorial` and movement scripts. |
| `res/text/route_202.json` | Replaced mentions of catching tutorial with Potion gifts. |

## Known Issues
None.
