# Architecture

## Overview

StruxDB is a two-module C application. The entry point and authentication layer live in `regndlog.c`; all database and workspace logic lives in `dbms.c`. The two files are compiled together into a single binary.

```
┌─────────────────────────────────────────────┐
│                  bin/strux                   │
│                                             │
│  ┌──────────────────┐  ┌─────────────────┐  │
│  │   regndlog.c     │  │    dbms.c       │  │
│  │                  │  │                 │  │
│  │  main()          │─▶│  dbms()         │  │
│  │  registr()       │  │  dataset_menu() │  │
│  │  login()         │  │  create_dataset │  │
│  │  usercheck()     │  │  open_dataset   │  │
│  │  analyse_username│  │  dataset_add    │  │
│  │  analyse_password│  │  dataset_search │  │
│  └──────────────────┘  └─────────────────┘  │
└─────────────────────────────────────────────┘
```

---

## Modules

### `src/regndlog.c` — Authentication

Owns the program entry point and all user identity logic.

| Function | Responsibility |
|----------|----------------|
| `main()` | Displays the top-level menu (Register / Login / Exit) and dispatches to the appropriate handler. |
| `registr()` | Collects a new username and password, validates both, confirms password match, and appends the credential pair to `data/credentials.strux`. |
| `usercheck()` | Scans `data/credentials.strux` to verify a requested username is not already taken. |
| `login()` | Reads credentials from stdin, scans `data/credentials.strux` for a matching pair, and on success calls `dbms()` with the authenticated username. |
| `analyse_username()` | Enforces username rules: 3–32 characters, letters/digits/underscores/periods only, cannot start with a digit or underscore, cannot be all digits. |
| `analyse_password()` | Enforces password rules: 8–128 characters, must contain uppercase, lowercase, digit, and symbol; no whitespace; cannot equal the username. |

---

### `src/dbms.c` — Database Management

Owns the per-user workspace and all dataset I/O.

| Function | Responsibility |
|----------|----------------|
| `dbms()` | Entry point called after login. Ensures the user's directory exists, then runs the workspace menu loop. |
| `ensure_user_dir()` | Creates `data/users/` and `data/users/<username>/` if they do not already exist. |
| `list_datasets()` | Reads the user's directory with `opendir` and prints all dataset filenames. |
| `create_dataset()` | Prompts for a filename and a comma-separated field list, converts commas to `\|`, and writes the header line to a new `.strux` file. |
| `open_dataset()` | Resolves a filename to a full path, verifies it exists, and hands off to `dataset_menu()`. |
| `dataset_menu()` | Inner menu loop for an open dataset: Add / Search / Show all / Back. |
| `dataset_add_entry()` | Reads the schema from the file header, prompts for each field value, and appends a pipe-delimited record line. |
| `dataset_search()` | Full-text scan of all record lines using `strstr`; prints matching records with field labels. |
| `dataset_show_entries()` | Reads and pretty-prints every record in the file with field labels. |
| `parse_fields()` | Tokenises a pipe-delimited header line into a field-name array using `strtok`. |
| `read_line()` | Thin wrapper around `fgets` that strips the trailing newline. |

---

## Data Layout

```
data/
├── credentials.strux          # One "username password" pair per line (plaintext)
└── users/
    └── <username>/
        ├── phonebook.strux  # Example dataset
        └── inventory.strux  # Another dataset
```

### `data/credentials.strux`

Each line holds one registered account in the form:

```
username password
```

Passwords are stored in plaintext in the current version. See [Future Goals](../README.md#future-goals) for the planned hashing upgrade.

### `data/users/<username>/`

Created automatically on first login. Contains one file per dataset the user has created.

### `.strux` Dataset File Format (current)

The first line of every `.strux` file is the schema header — a pipe-delimited list of field names written when the dataset is created:

```
name|phone|email
```

Every subsequent line is a data record in the same pipe-delimited format:

```
Alice|555-1234|alice@example.com
Bob|555-5678|bob@example.com
```

> **Planned:** The `.strux` format will be replaced by an encrypted binary format in a future release. See [Future Goals](../README.md#future-goals) for the full specification.

---

## Control Flow

```
main()
 ├── registr()
 │    ├── analyse_username()
 │    ├── usercheck()
 │    └── analyse_password()
 │
 └── login()
      └── dbms(username)
           ├── ensure_user_dir()
           ├── list_datasets()
           ├── create_dataset()
           └── open_dataset()
                └── dataset_menu()
                     ├── dataset_add_entry()
                     ├── dataset_search()
                     └── dataset_show_entries()
```

---

## Error Handling Conventions

Functions return one of three values:

| Return | Meaning |
|--------|---------|
| `0` | Success |
| `1` | Recoverable user error (invalid input, not found, etc.) — caller should retry or continue |
| `2` | Fatal error (file I/O failure, unexpected system error) — propagated up to `main`, which terminates with exit code `2` |

---

## Known Limitations & Planned Changes

| Area | Current State | Planned |
|------|--------------|---------|
| Password storage | Plaintext in `credentials.strux` | Argon2id hashing via libsodium |
| Dataset storage | Plaintext pipe-delimited `.strux` | AES-256-GCM encrypted binary `.strux` |
| Networking | None (local only) | TCP client–server with TLS |
| Concurrency | None | File locking or a server-side queue |
| Entry mutation | Add only | Update and delete operations |

See [README — Future Goals](../README.md#future-goals) for full detail.
