## Changelog
After every code change, append a summary of what was changed to `hackcontext.md`



# Pokemon Platinum Decomp Hack — Working Brief

> Long-term project vision lives in `DESIGN.md`. This file tracks active scope.
> When a feature is done, update this file. Don't pull from DESIGN.md
> without explicit confirmation from the user.

## Project

This is `pret/pokeplatinum`, a byte-matching decompilation of Pokémon Platinum (US, rev 1) for Nintendo DS.

- Vanilla SHA1: `0862ec35b24de5c7e2dcb88c9eea0873110d755c`
- After intentional modifications, this hash will diverge — that's expected.
- Until a modification is intentional, the SHA1 must match.

## Environment

- GitHub Codespaces (Ubuntu Linux), Node.js + Claude Code installed.
- Build system: meson + ninja, invoked via `make` at the repo root.
- Compiler: mwccarm (CodeWarrior ARM), fetched as a meson subproject.
- Toolchain also includes arm-none-eabi-gcc for some asm/linker steps.
- The user is a beginner with C, ARM assembly, and the pret toolchain. Explain non-obvious things; don't assume domain knowledge of the codebase.

## Build & Verify Workflow

- Full build: `make`
- Clean build: `make clean && make`
- Update subprojects (after pulling main): `make update`
- Verify ROM: `sha1sum build/pokeplatinum.us.nds`
- Debug build: `make debug`

After ANY edit, run `make`. Report success or paste the actual error. Do not claim a build worked without verifying. After meaningful changes, also run `sha1sum`.

## v1 Scope (active milestones)

### 1. Permadeath Engine (Done)

- When a Pokémon's HP reaches 0 in battle, it is marked as permanently dead.
- Dead Pokémon are moved to a hidden "locked" PC box (Box 18) after the battle ends.
- `Party_HealAllMembers` (and similar effects) skip dead Pokémon.
- **Implemented in:** `src/pokemon.c`, `src/battle/battle_script.c`, `src/encounter.c`, `src/item_use_pokemon.c`.

### 2. Fast Forward / Intro Skip (Done)

- Skips Professor Rowan's introductory speech and naming/gender screens.
- Player spawns directly in Professor Rowan's Lab.
- Starter selection, Pokedex, Running Shoes, and basic inventory given.
- Route 202 catching tutorial skipped; NPC gives Potions instead of balls.
- **Implemented in:** `res/field/scripts/`, `res/text/route_202.json`.

### 3. Poké Ball Removal (Done)

- Removed all Poké Ball variants from shops and scripts.
- Catch action becomes unreachable.
- **Implemented in:** Shop inventories, field scripts.

### 4. 7-mon Initial Draft (Done)

- Replaces the Rowan Lab starter script with a "Draft" interface (currently random 7).
- First 6 added to party; 7th added to PC Box 1.
- Properly sets OTID and met data for player ownership.
- Fixed UI glitches (colors/transparency) and Pokédex registration (force-unlocks National Dex).
- **Implemented in:** `src/roll_mechanic.c`, `src/scrcmd_party.c`.

### 5. Free Rare Candies (Done)

- Every Poké Mart sells Rare Candies for 0 ¥.
- Rare Candies are unsellable.
- **Implemented in:** `src/overlay007/shop_menu.c`, `include/data/mart_items.h`, `src/applications/bag/main.c`.

### 6. Combat Bag Disable (Done)

- Player's bag menu is blocked in battle; shows "Items can't be used here."
- AI-controlled partners and enemies are unaffected.
- **Implemented in:** `src/battle/battle_controller_player.c`.

### 7. Post-Gym Rolls (Done)

- After each of the 8 gyms, player gets one roll via `ExecuteGymRoll`.
- Roll level scales to the gym leader's ace: Roark→14, Gardenia→22, Fantina→26, Maylene→32, Wake→37, Byron→41, Candice→44, Volkner→50.
- Party-full case: Pokémon sent to PC Box 1 automatically.
- **Implemented in:** `src/scrcmd_party.c`, `src/roll_mechanic.c`, all 8 gym leader scripts.

### 8. HM Overhaul (Done)

- All 8 HMs converted to Key Items (Machete, Hang Glider, Surfboard, Power Belt, Defogger, Pickaxe, Clamp, Pitons).
- Field obstacle scripts check for key items instead of party moves.
- HM moves teachable via new TM93–TM100 (added to POCKET_TMHMS).
- Hang Glider opens the Fly map from the Key Items pocket.
- **Implemented in:** `res/items/pl_item_data.csv`, `src/item.c`, `src/item_use_functions.c`, `src/applications/party_menu/main.c`, `src/unk_0205DFC4.c`, `res/field/scripts/scripts_field_moves.s`, `res/text/item_names.json`, `res/text/item_descriptions.json`, `generated/items.txt`.

## Not Yet Implemented (do not implement without explicit confirmation)

These are designed but not yet active. See DESIGN.md for full specs:

- In-game trade replacement
- Forced gift/event replacement (Eevee, eggs, Porygon, Giratina, etc.)
- Stat-based ban algorithm
- Visible locked box memorial UI

## Rules for Claude

- Never edit a file without reading it first.
- For any change to a `.s` file or anything that compiles to a specific instruction sequence, build and report.
- If a proposed change would touch >3 files, outline the plan and wait for confirmation.
- When you don't know a struct field, function signature, or constant name, **grep the codebase**.
- Cross-overlay calls require checking the overlay map (`platinum.us/main.lsf`).

## Communication

- Direct technical answers. No marketing fluff. No over-apologizing.
- The user is learning Linux, git, C, and ARM while doing this. Brief explanations for non-pokeplatinum-specific commands are welcome on first use.
