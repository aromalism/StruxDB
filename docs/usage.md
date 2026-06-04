# Usage Guide

## Prerequisites

- GCC or any C99-compatible compiler
- GNU Make
- A POSIX-compatible OS (Linux or macOS)

---

## Building

Run the following from the repository root:

```bash
make
```

This compiles `src/regndlog.c` and `src/dbms.c` into `bin/dbms`.

To remove the compiled binary:

```bash
make clean
```

---

## Running

```bash
./bin/dbms
```

The application creates the `data/` directory and all required subdirectories automatically on first use.

---

## Main Menu

On launch you are presented with the main menu:

```
Welcome to Strux Database

-----
menu
-----
1. Register
2. Login
3. Exit
```

---

## Registering an Account

Select **1** from the main menu.

**Username rules:**

- 3 to 32 characters
- Letters, digits, underscores (`_`), and periods (`.`) only
- Cannot begin with a digit or underscore
- Cannot be entirely digits
- Case-insensitive (stored and matched in lowercase)

**Password rules:**

- 8 to 128 characters
- At least one uppercase letter (A–Z)
- At least one lowercase letter (a–z)
- At least one digit (0–9)
- At least one symbol (e.g. `!`, `@`, `#`, `$`)
- No whitespace
- Cannot be identical to the username

If any rule is violated the application will explain which requirement was not met and prompt again. After a valid password is entered you will be asked to re-enter it to confirm. Once both entries match, the account is saved and you are returned to the main menu.

> **Warning:** Passwords are stored in plaintext. Do not use a real or important password.

---

## Logging In

Select **2** from the main menu.

Enter your registered username and password. The username match is case-insensitive. On success you are taken to your personal workspace.

---

## Workspace

Each user has an isolated workspace stored under `data/users/<username>/`. From here you can manage your datasets.

```
------
Workspace
------
1. List datasets
2. Create dataset
3. Open dataset
4. Exit
```

### List Datasets

Displays all `.strux` dataset files in your workspace. If no datasets exist yet, a message is shown.

### Create Dataset

1. Enter a filename for the new dataset (e.g. `phonebook.strux`).
   - Must be a simple name with no path separators (`/`) or `..` sequences.
   - The `.strux` extension is recommended but not enforced.
2. Enter the field names as a comma-separated list (e.g. `name,phone,email`).

The dataset file is created immediately with the schema written as the first line. No entries are added at creation time.

### Open Dataset

Lists your datasets and prompts for the name of one to open. On success you enter the dataset menu (see below).

### Exit

Returns to the main menu.

---

## Dataset Menu

Once a dataset is open you can work with its records.

```
------
Dataset menu
------
1. Add entry
2. Search entries
3. Show all entries
4. Back
```

### Add Entry

Prompts for each field defined in the dataset schema. Enter the value for each field when asked. Field values cannot contain the `|` character (it is used as the internal delimiter).

Example session for a `name|phone|email` schema:

```
Enter name: Alice
Enter phone: 555-1234
Enter email: alice@example.com
Entry added.
```

### Search Entries

Prompts for a search query. The query is matched case-sensitively against the raw content of every record line. Any record containing the query string in any field is displayed with full field labels.

```
Enter search query: Alice

Entry:
name: Alice
phone: 555-1234
email: alice@example.com
```

### Show All Entries

Displays every record in the dataset, numbered and labelled by field.

```
Entry 1:
name: Alice
phone: 555-1234
email: alice@example.com

Entry 2:
name: Bob
phone: 555-5678
email: bob@example.com
```

### Back

Returns to the workspace menu without closing the dataset file.

---

## Data Storage Reference

| Path | Contents |
|------|----------|
| `data/credentials.txt` | Registered usernames and passwords, one per line |
| `data/users/<username>/` | Your personal dataset directory |
| `data/users/<username>/*.strux` | Your dataset files |

---

## Notes & Caveats

- **Plaintext storage** — both credentials and dataset entries are stored as plain text. Do not store sensitive personal data.
- **No edit or delete** — once an entry is added it cannot be modified or removed through the application. To remove records you can edit the `.strux` file directly in a text editor (each line after the header is one record).
- **Case-insensitive usernames** — usernames are normalised to lowercase at registration and login, so `Alice` and `alice` refer to the same account.
- **Search is case-sensitive** — the search query must match the stored value exactly in terms of case. Searching `alice` will not match `Alice`.
- **No concurrent access** — opening the same dataset from two processes simultaneously is unsafe and may corrupt the file.
