#include <iostream>
#include "mmu-tests.h"
#include "util.h"

// iopmp ok

void test_iopmp_ok() {
    processor_t* proc              = create_processor();
    srcmd_csr_t_p srcmd0           = get_srcmd(proc, 0);
    mdcfg_csr_t_p mdcfg0           = get_mdcfg(proc, 0);
    entry_addr_csr_t_p entry_addr0 = get_entry_addr(proc, 0);
    mmu_t* mmu                     = create_mmu(proc);

    //Source id 0
    reg_t sid = 0;
    //Associate source id 0 with memory domain 0
    srcmd0->write(1 << SRCMD_BITMAP_BASE);
    //Memory domain 0 owns entry 0
    mdcfg0->write(1);
    //Entry 0 configured as TOR and read only
    write_entry_cfg(proc, 0, ENTRY_CFG_TOR | ENTRY_CFG_R);
    //Entry 0 has a physical top address of 0xC (matching [0, 12))
    store_paddr(entry_addr0, 0xC);

    assert(("When the first srcmd, mdcfg and entry addr are linked, and the entry addr and configuration match the transaction, the transaction is legal",  mmu->iopmp_ok(sid, 0, 12, LOAD)));
    assert(("When the first srcmd, mdcfg and entry addr are linked, and the configuration does not match the transaction, the transaction is illegal",     !mmu->iopmp_ok(sid, 0, 12, STORE)));
    assert(("When the first srcmd, mdcfg and entry addr are linked, and the entry addr (TOR) does not match the transaction, the transaction is illegal",  !mmu->iopmp_ok(sid, 0, 13, LOAD)));
    assert(("When the sid is equal to the maximum value for uint64_t and the transaction is illegal according to the iopmp csrs, the transaction is legal", mmu->iopmp_ok(UINT64_MAX, 0, 13, LOAD)));
}

void test_iopmp_ok_source_linked_with_no_md() {
    processor_t* proc              = create_processor();
    srcmd_csr_t_p srcmd0           = get_srcmd(proc, 0);
    mdcfg_csr_t_p mdcfg0           = get_mdcfg(proc, 0);
    entry_addr_csr_t_p entry_addr0 = get_entry_addr(proc, 0);
    mmu_t* mmu                     = create_mmu(proc);

    //Source id 0
    reg_t sid = 0;
    //Associate source id 0 with no memory domain
    srcmd0->write(0);
    //Memory domain 0 owns entry 0
    mdcfg0->write(1);
    //Entry 0 configured as TOR and read only
    write_entry_cfg(proc, 0, ENTRY_CFG_TOR | ENTRY_CFG_R);
    //Entry 0 has a physical top address of 0xC (matching [0, 12))
    store_paddr(entry_addr0, 0xC);

    assert(("When the first source is linked with no memory domain, the transaction is illegal", !mmu->iopmp_ok(sid, 0, 12, LOAD)));
}

void test_iopmp_ok_md_linked_with_no_entry() {
    processor_t* proc              = create_processor();
    srcmd_csr_t_p srcmd0           = get_srcmd(proc, 0);
    mdcfg_csr_t_p mdcfg0           = get_mdcfg(proc, 0);
    entry_addr_csr_t_p entry_addr0 = get_entry_addr(proc, 0);
    mmu_t* mmu                     = create_mmu(proc);

    //Source id 0
    reg_t sid = 0;
    //Associate source id 0 with memory domain 0
    srcmd0->write(1 << SRCMD_BITMAP_BASE);
    //Memory domain 0 owns no entry
    mdcfg0->write(0);
    //Entry 0 configured as TOR and read only
    write_entry_cfg(proc, 0, ENTRY_CFG_TOR | ENTRY_CFG_R);
    //Entry 0 has a physical top address of 0xC (matching [0, 12))
    store_paddr(entry_addr0, 0xC);

    assert(("When the first memory domain is linked with no entry, the transaction is illegal", !mmu->iopmp_ok(sid, 0, 12, LOAD)));
}

void test_iopmp_ok_two_entries_match() {
    processor_t* proc              = create_processor();
    srcmd_csr_t_p srcmd0           = get_srcmd(proc, 0);
    mdcfg_csr_t_p mdcfg0           = get_mdcfg(proc, 0);
    entry_addr_csr_t_p entry_addr0 = get_entry_addr(proc, 0);
    entry_addr_csr_t_p entry_addr1 = get_entry_addr(proc, 1);
    entry_addr_csr_t_p entry_addr2 = get_entry_addr(proc, 2);
    mmu_t* mmu                     = create_mmu(proc);

    //Source id 0
    reg_t sid = 0;
    //Associate source id 0 with memory domain 0
    srcmd0->write(1 << SRCMD_BITMAP_BASE);
    //Memory domain 0 owns entry 0,1
    mdcfg0->write(2);
    //Entry 0 configured as TOR and read only
    write_entry_cfg(proc, 0, ENTRY_CFG_TOR | ENTRY_CFG_R);
    //Entry 0 has a physical top address of 0xC (matching [0, 12))
    store_paddr(entry_addr0, 0xC);
    //Entry 1 configured as TOR and write only
    write_entry_cfg(proc, 1, ENTRY_CFG_TOR | ENTRY_CFG_W);
    //Entry 1 has a physical top address of 0x0
    store_paddr(entry_addr1, 0x0);
    //Entry 2 configured as TOR and write only
    write_entry_cfg(proc, 2, ENTRY_CFG_TOR | ENTRY_CFG_W);
    //Entry 2 has a physical top address of 0xC (matching [0, 12))
    store_paddr(entry_addr2, 0xC);

    assert(("When the two entries match the same address range and the transaction is legal according to the first entry, the transaction is legal",  mmu->iopmp_ok(sid, 0, 12, LOAD)));
    assert(("When the two entries match the same address range and the transaction is legal according to the second entry, the transaction is legal", mmu->iopmp_ok(sid, 0, 12, STORE)));
}


void run_mmu_tests() {
    std::cout << "Running MMU tests" << std::endl;
}