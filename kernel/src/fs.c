// kernel/src/fs.c - 简单文件系统
#include "terminal.h"
#include "fs.h"

#define NULL 0

static filesystem_t fs;

static char* itoa_local(int num) {
    static char buf[32];
    int i = 0;
    if (num == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }
    if (num < 0) {
        buf[i++] = '-';
        num = -num;
    }
    char temp[32];
    int j = 0;
    while (num > 0) {
        temp[j++] = '0' + (num % 10);
        num /= 10;
    }
    while (j > 0) {
        buf[i++] = temp[--j];
    }
    buf[i] = '\0';
    return buf;
}

static int str_cmp(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++; b++;
    }
    return *a == *b;
}

static void str_cpy(char* dest, const char* src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void fs_init() {
    fs.file_count = 0;
    fs.current_dir = -1;
    
    file_entry_t* root = &fs.files[fs.file_count++];
    root->type = FILE_TYPE_DIR;
    root->size = 0;
    root->start_sector = 0;
    root->parent = -1;
    str_cpy(root->name, "/");
    
    for (int i = 0; i < 1024 * 1024; i++) {
        fs.disk[i] = 0;
    }
    
    fs_create_dir("home");
    fs_cd("home");
    fs_create_dir("user");
    fs_cd("/");
    fs_create_file("README.txt");
    fs_create_file("hello.txt");
    fs_create_dir("bin");
    fs_create_dir("etc");
    fs_create_file("kernel.elf");
    fs_create_file("shell.c");
    fs_create_file("fs.c");
    
    terminal_write("Filesystem initialized\n");
    terminal_write("  Created: /, /home, /home/user, /README.txt, /hello.txt, /bin, /etc\n");
}

static int find_file(const char* name) {
    for (int i = 0; i < fs.file_count; i++) {
        if (fs.files[i].parent != fs.current_dir) continue;
        if (fs.files[i].parent == -2) continue;
        if (str_cmp(fs.files[i].name, name)) {
            return i;
        }
    }
    return -1;
}

int fs_create_file(const char* name) {
    if (fs.file_count >= MAX_FILES) return -1;
    if (find_file(name) != -1) return -1;
    
    file_entry_t* f = &fs.files[fs.file_count++];
    str_cpy(f->name, name);
    f->type = FILE_TYPE_FILE;
    f->size = 0;
    f->start_sector = fs.file_count * 512;
    f->parent = fs.current_dir;
    return 0;
}

int fs_create_dir(const char* name) {
    if (fs.file_count >= MAX_FILES) return -1;
    if (find_file(name) != -1) return -1;
    
    file_entry_t* f = &fs.files[fs.file_count++];
    str_cpy(f->name, name);
    f->type = FILE_TYPE_DIR;
    f->size = 0;
    f->start_sector = 0;
    f->parent = fs.current_dir;
    return 0;
}

int fs_delete(const char* name) {
    int idx = find_file(name);
    if (idx == -1) return -1;
    fs.files[idx].parent = -2;
    return 0;
}

int fs_open(const char* name) {
    return find_file(name);
}

int fs_read(int fd, char* buf, int size) {
    if (fd < 0 || fd >= fs.file_count) return -1;
    file_entry_t* f = &fs.files[fd];
    if (f->type != FILE_TYPE_FILE) return -1;
    
    int read_size = size < f->size ? size : f->size;
    for (int i = 0; i < read_size; i++) {
        buf[i] = fs.disk[f->start_sector + i];
    }
    return read_size;
}

int fs_write(int fd, const char* buf, int size) {
    if (fd < 0 || fd >= fs.file_count) return -1;
    file_entry_t* f = &fs.files[fd];
    if (f->type != FILE_TYPE_FILE) return -1;
    
    int write_size = size < MAX_FILE_SIZE ? size : MAX_FILE_SIZE;
    for (int i = 0; i < write_size; i++) {
        fs.disk[f->start_sector + i] = buf[i];
    }
    f->size = write_size;
    return write_size;
}

void fs_list_files() {
    int count = 0;
    terminal_write("\n");
    for (int i = 0; i < fs.file_count; i++) {
        if (fs.files[i].parent != fs.current_dir) continue;
        if (fs.files[i].parent == -2) continue;
        
        count++;
        if (fs.files[i].type == FILE_TYPE_DIR) {
            terminal_write("[DIR]  ");
        } else {
            terminal_write("[FILE] ");
        }
        terminal_write(fs.files[i].name);
        
        if (fs.files[i].type == FILE_TYPE_FILE) {
            terminal_write("  (");
            terminal_write(itoa_local(fs.files[i].size));
            terminal_write(" bytes)");
        }
        terminal_write("\n");
    }
    if (count == 0) {
        terminal_write("  (empty)\n");
    }
    terminal_write("\n");
}

void fs_cd(const char* name) {
    if (name[0] == '/' && name[1] == '\0') {
        fs.current_dir = -1;
        return;
    }
    if (name[0] == '.' && name[1] == '.' && name[2] == '\0') {
        if (fs.current_dir != -1) {
            fs.current_dir = fs.files[fs.current_dir].parent;
        }
        return;
    }
    int idx = find_file(name);
    if (idx != -1 && fs.files[idx].type == FILE_TYPE_DIR) {
        fs.current_dir = idx;
    } else {
        terminal_write("Directory not found: ");
        terminal_write(name);
        terminal_write("\n");
    }
}

// ========== 修复 fs_pwd（防止死循环） ==========
void fs_pwd() {
    if (fs.current_dir == -1) {
        terminal_write("/");
        return;
    }
    
    char parts[16][32];
    int part_count = 0;
    int current = fs.current_dir;
    int max_depth = 16;
    
    while (current != -1 && max_depth > 0) {
        if (current < 0 || current >= MAX_FILES) break;
        str_cpy(parts[part_count++], fs.files[current].name);
        current = fs.files[current].parent;
        max_depth--;
    }
    
    for (int i = part_count - 1; i >= 0; i--) {
        terminal_write("/");
        terminal_write(parts[i]);
    }
}

file_entry_t* fs_get_children(int* count) {
    static file_entry_t* children[MAX_FILES];
    *count = 0;
    for (int i = 0; i < fs.file_count; i++) {
        if (fs.files[i].parent == fs.current_dir && fs.files[i].parent != -2) {
            children[(*count)++] = &fs.files[i];
        }
    }
    return (file_entry_t*)children;
}

int fs_get_file_count() {
    return fs.file_count;
}

file_entry_t* fs_get_file(int index) {
    if (index < 0 || index >= fs.file_count) return NULL;
    return &fs.files[index];
}