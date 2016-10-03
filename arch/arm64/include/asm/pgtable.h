/*
 * Copyright (C) 2012 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __ASM_PGTABLE_H
#define __ASM_PGTABLE_H

#include <asm/proc-fns.h>

#include <asm/memory.h>
#include <asm/pgtable-hwdef.h>

#ifdef CONFIG_TIMA_RKP
#include <linux/rkp_entry.h> 
#endif /* CONFIG_TIMA_RKP */

/*
 * Software defined PTE bits definition.
 */
#define PTE_VALID		(_AT(pteval_t, 1) << 0)
#define PTE_FILE		(_AT(pteval_t, 1) << 2)	/* only when !pte_present() */
#define PTE_DIRTY		(_AT(pteval_t, 1) << 55)
#define PTE_SPECIAL		(_AT(pteval_t, 1) << 56)
#define PTE_WRITE		(_AT(pteval_t, 1) << 57)
#define PTE_PROT_NONE		(_AT(pteval_t, 1) << 58) /* only when !PTE_VALID */

/*
 * VMALLOC and SPARSEMEM_VMEMMAP ranges.
 */
#define VMALLOC_START		(UL(0xffffffffffffffff) << VA_BITS)
#define VMALLOC_END		(PAGE_OFFSET - UL(0x400000000) - SZ_64K)

#define vmemmap			((struct page *)(VMALLOC_END + SZ_64K))

#define FIRST_USER_ADDRESS	0

#ifndef __ASSEMBLY__
extern void __pte_error(const char *file, int line, unsigned long val);
extern void __pmd_error(const char *file, int line, unsigned long val);
extern void __pgd_error(const char *file, int line, unsigned long val);

#define pte_ERROR(pte)		__pte_error(__FILE__, __LINE__, pte_val(pte))
#ifndef CONFIG_ARM64_64K_PAGES
#define pmd_ERROR(pmd)		__pmd_error(__FILE__, __LINE__, pmd_val(pmd))
#endif
#define pgd_ERROR(pgd)		__pgd_error(__FILE__, __LINE__, pgd_val(pgd))

#ifdef CONFIG_SMP
#define PROT_DEFAULT		(PTE_TYPE_PAGE | PTE_AF | PTE_SHARED)
#define PROT_SECT_DEFAULT	(PMD_TYPE_SECT | PMD_SECT_AF | PMD_SECT_S)
#else
#define PROT_DEFAULT		(PTE_TYPE_PAGE | PTE_AF)
#define PROT_SECT_DEFAULT	(PMD_TYPE_SECT | PMD_SECT_AF)
#endif

#define PROT_DEVICE_nGnRE	(PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_ATTRINDX(MT_DEVICE_nGnRE))
#define PROT_NORMAL_NC		(PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_ATTRINDX(MT_NORMAL_NC))
#define PROT_NORMAL		(PROT_DEFAULT | PTE_PXN | PTE_UXN | PTE_ATTRINDX(MT_NORMAL))

#define PROT_SECT_DEVICE_nGnRE	(PROT_SECT_DEFAULT | PMD_SECT_PXN | PMD_SECT_UXN | PMD_ATTRINDX(MT_DEVICE_nGnRE))
#define PROT_SECT_NORMAL_NC	(PROT_SECT_DEFAULT | PMD_SECT_PXN | PMD_SECT_UXN | PMD_ATTRINDX(MT_NORMAL_NC))
#define PROT_SECT_NORMAL	(PROT_SECT_DEFAULT | PMD_SECT_PXN | PMD_SECT_UXN | PMD_ATTRINDX(MT_NORMAL))
#define PROT_SECT_NORMAL_EXEC	(PROT_SECT_DEFAULT | PMD_SECT_UXN | PMD_ATTRINDX(MT_NORMAL))

#define _PAGE_DEFAULT		(PROT_DEFAULT | PTE_ATTRINDX(MT_NORMAL))

#define PAGE_KERNEL		__pgprot(_PAGE_DEFAULT | PTE_PXN | PTE_UXN | PTE_DIRTY | PTE_WRITE)
#define PAGE_KERNEL_EXEC	__pgprot(_PAGE_DEFAULT | PTE_UXN | PTE_DIRTY | PTE_WRITE)

#define PAGE_HYP		__pgprot(_PAGE_DEFAULT | PTE_HYP)
#define PAGE_HYP_DEVICE		__pgprot(PROT_DEVICE_nGnRE | PTE_HYP)

#define PAGE_S2			__pgprot(PROT_DEFAULT | PTE_S2_MEMATTR(MT_S2_NORMAL) | PTE_S2_RDONLY)
#define PAGE_S2_DEVICE		__pgprot(PROT_DEFAULT | PTE_S2_MEMATTR(MT_S2_DEVICE_nGnRE) | PTE_S2_RDWR | PTE_UXN)

#define PAGE_NONE		__pgprot(((_PAGE_DEFAULT) & ~PTE_TYPE_MASK) | PTE_PROT_NONE | PTE_PXN | PTE_UXN)
#define PAGE_SHARED		__pgprot(_PAGE_DEFAULT | PTE_USER | PTE_NG | PTE_PXN | PTE_UXN | PTE_WRITE)
#define PAGE_SHARED_EXEC	__pgprot(_PAGE_DEFAULT | PTE_USER | PTE_NG | PTE_PXN | PTE_WRITE)
#define PAGE_COPY		__pgprot(_PAGE_DEFAULT | PTE_USER | PTE_NG | PTE_PXN | PTE_UXN)
#define PAGE_COPY_EXEC		__pgprot(_PAGE_DEFAULT | PTE_USER | PTE_NG | PTE_PXN)
#define PAGE_READONLY		__pgprot(_PAGE_DEFAULT | PTE_USER | PTE_NG | PTE_PXN | PTE_UXN)
#define PAGE_READONLY_EXEC	__pgprot(_PAGE_DEFAULT | PTE_USER | PTE_NG | PTE_PXN)

#define __P000  PAGE_NONE
#define __P001  PAGE_READONLY
#define __P010  PAGE_COPY
#define __P011  PAGE_COPY
#define __P100  PAGE_READONLY_EXEC
#define __P101  PAGE_READONLY_EXEC
#define __P110  PAGE_COPY_EXEC
#define __P111  PAGE_COPY_EXEC

#define __S000  PAGE_NONE
#define __S001  PAGE_READONLY
#define __S010  PAGE_SHARED
#define __S011  PAGE_SHARED
#define __S100  PAGE_READONLY_EXEC
#define __S101  PAGE_READONLY_EXEC
#define __S110  PAGE_SHARED_EXEC
#define __S111  PAGE_SHARED_EXEC

/*
 * ZERO_PAGE is a global shared page that is always zero: used
 * for zero-mapped memory areas etc..
 */
extern struct page *empty_zero_page;
#define ZERO_PAGE(vaddr)	(empty_zero_page)

#define pte_pfn(pte)		((pte_val(pte) & PHYS_MASK) >> PAGE_SHIFT)

#define pfn_pte(pfn,prot)	(__pte(((phys_addr_t)(pfn) << PAGE_SHIFT) | pgprot_val(prot)))

#define pte_none(pte)		(!pte_val(pte))
#define pte_clear(mm,addr,ptep)	set_pte(ptep, __pte(0))
#define pte_page(pte)		(pfn_to_page(pte_pfn(pte)))
#define pte_offset_kernel(dir,addr)	(pmd_page_vaddr(*(dir)) + pte_index(addr))

#define pte_offset_map(dir,addr)	pte_offset_kernel((dir), (addr))
#define pte_offset_map_nested(dir,addr)	pte_offset_kernel((dir), (addr))
#define pte_unmap(pte)			do { } while (0)
#define pte_unmap_nested(pte)		do { } while (0)

/*
 * The following only work if pte_present(). Undefined behaviour otherwise.
 */
#define pte_present(pte)	(!!(pte_val(pte) & (PTE_VALID | PTE_PROT_NONE)))
#define pte_dirty(pte)		(!!(pte_val(pte) & PTE_DIRTY))
#define pte_young(pte)		(!!(pte_val(pte) & PTE_AF))
#define pte_special(pte)	(!!(pte_val(pte) & PTE_SPECIAL))
#define pte_write(pte)		(!!(pte_val(pte) & PTE_WRITE))
#define pte_exec(pte)		(!(pte_val(pte) & PTE_UXN))

#define pte_valid_user(pte) \
	((pte_val(pte) & (PTE_VALID | PTE_USER)) == (PTE_VALID | PTE_USER))

static inline pte_t pte_wrprotect(pte_t pte)
{
	pte_val(pte) &= ~PTE_WRITE;
	return pte;
}

static inline pte_t pte_mkwrite(pte_t pte)
{
	pte_val(pte) |= PTE_WRITE;
	return pte;
}

static inline pte_t pte_mkclean(pte_t pte)
{
	pte_val(pte) &= ~PTE_DIRTY;
	return pte;
}

static inline pte_t pte_mkdirty(pte_t pte)
{
	pte_val(pte) |= PTE_DIRTY;
	return pte;
}

static inline pte_t pte_mkold(pte_t pte)
{
	pte_val(pte) &= ~PTE_AF;
	return pte;
}

static inline pte_t pte_mkyoung(pte_t pte)
{
	pte_val(pte) |= PTE_AF;
	return pte;
}

static inline pte_t pte_mkspecial(pte_t pte)
{
	pte_val(pte) |= PTE_SPECIAL;
	return pte;
}
#ifdef CONFIG_TIMA_RKP
extern  int printk(const char *s, ...);
extern void panic(const char *fmt, ...);
#endif /* CONFIG_TIMA_RKP */
static inline void set_pte(pte_t *ptep, pte_t pte)
{
#ifdef CONFIG_TIMA_RKP
	if (rkp_is_pg_dbl_mapped((u64)(pte)) ) {
		panic("TIMA RKP : Double mapping Detected");
		return;
	}
	if (rkp_is_pg_protected((u64)ptep)) {
		rkp_call(RKP_PTE_SET, (unsigned long)ptep, pte_val(pte), 0, 0, 0);
	} else {
		asm volatile("mov x1, %0\n"
					  "mov x2, %1\n"
 					  "str x2, [x1]\n"
		:
		: "r" (ptep), "r" (pte)
		: "x1", "x2", "memory" );
	}
#else
	*ptep = pte;
#endif /* CONFIG_TIMA_RKP */

}

extern void __sync_icache_dcache(pte_t pteval, unsigned long addr);

static inline void set_pte_at(struct mm_struct *mm, unsigned long addr,
			      pte_t *ptep, pte_t pte)
{
	if (pte_valid_user(pte)) {
		if (!pte_special(pte) && pte_exec(pte))
			__sync_icache_dcache(pte, addr);
		if (pte_dirty(pte) && pte_write(pte))
			pte_val(pte) &= ~PTE_RDONLY;
		else
			pte_val(pte) |= PTE_RDONLY;
	}

	set_pte(ptep, pte);
}

/*
 * Huge pte definitions.
 */
#define pte_huge(pte)		(!(pte_val(pte) & PTE_TABLE_BIT))
#define pte_mkhuge(pte)		(__pte(pte_val(pte) & ~PTE_TABLE_BIT))

/*
 * Hugetlb definitions.
 */
#define HUGE_MAX_HSTATE		2
#define HPAGE_SHIFT		PMD_SHIFT
#define HPAGE_SIZE		(_AC(1, UL) << HPAGE_SHIFT)
#define HPAGE_MASK		(~(HPAGE_SIZE - 1))
#define HUGETLB_PAGE_ORDER	(HPAGE_SHIFT - PAGE_SHIFT)

#define __HAVE_ARCH_PTE_SPECIAL

static inline pte_t pmd_pte(pmd_t pmd)
{
	return __pte(pmd_val(pmd));
}

static inline pmd_t pte_pmd(pte_t pte)
{
	return __pmd(pte_val(pte));
}

/*
 * THP definitions.
 */

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
#define pmd_trans_huge(pmd)	(pmd_val(pmd) && !(pmd_val(pmd) & PMD_TABLE_BIT))
#define pmd_trans_splitting(pmd)	pte_special(pmd_pte(pmd))
#endif

#define pmd_young(pmd)		pte_young(pmd_pte(pmd))
#define pmd_wrprotect(pmd)	pte_pmd(pte_wrprotect(pmd_pte(pmd)))
#define pmd_mksplitting(pmd)	pte_pmd(pte_mkspecial(pmd_pte(pmd)))
#define pmd_mkold(pmd)		pte_pmd(pte_mkold(pmd_pte(pmd)))
#define pmd_mkwrite(pmd)	pte_pmd(pte_mkwrite(pmd_pte(pmd)))
#define pmd_mkdirty(pmd)	pte_pmd(pte_mkdirty(pmd_pte(pmd)))
#define pmd_mkyoung(pmd)	pte_pmd(pte_mkyoung(pmd_pte(pmd)))
#define pmd_mknotpresent(pmd)	(__pmd(pmd_val(pmd) &= ~PMD_TYPE_MASK))

#define __HAVE_ARCH_PMD_WRITE
#define pmd_write(pmd)		pte_write(pmd_pte(pmd))

#define pmd_mkhuge(pmd)		(__pmd(pmd_val(pmd) & ~PMD_TABLE_BIT))

#define pmd_pfn(pmd)		(((pmd_val(pmd) & PMD_MASK) & PHYS_MASK) >> PAGE_SHIFT)
#define pfn_pmd(pfn,prot)	(__pmd(((phys_addr_t)(pfn) << PAGE_SHIFT) | pgprot_val(prot)))
#define mk_pmd(page,prot)	pfn_pmd(page_to_pfn(page),prot)

#define pmd_page(pmd)           pfn_to_page(__phys_to_pfn(pmd_val(pmd) & PHYS_MASK))

#define set_pmd_at(mm, addr, pmdp, pmd)	set_pte_at(mm, addr, (pte_t *)pmdp, pmd_pte(pmd))

static inline int has_transparent_hugepage(void)
{
	return 1;
}

#define __pgprot_modify(prot,mask,bits) \
	__pgprot((pgprot_val(prot) & ~(mask)) | (bits))

/*
 * Mark the prot value as uncacheable and unbufferable.
 */
#define pgprot_noncached(prot) \
	__pgprot_modify(prot, PTE_ATTRINDX_MASK, PTE_ATTRINDX(MT_DEVICE_nGnRE) | PTE_PXN | PTE_UXN)
#define pgprot_writecombine(prot) \
	__pgprot_modify(prot, PTE_ATTRINDX_MASK, PTE_ATTRINDX(MT_NORMAL_NC) | PTE_PXN | PTE_UXN)
#define pgprot_dmacoherent(prot) \
	__pgprot_modify(prot, PTE_ATTRINDX_MASK, PTE_ATTRINDX(MT_NORMAL_NC) | PTE_PXN | PTE_UXN)
#define pgprot_iotable_init(prot) \
	__pgprot_modify(prot, PTE_ATTRINDX_MASK, PTE_ATTRINDX(MT_DEVICE_nGnRE) | PTE_PXN | PTE_UXN)
#define __HAVE_PHYS_MEM_ACCESS_PROT
struct file;
extern pgprot_t phys_mem_access_prot(struct file *file, unsigned long pfn,
				     unsigned long size, pgprot_t vma_prot);

#define pmd_none(pmd)		(!pmd_val(pmd))
#define pmd_present(pmd)	(pmd_val(pmd))

#define pmd_bad(pmd)		(!(pmd_val(pmd) & 2))

#ifdef CONFIG_TIMA_RKP
#define pmd_block(pmd)      ((pmd_val(pmd) & 0x3)  == 1)
#endif

static inline void set_pmd(pmd_t *pmdp, pmd_t pmd)
{
#ifdef CONFIG_TIMA_RKP
	if (rkp_is_pg_protected((u64)pmdp)) {
		rkp_call(RKP_PMD_SET, (unsigned long)pmdp, pmd_val(pmd), 0, 0, 0);
	} else {
		asm volatile("mov x1, %0\n"
					  "mov x2, %1\n"
 					  "str x2, [x1]\n"
		:
		: "r" (pmdp), "r" (pmd)
		: "x1", "x2", "memory" );
	}
#else 
	*pmdp = pmd;
#endif /* CONFIG_TIMA_RKP */
	dsb(ishst);
}

static inline void pmd_clear(pmd_t *pmdp)
{
	set_pmd(pmdp, __pmd(0));
}

static inline pte_t *pmd_page_vaddr(pmd_t pmd)
{
	return __va(pmd_val(pmd) & PHYS_MASK & (s32)PAGE_MASK);
}

#define pmd_page(pmd)		pfn_to_page(__phys_to_pfn(pmd_val(pmd) & PHYS_MASK))

/*
 * Conversion functions: convert a page and protection to a page entry,
 * and a page entry and page directory to the page they refer to.
 */
#define mk_pte(page,prot)	pfn_pte(page_to_pfn(page),prot)

#ifndef CONFIG_ARM64_64K_PAGES

#define pud_none(pud)		(!pud_val(pud))
#define pud_bad(pud)		(!(pud_val(pud) & 2))
#define pud_present(pud)	(pud_val(pud))

static inline void set_pud(pud_t *pudp, pud_t pud)
{
#ifdef CONFIG_TIMA_RKP
	if (rkp_is_pg_protected((u64)pudp)) {
		rkp_call(RKP_PGD_SET, (unsigned long)pudp, pud_val(pud), 0, 0, 0);
	} else {
		asm volatile("mov x1, %0\n"
					  "mov x2, %1\n"
 					  "str x2, [x1]\n"
		:
		: "r" (pudp), "r" (pud)
		: "x1", "x2", "memory" );
	}
#else
	*pudp = pud;
#endif
	dsb(ishst);
}

static inline void pud_clear(pud_t *pudp)
{
	set_pud(pudp, __pud(0));
}

static inline pmd_t *pud_page_vaddr(pud_t pud)
{
	return __va(pud_val(pud) & PHYS_MASK & (s32)PAGE_MASK);
}

#endif	/* CONFIG_ARM64_64K_PAGES */

/* to find an entry in a page-table-directory */
#define pgd_index(addr)		(((addr) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))

#define pgd_offset(mm, addr)	((mm)->pgd+pgd_index(addr))

/* to find an entry in a kernel page-table-directory */
#define pgd_offset_k(addr)	pgd_offset(&init_mm, addr)

/* Find an entry in the second-level page table.. */
#ifndef CONFIG_ARM64_64K_PAGES
#define pmd_index(addr)		(((addr) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))
static inline pmd_t *pmd_offset(pud_t *pud, unsigned long addr)
{
	return (pmd_t *)pud_page_vaddr(*pud) + pmd_index(addr);
}
#endif

/* Find an entry in the third-level page table.. */
#define pte_index(addr)		(((addr) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))

static inline pte_t pte_modify(pte_t pte, pgprot_t newprot)
{
	const pteval_t mask = PTE_USER | PTE_PXN | PTE_UXN | PTE_RDONLY |
			      PTE_PROT_NONE | PTE_VALID | PTE_WRITE;
	pte_val(pte) = (pte_val(pte) & ~mask) | (pgprot_val(newprot) & mask);
	return pte;
}

static inline pmd_t pmd_modify(pmd_t pmd, pgprot_t newprot)
{
	return pte_pmd(pte_modify(pmd_pte(pmd), newprot));
}

extern pgd_t swapper_pg_dir[PTRS_PER_PGD];
extern pgd_t idmap_pg_dir[PTRS_PER_PGD];

#define SWAPPER_DIR_SIZE	(3 * PAGE_SIZE)
#define IDMAP_DIR_SIZE		(2 * PAGE_SIZE)

/*
 * Encode and decode a swap entry:
 *	bits 0-1:	present (must be zero)
 *	bit  2:		PTE_FILE
 *	bits 3-8:	swap type
 *	bits 9-57:	swap offset
 */
#define __SWP_TYPE_SHIFT	3
#define __SWP_TYPE_BITS		6
#define __SWP_OFFSET_BITS	49
#define __SWP_TYPE_MASK		((1 << __SWP_TYPE_BITS) - 1)
#define __SWP_OFFSET_SHIFT	(__SWP_TYPE_BITS + __SWP_TYPE_SHIFT)
#define __SWP_OFFSET_MASK	((1UL << __SWP_OFFSET_BITS) - 1)

#define __swp_type(x)		(((x).val >> __SWP_TYPE_SHIFT) & __SWP_TYPE_MASK)
#define __swp_offset(x)		(((x).val >> __SWP_OFFSET_SHIFT) & __SWP_OFFSET_MASK)
#define __swp_entry(type,offset) ((swp_entry_t) { ((type) << __SWP_TYPE_SHIFT) | ((offset) << __SWP_OFFSET_SHIFT) })

#define __pte_to_swp_entry(pte)	((swp_entry_t) { pte_val(pte) })
#define __swp_entry_to_pte(swp)	((pte_t) { (swp).val })

/*
 * Ensure that there are not more swap files than can be encoded in the kernel
 * PTEs.
 */
#define MAX_SWAPFILES_CHECK() BUILD_BUG_ON(MAX_SWAPFILES_SHIFT > __SWP_TYPE_BITS)

/*
 * Encode and decode a file entry:
 *	bits 0-1:	present (must be zero)
 *	bit  2:		PTE_FILE
 *	bits 3-57:	file offset / PAGE_SIZE
 */
#define pte_file(pte)		(pte_val(pte) & PTE_FILE)
#define pte_to_pgoff(x)		(pte_val(x) >> 3)
#define pgoff_to_pte(x)		__pte(((x) << 3) | PTE_FILE)

#define PTE_FILE_MAX_BITS	55

extern int kern_addr_valid(unsigned long addr);

#include <asm-generic/pgtable.h>

/*
 * remap a physical page `pfn' of size `size' with page protection `prot'
 * into virtual address `from'
 */
#define io_remap_pfn_range(vma,from,pfn,size,prot) \
		remap_pfn_range(vma, from, pfn, size, prot)

#define pgtable_cache_init() do { } while (0)

#endif /* !__ASSEMBLY__ */

#endif /* __ASM_PGTABLE_H */
