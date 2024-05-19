#include <iostream>
#include "csr-tests.h"
#include "util.h"

// --------------------- srcmd CSR ---------------------

// Unlogged write

void test_unlogged_write_srcmd() {
    //Create a processor and retrieve srcmd0
    processor_t* proc    = create_processor();
    srcmd_csr_t_p srcmd0 = get_srcmd(proc, 0);
    reg_t val            = ALL_MD_ASSOCIATED;

    srcmd0->write(val);

    ASSERT("When writing a value to srcmd0, the same value should be read back", srcmd0->read() == val);
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

    ASSERT("When only 1 srcmd csr is enabled, the value should change after writing to the enabled csr srcmd0",            srcmd0->read() == val);
    ASSERT("When only 1 srcmd csr is enabled, the value should remain unchanged after writing to the disabled csr srcmd1", srcmd1->read() == old_val1);
    ASSERT("When only 1 srcmd csr is enabled, the value should remain unchanged after writing to the disabled csr srcmd2", srcmd2->read() == old_val2);
}

void test_unlogged_write_to_locked_srcmd() {
    //Create a processor and retrieve srcmd0, srcmd1
    processor_t* proc    = create_processor();
    srcmd_csr_t_p srcmd0 = get_srcmd(proc, 0);
    srcmd_csr_t_p srcmd1 = get_srcmd(proc, 1);
    reg_t val            = SRCMD_L;
    srcmd0->write(val);
    reg_t old_val0       = srcmd0->read();

    srcmd0->write(ALL_ONE_BITS);
    srcmd1->write(ALL_ONE_BITS);
    ASSERT("When writing a value to locked srcmd0, the value should remain unchanged after writing", srcmd0->read() == old_val0);
    ASSERT("When writing a value to unlocked srcmd1, the same value should be read back",     srcmd1->read() == ALL_ONE_BITS);

    srcmd0->write(0);
    ASSERT("When writing a value to locked srcmd0 changing the lock value, the value should remain unchanged after writing", srcmd0->read() == old_val0);
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
                ASSERT("When sid 0 and md " + std::to_string(md_idx) + " are associated, verify_association should return true",       srcmd0->verify_association(md_idx));
            } else {
                ASSERT("When sid 0 and md " + std::to_string(md_idx) + " are not associated, verify_association should return false", !srcmd0->verify_association(md_idx));
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

    ASSERT("When the memory domain is outside the lower bound of possible md indexes, the function should return false by default", !srcmd0->verify_association(-1));
    ASSERT("When the memory domain is outside the upper bound of possible md indexes, the function should return false by default", !srcmd0->verify_association(proc->get_state()->max_mdcfg));
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
        ASSERT("When memory domain " + std::to_string(md_idx) + " is associated with srcmd0, the index of the memory domain should be present inside the vector returned by associated_mds", associated_result[md_idx] == md_idx);
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

    ASSERT("When writing a value to mdcfg0, the same value should be read back", mdcfg0->read() == val);
}

void test_unlogged_write_clear_reserved_bits_mdcfg() {
    //Create a processor and retrieve mdcfg0
    processor_t* proc    = create_processor();
    mdcfg_csr_t_p mdcfg0 = get_mdcfg(proc, 0);
    reg_t val            = MDCFG_RSV;

    mdcfg0->write(val);
    
    ASSERT("When writing a value to mdcfg0, the reserved bits should be set to zero", mdcfg0->read() == 0);
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

    ASSERT("When only one mdcfg csr is enabled, the value should change after writing to the enabled csr mdcfg0",            mdcfg0->read() == val);
    ASSERT("When only one mdcfg csr is enabled, the value should remain unchanged after writing to the disabled csr mdcfg1", mdcfg1->read() == old_val1);
    ASSERT("When only one mdcfg csr is enabled, the value should remain unchanged after writing to the disabled csr mdcfg2", mdcfg2->read() == old_val2);
}

void test_unlogged_write_to_locked_mdcfg() {
    //Create a processor and retrieve mdcfg0, mdcfg1
    processor_t* proc         = create_processor();
    mdcfg_csr_t_p mdcfg0      = get_mdcfg(proc, 0);
    mdcfg_csr_t_p mdcfg1      = get_mdcfg(proc, 1);
    mdcfglck_csr_t_p mdcfglck = get_mdcfglck(proc);
    reg_t val                 = 1 << MDCFGLCK_F_SHIFT;
    mdcfg0->write(1);
    mdcfglck->write(val);
    reg_t old_val0            = mdcfg0->read();

    mdcfg0->write(2);
    mdcfg1->write(2);
    ASSERT("When writing a value to locked mdcfg0, the value should remain unchanged after writing", mdcfg0->read() == old_val0);
    ASSERT("When writing a value to unlocked mdcfg1, the same value should be read back",            mdcfg1->read() == 2);
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
    ASSERT("First writing 1 to mdcfg1 should not succeed as this breaks the monotonically increasing property", mdcfg1->read() == old_val1);
    mdcfg0->write(0);
    ASSERT("Writing 0 to mdcfg0 should succeed as this does not break the monotonically increasing property", mdcfg0->read() == 0);
    mdcfg1->write(0);
    ASSERT("Writing 0 to mdcfg1 (mdcfgs state before writing = [0]) should succeed as this does not break the monotonically increasing property", mdcfg1->read() == 0);
    mdcfg2->write(1);
    ASSERT("Writing 1 to mdcfg2 (mdcfgs state before writing = [0, 0]) should succeed as this does not break the monotonically increasing property", mdcfg2->read() == 1);
    mdcfg1->write(1);
    ASSERT("Writing 1 to mdcfg1 (mdcfgs state before writing = [0, 0, 1]) should succeed as this does not break the monotonically increasing property", mdcfg1->read() == 1);
    mdcfg1->write(5);
    ASSERT("Writing 5 to mdcfg1 (mdcfgs state before writing = [0, 1, 1]) should not succeed as this breaks the monotonically increasing property", mdcfg1->read() == 1);
    mdcfg3->write(0);
    ASSERT("Writing 0 to mdcfg3 (mdcfgs state before writing = [0, 1, 1]) should not succeed as this breaks the monotonically increasing property", mdcfg3->read() == old_val3);

    last->write(0);
    ASSERT("Writing 0 to the last memory domain should not succeed as this breaks the monotonically increasing property", last->read() == old_val_last);

    //Write 1 to mdcfgs from 3 to second to last
    for (reg_t i = 3; i < last_index; i++) {
        mdcfg_csr_t_p mdcfg = get_mdcfg(proc, i);
        mdcfg->write(1);
    }

    last->write(proc->entry_num);
    ASSERT("Writing the maximum value to the last memory domain should succeed as this does not break the monotonically increasing property", last->read() == proc->entry_num);
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
    ASSERT("When the mdcfgs array is initialized to [0], no entries should belong to the first memory domain",  entries_results_0.empty());
    ASSERT("When the mdcfgs array is initialized to [0], no entries should belong to the second memory domain", entries_results_1.empty());

    mdcfg0->write(2);
    mdcfg0->entries_belonging_to_md(&entries_results_0);
    mdcfg1->entries_belonging_to_md(&entries_results_1);
    ASSERT("When the mdcfgs array is initialized to [2], the entries 0,1 should belong to the first memory domain", entries_results_0[0] == 0 && entries_results_0[1] == 1);
    ASSERT("When the mdcfgs array is initialized to [2], no entries should belong to the second memory domain", entries_results_1.empty());

    mdcfg1->write(2);
    mdcfg0->entries_belonging_to_md(&entries_results_0);
    mdcfg1->entries_belonging_to_md(&entries_results_1);
    ASSERT("When the mdcfgs array is initialized to [2, 2], the entries 0,1 should belong to the first memory domain", entries_results_0[0] == 0 && entries_results_0[1] == 1);
    ASSERT("When the mdcfgs array is initialized to [2, 2], no entries should belong to the second memory domain", entries_results_1.empty());

    mdcfg2->write(6);
    mdcfg0->entries_belonging_to_md(&entries_results_0);
    mdcfg1->entries_belonging_to_md(&entries_results_1);
    mdcfg2->entries_belonging_to_md(&entries_results_2);
    ASSERT("When the mdcfgs array is initialized to [2, 2, 6], the entries 0,1 should belong to the first memory domain", entries_results_0[0] == 0 && entries_results_0[1] == 1);
    ASSERT("When the mdcfgs array is initialized to [2, 2, 6], no entries should belong to the second memory domain",     entries_results_1.empty());
    ASSERT("When the mdcfgs array is initialized to [2, 2, 6], the entries 2,3,4,5 should belong to the third memory domain", 
            entries_results_2[0] == 2 && 
            entries_results_2[1] == 3 &&
            entries_results_2[2] == 4 && 
            entries_results_2[3] == 5);
}

// --------------------- mdcfglck CSR ---------------------

// Unlogged write

void test_unlogged_write_mdcfglck() {
    //Create a processor and retrieve mdcfglck
    processor_t* proc         = create_processor();
    mdcfglck_csr_t_p mdcfglck = get_mdcfglck(proc);
    reg_t val                 = MDCFGLCK_F | MDCFGLCK_L;

    mdcfglck->write(val);

    ASSERT("When writing a value to mdcfglck, the same value should be read back", mdcfglck->read() == val);
}

void test_unlogged_write_clear_reserved_bits_mdcfglck() {
    //Create a processor and retrieve mdcfglck
    processor_t* proc         = create_processor();
    mdcfglck_csr_t_p mdcfglck = get_mdcfglck(proc);
    reg_t val                 = MDCFGLCK_RSV;

    mdcfglck->write(val);
    
    ASSERT("When writing a value to mdcfglck, the reserved bits should be set to zero", mdcfglck->read() == 0);
}

void test_unlogged_write_incremental_only_mdcfglck() {
    //Create a processor and retrieve mdcfglck
    processor_t* proc         = create_processor();
    mdcfglck_csr_t_p mdcfglck = get_mdcfglck(proc);
    reg_t val                 = 1 << MDCFGLCK_F_SHIFT;

    mdcfglck->write(val);
    ASSERT("When writing a larger f-field value to entrylck, the write should succeed", mdcfglck->read() == val);

    val = 3 << MDCFGLCK_F_SHIFT;
    mdcfglck->write(val);
    ASSERT("When writing a larger f-field value to entrylck, the write should succeed", mdcfglck->read() == val);

    reg_t new_val = 2 << MDCFGLCK_F_SHIFT;
    mdcfglck->write(new_val);
    ASSERT("When writing a smaller f-field value to entrylck, the write should not succeed", mdcfglck->read() == val);
}

void test_unlogged_write_to_locked_mdcfglck() {
    //Create a processor and retrieve mdcfglck
    processor_t* proc         = create_processor();
    mdcfglck_csr_t_p mdcfglck = get_mdcfglck(proc);
    reg_t val                 = (2 << MDCFGLCK_F_SHIFT) | MDCFGLCK_L;
    reg_t updated_val         = (3 << MDCFGLCK_F_SHIFT) | MDCFGLCK_L;
    reg_t no_lock_val         = (2 << MDCFGLCK_F_SHIFT);
    mdcfglck->write(val);

    mdcfglck->write(updated_val);
    ASSERT("When writing a value to a locked mdcfglck, the value should remain unchanged after writing", mdcfglck->read() == val);

    mdcfglck->write(no_lock_val);
    ASSERT("hen changing the lock value of a locked mdcfglck, the value should remain unchanged after writing", mdcfglck->read() == val);
}

// --------------------- entry_addr CSR ---------------------

// Unlogged write

void test_unlogged_write_entry_addr() {
    //Create a processor and retrieve entry_addr0
    processor_t* proc              = create_processor();
    entry_addr_csr_t_p entry_addr0 = get_entry_addr(proc, 0);
    reg_t val                      = ALL_ONE_BITS;

    entry_addr0->write(val);

    ASSERT("When writing a value to entry_addr0, the same value should be read back", entry_addr0->read() == val);
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

    ASSERT("When only one entry addr csr is enabled, the value should change after writing to the enabled csr entry_addr0",            entry_addr0->read() == val);
    ASSERT("When only one entry addr csr is enabled, the value should remain unchanged after writing to the disabled csr entry_addr1", entry_addr1->read() == old_val1);
    ASSERT("When only one entry addr csr is enabled, the value should remain unchanged after writing to the disabled csr entry_addr2", entry_addr2->read() == old_val2);
}

void test_unlogged_write_to_locked_entry_addr() {
    //Create a processor and retrieve entry_addr0, entry_addr1
    processor_t* proc              = create_processor();
    entry_addr_csr_t_p entry_addr0 = get_entry_addr(proc, 0);
    entry_addr_csr_t_p entry_addr1 = get_entry_addr(proc, 1);
    entrylck_csr_t_p entrylck      = get_entrylck(proc);
    reg_t val                      = 1 << ENTRYLCK_F_SHIFT;
    entrylck->write(val);
    reg_t old_val0                 = entry_addr0->read();

    entry_addr0->write(ALL_ONE_BITS);
    entry_addr1->write(ALL_ONE_BITS);
    ASSERT("When writing a value to locked entry_addr0, the value should remain unchanged after writing", entry_addr0->read() == old_val0);
    ASSERT("When writing a value to unlocked entry_addr1, the same value should be read back",            entry_addr1->read() == ALL_ONE_BITS);
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
    ASSERT("When the address mode is set to OFF, the entry should be disabled even though this would succeed for TOR", !entry_addr0->match(1, 10));

    //Address mode TOR
    write_entry_cfg(proc, 0, ENTRY_CFG_TOR);
    write_entry_cfg(proc, 1, ENTRY_CFG_TOR);
    ASSERT("When the first entry is configured as TOR with an address range of [0, 12), it should match with the transaction [1, 10]",       entry_addr0->match(1, 10));
    ASSERT("When the first entry is configured as TOR with an address range of [0, 12), it should match with the transaction [0, 11]",       entry_addr0->match(0, 12));
    ASSERT("When the first entry is configured as TOR with an address range of [0, 12), it should not match with the transaction [1, 12]",   !entry_addr0->match(1, 12));
    ASSERT("When the first entry is configured as TOR with an address range of [0, 12), it should not match with the transaction [1, 13]",   !entry_addr0->match(1, 13));
    ASSERT("When the first entry is configured as TOR with an address range of [0, 12), it should not match with the transaction [12, 15]",  !entry_addr0->match(12, 4));
    ASSERT("When the first entry is configured as TOR with an address range of [0, 12), it should not match with the transaction [13, 15]",  !entry_addr0->match(13, 3));
    
    ASSERT("When the second entry is configured as TOR with an address range of [12, 24), it should match with the transaction [13, 22]",     entry_addr1->match(13, 10));
    ASSERT("When the second entry is configured as TOR with an address range of [12, 24), it should match with the transaction [12, 23]",     entry_addr1->match(12, 12));
    ASSERT("When the second entry is configured as TOR with an address range of [12, 24), it should not match with the transaction [15, 26]", !entry_addr1->match(15, 12));
    ASSERT("When the second entry is configured as TOR with an address range of [12, 24), it should not match with the transaction [10, 15]", !entry_addr1->match(10, 6));
    ASSERT("When the second entry is configured as TOR with an address range of [12, 24), it should not match with the transaction [0, 12]",  !entry_addr1->match(0, 13));
}

// Access ok

void test_access_ok_entry_addr() {
    //Create a processor and retrieve entry_addr0
    processor_t* proc = create_processor();
    entry_addr_csr_t_p entry_addr0 = get_entry_addr(proc, 0);
    
    // Configure the first entry to allow read permissions
    write_entry_cfg(proc, 0, ENTRY_CFG_R);
    ASSERT("When the entry is configured to allow read permission and the transaction is a load, access should be allowed",      entry_addr0->access_ok(LOAD));
    ASSERT("When the entry is configured to allow read permission and the transaction is a store, access should not be allowed", !entry_addr0->access_ok(STORE));
    ASSERT("When the entry is configured to allow read permission and the transaction is a fetch, access should not be allowed", !entry_addr0->access_ok(FETCH));

    // Configure the first entry to allow write permissions
    write_entry_cfg(proc, 0, ENTRY_CFG_W);
    ASSERT("When the entry is configured to allow write permission and the transaction is a load, access should not be allowed",  !entry_addr0->access_ok(LOAD));
    ASSERT("When the entry is configured to allow write permission and the transaction is a store, access should be allowed",     entry_addr0->access_ok(STORE));
    ASSERT("When the entry is configured to allow write permission and the transaction is a fetch, access should not be allowed", !entry_addr0->access_ok(FETCH));

    // Configure the first entry to allow execute permissions
    write_entry_cfg(proc, 0, ENTRY_CFG_X);
    ASSERT("When the entry is configured to allow execute permission and the transaction is a load, access should not be allowed",  !entry_addr0->access_ok(LOAD));
    ASSERT("When the entry is configured to allow execute permission and the transaction is a store, access should not be allowed", !entry_addr0->access_ok(STORE));
    ASSERT("When the entry is configured to allow execute permission and the transaction is a fetch, access should be allowed",     entry_addr0->access_ok(FETCH));

    // Configure the first entry to allow read and write permissions
    write_entry_cfg(proc, 0, ENTRY_CFG_R | ENTRY_CFG_W);
    ASSERT("When the entry is configured to allow read and write permission and the transaction is a load, access should be allowed",      entry_addr0->access_ok(LOAD));
    ASSERT("When the entry is configured to allow read and write permission and the transaction is a store, access should be allowed",     entry_addr0->access_ok(STORE));
    ASSERT("When the entry is configured to allow read and write permission and the transaction is a fetch, access should not be allowed", !entry_addr0->access_ok(FETCH));
}

// --------------------- entry_cfg CSR ---------------------

// Unlogged write

void test_unlogged_write_entry_cfg() {
    //Create a processor
    processor_t* proc = create_processor();
    reg_t val         = ENTRY_CFG_R | ENTRY_CFG_W | ENTRY_CFG_X | ENTRY_CFG_TOR;

    write_entry_cfg(proc, 0, val);

    ASSERT("When writing a value to entry_cfg0, the same value should be read back", read_entry_cfg(proc, 0) == val);
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

    ASSERT("When only one entry cfg csr is enabled, the value should change after writing to the enabled csr entry_cfg0",            read_entry_cfg(proc, 0) == val);
    ASSERT("When only one entry cfg csr is enabled, the value should remain unchanged after writing to the disabled csr entry_cfg1", read_entry_cfg(proc, 1) == old_val1);
    ASSERT("When only one entry cfg csr is enabled, the value should remain unchanged after writing to the disabled csr entry_cfg2", read_entry_cfg(proc, 2) == old_val2);
}

void test_unlogged_write_to_locked_entry_cfg() {
    //Create a processor
    processor_t* proc         = create_processor();
    entrylck_csr_t_p entrylck = get_entrylck(proc);
    reg_t val                 = 1 << ENTRYLCK_F_SHIFT;
    reg_t val_cfg             = ENTRY_CFG_R | ENTRY_CFG_W | ENTRY_CFG_X | ENTRY_CFG_TOR;
    entrylck->write(val);
    reg_t old_val0            = read_entry_cfg(proc, 0);

    write_entry_cfg(proc, 0, val_cfg);
    write_entry_cfg(proc, 1, val_cfg);
    ASSERT("When writing a value to locked entry_cfg0, the value should remain unchanged after writing", read_entry_cfg(proc, 0) == old_val0);
    ASSERT("When writing a value to unlocked entry_cfg1, the same value should be read back",            read_entry_cfg(proc, 1) == val_cfg);
}


void test_unlogged_write_clear_reserved_bits_entry_cfg() {
    //Create a processor
    processor_t* proc = create_processor();
    reg_t val         = ENTRY_CFG_RSV;

    write_entry_cfg(proc, 0, val);

    ASSERT("When writing a value to entry_cfg0, the reserved bits should be set to zero", read_entry_cfg(proc, 0) == 0);
}

// --------------------- entrylck CSR ---------------------

// Unlogged write

void test_unlogged_write_entrylck() {
    //Create a processor and retrieve entrylck
    processor_t* proc         = create_processor();
    entrylck_csr_t_p entrylck = get_entrylck(proc);
    reg_t val                 = ENTRYLCK_F | ENTRYLCK_L;

    entrylck->write(val);

    ASSERT("When writing a value to entrylck, the same value should be read back", entrylck->read() == val);
}

void test_unlogged_write_clear_reserved_bits_entrylck() {
    //Create a processor and retrieve entrylck
    processor_t* proc         = create_processor();
    entrylck_csr_t_p entrylck = get_entrylck(proc);
    reg_t val                 = ENTRYLCK_RSV;

    entrylck->write(val);
    
    ASSERT("When writing a value to entrylck, the reserved bits should be set to zero", entrylck->read() == 0);
}

void test_unlogged_write_incremental_only_entrylck() {
    //Create a processor and retrieve entrylck
    processor_t* proc         = create_processor();
    entrylck_csr_t_p entrylck = get_entrylck(proc);
    reg_t val                 = 1 << ENTRYLCK_F_SHIFT;

    entrylck->write(val);
    ASSERT("When writing a larger f-field value to entrylck, the write should succeed", entrylck->read() == val);

    val = 3 << ENTRYLCK_F_SHIFT;
    entrylck->write(val);
    ASSERT("When writing a larger f-field value to entrylck, the write should succeed", entrylck->read() == val);

    reg_t new_val = 2 << ENTRYLCK_F_SHIFT;
    entrylck->write(new_val);
    ASSERT("When writing a smaller f-field value to entrylck, the write should not succeed", entrylck->read() == val);
}

void test_unlogged_write_to_locked_entrylck() {
    //Create a processor and retrieve entrylck
    processor_t* proc         = create_processor();
    entrylck_csr_t_p entrylck = get_entrylck(proc);
    reg_t val                 = (2 << ENTRYLCK_F_SHIFT) | ENTRYLCK_L;
    reg_t updated_val         = (3 << ENTRYLCK_F_SHIFT) | ENTRYLCK_L;
    reg_t no_lock_val         = (2 << ENTRYLCK_F_SHIFT);
    entrylck->write(val);

    entrylck->write(updated_val);
    ASSERT("When writing a value to a locked entrylck, the value should remain unchanged after writing", entrylck->read() == val);

    entrylck->write(no_lock_val);
    ASSERT("When changing the lock value of a locked entrylck, the value should remain unchanged after writing", entrylck->read() == val);
}

void run_csr_tests() {
    std::cout << "IOPMP CSR tests" << std::endl;

    // SRCMD CSR
    test_unlogged_write_srcmd();
    test_unlogged_write_to_disabled_srcmd();
    test_unlogged_write_to_locked_srcmd();
    test_verify_association();
    test_verify_association_boundaries();
    test_associated_mds();

    // MDCFG CSR
    test_unlogged_write_mdcfg();
    test_unlogged_write_clear_reserved_bits_mdcfg();
    test_unlogged_write_to_disabled_mdcfg();
    test_unlogged_write_to_locked_mdcfg();
    test_unlogged_write_monotonically_increasing_mdcfg();
    test_entries_belonging_to_md();

    // MDCFGLK CSR
    test_unlogged_write_mdcfglck();
    test_unlogged_write_clear_reserved_bits_mdcfglck();
    test_unlogged_write_incremental_only_mdcfglck();
    test_unlogged_write_to_locked_mdcfglck();

    // ENTRY_ADDR CSR
    test_unlogged_write_entry_addr();
    test_unlogged_write_to_disabled_entry_addr();
    test_unlogged_write_to_locked_entry_addr();
    test_match_entry_addr();
    test_access_ok_entry_addr();

    // ENTRY_CFG CSR
    test_unlogged_write_entry_cfg();
    test_unlogged_write_to_disabled_entry_cfg();
    test_unlogged_write_to_locked_entry_cfg();
    test_unlogged_write_clear_reserved_bits_entry_cfg();

    // ENTRYLK CSR
    test_unlogged_write_entrylck();
    test_unlogged_write_clear_reserved_bits_entrylck();
    test_unlogged_write_incremental_only_entrylck();
    test_unlogged_write_to_locked_entrylck();

    end_test();
}