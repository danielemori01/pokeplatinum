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

## v1 Scope (only these three subsystems)

### 1. Permadeath Engine

- When a Pokémon's HP reaches 0 in battle, do not apply STATUS_FAINTED.
- Instead, mark the Pokémon as permanently dead and move its struct to a hidden "locked" PC box.
- Dead Pokémon cannot be revived, withdrawn, or used in any way.
- Dead Pokémon do not gain XP from subsequent battles.
- Locked box is hidden from the PC UI in v1 (visible memorial is v4 scope — see DESIGN.md).
- **Code areas (expected):** battle HP-decrement path, faint handling, status flags, PC box code, experience-distribution code.
- **Risk:** cross-overlay state. Confirm overlay residency (`platinum.us/main.lsf`) before assuming a function is callable.
- **Not implemented.**


### 2. Fast Forward / Intro Skip

Bypasses the vanilla introductory sequence.

- Skips Professor Rowan's introductory speech and naming/gender screens.
- Player character is automatically named **LUCAS** and the rival is named **BARRY**.
- Player spawns directly in Professor Rowan's Lab in Sandgem Town.
- Starter selection, Pokedex, Running Shoes, Journal, and Parcel are given immediately.
- All early narrative blockers and cutscenes in Twinleaf Town, Route 201, and Lake Verity are disabled/cleared.
- **Done.**

### 3. Poké Ball Removal

- Remove all Poké Ball variants (Poké Ball, Great Ball, Ultra Ball, Master Ball, Premier Ball, Heal Ball, Net Ball, Dive Ball, Nest Ball, Repeat Ball, Timer Ball, Luxury Ball, Dusk Ball, Quick Ball, Cherish Ball, Park Ball, etc.) from the game.
- Not in shop inventories. Not as field/hidden items. Not from NPC gifts. Not as Pickup-ability rewards.
- Wild battles still occur for XP. Catch action becomes unreachable de facto (bag is not yet disabled in v1; the deterrent is just that there are no balls to use).
- **Code areas (expected):** shop item tables, field item placements, NPC gift scripts, Pickup table.
- all Done"""
## Out of v1 Scope (do not implement until graduated to CLAUDE.md)

These are designed but not yet active. See DESIGN.md for full specs:

- Post-gym rolls
- Initial 7-Roll Draft: Sequential rolls at game start, replacing starter briefcase. (Needs implementation)
- In-game trade replacement
- Forced gift/event replacement (Eevee, eggs, Porygon, Giratina, etc.)
- Combat bag disable
- Free unsellable Rare Candies
- HM overhaul (Machete etc.)
- Stat-based ban algorithm
- Visible locked box memorial UI

If the user asks Claude to implement any of the above, point them at DESIGN.md and ask whether they want to graduate it to v1 (which expands current scope and timeline). Do not silently start working on out-of-scope features.

## Rules for Claude

- Never edit a file without reading it first.
- For any change to a `.s` file or anything that compiles to a specific instruction sequence, build and report. Don't claim it works without verifying.
- If a proposed change would touch >3 files, outline the plan and wait for confirmation before editing.
- When you don't know a struct field, function signature, or constant name, **grep the codebase**. Don't guess. The user has been burned by hallucinated identifiers in the past.
- pokeplatinum uses MWCC (CodeWarrior), not GCC. Compiler-specific behavior follows MWCC rules.
- Cross-overlay calls require checking the overlay map (`platinum.us/main.lsf`).
- For each open question listed in the v1 subsystems above, the user must give an explicit answer before that part of the implementation begins. Don't pick an answer for them.

## Communication

- Direct technical answers. No marketing fluff. No over-apologizing.
- The user is learning Linux, git, C, and ARM while doing this. Brief explanations for non-pokeplatinum-specific commands are welcome on first use; don't repeat them every time.