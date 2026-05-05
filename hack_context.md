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
| `include/scrcmd_party.h` | Added `ScrCmd_ExecuteStarterDraft` prototype. |
| `src/scrcmd_party.c` | Implemented `ScrCmd_ExecuteStarterDraft`. |
| `include/data/scripts/scrcmd.h` | Added `SCRCMD_EXECUTESTARTERDRAFT` to the command table. |
| `asm/macros/scrcmd.inc` | Added `ExecuteStarterDraft` macro. |
| `src/roll_mechanic.c` | Updated `StarterDraft_Execute` to set the player starter variable. |
| `src/meson.build` | Added `roll_mechanic.c` to the build. |
| `src/scrcmd_party.c` | Temporarily disabled `ScrCmd_ExecuteStarterDraft` logic for crash debugging. |
| `src/scrcmd_party.c` | Replaced `return TRUE` with `return FALSE` in `ScrCmd_ExecuteStarterDraft` to avoid blocking script execution. |
| `src/scrcmd_party.c` | Added NULL pointer safety checks and re-enabled `ScrCmd_ExecuteStarterDraft`. |
| `src/roll_mechanic.c` | Refactored `StarterDraft_Execute` to manually initialize and add Pokémon to the party, bypassing `SaveData_UpdateCatchRecords` to prevent a crash when the Pokédex is not yet initialized. |
| `src/roll_mechanic.c` | Increased draft to 7 Pokémon; the 7th Pokémon is sent to the PC (Box 1). |
| `src/roll_mechanic.c` | Fixed Pokémon obedience by properly setting the player's OTID and met terrain (`TERRAIN_MAX`) for drafted Pokémon. |

## Known Issues
None.

## Changelog (cont.)
- **Draft menu linker fixes (`src/roll_mechanic.c`):**
    - Added `#include "field_message.h"` (provides `FieldMessage_FinishedPrinting`).
    - Replaced undefined `IsPrinterActive` with `FieldMessage_FinishedPrinting`; logic inverted accordingly (`!FieldMessage_FinishedPrinting` to wait while printing).
    - Replaced undefined `ScriptContext_Resume` with the correct engine pattern: `ScriptContext_Pause(ctx, DraftIsDone)` called in `DraftManager_New`, storing the DraftManager pointer in `ctx->data[0]`.
    - Added `BOOL done` field to `DraftManager` struct.
    - `DraftManager_Delete` now sets `dm->done = TRUE` instead of calling Resume; does not free dm.
    - Added `DraftIsDone(ScriptContext *ctx)` resume callback: frees dm and clears `ctx->data[0]` when done, returning TRUE to unblock the script.
- **Draft menu input fix (`src/roll_mechanic.c`):**
    - `LIST_MENU_NO_SELECTION_YET` (0xEEEE) is a user-managed placeholder — `ListMenu_ProcessInput` never returns it. The correct "nothing chosen" value is `MENU_NOTHING_CHOSEN` (-1). The bad check caused every frame to fall through to `currentPool[-1]` (garbage species) and call `DraftManager_AddPokemon` each frame, corrupting party data and crashing the battle.
    - Fixed: replaced constant check with `(s32)input < 0` which correctly catches both `MENU_NOTHING_CHOSEN` (-1) and `MENU_CANCEL` (-2).
    - Added bounds guard `index >= DRAFT_POOL_SIZE` for safety.
- **Draft message ID fix (`src/roll_mechanic.c`):**
    - ID 871 is "Pokémon's data was added to the Pokédex" (explains the `'s …` prefix the user saw), not a caught message.
    - ID 873 is "A sandstorm is raging." (battle weather string), not a sent-to-PC message.
    - Correct IDs: 867 = "Gotcha! {STRVAR_1 1,0,0} was caught!" (party), 869 = "{STRVAR_1 1,0,0} was sent to someone's PC." (box). Both use slot 1, matching `StringTemplate_SetSpeciesNameWithArticleByID(..., 1, species)`.
