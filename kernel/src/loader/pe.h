#ifndef PE_H
#define PE_H

// DOS Header
typedef struct {
    unsigned short e_magic;     // MZ
    unsigned short e_cblp;
    unsigned short e_cp;
    unsigned short e_crlc;
    unsigned short e_cparhdr;
    unsigned short e_minalloc;
    unsigned short e_maxalloc;
    unsigned short e_ss;
    unsigned short e_sp;
    unsigned short e_csum;
    unsigned short e_ip;
    unsigned short e_cs;
    unsigned short e_lfarlc;
    unsigned short e_ovno;
    unsigned short e_res[4];
    unsigned short e_oemid;
    unsigned short e_oeminfo;
    unsigned short e_res2[10];
    unsigned long e_lfanew;
} IMAGE_DOS_HEADER;

// PE Header
typedef struct {
    unsigned long Signature;    // PE\0\0
    unsigned short Machine;
    unsigned short NumberOfSections;
    unsigned long TimeDateStamp;
    unsigned long PointerToSymbolTable;
    unsigned long NumberOfSymbols;
    unsigned short SizeOfOptionalHeader;
    unsigned short Characteristics;
} IMAGE_FILE_HEADER;

// 函数声明
int pe_load(const char* filename, void** entry);

#endif