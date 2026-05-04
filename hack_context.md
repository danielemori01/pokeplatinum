# Hack Context — Pokémon Platinum Nuzlocke ROM Hack

Reference document for all intentional modifications to the vanilla decomp. Use this when you need to find where something was changed, what flags control a feature, or which files define a custom system.

### Changelog
- Fixed crash after encounters by resolving `SAVE_TABLE_ENTRY_MAX` mismatch in `include/constants/savedata/save_table.h`.
- Skipped Route 202 catching tutorial; Dawn/Lucas now give 5 Potions and leave immediately.
- Refactored `LockedBox_TransferDeadMons` in `src/locked_box.c` to use a reverse loop, preventing crashes when multiple Pokémon faint simultaneously and are removed from the party.

## Permadeath System (Planned - Not Yet Implemented)

When a player-side Pokémon's HP reaches 0 in battle, the intention is to set an `isDead` flag, move the mon to PC Box 17 (the "locked box"), and remove it from the party. 

### Implementation Files (Proposed/Partial)

| File | Purpose |
| --- | --- |
| `src/battle/battle_script.c` | Proposed: Set `MON_DATA_IS_DEAD` = 1 when a player mon's HP hits 0 |
| `src/field_battle_data_transfer.c` | Proposed: Update field system with isDead flags |
| `src/encounter.c` | Proposed: Call `LockedBox_TransferDeadMons` |
| `src/locked_box.c` | Implements `LockedBox_TransferDeadMons` (Needs integration) |
| `include/locked_box.h` | Declares `LOCKED_BOX_INDEX 17` and `LockedBox_TransferDeadMons` |
| `src/pokemon.c` | Implements `MON_DATA_IS_DEAD` support in data getters/setters |
| `include/struct_defs/pokemon.h:122` | Added `u16 isDead : 1` bitfield to the `BoxPokemon` struct |
| `src/item_use_pokemon.c` | Proposed: Prevent using healing/revive items on dead mons |

## Build System Notes

- Meson build: Use `make` (alias for meson/ninja) to build the ROM.
- Script changes: If modifying `.narc` sources (e.g. scripts), check `build/res/battle/scripts/sub_seq.narc.p/<script_name>` and re-run `ninja <narc_path>` directly, then `ninja` to link the ROM.
- Python path bug in `build.ninja`: if meson regenerates the build file, the Python shebang path may break. Run ninja twice or patch the path manually.
