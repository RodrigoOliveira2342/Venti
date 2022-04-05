
/* auto-generated by gen_syscalls.py, don't edit */
#ifndef Z_INCLUDE_SYSCALLS_IPM_H
#define Z_INCLUDE_SYSCALLS_IPM_H


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

extern int z_impl_ipm_send(const struct device * ipmdev, int wait, uint32_t id, const void * data, int size);

__pinned_func
static inline int ipm_send(const struct device * ipmdev, int wait, uint32_t id, const void * data, int size)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		/* coverity[OVERRUN] */
		return (int) arch_syscall_invoke5(*(uintptr_t *)&ipmdev, *(uintptr_t *)&wait, *(uintptr_t *)&id, *(uintptr_t *)&data, *(uintptr_t *)&size, K_SYSCALL_IPM_SEND);
	}
#endif
	compiler_barrier();
	return z_impl_ipm_send(ipmdev, wait, id, data, size);
}


extern int z_impl_ipm_max_data_size_get(const struct device * ipmdev);

__pinned_func
static inline int ipm_max_data_size_get(const struct device * ipmdev)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		/* coverity[OVERRUN] */
		return (int) arch_syscall_invoke1(*(uintptr_t *)&ipmdev, K_SYSCALL_IPM_MAX_DATA_SIZE_GET);
	}
#endif
	compiler_barrier();
	return z_impl_ipm_max_data_size_get(ipmdev);
}


extern uint32_t z_impl_ipm_max_id_val_get(const struct device * ipmdev);

__pinned_func
static inline uint32_t ipm_max_id_val_get(const struct device * ipmdev)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		/* coverity[OVERRUN] */
		return (uint32_t) arch_syscall_invoke1(*(uintptr_t *)&ipmdev, K_SYSCALL_IPM_MAX_ID_VAL_GET);
	}
#endif
	compiler_barrier();
	return z_impl_ipm_max_id_val_get(ipmdev);
}


extern int z_impl_ipm_set_enabled(const struct device * ipmdev, int enable);

__pinned_func
static inline int ipm_set_enabled(const struct device * ipmdev, int enable)
{
#ifdef CONFIG_USERSPACE
	if (z_syscall_trap()) {
		/* coverity[OVERRUN] */
		return (int) arch_syscall_invoke2(*(uintptr_t *)&ipmdev, *(uintptr_t *)&enable, K_SYSCALL_IPM_SET_ENABLED);
	}
#endif
	compiler_barrier();
	return z_impl_ipm_set_enabled(ipmdev, enable);
}


#ifdef __cplusplus
}
#endif

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif

#endif
#endif /* include guard */
