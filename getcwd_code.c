#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *my_getcwd(char *buffer, size_t size)
{
    ino_t current_inode;
    dev_t current_device;
    struct stat statbuf;

    // Get inode and device number of current directory
    if (stat(".", &statbuf) != 0)
    {
        perror("Failed to get current directory stats");
        return NULL;
    }
    current_inode = statbuf.st_ino;
    current_device = statbuf.st_dev;

    // Initialize the buffer
    if (buffer == NULL)
    {
        buffer = malloc(size);
        if (buffer == NULL)
        {
            perror("Malloc failed");
            return NULL;
        }
    }
    buffer[0] = '\0';

    // Start with the current directory and move upwards
    // Note: This is a highly simplified version and does not cover all cases
    while (1)
    {
        struct dirent *entry;
        DIR *dir = opendir("..");
        if (dir == NULL)
        {
            perror("Failed to open parent directory");
            return NULL;
        }

        // Try to find current directory entry in parent directory
        int found = 0;
        while ((entry = readdir(dir)) != NULL)
        {
            if (stat(entry->d_name, &statbuf) != 0)
            {
                continue;
            }
            if (statbuf.st_ino == current_inode && statbuf.st_dev == current_device)
            {
                found = 1;
                break;
            }
        }

        if (!found)
        {
            perror("Current directory not found in parent");
            closedir(dir);
            return NULL;
        }

        // Prepend the found directory name to buffer
        size_t len = strlen(entry->d_name);
        if (len + 2 > size)
        { // including '/' and null terminator
            perror("Buffer size too small");
            closedir(dir);
            return NULL;
        }
        memmove(buffer + len + 1, buffer, strlen(buffer) + 1);
        memcpy(buffer, entry->d_name, len);
        buffer[len] = '/';
        buffer[len + 1] = '\0';

        // Move to the parent directory
        if (chdir("..") != 0)
        {
            perror("Failed to change to parent directory");
            closedir(dir);
            return NULL;
        }

        // Update current inode and device to parent's
        if (stat(".", &statbuf) != 0)
        {
            perror("Failed to get parent directory stats");
            closedir(dir);
            return NULL;
        }
        current_inode = statbuf.st_ino;
        current_device = statbuf.st_dev;

        // Check if root directory is reached
        if (current_inode == statbuf.st_ino && current_device == statbuf.st_dev)
        {
            break;
        }

        closedir(dir);
    }

    // Final path adjustment
    if (buffer[0] == '\0')
    {
        strcpy(buffer, "/");
    }
    else
    {
        size_t len = strlen(buffer);
        if (buffer[len - 1] == '/')
        {
            buffer[len - 1] = '\0'; // Remove trailing slash
        }
    }

    return buffer;
}

int main()
{
    char *cwd = my_getcwd(NULL, 1024);
    if (cwd)
    {
        printf("Current directory: %s\n", cwd);
        free(cwd);
    }
    return 0;
}