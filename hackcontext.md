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
- **Draft Pokémon ownership and Pokédex fix (`src/roll_mechanic.c`):**
    - Added `Pokedex_Capture` calls to `DraftManager_AddPokemon` to ensure drafted Pokémon are correctly added to the Pokédex (seen and caught).
    - Added `Pokedex_ObtainNationalDex` to `DraftManager_AddPokemon` to force-unlock the National Pokédex upon first capture, ensuring all Pokémon (including non-Sinnoh species like Scyther) are correctly counted.
    - Verified that `Pokemon_InitWith` and `Pokemon_SetCatchData` are correctly using player-specific `TrainerInfo` and `metLocation` data to correctly establish the drafted Pokémon as player-owned.
- **Fixes:**
    - Resolved `SAVE_TABLE_ENTRY_MAX` mismatch crash in `include/constants/savedata/save_table.h`.
- **Combat Bag Disable:**
    - Modified `BattleControllerPlayer_CommandSelectionInput` in `src/battle/battle_controller_player.c` to intercept `PLAYER_INPUT_ITEM` for non-AI battlers.
    - Triggers a "Items can’t be used here." alert message instead of opening the bag menu.
    - AI-controlled partners and enemies are still permitted to use items if not otherwise restricted by battle type.
- **Rare Candies:**
    - Modified `Shop_GetItemPrice` in `src/overlay007/shop_menu.c` to override price to 0 for `ITEM_RARE_CANDY`, making them free to buy.
    - Added `ITEM_RARE_CANDY` to `PokeMartCommonItems` in `include/data/mart_items.h` to make them available in all standard Poké Marts.
    - Modified `ProcessItemListInput_SellItems` in `src/applications/bag/main.c` to prevent selling `ITEM_RARE_CANDY`.

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

### Combat Bag Disable

| File | Change |
| --- | --- |
| `src/battle/battle_controller_player.c` | Intercept `PLAYER_INPUT_ITEM` for human players and show alert 593. |

## Known Issues
None.

## Changelog (cont.)
- **Field move text updated to key item names (`res/text/field_moves.json`):**
    - All interaction prompts and action messages updated to use key item names instead of move names: Cut→Machete, Rock Smash→Pickaxe, Strength→Power Belt, Surf→Surfboard, Defog→Defogger, Rock Climb→Pitons, Waterfall→Clamp.
    - "Used X!" messages no longer reference Pokémon names since moves no longer do the action.
    - No-item error messages also updated to name the required key item.

## Changelog (cont.)
- **TM93–TM100 renamed:**
    - Updated `res/text/item_names.json`: TM93→"TM93 Cut", TM94→"TM94 Fly", TM95→"TM95 Surf", TM96→"TM96 Strength", TM97→"TM97 Defog", TM98→"TM98 Rock Smash", TM99→"TM99 Waterfall", TM100→"TM100 Rock Climb".
- **Auto-give TM when HM key item received:**
    - Modified `ScrCmd_AddItem` in `src/scrcmd_item.c`: after any `AddItem` script command that gives ITEM_HM01–ITEM_HM08, the corresponding TM93–TM100 is also silently added to the bag. Mapping is `ITEM_TM93 + (item - ITEM_HM01)`. Added `#include "constants/items.h"` for the item constants.

## Changelog (cont.)
- **Combat Bag Disable (re-enabled):**
    - In `src/battle/battle_controller_player.c` (`BattleControllerPlayer_CommandSelectionInput`), changed the `PLAYER_INPUT_ITEM` block condition from `BATTLE_TYPE_NO_ITEMS` only to also trigger when the battler's boot state is not `BATTLER_BOOT_STATE_AI`. Human players always see "Items can't be used here." (message 593); AI-controlled partners can still use items normally.

## Changelog (cont.)
- **HM Key Item Overhaul:**
    - Converted all 8 HM items (ITEM_HM01–ITEM_HM08) from POCKET_TMHMS to POCKET_KEY_ITEMS in `res/items/pl_item_data.csv`. ITEM_USE_FUNC changed to ITEM_USE_FUNC_NONE for HM01/HM03–HM08; HM02 uses ITEM_USE_FUNC_HANG_GLIDER.
    - Renamed items in `res/text/item_names.json`: HM01→Machete, HM02→Hang Glider, HM03→Surfboard, HM04→Power Belt, HM05→Defogger, HM06→Pickaxe, HM07→Clamp, HM08→Pitons.
    - Updated item descriptions in `res/text/item_descriptions.json` to match new key item identities.
    - Added ITEM_USE_FUNC_HANG_GLIDER = 25, FIRST_NEW_TM_IDX, LAST_NEW_TM_IDX to `include/constants/items.h`.
    - Nulled out TMHM_ID(HM01)–TMHM_ID(HM08) entries in `src/item.c` sTMHMMoves (HMs no longer teach moves). Added sNewTMMoves[] for TM93–TM100. Updated Item_MoveForTMHM and Item_TMHMNumber to handle both old and new TM ranges.
    - Zeroed out the 8 HM entries in sFieldMoves[] in `src/applications/party_menu/main.c` (MOVE_NONE for CUT through ROCK_CLIMB). HM field moves no longer appear as party menu options.
    - Updated `src/unk_0205DFC4.c` Item_IsTMHM to use pocket-based check (POCKET_TMHMS) so TM93–TM100 are recognized as TMs despite non-contiguous IDs.
    - Implemented UseHangGliderFromMenu in `src/item_use_functions.c`: opens the town map in TOWN_MAP_MODE_FLY. Added ITEM_USE_FUNC_HANG_GLIDER entry to sItemUseFuncs. Added #include for applications/town_map/main.h.
    - Updated `res/field/scripts/scripts_field_moves.s`: replaced FindPartySlotWithMove + GoToIfEq 6 (move-based eligibility checks) with CheckItem ITEM_HMxx + GoToIfEq 0 (key item checks) for all 6 obstacle-based HMs (Cut, Rock Smash, Strength, Rock Climb, Defog, Waterfall). Added Surfboard item check at the top of FieldMoves_Water (it had no script-level eligibility check before). In execute blocks, replaced FindPartySlotWithMove + SetVar + BufferPartyMonNickname with SetVar 0 + BufferPartyMonNickname 0 (uses party slot 0 for animation).
- **New TM93–TM100 (HM moves as TMs):**
    - Added ITEM_TM93–ITEM_TM100 to `generated/items.txt` (after ITEM_SECRET_KEY, before MAX_ITEMS).
    - Added CSV rows in `res/items/pl_item_data.csv` and name/description entries in `res/text/item_names.json` and `item_descriptions.json`.
    - TM93→Cut, TM94→Fly, TM95→Surf, TM96→Strength, TM97→Defog, TM98→Rock Smash, TM99→Waterfall, TM100→Rock Climb. Item_TMHMNumber maps TM93–TM100 to the former HM01–HM08 learnset bits (reuse), so all species that could learn each HM can now learn the corresponding TM.
    - Added sItemArchiveIDs entries for ITEM_TM93–ITEM_TM100 in `src/item.c` with dataIDs 0x1BE–0x1C5 (sequential NARC members after ITEM_SECRET_KEY at 0x1BD). This was the root cause of the battle-entry crash: ItemTable_Load calls Item_FileID(NUM_ITEMS, ...) = sItemArchiveIDs[475] which was out-of-bounds, producing a garbage maxItem, crashing the battle heap allocator (OS_Panic in ITCM at 0x01FF802C).

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
- **Draft menu ID fix (`src/roll_mechanic.c`):**
    - ID 871 is "Pokémon's data was added to the Pokédex" (explains the `'s …` prefix the user saw), not a caught message.
    - ID 873 is "A sandstorm is raging." (battle weather string), not a sent-to-PC message.
    - Correct IDs: 867 = "Gotcha! {STRVAR_1 1,0,0} was caught!" (party), 869 = "{STRVAR_1 1,0,0} was sent to someone's PC." (box). Both use slot 1, matching `StringTemplate_SetSpeciesNameWithArticleByID(..., 1, species)`.
- **Draft menu UI and formatting fixes (`src/roll_mechanic.c`):**
    - Fixed graphical glitches (inverted colors) by switching to palette 13 (standard text palette) for window content and using `Window_FillTilemap` with color 15 (white). This ensures a white background with black text.
    - Fixed missing Pokémon names in messages by setting both index 0 and 1 in the local `StringTemplate`, ensuring compatibility with the battle string variables.
    - Optimized message box visibility: the box now only appears when a message is being shown and is erased/transparent during the Pokémon selection phase, preventing an empty box from cluttering the screen.
- **Draft Pokémon ownership and Pokédex fix (`src/roll_mechanic.c`):**
    - Added `Pokedex_Capture` calls to `DraftManager_AddPokemon` to ensure drafted Pokémon are correctly added to the Pokédex (seen and caught).
    - Added `Pokedex_ObtainNationalDex` to `DraftManager_AddPokemon` to force-unlock the National Pokédex upon first capture, ensuring all Pokémon (including non-Sinnoh species like Scyther) are correctly counted.
    - Verified that `Pokemon_InitWith` and `Pokemon_SetCatchData` are correctly using player-specific `TrainerInfo` and `metLocation` data to correctly establish the drafted Pokémon as player-owned.
- **Fixes:**
    - Resolved Twinleaf Town out-of-bounds movement crash by changing `VAR_PLAYER_HOUSE_STATE` from 3 to 7 in `res/field/scripts/scripts_sandgem_town_pokemon_research_lab.s`. This skips the running shoes cutscene which assumes the player is coming down the stairs, rather than entering from the front door.
- **HM Key Item Overhaul:**
    - Converted all 8 HM items (ITEM_HM01–ITEM_HM08) from POCKET_TMHMS to POCKET_KEY_ITEMS in `res/items/pl_item_data.csv`. ITEM_USE_FUNC changed to ITEM_USE_FUNC_NONE (not usable from bag menu) for all except HM02.
    - Renamed items in `res/text/item_names.json`: HM01→Machete, HM02→Hang Glider, HM03→Surfboard, HM04→Power Belt, HM05→Defogger, HM06→Pickaxe, HM07→Clamp, HM08→Pitons.
    - Updated item descriptions in `res/text/item_descriptions.json` to match new key item identities.
    - Nulled out TMHM_ID(HM01)–TMHM_ID(HM08) entries in `src/item.c` sTMHMMoves (dead code since HMs are no longer TM/HM pocket items).
    - Zeroed out the 8 HM entries in sFieldMoves[] in `src/applications/party_menu/main.c` (MOVE_NONE for CUT through ROCK_CLIMB and FLY). HM field moves no longer appear as party menu options.
    - Added ITEM_USE_FUNC_HANG_GLIDER = 25 to `include/constants/items.h`.
    - Implemented UseHangGliderFromMenu in `src/item_use_functions.c`: opens the town map in TOWN_MAP_MODE_FLY so the player can fly to visited towns by using the Hang Glider from the Key Items pocket. Added ITEM_USE_FUNC_HANG_GLIDER entry to sItemUseFuncs table. Added #include for applications/town_map/main.h.
    - Updated `res/field/scripts/scripts_field_moves.s`: replaced FindPartySlotWithMove + GoToIfEq 6 (move-based eligibility checks) with CheckItem ITEM_HMxx + GoToIfEq 0 (key item checks) for all 7 obstacle-based HMs (Cut, Rock Smash, Strength, Rock Climb, Surf, Defog, Waterfall). In the execute blocks, replaced second FindPartySlotWithMove + SetVar + BufferPartyMonNickname with SetVar 0 + BufferPartyMonNickname 0 (uses party slot 0 for animation). Added Surfboard/Clamp item checks at the top of FieldMoves_Water and FieldMoves_Waterfall scripts (they had no script-level eligibility check before). Badge requirements preserved unchanged.
- **New TM93–TM100 (HM moves as TMs):**
    - Added ITEM_TM93–ITEM_TM100 to `generated/items.txt` (IDs 468–475, after ITEM_SECRET_KEY). These are regular POCKET_TMHMS items with ITEM_USE_FUNC_TM_HM, price 3000.
    - Added CSV rows in `res/items/pl_item_data.csv` and name/description entries in `res/text/item_names.json` and `item_descriptions.json` (NARC indices 468–475).
    - Added `[TMHM_ID(TM93)]`–`[TMHM_ID(TM100)]` entries to sTMHMMoves in `src/item.c`: TM93→Cut, TM94→Fly, TM95→Surf, TM96→Strength, TM97→Defog, TM98→Rock Smash, TM99→Waterfall, TM100→Rock Climb.
    - Reverted the 8 previously displaced TMs back to their original moves: TM21→Frustration, TM43→Secret Power, TM49→Snatch, TM54→False Swipe, TM58→Endure, TM67→Recycle, TM78→Captivate, TM83→Natural Gift.
    - Modified `Item_IsTMHM` in `src/unk_0205DFC4.c` to use pocket-based check (`Item_LoadParam(..., ITEM_PARAM_FIELD_POCKET) == POCKET_TMHMS`) instead of ID range, so TM93–TM100 are recognized as TMs despite non-contiguous IDs. Added `#include "item.h"`.
    - Added `FIRST_NEW_TM_IDX` and `LAST_NEW_TM_IDX` constants to `include/constants/items.h`. Updated `Item_MoveForTMHM` and `Item_TMHMNumber` in `src/item.c` to accept both the original TM01–HM08 range and the new TM93–TM100 range.
- **Test warp fixes + Surf test NPC (iteration 2):**
    - `res/field/scripts/scripts_sandgem_town.s`: Changed waterfall test warp target from `MAP_HEADER_MT_CORONET_1F_NORTH_ROOM_2` to `MAP_HEADER_ROUTE_224` at (908, 500) — an outdoor waterfall, no dungeon. Added new `SandgemTown_TestWarpSurf` script (ScriptEntry index 13) that warps to `MAP_HEADER_ROUTE_219` at (179, 867) — the ocean beach south of Sandgem where Surf can be triggered.
    - `res/field/events/events_sandgem_town.json`: Added `LOCALID_TEST_WARP_SURF` pokeball NPC at (172, 843) with `script: 14`. The existing waterfall pokeball stays at (171, 843).
    - `res/field/scripts/scripts_sandgem_town_pokemon_research_lab.s`: Added `AddItem ITEM_MAX_REPEL, 5, VAR_RESULT` to the test items block so the player starts with 5 Max Repels to suppress wild encounters during testing.
- **Surf/Waterfall C-level gate fix (`src/overlay005/field_control.c`):**
    - The tile-interaction A-button handler had two gates before firing global script 10004 (Surf): `TrainerInfo_HasBadge(info, 3)` (Fen Badge) AND `Party_HasMonWithMove(party, MOVE_SURF)`. Both always failed — no party Pokémon has MOVE_SURF since HMs no longer teach moves. Removed both checks; `PlayerAvatar_CanUseSurf` tile check alone now suffices. All item/badge validation is handled by the script itself.
    - The movement-based waterfall trigger (walking into a waterfall tile while surfing) was gated by `Party_HasMonWithMove(party, MOVE_WATERFALL)`. Removed that check; the flag is now always set and the tile behavior determines whether it fires.
- **Surf warp fix (iteration 3):**
    - Surf test warp changed from MAP_HEADER_ROUTE_219 (all ocean — player landed on water, Surf trigger requires standing on land) to MAP_HEADER_SANDGEM_TOWN at (180, 866) DIR_SOUTH — the last sand tile at the south shore, adjacent to Route 219 ocean. Pressing A facing south from here fires FieldMoves_Water.


- **Post-Gym Draft Rolls (v2):**
    - Registered `SCRCMD_EXECUTEGYMROLL` in `include/data/scripts/scrcmd.h`.
    - Added `ExecuteGymRoll` macro to `asm/macros/scrcmd.inc`.
    - Injected `ExecuteGymRoll` into all 8 gym leader scripts after the badge is awarded:
        - Roark: Level 15
        - Gardenia: Level 25
        - Fantina: Level 30
        - Maylene: Level 35
        - Crasher Wake: Level 40
        - Byron: Level 45
        - Candice: Level 50
        - Volkner: Level 55
    - The rolls use `targetSlot -1` to automatically add the Pokémon to the party or PC.
- **Post-Gym Roll level scaling updated:**
    - Roll levels changed to match each gym leader's ace Pokémon: Roark→14 (Cranidos), Gardenia→22 (Roserade), Fantina→26 (Mismagius), Maylene→32 (Lucario), Wake→37 (Floatzel), Byron→41 (Bastiodon), Candice→44 (Froslass), Volkner→50 (Electivire).
- **Post-Gym Roll party-full and message bugs fixed:**
    - `ScrCmd_ExecuteGymRoll` in `src/scrcmd_party.c` was reading `targetSlot` as `u16`, so `-1` from the script arrived as `0xFFFF` (65535). The `dm->targetSlot == -1` check never fired, routing every roll into the specific-slot branch which ignored the `Party_AddPokemon` return value and always set `sentToPC = FALSE`.
    - Fixed by casting `ScriptContext_GetVar` result to `s16` so -1 is preserved.
    - `DraftManager_AddPokemon` in `src/roll_mechanic.c` now uses `Party_AddPokemon`'s return value to decide party-vs-box (returns FALSE when full), sets `dm->sentToPC` accordingly, and calls `PCBoxes_TryStoreBoxMon(&mon->box)` on full party.
