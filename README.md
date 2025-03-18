# Smol DB

A simple command-line program to manage an employee database. The program allows you to add employees, remove employees, and list all employees in the database.

## Features

- Add new employees with name, address and hours worked
- Remove existing employees
- List all employees in the database
- Persistent storage using binary file format

## Building

To build the program, run `make`.

## Run

The program can be run using the following command format:

```bash
./bin/dbview [options]
```

Available options:

- `-f <filename>` : Specify the database file (e.g., ./mynewdb.db)
- `-n` : Create a new database
- `-a "<name>, <address>, <hours>"` : Add a new employee
- `-r <name>` : Remove an employee by name
- `-l` : List all employees in the database

Examples:

```bash
# Create a new database
./bin/dbview -f ./mynewdb.db -n

# Add an employee
./bin/dbview -f ./mynewdb.db -a "Jean M., 12 rue de la Poterie, 200"

# Remove an employee
./bin/dbview -f ./mynewdb.db -r "Jean M., 12 rue de la Poterie, 200"

# List all employees
./bin/dbview -f ./mynewdb.db -l
```
