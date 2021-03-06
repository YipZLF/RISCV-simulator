/* loader.cpp - the ELF file loader
 * Author: Ye Zilingfeng
 */
#include "loader.h"

// Mapping from vaddr to paddr is identical mapping
int loadSegment(CPU* cpu,
                FILE* fd, 
                unsigned long offset, 
                unsigned long filesz, 
                unsigned long vaddr){
  if(vaddr + filesz > MEMORY_SIZE){
    printf("ERROR: memory too small! needed %lu but only got %d\n",vaddr+filesz, MEMORY_SIZE);
    return ERROR;
  }
  fseek(fd, offset, SEEK_SET);
  fread(cpu->memory->mem + vaddr, 1, filesz, fd);
  #ifdef DEBUG
  for(int j = 0 ;j < filesz; j+=8){
    printf("@0x%lx:\t",vaddr+j);
    for(int i = 0; i < 4; ++i)
      printf("%02x%02x ",cpu->memory->mem[vaddr+j+i*2],cpu->memory->mem[vaddr+j+i*2+1]);
    printf("\n");
  }
  #endif

  return 0;
}

int loadELF(CPU* cpu, FILE* elf_file){
  Elf64_Ehdr elf_hdr;
  Elf64_Phdr elf_phdr;

  memset(&elf_hdr, 0 ,sizeof(Elf64_Ehdr));
  if( fread(&elf_hdr,1,sizeof(Elf64_Ehdr),elf_file) < sizeof(Elf64_Ehdr))
    return ERROR; // sanity check;


  // check elf header here


  int prog_cnt = elf_hdr.e_phnum;
  int ph_off = elf_hdr.e_phoff;
  int ph_size = elf_hdr.e_phentsize;

  for(int id=0; id < prog_cnt; ++id){
    memset(&elf_phdr, 0 ,sizeof(Elf64_Phdr));
    fseek(elf_file, ph_off, SEEK_SET);

    if( fread(&elf_phdr, 1, sizeof(Elf64_Phdr), elf_file) < sizeof(Elf64_Phdr)){
      printf("Failed to read prog headers.\n");
      return ERROR;
    }

    if( PT_LOAD == elf_phdr.p_type ){// if this seg should be loaded into mem
      if(loadSegment(cpu, elf_file, elf_phdr.p_offset, elf_phdr.p_filesz, elf_phdr.p_vaddr) == ERROR)  {
        printf("Failed to load segments\n");
        return ERROR;
      }
    }
    ph_off += elf_hdr.e_phentsize;
  }

  //assign heap pointer;
  cpu->setStackPtr(MEMORY_SIZE);//assign stack pointer;
  cpu->setEntry(elf_hdr.e_entry);//set entry point;
  fclose(elf_file);
  return 0;
}
