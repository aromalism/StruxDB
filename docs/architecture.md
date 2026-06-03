# Architecture

## Components

- `src/regndlog.c`
  - Handles registration and login flows.
  - Stores user credentials in `data/credentials.txt`.
  - Launches the DBMS menu after successful login.

- `src/dbms.c`
  - Handles the user-specific database menu.
  - Adds records to `data/datadir/<username>.txt`.
  - Searches records by name or phone number.

## Data layout

- `data/credentials.txt`
  - Plaintext storage of registered usernames and passwords.

- `data/datadir/<username>.txt`
  - User-specific record file created after login.

## Project structure

- `src/` — source code files
- `bin/` — produced executables
- `data/` — runtime storage
- `docs/` — supporting documentation
