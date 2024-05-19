#include "cfg.h"
#include "csrs.h"
#include "processor.h"
#include "encoding.h"
#include "mmu.h"

#define MD    63
#define SID   16
#define ENTRY 16

#define ALL_MD_ASSOCIATED 0xFFFFFFFFFFFFFFFE
#define ALL_ONE_BITS      0xFFFFFFFFFFFFFFFF

#define SRCMD_L    0x1
#define MDCFGLCK_L 0x1
#define ENTRYLCK_L 0x1

#define MDCFGLCK_F_SHIFT  1
#define ENTRYLCK_F_SHIFT  1

#define MDCFGLCK_RSV      0xFFFFFFFFFFFFFF00
#define ENTRYLCK_RSV      0xFFFFFFFFFFFE0000

extern int passed_tests;
extern int failed_tests;

#define ASSERT(message, condition) \
    if (!(condition)) { \
        std::cerr << "spike: " << __FILE__ << ":" << __LINE__ << ": " << __func__ << ": " << message << std::endl; \
        std::cerr << "Assertion failed: " << #condition << std::endl; \
        failed_tests++; \
    } else { \
        passed_tests++; \
    } \

cfg_t* create_cfg();
processor_t* create_processor();
mmu_t* create_mmu(processor_t* proc);
srcmd_csr_t_p get_srcmd(processor_t* proc, int srcmd_idx);
mdcfg_csr_t_p get_mdcfg(processor_t* proc, int mdcfg_idx);
mdcfglck_csr_t_p get_mdcfglck(processor_t* proc);
entry_addr_csr_t_p get_entry_addr(processor_t* proc, int entry_addr_idx);
void store_paddr(entry_addr_csr_t_p entry_addr, reg_t paddr);
void write_entry_cfg(processor_t* proc, int entry_cfg_idx, reg_t val);
reg_t read_entry_cfg(processor_t* proc, int entry_cfg_idx);
entrylck_csr_t_p get_entrylck(processor_t* proc);
void end_test();