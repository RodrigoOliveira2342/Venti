
/* auto-generated by gen_syscalls.py, don't edit */
#ifndef Z_INCLUDE_SYSCALLS_UART_MUX_H
#define Z_INCLUDE_SYSCALLS_UART_MUX_H


#ifndef _ASMLANGUAGE

#include <syscall_list.h>
#include <syscall.h>

#include <linker/sections.h>

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#if !defined(__XCC__)
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern const struct device * z_impl_uart_mux_find(int dlci_address);

__pinned_func
static inline const struct device * uart_mux_find(int dlci_address)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		/* coverity[OVERRUN] */
		return (const struct device *) arch_syscall_invoke1(*(uintptr_t *)&dlci_address, K_SYSCALL_UART_MUX_FIND);
	}
#endif
	compiler_barrier();
	return z_impl_uart_mux_find(dlci_address);
}


#ifdef __cplusplus
}
#endif

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif

#endif
#endif /* include guard */
