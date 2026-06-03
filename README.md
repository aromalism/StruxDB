# StruxDB

A small C-based database application for registration, login, adding records, and searching saved user data.

## Project layout

- `src/` — application source files
- `bin/` — compiled executables
- `data/` — runtime storage for credentials and saved records
- `docs/` — project documentation and architecture notes
- `Makefile` — build helper
- `.gitignore` — ignored files for development
- `LICENSE` — project license

## Build

From the project root:

```bash
make
```

This builds `bin/dbms` from `src/regndlog.c` and `src/dbms.c`.

## Run

```bash
./bin/dbms
```

The application stores credentials in `data/credentials.txt` and logged-in user records in `data/datadir/<username>.txt`.

## Documentation

See `docs/architecture.md` and `docs/usage.md` for more details.
