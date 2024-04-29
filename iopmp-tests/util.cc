#include "cfg.h"
#include "csrs.h"
#include "processor.h"
#include "encoding.h"
#include "mmu.h"
#include "util.h"

cfg_t* create_cfg() {
    cfg_t* cfg = new cfg_t();
    return cfg;
}

//Creates a processor object with dummy values
processor_t* create_processor() {
    cfg_t* cfg = create_cfg();
    cfg->memorydomains = MD;
    cfg->sourceids     = SID;
    cfg->entrynum      = ENTRY;
    //Create dummy values for the other parameters
    isa_parser_t isa = isa_parser_t("", NULL);
    simif_t* sim = nullptr;
    uint32_t id = 0;
    bool halt_on_reset = false;
    FILE* log_file = stdout;
    std::ostream& sout_ = std::cout;
    
    processor_t* proc = new processor_t(&isa, cfg, sim, id, halt_on_reset, log_file, sout_);
    return proc;
}

mmu_t* create_mmu(processor_t* proc) {
    simif_t* sim = nullptr;
    cfg_t* cfg   = create_cfg();
    mmu_t* mmu   = new mmu_t(sim, cfg->endianness, proc, NULL);
    return mmu;
}

srcmd_csr_t_p get_srcmd(processor_t* proc, int srcmd_idx) {
    return proc->get_state()->srcmd[srcmd_idx];
}

mdcfg_csr_t_p get_mdcfg(processor_t* proc, int mdcfg_idx) {
    return proc->get_state()->mdcfg[mdcfg_idx];
}

entry_addr_csr_t_p get_entry_addr(processor_t* proc, int entry_addr_idx) {
    return proc->get_state()->entry_addr[entry_addr_idx];
}

void store_paddr(entry_addr_csr_t_p entry_addr, reg_t paddr) {
    entry_addr->write(paddr>>ENTRY_ADDR_SHIFT);
}

void write_entry_cfg(processor_t* proc, int entry_cfg_idx, reg_t val) {
    proc->put_csr(CSR_ENTRY_CFG0 + entry_cfg_idx * 16, val);
}

reg_t read_entry_cfg(processor_t* proc, int entry_cfg_idx) {
    return proc->get_csr(CSR_ENTRY_CFG0 + entry_cfg_idx * 16);
}