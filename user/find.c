#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *
fmtname(char *path)
{
    // static char buf[DIRSIZ + 1];
    char *p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    // if (strlen(p) >= DIRSIZ)
    //     return p;
    // memmove(buf, p, strlen(p));
    // memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
    return p;
}

void find(char *path, char *filename)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // open and get its metadata then
    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    // get info
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type)
    {
    // case T_FILE:
    // {
    //     if (strcmp(fmtname(path), filename) == 0)//slower
    //     {
    //         printf("%s\n", path);
    //     }
    //     break;
    // }

    case T_DIR:
    {
        // curr path + '/'+next level + '\0'
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        // next level
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue;
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
            {
                continue;
            }
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0)
            {
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            if (st.type == T_DIR)
            {
            find(buf, filename);
            }
            else if(st.type== T_FILE&&strcmp(de.name,filename)==0)
            {
                printf("%s\n",buf);
            }
        }
        break;
    }
    }
    close(fd);
}

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        fprintf(2, "Usage:find <directory> <file>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
