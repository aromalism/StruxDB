# Usage

## Build

Run from the repository root:

```bash
make
```

This compiles the application into `bin/dbms`.

## Run

```bash
./bin/dbms
```

## Workflow

1. Register a new user.
2. Log in with the registered username and password.
3. Use the DB menu to add and search entries.

## Notes

- User credentials and records are stored under `data/`.
- If `data/` does not exist, the application creates it automatically.
- The project currently stores passwords in plaintext, so avoid using real credentials.
