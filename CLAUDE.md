## Changelog
After every code change, append a summary of what was changed to `hackcontext.md` using this format:



# Pokemon Platinum Decomp Hack — v1 Working Brief

> Long-term project vision lives in `DESIGN.md`. This file is **only v1 scope.**
> When a feature finishes v1, update this file. Don't pull from DESIGN.md
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

### 4. 7-mon Initial Draft (In Development)

- Replace the Rowan Lab starter script with a "Draft" interface.
- Ensure the 7 drafted Pokémon are added to the party/PC correctly at start.
- **Status:** Planning phase.

## Out of v1 Scope (do not implement until graduated to CLAUDE.md)

These are designed but not yet active. See DESIGN.md for full specs:

- Post-gym rolls
- In-game trade replacement
- Forced gift/event replacement (Eevee, eggs, Porygon, Giratina, etc.)
- Combat bag disable
- Free unsellable Rare Candies
- HM overhaul (Machete etc.)
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
