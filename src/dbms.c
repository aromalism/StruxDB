/*
 * Project: Strux Database.
 * File: dbms.c ( Database Management System );
 * Author: Aromal J
 *
 * Purpose: Handles Userdata Management
 *
 */

//================================================= HEADERS ========================================================

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>

//================================================= GLOBAL  ========================================================

#define USER_ROOT "data/users"
#define MAX_PATH 512
#define MAX_FIELDS 32
#define MAX_FIELD_NAME 64
#define MAX_FIELD_VALUE 256
#define MAX_LINE 2048

//================================================= PROTOTYPES =====================================================

int dbms(char *name);
int ensure_user_dir(char *name, char *user_dir, size_t size);
void list_datasets(const char *user_dir);
int create_dataset(const char *user_dir);
int open_dataset(const char *user_dir);
int dataset_menu(const char *dataset_path);
int dataset_add_entry(const char *dataset_path);
int dataset_search(const char *dataset_path);
int dataset_show_entries(const char *dataset_path);
int parse_fields(char *line, char fields[MAX_FIELDS][MAX_FIELD_NAME], int *count);
int read_line(char *buffer, size_t size);

//================================================ DBMS ===========================================================

int dbms(char *name)
{
    char input[32];
    char user_dir[MAX_PATH];

    if (ensure_user_dir(name, user_dir, sizeof(user_dir)) != 0)
    {
        return 2;
    }

    printf("\nWelcome to %s's workspace. Your files are stored in %s\n", name, user_dir);

    while (1)
    {
        printf("\n------\nWorkspace\n------\n1.List datasets\n2.Create dataset\n3.Open dataset\n4.Exit\n\nOption: ");
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }

        int option = 0;
        if (sscanf(input, "%d", &option) != 1)
        {
            printf("Invalid option. Please enter a valid number! [1/2/3/4]\n");
            continue;
        }

        if (option == 1)
        {
            list_datasets(user_dir);
        }
        else if (option == 2)
        {
            create_dataset(user_dir);
        }
        else if (option == 3)
        {
            if (open_dataset(user_dir) == 2)
            {
                return 2;
            }
        }
        else if (option == 4)
        {
            return 0;
        }
        else
        {
            printf("Invalid option!\n");
        }
    }

    return 0;
}

//======================================== ENSURE USER DIRECTORIES =================================================

int ensure_user_dir(char *name, char *user_dir, size_t size)
{
    snprintf(user_dir, size, "%s/%s", USER_ROOT, name);

    struct stat st;
    if (stat(USER_ROOT, &st) != 0)
    {
        if (errno == ENOENT)
        {
            if (mkdir(USER_ROOT, 0755) != 0)
            {
                perror("Unable to create user root directory");
                return 2;
            }
        }
        else
        {
            perror("Unable to access user root directory");
            return 2;
        }
    }
    else if (!S_ISDIR(st.st_mode))
    {
        fprintf(stderr, "%s exists and is not a directory\n", USER_ROOT);
        return 2;
    }

    if (stat(user_dir, &st) != 0)
    {
        if (errno == ENOENT)
        {
            if (mkdir(user_dir, 0755) != 0)
            {
                perror("Unable to create user directory");
                return 2;
            }
        }
        else
        {
            perror("Unable to access user directory");
            return 2;
        }
    }
    else if (!S_ISDIR(st.st_mode))
    {
        fprintf(stderr, "%s exists and is not a directory\n", user_dir);
        return 2;
    }

    return 0;
}

//============================================ LIST DATASETS =======================================================

void list_datasets(const char *user_dir)
{
    DIR *dir = opendir(user_dir);
    if (dir == NULL)
    {
        perror("Unable to open user directory");
        return;
    }

    printf("\nDatasets in your workspace:\n");
    struct dirent *entry;
    int count = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.')
        {
            continue;
        }
        printf("- %s\n", entry->d_name);
        count++;
    }
    closedir(dir);

    if (count == 0)
    {
        printf("(no datasets yet)\n");
    }
}

//=========================================== CREATE DATASETS ======================================================

int create_dataset(const char *user_dir)
{
    char filename[128];
    char header_line[MAX_LINE];
    char path[MAX_PATH];

    printf("\nEnter dataset file name (for example phonebook.strux): ");
    if (read_line(filename, sizeof(filename)) != 0)
    {
        return 0;
    }

    if (filename[0] == '\0' || strchr(filename, '/') != NULL || strstr(filename, "..") != NULL)
    {
        printf("Invalid file name. Use a simple name without path separators.\n");
        return 0;
    }

    snprintf(path, sizeof(path), "%s/%s", user_dir, filename);
    FILE *file = fopen(path, "r");
    if (file != NULL)
    {
        fclose(file);
        printf("A dataset with that name already exists. Choose a different name.\n");
        return 0;
    }

    printf("Enter field names separated by commas (for example name,phone,email): ");
    if (read_line(header_line, sizeof(header_line)) != 0)
    {
        return 0;
    }

    if (header_line[0] == '\0')
    {
        printf("Field names cannot be empty.\n");
        return 0;
    }

    file = fopen(path, "w");
    if (file == NULL)
    {
        perror("Unable to create dataset file");
        return 2;
    }

    for (char *p = header_line; *p; ++p)
    {
        if (*p == ',')
        {
            *p = '|';
        }
    }

    fprintf(file, "%s\n", header_line);
    fclose(file);

    printf("Dataset '%s' created with schema: %s\n", filename, header_line);
    return 0;
}

//============================================ OPEN DATASETS =======================================================

int open_dataset(const char *user_dir)
{
    char filename[128];
    char path[MAX_PATH];

    list_datasets(user_dir);
    printf("\nEnter dataset file name to open: ");
    if (read_line(filename, sizeof(filename)) != 0)
    {
        return 0;
    }

    if (filename[0] == '\0')
    {
        printf("Dataset name cannot be empty.\n");
        return 0;
    }

    snprintf(path, sizeof(path), "%s/%s", user_dir, filename);
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        if (errno == ENOENT)
        {
            printf("Dataset '%s' does not exist.\n", filename);
            return 0;
        }
        perror("Unable to open dataset file");
        return 2;
    }
    fclose(file);

    return dataset_menu(path);
}

//============================================= DATASET MENU =======================================================

int dataset_menu(const char *dataset_path)
{
    char input[32];

    printf("\nOpened dataset: %s\n", dataset_path);

    while (1)
    {
        printf("\n------\nDataset menu\n------\n1.Add entry\n2.Search entries\n3.Show all entries\n4.Back\n\nOption: ");
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }

        int option = 0;
        if (sscanf(input, "%d", &option) != 1)
        {
            printf("Invalid option. Please enter a valid number! [1/2/3/4]\n");
            continue;
        }

        if (option == 1)
        {
            dataset_add_entry(dataset_path);
        }
        else if (option == 2)
        {
            dataset_search(dataset_path);
        }
        else if (option == 3)
        {
            dataset_show_entries(dataset_path);
        }
        else if (option == 4)
        {
            break;
        }
        else
        {
            printf("Invalid option!\n");
        }
    }

    return 0;
}

//========================================== DATASET ADD ENTRY =====================================================

int dataset_add_entry(const char *dataset_path)
{
    FILE *file = fopen(dataset_path, "r");
    if (file == NULL)
    {
        perror("Unable to open dataset file");
        return 2;
    }

    char header[MAX_LINE];
    if (fgets(header, sizeof(header), file) == NULL)
    {
        fclose(file);
        printf("Dataset file is empty or corrupted.\n");
        return 0;
    }
    fclose(file);

    char fields[MAX_FIELDS][MAX_FIELD_NAME];
    int field_count = 0;
    if (parse_fields(header, fields, &field_count) != 0 || field_count == 0)
    {
        printf("Invalid dataset schema.\n");
        return 0;
    }

    char values[MAX_FIELD_VALUE];
    char buffer[MAX_LINE] = "";
    for (int i = 0; i < field_count; i++)
    {
        printf("Enter %s: ", fields[i]);
        if (read_line(values, sizeof(values)) != 0)
        {
            return 0;
        }
        if (strchr(values, '|') != NULL)
        {
            printf("Field values cannot contain the '|' character.\n");
            return 0;
        }
        if (i > 0)
        {
            strncat(buffer, "|", sizeof(buffer) - strlen(buffer) - 1);
        }
        strncat(buffer, values, sizeof(buffer) - strlen(buffer) - 1);
    }

    FILE *out = fopen(dataset_path, "a");
    if (out == NULL)
    {
        perror("Unable to open dataset file for append");
        return 2;
    }

    fprintf(out, "%s\n", buffer);
    fclose(out);
    printf("Entry added.\n");
    return 0;
}

//============================================ DATASET SEARCH ======================================================

int dataset_search(const char *dataset_path)
{
    char query[MAX_FIELD_VALUE];
    printf("\nEnter search query: ");
    if (read_line(query, sizeof(query)) != 0)
    {
        return 0;
    }
    if (query[0] == '\0')
    {
        printf("Search query cannot be empty.\n");
        return 0;
    }

    FILE *file = fopen(dataset_path, "r");
    if (file == NULL)
    {
        perror("Unable to open dataset file");
        return 2;
    }

    char header[MAX_LINE];
    if (fgets(header, sizeof(header), file) == NULL)
    {
        fclose(file);
        printf("Dataset file is empty.\n");
        return 0;
    }

    char fields[MAX_FIELDS][MAX_FIELD_NAME];
    int field_count = 0;
    parse_fields(header, fields, &field_count);

    char line[MAX_LINE];
    int found = 0;
    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, query) != NULL)
        {
            found = 1;
            char row[MAX_LINE];
            strncpy(row, line, sizeof(row) - 1);
            row[sizeof(row) - 1] = '\0';
            char *token = strtok(row, "|\n");
            int index = 0;
            printf("\nEntry:\n");
            while (token != NULL && index < field_count)
            {
                printf("%s: %s\n", fields[index], token);
                token = strtok(NULL, "|\n");
                index++;
            }
        }
    }

    fclose(file);
    if (!found)
    {
        printf("No matching entries found.\n");
    }
    return 0;
}

//========================================= DATASET SHOW ENTRIES ===================================================

int dataset_show_entries(const char *dataset_path)
{
    FILE *file = fopen(dataset_path, "r");
    if (file == NULL)
    {
        perror("Unable to open dataset file");
        return 2;
    }

    char header[MAX_LINE];
    if (fgets(header, sizeof(header), file) == NULL)
    {
        fclose(file);
        printf("Dataset file is empty.\n");
        return 0;
    }

    char fields[MAX_FIELDS][MAX_FIELD_NAME];
    int field_count = 0;
    parse_fields(header, fields, &field_count);

    char line[MAX_LINE];
    int entry_number = 0;
    while (fgets(line, sizeof(line), file))
    {
        entry_number++;
        printf("\nEntry %d:\n", entry_number);
        char row[MAX_LINE];
        strncpy(row, line, sizeof(row) - 1);
        row[sizeof(row) - 1] = '\0';
        char *token = strtok(row, "|\n");
        int index = 0;
        while (token != NULL && index < field_count)
        {
            printf("%s: %s\n", fields[index], token);
            token = strtok(NULL, "|\n");
            index++;
        }
    }

    fclose(file);
    if (entry_number == 0)
    {
        printf("No entries found in this dataset.\n");
    }
    return 0;
}

//============================================= PARSE FIELDS =======================================================

int parse_fields(char *line, char fields[MAX_FIELDS][MAX_FIELD_NAME], int *count)
{
    *count = 0;
    char *token = strtok(line, "|\n");
    while (token != NULL && *count < MAX_FIELDS)
    {
        strncpy(fields[*count], token, MAX_FIELD_NAME - 1);
        fields[*count][MAX_FIELD_NAME - 1] = '\0';
        (*count)++;
        token = strtok(NULL, "|\n");
    }
    return 0;
}

//================================================ READ LINE ===========================================================

int read_line(char *buffer, size_t size)
{
    if (fgets(buffer, size, stdin) == NULL)
    {
        return 1;
    }
    buffer[strcspn(buffer, "\n")] = 0;
    return 0;
}

//================================================ END ===========================================================
