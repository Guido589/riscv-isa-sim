#include <iostream>
#include "csr-tests.h"
#include "cfg.h"
#include "csrs.h"
#include "processor.h"
#include "util.h"

// --------------------- srcmd CSR ---------------------

// Unlogged write

void test_unlogged_write_srcmd() {
    //Create a processor and retrieve srcmd0
    processor_t* proc    = create_processor();
    srcmd_csr_t_p srcmd0 = get_srcmd(proc, 0);
    reg_t val            = ALL_MD_ASSOCIATED;

    srcmd0->write(val);

    assert(("When writing a value to srcmd0, the same value should be read back", srcmd0->read() == val));
}

void test_unlogged_write_to_disabled_srcmd() {
    //Create a processor and retrieve srcmd0, srcmd1, srcmd2
    processor_t* proc    = create_processor();
    srcmd_csr_t_p srcmd0 = get_srcmd(proc, 0);
    srcmd_csr_t_p srcmd1 = get_srcmd(proc, 1);
    srcmd_csr_t_p srcmd2 = get_srcmd(proc, 2);
    //Limit the number of srcmds to 1
    proc->sid_num        = 1;
    reg_t val            = ALL_MD_ASSOCIATED;
    reg_t old_val1       = srcmd1->read();
    reg_t old_val2       = srcmd2->read();

    srcmd0->write(val);
    srcmd1->write(val);
    srcmd2->write(val);

    assert(("When only 1 srcmd csr is enabled, the value should change after writing to the enabled csr srcmd0",            srcmd0->read() == val));
    assert(("When only 1 srcmd csr is enabled, the value should remain unchanged after writing to the disabled csr srcmd1", srcmd1->read() == old_val1));
    assert(("When only 1 srcmd csr is enabled, the value should remain unchanged after writing to the disabled csr srcmd2", srcmd2->read() == old_val2));
}

// Verify association

void test_verify_association() {
    //Create a processor and retrieve srcmd0
    processor_t* proc    = create_processor();
    srcmd_csr_t_p srcmd0 = get_srcmd(proc, 0);
    reg_t val            = 1 << SRCMD_BITMAP_BASE;

    //Test each memory domain
    for (reg_t assoc_md = 0; assoc_md < proc->md_num; assoc_md++) {
        srcmd0->write(val); //Associate the current memory domain with the source
        
        //Verify the association information
        for (reg_t md_idx = 0; md_idx < proc->md_num; md_idx++) {
            //Check if the memory domain should be associated
            if (assoc_md == md_idx) {
                assert(("When sid 0 and md " + std::to_string(md_idx) + " are associated, verify_association should return true",       srcmd0->verify_association(md_idx)));
            } else {
                assert(("When sid 0 and md " + std::to_string(md_idx) + " are not associated, verify_association should return false", !srcmd0->verify_association(md_idx)));
            }
        }
        //Associate the next memory domain with the source
        val <<= 1;
    }
}

void test_verify_association_boundaries() {
    //Create a processor and retrieve srcmd0
    processor_t* proc    = create_processor();
    srcmd_csr_t_p srcmd0 = get_srcmd(proc, 0);
    reg_t val            = ALL_ONE_BITS;
    srcmd0->write(val);

    assert(("When the memory domain is outside the lower bound of possible md indexes, the function should return false by default", !srcmd0->verify_association(-1)));
    assert(("When the memory domain is outside the upper bound of possible md indexes, the function should return false by default", !srcmd0->verify_association(proc->get_state()->max_mdcfg)));
}

// Associated memory domains

void test_associated_mds() {
    //Create a processor and retrieve srcmd0
    processor_t* proc    = create_processor();
    srcmd_csr_t_p srcmd0 = get_srcmd(proc, 0);
    //Associate all memory domains with srcmd0
    reg_t val            = ALL_MD_ASSOCIATED;

    srcmd0->write(val);
    //Retrieve the associated memory domains
    std::vector<reg_t> associated_result = srcmd0->associated_mds();

    //Check if the associated memory domains are present
    for (reg_t md_idx = 0; md_idx < proc->md_num; md_idx++) {
        assert(("When memory domain " + std::to_string(md_idx) + " is associated with srcmd0, the index of the memory domain should be present inside the vector returned by associated_mds", associated_result[md_idx] == md_idx));
    }
}

// --------------------- mdcfg CSR ---------------------

// Unlogged write

void test_unlogged_write_mdcfg() {
    //Create a processor and retrieve mdcfg0
    processor_t* proc    = create_processor();
    mdcfg_csr_t_p mdcfg0 = get_mdcfg(proc, 0);
    reg_t val            = 0;

    mdcfg0->write(val);

    assert(("When writing a value to mdcfg0, the same value should be read back", mdcfg0->read() == val));
}

void test_unlogged_write_clear_reserved_bits_mdcfg() {
    //Create a processor and retrieve mdcfg0
    processor_t* proc    = create_processor();
    mdcfg_csr_t_p mdcfg0 = get_mdcfg(proc, 0);
    reg_t val            = MDCFG_RSV;

    mdcfg0->write(val);
    
    assert(("When writing a value to mdcfg0, the reserved bits should be set to zero", mdcfg0->read() == 0));
}

void test_unlogged_write_to_disabled_mdcfg() {
    //Create a processor and retrieve mdcfg0, mdcfg1, mdcfg2
    processor_t* proc    = create_processor();
    mdcfg_csr_t_p mdcfg0 = get_mdcfg(proc, 0);
    mdcfg_csr_t_p mdcfg1 = get_mdcfg(proc, 1);
    mdcfg_csr_t_p mdcfg2 = get_mdcfg(proc, 2);
    //Limit the number of mdcfgs to 1
    proc->md_num   = 1;
    reg_t val      = 0;
    reg_t old_val1 = mdcfg1->read();
    reg_t old_val2 = mdcfg2->read();

    mdcfg0->write(val);
    mdcfg1->write(val);
    mdcfg2->write(val);

    assert(("When only one mdcfg csr is enabled, the value should change after writing to the enabled csr mdcfg0",            mdcfg0->read() == val));
    assert(("When only one mdcfg csr is enabled, the value should remain unchanged after writing to the disabled csr mdcfg1", mdcfg1->read() == old_val1));
    assert(("When only one mdcfg csr is enabled, the value should remain unchanged after writing to the disabled csr mdcfg2", mdcfg2->read() == old_val2));
}

void test_unlogged_write_monotonically_increasing_mdcfg() {
    //Create a processor and retrieve mdcfg0, mdcfg1, mdcfg2, mdcfg3, last mdcfg
    processor_t* proc     = create_processor();
    mdcfg_csr_t_p mdcfg0  = get_mdcfg(proc, 0);
    mdcfg_csr_t_p mdcfg1  = get_mdcfg(proc, 1);
    mdcfg_csr_t_p mdcfg2  = get_mdcfg(proc, 2);
    mdcfg_csr_t_p mdcfg3  = get_mdcfg(proc, 3);
    reg_t last_index      = proc->md_num - 1;
    mdcfg_csr_t_p last    = get_mdcfg(proc, last_index);
    reg_t old_val1        = mdcfg1->read();
    reg_t old_val3        = mdcfg3->read();
    reg_t old_val_last    = last->read();

    mdcfg1->write(1);
    assert(("First writing 1 to mdcfg1 should not succeed as this breaks the monotonically increasing property", mdcfg1->read() == old_val1));
    mdcfg0->write(0);
    assert(("Writing 0 to mdcfg0 should succeed as this does not break the monotonically increasing property", mdcfg0->read() == 0));
    mdcfg1->write(0);
    assert(("Writing 0 to mdcfg1 (mdcfgs state before writing = [0]) should succeed as this does not break the monotonically increasing property", mdcfg1->read() == 0));
    mdcfg2->write(1);
    assert(("Writing 1 to mdcfg2 (mdcfgs state before writing = [0, 0]) should succeed as this does not break the monotonically increasing property", mdcfg2->read() == 1));
    mdcfg1->write(1);
    assert(("Writing 1 to mdcfg1 (mdcfgs state before writing = [0, 0, 1]) should succeed as this does not break the monotonically increasing property", mdcfg1->read() == 1));
    mdcfg1->write(5);
    assert(("Writing 5 to mdcfg1 (mdcfgs state before writing = [0, 1, 1]) should not succeed as this breaks the monotonically increasing property", mdcfg1->read() == 1));
    mdcfg3->write(0);
    assert(("Writing 0 to mdcfg3 (mdcfgs state before writing = [0, 1, 1]) should not succeed as this breaks the monotonically increasing property", mdcfg3->read() == old_val3));

    last->write(0);
    assert(("Writing 0 to the last memory domain should not succeed as this breaks the monotonically increasing property", last->read() == old_val_last));

    //Write 1 to mdcfgs from 3 to second to last
    for (reg_t i = 3; i < last_index; i++) {
        mdcfg_csr_t_p mdcfg = get_mdcfg(proc, i);
        mdcfg->write(1);
    }

    last->write(proc->entry_num);
    assert(("Writing the maximum value to the last memory domain should succeed as this does not break the monotonically increasing property", last->read() == proc->entry_num));
}

// Entries belonging to memory domain

void test_entries_belonging_to_md() {
    //Create a processor and retrieve mdcfg0
    processor_t* proc    = create_processor();
    mdcfg_csr_t_p mdcfg0 = get_mdcfg(proc, 0);
    mdcfg_csr_t_p mdcfg1 = get_mdcfg(proc, 1);
    mdcfg_csr_t_p mdcfg2 = get_mdcfg(proc, 2);
    std::vector<reg_t> entries_results_0, 
                       entries_results_1, 
                       entries_results_2;

    mdcfg0->write(0);
    mdcfg0->entries_belonging_to_md(&entries_results_0);
    mdcfg1->entries_belonging_to_md(&entries_results_1);
    assert(("When the mdcfgs array is initialized to [0], no entries should belong to the first memory domain",  entries_results_0.empty()));
    assert(("When the mdcfgs array is initialized to [0], no entries should belong to the second memory domain", entries_results_1.empty()));

    mdcfg0->write(2);
    mdcfg0->entries_belonging_to_md(&entries_results_0);
    mdcfg1->entries_belonging_to_md(&entries_results_1);
    assert(("When the mdcfgs array is initialized to [2], the entries 0,1 should belong to the first memory domain", entries_results_0[0] == 0 && entries_results_0[1] == 1));
    assert(("When the mdcfgs array is initialized to [2], no entries should belong to the second memory domain", entries_results_1.empty()));

    mdcfg1->write(2);
    mdcfg0->entries_belonging_to_md(&entries_results_0);
    mdcfg1->entries_belonging_to_md(&entries_results_1);
    assert(("When the mdcfgs array is initialized to [2, 2], the entries 0,1 should belong to the first memory domain", entries_results_0[0] == 0 && entries_results_0[1] == 1));
    assert(("When the mdcfgs array is initialized to [2, 2], no entries should belong to the second memory domain", entries_results_1.empty()));

    mdcfg2->write(6);
    mdcfg0->entries_belonging_to_md(&entries_results_0);
    mdcfg1->entries_belonging_to_md(&entries_results_1);
    mdcfg2->entries_belonging_to_md(&entries_results_2);
    assert(("When the mdcfgs array is initialized to [2, 2, 6], the entries 0,1 should belong to the first memory domain", entries_results_0[0] == 0 && entries_results_0[1] == 1));
    assert(("When the mdcfgs array is initialized to [2, 2, 6], no entries should belong to the second memory domain",     entries_results_1.empty()));
    assert(("When the mdcfgs array is initialized to [2, 2, 6], the entries 2,3,4,5 should belong to the third memory domain", 
            entries_results_2[0] == 2 && 
            entries_results_2[1] == 3 &&
            entries_results_2[2] == 4 && 
            entries_results_2[3] == 5));
}

// --------------------- entry_addr CSR ---------------------

// Unlogged write

void test_unlogged_write_entry_addr() {
    //Create a processor and retrieve entry_addr0
    processor_t* proc              = create_processor();
    entry_addr_csr_t_p entry_addr0 = get_entry_addr(proc, 0);
    reg_t val                      = ALL_ONE_BITS;

    entry_addr0->write(val);

    assert(("When writing a value to entry_addr0, the same value should be read back", entry_addr0->read() == val));
}

void test_unlogged_write_to_disabled_entry_addr() {
    //Create a processor and retrieve entry_addr0, entry_addr1, entry_addr2
    processor_t* proc              = create_processor();
    entry_addr_csr_t_p entry_addr0 = get_entry_addr(proc, 0);
    entry_addr_csr_t_p entry_addr1 = get_entry_addr(proc, 1);
    entry_addr_csr_t_p entry_addr2 = get_entry_addr(proc, 2);
    //Limit the number of entry addrs to 1
    proc->entry_num = 1;
    reg_t val       = ALL_ONE_BITS;
    reg_t old_val1  = entry_addr1->read();
    reg_t old_val2  = entry_addr2->read();

    entry_addr0->write(val);
    entry_addr1->write(val);
    entry_addr2->write(val);

    assert(("When only one entry addr csr is enabled, the value should change after writing to the enabled csr entry_addr0",            entry_addr0->read() == val));
    assert(("When only one entry addr csr is enabled, the value should remain unchanged after writing to the disabled csr entry_addr1", entry_addr1->read() == old_val1));
    assert(("When only one entry addr csr is enabled, the value should remain unchanged after writing to the disabled csr entry_addr2", entry_addr2->read() == old_val2));
}

// Match 

void test_match_entry_addr() {
    //Create a processor and retrieve entry_addr0, entry_addr1
    processor_t* proc              = create_processor();
    entry_addr_csr_t_p entry_addr0 = get_entry_addr(proc, 0);
    entry_addr_csr_t_p entry_addr1 = get_entry_addr(proc, 1);
    //0 -- 12 -- 24
    reg_t paddr_top_0 = 0xC;  //12
    reg_t paddr_top_1 = 0x18; //24
    store_paddr(entry_addr0, paddr_top_0);
    store_paddr(entry_addr1, paddr_top_1);

    //Address mode OFF for the first entry
    write_entry_cfg(proc, 0, ENTRY_CFG_OFF);
    assert(("When the address mode is set to OFF, the entry should be disabled even though this would succeed for TOR", !entry_addr0->match(1, 10)));

    //Address mode TOR
    write_entry_cfg(proc, 0, ENTRY_CFG_TOR);
    write_entry_cfg(proc, 1, ENTRY_CFG_TOR);
    assert(("When the first entry is configured as TOR with an address range of [0, 12), it should match with the transaction [1, 10]",       entry_addr0->match(1, 10)));
    assert(("When the first entry is configured as TOR with an address range of [0, 12), it should match with the transaction [0, 11]",       entry_addr0->match(0, 12)));
    assert(("When the first entry is configured as TOR with an address range of [0, 12), it should not match with the transaction [1, 12]",   !entry_addr0->match(1, 12)));
    assert(("When the first entry is configured as TOR with an address range of [0, 12), it should not match with the transaction [1, 13]",   !entry_addr0->match(1, 13)));
    assert(("When the first entry is configured as TOR with an address range of [0, 12), it should not match with the transaction [12, 15]",  !entry_addr0->match(12, 4)));
    assert(("When the first entry is configured as TOR with an address range of [0, 12), it should not match with the transaction [13, 15]",  !entry_addr0->match(13, 3)));
    
    assert(("When the second entry is configured as TOR with an address range of [12, 24), it should match with the transaction [13, 22]",     entry_addr1->match(13, 10)));
    assert(("When the second entry is configured as TOR with an address range of [12, 24), it should match with the transaction [12, 23]",     entry_addr1->match(12, 12)));
    assert(("When the second entry is configured as TOR with an address range of [12, 24), it should not match with the transaction [15, 26]", !entry_addr1->match(15, 12)));
    assert(("When the second entry is configured as TOR with an address range of [12, 24), it should not match with the transaction [10, 15]", !entry_addr1->match(10, 6)));
    assert(("When the second entry is configured as TOR with an address range of [12, 24), it should not match with the transaction [0, 12]",  !entry_addr1->match(0, 13)));
}

// Access ok

void test_access_ok_entry_addr() {
    //Create a processor and retrieve entry_addr0
    processor_t* proc = create_processor();
    entry_addr_csr_t_p entry_addr0 = get_entry_addr(proc, 0);
    
    // Configure the first entry to allow read permissions
    write_entry_cfg(proc, 0, ENTRY_CFG_R);
    assert(("When the entry is configured to allow read permission and the transaction is a load, access should be allowed",      entry_addr0->access_ok(LOAD)));
    assert(("When the entry is configured to allow read permission and the transaction is a store, access should not be allowed", !entry_addr0->access_ok(STORE)));
    assert(("When the entry is configured to allow read permission and the transaction is a fetch, access should not be allowed", !entry_addr0->access_ok(FETCH)));

    // Configure the first entry to allow write permissions
    write_entry_cfg(proc, 0, ENTRY_CFG_W);
    assert(("When the entry is configured to allow write permission and the transaction is a load, access should not be allowed",  !entry_addr0->access_ok(LOAD)));
    assert(("When the entry is configured to allow write permission and the transaction is a store, access should be allowed",     entry_addr0->access_ok(STORE)));
    assert(("When the entry is configured to allow write permission and the transaction is a fetch, access should not be allowed", !entry_addr0->access_ok(FETCH)));

    // Configure the first entry to allow execute permissions
    write_entry_cfg(proc, 0, ENTRY_CFG_X);
    assert(("When the entry is configured to allow execute permission and the transaction is a load, access should not be allowed",  !entry_addr0->access_ok(LOAD)));
    assert(("When the entry is configured to allow execute permission and the transaction is a store, access should not be allowed", !entry_addr0->access_ok(STORE)));
    assert(("When the entry is configured to allow execute permission and the transaction is a fetch, access should be allowed",     entry_addr0->access_ok(FETCH)));

    // Configure the first entry to allow read and write permissions
    write_entry_cfg(proc, 0, ENTRY_CFG_R | ENTRY_CFG_W);
    assert(("When the entry is configured to allow read and write permission and the transaction is a load, access should be allowed",      entry_addr0->access_ok(LOAD)));
    assert(("When the entry is configured to allow read and write permission and the transaction is a store, access should be allowed",     entry_addr0->access_ok(STORE)));
    assert(("When the entry is configured to allow read and write permission and the transaction is a fetch, access should not be allowed", !entry_addr0->access_ok(FETCH)));
}

// --------------------- entry_cfg CSR ---------------------

// Unlogged write

void test_unlogged_write_entry_cfg() {
    //Create a processor
    processor_t* proc = create_processor();
    reg_t val         = ENTRY_CFG_R | ENTRY_CFG_W | ENTRY_CFG_X | ENTRY_CFG_TOR;

    write_entry_cfg(proc, 0, val);

    assert(("When writing a value to entry_cfg0, the same value should be read back", read_entry_cfg(proc, 0) == val));
}

void test_unlogged_write_to_disabled_entry_cfg() {
    //Create a processor
    processor_t* proc = create_processor();
    //Limit the number of entry addrs to 1
    proc->entry_num   = 1;
    reg_t val         = ENTRY_CFG_R | ENTRY_CFG_W | ENTRY_CFG_X | ENTRY_CFG_TOR;
    reg_t old_val1    = read_entry_cfg(proc, 1);
    reg_t old_val2    = read_entry_cfg(proc, 2);

    write_entry_cfg(proc, 0, val);
    write_entry_cfg(proc, 1, val);
    write_entry_cfg(proc, 2, val);

    assert(("When only one entry cfg csr is enabled, the value should change after writing to the enabled csr entry_cfg0",            read_entry_cfg(proc, 0) == val));
    assert(("When only one entry cfg csr is enabled, the value should remain unchanged after writing to the disabled csr entry_cfg1", read_entry_cfg(proc, 1) == old_val1));
    assert(("When only one entry cfg csr is enabled, the value should remain unchanged after writing to the disabled csr entry_cfg2", read_entry_cfg(proc, 2) == old_val2));
}

void test_unlogged_write_clear_reserved_bits_entry_cfg() {
    //Create a processor
    processor_t* proc = create_processor();
    reg_t val         = ENTRY_CFG_RSV;

    write_entry_cfg(proc, 0, val);

    assert(("When writing a value to entry_cfg0, the reserved bits should be set to zero", read_entry_cfg(proc, 0) == 0));
}

void run_csr_tests() {
    std::cout << "Running CSR tests" << std::endl;
}